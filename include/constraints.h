/**
 * @file        constraints.h
 * @author      Sergio Guerrero Blanco
 * @date        <2025-11-25>
 * @version     1.0.0
 *
 * @brief       Value constraints for parameters.
 *
 * @details
 * Defines a generic constraint structure used to validate parameter values
 * during assignment and parsing.
 */

/*****************************************************************************/

#pragma once

/*****************************************************************************/

/* Standard libraries */
#include <string>
#include <vector>

/*****************************************************************************/

/* Namespace */
namespace jsonmp
{

/*****************************************************************************/
/**
 * @brief Primary template for parameter constraints.
 *
 * @details
 * The primary template is intentionally left empty. Concrete supported types
 * (int, double, std::string, bool, ...) are implemented via full
 * specializations.
 */
template <typename T>
struct Constraints
{
    // Empty by default – unsupported types will have no constraints.
};

/*****************************************************************************/
/**
 * @brief Constraints for integer parameters (`int`).
 *
 * @details
 * Typical usage:
 * ```cpp
 * Constraints<int> c;
 * c.has_min = true;
 * c.min     = 0;
 * c.has_max = true;
 * c.max     = 200;
 * ```
 */
template <>
struct Constraints<int>
{
    bool has_min = false; /**< Whether `min` is active. */
    int  min     = 0;     /**< Minimum allowed value (inclusive). */

    bool has_max = false; /**< Whether `max` is active. */
    int  max     = 0;     /**< Maximum allowed value (inclusive). */
};

/*****************************************************************************/
/**
 * @brief Constraints for floating-point parameters (`double`).
 *
 * @details
 * Typical usage:
 * ```cpp
 * Constraints<double> c;
 * c.has_min = true;
 * c.min     = 0.0;
 * c.has_max = true;
 * c.max     = 60.0;
 * ```
 */
template <>
struct Constraints<double>
{
    bool   has_min = false; /**< Whether `min` is active. */
    double min     = 0.0;   /**< Minimum allowed value (inclusive). */

    bool   has_max = false; /**< Whether `max` is active. */
    double max     = 0.0;   /**< Maximum allowed value (inclusive). */
};

/*****************************************************************************/
/**
 * @brief Constraints for string parameters (`std::string`).
 *
 * @details
 * Supports:
 *  - Minimum / maximum length.
 *  - A regular expression pattern (optional, not enforced yet).
 *  - A set of allowed values (whitelist).
 *
 * Typical usage:
 * ```cpp
 * Constraints<std::string> c;
 * c.has_min_length = true;
 * c.min_length     = 3;
 *
 * c.has_allowed = true;
 * c.allowed_values = {"AUTO", "MANUAL"};
 * ```
 */
template <>
struct Constraints<std::string>
{
    bool        has_min_length = false; /**< Whether `min_length` is active. */
    std::size_t min_length     = 0;     /**< Minimum length in characters.   */

    bool        has_max_length = false; /**< Whether `max_length` is active. */
    std::size_t max_length     = 0;     /**< Maximum length in characters.   */

    bool        has_regex = false; /**< Whether `regex` is active.      */
    std::string regex;             /**< Regular expression pattern.     */

    bool                     has_allowed = false; /**< Whether `allowed_values` is active. */
    std::vector<std::string> allowed_values;      /**< Whitelisted values.                  */
};

/*****************************************************************************/
/**
 * @brief Constraints for boolean parameters (`bool`).
 *
 * @details
 * For now this specialization is intentionally minimal; booleans rarely
 * require range checks. It is provided for symmetry and future extension.
 */
template <>
struct Constraints<bool>
{
    // Currently no boolean-specific constraints.
    // Placeholder for future flags (e.g., "read-only", etc.).
};

/*****************************************************************************/

}  // namespace jsonmp