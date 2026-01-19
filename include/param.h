/**
 * @file        param.h
 * @author      Sergio Guerrero Blanco
 * @date        <2025-11-25>
 * @version     1.0.0
 *
 * @brief       Typed parameter implementation.
 *
 * @details
 * Implements a strongly-typed parameter with validation rules (constraints),
 * string-based parsing, and formatting.
 */

/*****************************************************************************/

#pragma once

/*****************************************************************************/

/* Standard libraries */
#include <sstream>
#include <string>

/* Project libraries */
#include "constraints.h"
#include "param_base.h"
#include "value_type_traits.h"

/*****************************************************************************/

/* Namespace */
namespace jsonmp
{

/*****************************************************************************/

/**
 * @brief Validates a value against a Constraints<T> instance.
 *
 * @tparam T Parameter value type.
 * @param constraints Constraint set to be checked.
 * @param value Value to validate.
 * @param[out] error Filled with a human-readable reason on failure.
 * @return true if all enabled constraints are satisfied; false otherwise.
 *
 * @details
 * This function is specialized for supported types. The primary template
 * accepts all values (no constraints). Specializations implement:
 *  - int: min/max
 *  - double: min/max
 *  - std::string: length limits + allowed-values set
 */
template <typename T>
bool validate_constraints(const Constraints<T>& constraints, const T& value, std::string& error);

/*****************************************************************************/
/* Class Param */
/*****************************************************************************/

/**
 * @class Param
 * @brief Strongly-typed parameter with constraints.
 *
 * @tparam T Parameter value type.
 *
 * @details
 * Param<T> stores a value of type T and enforces validation rules through
 * Constraints<T>. It provides string-based conversion for generic
 * serialization and parsing.
 */
template <typename T>
class Param : public ParamBase
{
    /******************************************************************/

   public:
    /**
     * @brief Constructs a typed parameter.
     *
     * @param name Parameter name.
     * @param default_value Initial value.
     * @param constraints Validation rules applied on assignment.
     */
    Param(std::string name, T default_value, Constraints<T> constraints)
        : ParamBase(std::move(name), ValueTypeOf<T>::value),
          value(default_value),
          default_value(default_value),
          constraints(std::move(constraints))
    {
    }

    /**
     * @brief Assigns a new value after validating constraints.
     *
     * @param new_value New value.
     * @param[out] error Filled on validation failure.
     * @return true on success; false if constraints are violated.
     *
     * @details
     * This function takes the value by value to enable move semantics for movable types
     * such as std::string while still being efficient for small scalar types.
     */
    bool set(T new_value, std::string& error)
    {
        if (!validate_constraints<T>(constraints, new_value, error))
        {
            return false;
        }

        value = std::move(new_value);
        return true;
    }

    /**
     * @brief Returns the current value.
     */
    const T& get() const noexcept { return value; }

    /**
     * @brief Returns the default value.
     */
    const T& get_default() const noexcept { return default_value; }

    /**
     * @brief Resets the parameter to its default value.
     *
     * @param[out] error Filled on validation failure (should not happen if the default is valid).
     * @return true on success; false if constraints are violated by the default.
     */
    bool reset(std::string& error)
    {
        // Pass-by-value is intentionally used in set() to allow moves; here we pass a copy.
        return set(default_value, error);
    }

    /**
     * @brief Returns the current value as a textual representation.
     *
     * @details
     * The default implementation uses std::ostringstream and the stream operator<< for T.
     */
    std::string to_string() const override
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    /**
     * @brief Parses and assigns a value from its textual representation.
     *
     * @param repr Input textual representation.
     * @param[out] error Filled on parsing or validation failure.
     * @return true on success; false otherwise.
     */
    bool from_string(const std::string& repr, std::string& error) override
    {
        std::istringstream iss(repr);
        T                  parsed{};

        if (!(iss >> parsed))
        {
            error = "Failed to parse value from string: '" + repr + "'";
            return false;
        }

        return set(parsed, error);
    }

    /******************************************************************/

   private:
   private:
    /** @brief Current parameter value. */
    T value;

    /** @brief Default parameter value (used by reset()). */
    T default_value;

    /** @brief Validation rules applied to this parameter. */
    Constraints<T> constraints;
};

/*****************************************************************************/
/* String specialization */
/*****************************************************************************/

/**
 * @brief Specialization for std::string to avoid stream-based formatting/parsing.
 */
template <>
inline std::string Param<std::string>::to_string() const
{
    return value;
}

/**
 * @brief Specialization for std::string parsing.
 */
template <>
inline bool Param<std::string>::from_string(const std::string& repr, std::string& error)
{
    error.clear();
    return set(repr, error);
}

/*****************************************************************************/
/* validate_constraints primary template */
/*****************************************************************************/

template <typename T>
bool validate_constraints(const Constraints<T>& constraints, const T& value, std::string& error)
{
    (void)constraints;
    (void)value;
    error.clear();
    return true;
}

// ============================================================
//  validate_constraints<int>
// ============================================================

template <>
inline bool validate_constraints<int>(const Constraints<int>& constraints, const int& value,
                                      std::string& error)
{
    if (constraints.has_min && value < constraints.min)
    {
        error = "Value " + std::to_string(value) + " is below minimum " +
                std::to_string(constraints.min);
        return false;
    }

    if (constraints.has_max && value > constraints.max)
    {
        error = "Value " + std::to_string(value) + " is above maximum " +
                std::to_string(constraints.max);
        return false;
    }

    error.clear();
    return true;
}

// ============================================================
//  validate_constraints<double>
// ============================================================

template <>
inline bool validate_constraints<double>(const Constraints<double>& constraints,
                                         const double& value, std::string& error)
{
    if (constraints.has_min && value < constraints.min)
    {
        error = "Value " + std::to_string(value) + " is below minimum " +
                std::to_string(constraints.min);
        return false;
    }

    if (constraints.has_max && value > constraints.max)
    {
        error = "Value " + std::to_string(value) + " is above maximum " +
                std::to_string(constraints.max);
        return false;
    }

    error.clear();
    return true;
}

// ============================================================
//  validate_constraints<std::string>
// ============================================================

template <>
inline bool validate_constraints<std::string>(const Constraints<std::string>& constraints,
                                              const std::string& value, std::string& error)
{
    const std::size_t len = value.size();

    if (constraints.has_min_length && len < constraints.min_length)
    {
        error = "String length " + std::to_string(len) + " is below minimum " +
                std::to_string(constraints.min_length);
        return false;
    }

    if (constraints.has_max_length && len > constraints.max_length)
    {
        error = "String length " + std::to_string(len) + " is above maximum " +
                std::to_string(constraints.max_length);
        return false;
    }

    if (constraints.has_allowed)
    {
        bool found = false;
        for (const auto& allowed : constraints.allowed_values)
        {
            if (value == allowed)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            error = "Value '" + value + "' is not in allowed set";
            return false;
        }
    }

    error.clear();
    return true;
}

}  // namespace jsonmp