/**
 * @file        param_set.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-11-25>
 * @version     1.0.0
 *
 * @brief Implementation of ParamSet: schema management and serialization.
 *
 * @details
 * Implements deterministic compact serialization (sorted keys), compact-format escaping,
 * and strict parsing for both compact strings and JSON.
 */

/*****************************************************************************/

/* Standard libraries */
#include <algorithm>
#include <sstream>
#include <vector>

/* Project libraries */
#include "param_set.h"

/*****************************************************************************/

/* Namespace */
namespace jsonmp
{

/*****************************************************************************/

/* Public Methods */
std::string ParamSet::to_compact_string() const
{
    std::vector<std::string> names;
    names.reserve(params.size());

    for (const auto& entry : params)
    {
        names.push_back(entry.first);
    }

    std::sort(names.begin(), names.end());

    std::ostringstream oss;
    bool               first = true;

    for (const auto& name : names)
    {
        const auto it = params.find(name);
        if (it == params.end())
        {
            continue;  // Should never happen
        }

        if (!first)
        {
            oss << ';';
        }
        first = false;

        oss << name << '=' << escape_compact(it->second->to_string());
    }

    return oss.str();
}

bool ParamSet::from_compact_string(const std::string& s, std::string& error)
{
    error.clear();

    std::string token;
    token.reserve(s.size());

    bool escaping = false;

    auto flush_token = [&](const std::string& t) -> bool
    {
        if (t.empty())
            return true;

        // Find first unescaped '='
        std::size_t eq_pos      = std::string::npos;
        bool        eq_escaping = false;

        for (std::size_t i = 0; i < t.size(); ++i)
        {
            char c = t[i];
            if (eq_escaping)
            {
                eq_escaping = false;
                continue;
            }
            if (c == '\\')
            {
                eq_escaping = true;
                continue;
            }
            if (c == '=')
            {
                eq_pos = i;
                break;
            }
        }

        if (eq_pos == std::string::npos)
        {
            error = "Invalid token in compact string (missing '='): '" + t + "'";
            return false;
        }

        std::string name      = t.substr(0, eq_pos);
        std::string raw_value = t.substr(eq_pos + 1);

        auto it = params.find(name);
        if (it == params.end())
        {
            error = "Unknown parameter in compact string: '" + name + "'";
            return false;
        }

        std::string value;
        if (!unescape_compact(raw_value, value, error))
        {
            return false;
        }

        if (!it->second->from_string(value, error))
        {
            return false;
        }

        return true;
    };

    for (char c : s)
    {
        if (escaping)
        {
            token.push_back('\\');
            token.push_back(c);
            escaping = false;
            continue;
        }

        if (c == '\\')
        {
            escaping = true;
            continue;
        }

        if (c == ';')
        {
            if (!flush_token(token))
                return false;

            token.clear();
            continue;
        }

        token.push_back(c);
    }

    if (escaping)
    {
        error = "Invalid compact string: trailing backslash";
        return false;
    }

    // Flush last token
    if (!flush_token(token))
        return false;

    return true;
}

nlohmann::json ParamSet::to_json() const
{
    std::vector<std::string> names;
    names.reserve(params.size());

    for (const auto& entry : params)
    {
        names.push_back(entry.first);
    }

    std::sort(names.begin(), names.end());

    nlohmann::json j = nlohmann::json::object();

    for (const auto& name : names)
    {
        const auto it = params.find(name);
        if (it == params.end())
        {
            continue;  // Should never happen
        }

        const ParamBase*  param = it->second.get();
        const std::string repr  = param->to_string();

        switch (param->getType())
        {
            case ValueType::Integer:
                j[name] = std::stoi(repr);
                break;
            case ValueType::UnsignedInteger:
                j[name] = static_cast<std::uint64_t>(std::stoull(repr));
                break;
            case ValueType::FloatingPoint:
                j[name] = std::stod(repr);
                break;
            case ValueType::Boolean:
            {
                if (repr == "1" || repr == "true" || repr == "True")
                    j[name] = true;
                else if (repr == "0" || repr == "false" || repr == "False")
                    j[name] = false;
                else
                    j[name] = false;
                break;
            }
            case ValueType::String:
                j[name] = repr;
                break;
            default:
                j[name] = repr;
                break;
        }
    }

    return j;
}

bool ParamSet::from_json(const nlohmann::json& j, std::string& error)
{
    error.clear();

    if (!j.is_object())
    {
        error = "Expected JSON object for ParamSet::from_json()";
        return false;
    }
    for (auto it = j.begin(); it != j.end(); ++it)
    {
        const std::string& name = it.key();

        auto param_it = params.find(name);
        if (param_it == params.end())
        {
            error = "Unknown parameter in JSON: '" + name + "'";
            return false;
        }

        const ParamBase* param = param_it->second.get();

        // Convert JSON value to string representation accepted by from_string()
        std::string value;

        switch (param->getType())
        {
            case ValueType::Boolean:
            {
                if (it->is_boolean())
                    value = it->get<bool>() ? "1" : "0";
                else if (it->is_number_integer())
                    value = (it->get<long long>() != 0) ? "1" : "0";
                else if (it->is_string())
                    value = it->get<std::string>();
                else
                {
                    error = "Invalid JSON type for boolean parameter: '" + name + "'";
                    return false;
                }
                break;
            }

            case ValueType::Integer:
            case ValueType::UnsignedInteger:
            case ValueType::FloatingPoint:
            {
                if (it->is_number())
                {
                    // dump() gives textual number without quotes
                    value = it->dump();
                }
                else if (it->is_string())
                {
                    value = it->get<std::string>();
                }
                else
                {
                    error = "Invalid JSON type for numeric parameter: '" + name + "'";
                    return false;
                }
                break;
            }

            case ValueType::String:
            {
                if (it->is_string())
                    value = it->get<std::string>();
                else
                    value = it->dump();  // allow non-string but store its textual form
                break;
            }

            default:
            {
                // Fallback: use string form
                if (it->is_string())
                    value = it->get<std::string>();
                else
                    value = it->dump();
                break;
            }
        }

        if (!param_it->second->from_string(value, error))
        {
            return false;
        }
    }

    return true;
}

std::string ParamSet::escape_compact(const std::string& s)
{
    std::string out;
    out.reserve(s.size());

    for (char c : s)
    {
        if (c == '\\' || c == ';' || c == '=')
        {
            out.push_back('\\');
        }
        out.push_back(c);
    }

    return out;
}

bool ParamSet::unescape_compact(const std::string& s, std::string& out, std::string& error)
{
    out.clear();
    out.reserve(s.size());

    bool escaping = false;

    for (char c : s)
    {
        if (escaping)
        {
            // Accept any escaped character, but at minimum we expect \ ; =
            out.push_back(c);
            escaping = false;
            continue;
        }

        if (c == '\\')
        {
            escaping = true;
            continue;
        }

        out.push_back(c);
    }

    if (escaping)
    {
        error = "Invalid escape sequence: trailing backslash";
        return false;
    }

    return true;
}

/*****************************************************************************/

}  // namespace jsonmp
