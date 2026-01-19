/**
 * @file        param_set.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-11-25>
 * @version     1.0.0
 *
 * @brief       Parameter set (multi-parameter container).
 *
 * @details
 * Holds a collection of named parameters. It supports:
 *  - Schema definition (name, type, constraints).
 *  - Typed set/get operations by name.
 *  - Serialization to/from a compact string format and JSON.
 */

/*****************************************************************************/

#pragma once

/*****************************************************************************/

/* Standard libraries */
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

/* External libraries */
#include <nlohmann/json.hpp>

/* Project libraries */
#include "constraints.h"
#include "param.h"
#include "param_base.h"

/*****************************************************************************/

/*****************************************************************************/
/* Namespace */
/*****************************************************************************/

/**
 * @namespace jsonmp
 * @brief JSON Multi-Param library namespace.
 */
namespace jsonmp
{

/*****************************************************************************/
/* Class ParamSet */
/*****************************************************************************/

/**
 * @class ParamSet
 * @brief Named, typed multi-parameter container.
 *
 * @details
 * ParamSet stores a set of named parameters (Param<T>) using type erasure (ParamBase).
 *
 * Typical usage:
 *  1) Define a schema (names, types, constraints).
 *  2) Set/get values by name using typed APIs.
 *  3) Serialize/deserialize using JSON or a compact key=value format.
 *
 * Design notes:
 *  - Strict parsing: unknown parameters in incoming JSON/compact strings return an error.
 *  - Deterministic compact serialization: to_compact_string() emits keys sorted lexicographically.
 *  - Compact-string escaping: the characters '\\', ';' and '=' are escaped with a leading '\\'
 *    inside values.
 */
class ParamSet
{
    /******************************************************************/

   public:
    /**
     * @brief Defines a new parameter in the schema.
     *
     * @tparam T Parameter value type.
     * @param name Parameter unique name (key).
     * @param default_value Default value used after creation and when resetting.
     * @param constraints Constraints applied on every assignment (min/max/allowed values, etc).
     * @return A reference to the created Param<T>.
     *
     * @throws std::runtime_error If a parameter with the same name already exists.
     */
    template <typename T>
    Param<T>& add(const std::string& name, T default_value, Constraints<T> constraints = {})
    {
        auto it = params.find(name);
        if (it != params.end())
        {
            throw std::runtime_error("Parameter already exists: " + name);
        }

        auto param_ptr =
            std::unique_ptr<Param<T>>(new Param<T>(name, default_value, std::move(constraints)));

        Param<T>* raw_ptr = param_ptr.get();
        params.emplace(name, std::move(param_ptr));
        return *raw_ptr;
    }

    /**
     * @name Convenience helpers
     * @brief Convenience wrappers for supported types.
     * @{
     */

    /**
     * @brief Defines an int parameter.
     */
    Param<int>& addInt(const std::string& name, int default_value,
                       Constraints<int> constraints = {})
    {
        return add<int>(name, default_value, constraints);
    }

    /**
     * @brief Defines a double parameter.
     */
    Param<double>& addDouble(const std::string& name, double default_value,
                             Constraints<double> constraints = {})
    {
        return add<double>(name, default_value, constraints);
    }

    /**
     * @brief Defines a string parameter.
     */
    Param<std::string>& addString(const std::string& name, std::string default_value,
                                  Constraints<std::string> constraints = {})
    {
        return add<std::string>(name, std::move(default_value), constraints);
    }

    /**
     * @brief Defines a bool parameter.
     */
    Param<bool>& addBool(const std::string& name, bool default_value,
                         Constraints<bool> constraints = {})
    {
        return add<bool>(name, default_value, constraints);
    }

    /** @} */

    /**
     * @brief Sets a parameter value by name (typed).
     *
     * @tparam T Parameter value type.
     * @param name Parameter name.
     * @param value New value to assign.
     * @param[out] error Filled with a human-readable reason on failure.
     * @return true on success; false if the name is unknown, type mismatches, or constraints fail.
     */
    template <typename T>
    bool set(const std::string& name, T value, std::string& error)
    {
        auto it = params.find(name);
        if (it == params.end())
        {
            error = "Unknown parameter: " + name;
            return false;
        }

        auto* typed = dynamic_cast<Param<T>*>(it->second.get());
        if (!typed)
        {
            error = "Type mismatch for parameter: " + name;
            return false;
        }

        return typed->set(value, error);
    }

    /**
     * @brief Gets a parameter value by name (typed).
     *
     * @tparam T Parameter value type.
     * @param name Parameter name.
     * @return The current value.
     *
     * @throws std::runtime_error If the name does not exist or the requested type mismatches.
     */
    template <typename T>
    T get(const std::string& name) const
    {
        auto it = params.find(name);
        if (it == params.end())
        {
            throw std::runtime_error("Unknown parameter: " + name);
        }

        auto* typed = dynamic_cast<Param<T>*>(it->second.get());
        if (!typed)
        {
            throw std::runtime_error("Type mismatch for parameter: " + name);
        }

        return typed->get();
    }

    /**
     * @brief Gets a parameter value by name, or returns a fallback if missing.
     *
     * @tparam T Value type.
     * @param name Parameter name.
     * @param fallback Value returned if the parameter does not exist.
     * @return Parameter value if found, otherwise fallback.
     *
     * @throws std::runtime_error If the parameter exists but the requested type mismatches.
     */
    template <typename T>
    T get_or(const std::string& name, T fallback) const
    {
        auto it = params.find(name);
        if (it == params.end())
        {
            return fallback;
        }

        auto* typed = dynamic_cast<Param<T>*>(it->second.get());
        if (!typed)
        {
            throw std::runtime_error("Type mismatch for parameter: " + name);
        }

        return typed->get();
    }

    /**
     * @brief Serializes all parameters into a compact "key=value" string.
     *
     * @details
     * Output format: "key1=value1;key2=value2;..."
     *  - Keys are emitted in lexicographical order (deterministic output).
     *  - Values are escaped so that '\\', ';' and '=' are prefixed with '\\'.
     *
     * @return Compact serialized representation.
     */
    std::string to_compact_string() const;

    /**
     * @brief Parses and applies a compact "key=value" string.
     *
     * @details
     * Strict mode: returns false if an unknown key is found.
     * Escaping rules: '\\', ';' and '=' may be escaped with a leading '\\' in values.
     *
     * @param s Compact string to parse.
     * @param[out] error Filled on failure.
     * @return true if all tokens were successfully parsed and applied; false otherwise.
     */
    bool from_compact_string(const std::string& s, std::string& error);

    /**
     * @brief Serializes all parameters into a JSON object.
     *
     * @details
     * JSON values are emitted with the parameter's declared type:
     *  - Integer/Unsigned/FloatingPoint -> JSON number
     *  - Boolean -> JSON boolean
     *  - String -> JSON string
     *
     * @note JSON object key ordering is not part of the contract. Do not rely on ordering.
     */
    nlohmann::json to_json() const;

    /**
     * @brief Parses and applies a JSON object.
     *
     * @details
     * Strict mode: returns false if an unknown key is found.
     * Type handling:
     *  - Numeric params accept JSON numbers or strings.
     *  - Bool params accept JSON booleans, integer numbers (0/!=0), or strings.
     *
     * @param j JSON object.
     * @param[out] error Filled on failure.
     * @return true if all entries were successfully parsed and applied; false otherwise.
     */
    bool from_json(const nlohmann::json& j, std::string& error);

    /******************************************************************/

   private:
    /**
     * @brief Escapes value characters used as compact-format separators.
     *
     * @details
     * The characters '\\', ';' and '=' are escaped by prefixing them with '\\'.
     */
    static std::string escape_compact(const std::string& s);

    /**
     * @brief Unescapes a compact-format value.
     *
     * @param s Escaped input.
     * @param[out] out Unescaped output.
     * @param[out] error Filled if the input contains an invalid escape sequence.
     * @return true on success; false otherwise.
     */
    static bool unescape_compact(const std::string& s, std::string& out, std::string& error);

    std::unordered_map<std::string, std::unique_ptr<ParamBase>> params;
};

}  // namespace jsonmp