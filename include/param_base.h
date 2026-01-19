/**
 * @file        param_base.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-11-25>
 * @version     1.0.0
 *
 * @brief       Abstract base class for typed parameters.
 *
 * @details
 * ParamBase provides the non-templated interface used by ParamSet to store and handle
 * heterogeneous typed parameters (Param<T>) in a single container.
 *
 * A parameter has:
 *  - A stable logical name (key).
 *  - A logical value type (ValueType).
 *  - String-based conversion for serialization and parsing.
 */

/*****************************************************************************/

#pragma once

/*****************************************************************************/

/* Standard libraries */
#include <string>

/*****************************************************************************/

/* Namespace */
namespace jsonmp
{

/*****************************************************************************/

/**
 * @enum ValueType
 * @brief Logical type of a parameter's value.
 *
 * @details
 * This is a lightweight runtime tag for parameter types. It is primarily used to:
 *  - Describe a parameter in a generic way (e.g., for serialization).
 *  - Validate JSON input/output conversions.
 */
enum class ValueType
{
    /** Signed integer value (e.g., int, long). */
    Integer,

    /** Unsigned integer value (e.g., unsigned, std::uint64_t). */
    UnsignedInteger,

    /** Floating-point value (e.g., float, double). */
    FloatingPoint,

    /** Boolean value (true/false). */
    Boolean,

    /** Text value (std::string). */
    String
};

/*****************************************************************************/
/* Class ParamBase */
/*****************************************************************************/

/**
 * @class ParamBase
 * @brief Abstract base class for all parameters.
 *
 * @details
 * ParamBase defines the type-erased interface used by ParamSet. Concrete implementations
 * (Param<T>) provide typed storage, constraints checking, and parsing/formatting logic.
 *
 * Copy and move operations are disabled to keep a clear single-owner model and avoid
 * accidental slicing or ownership confusion when used through base pointers.
 */
class ParamBase
{
    /******************************************************************/

   public:
    /**
     * @brief Constructs a base parameter.
     *
     * @param name Parameter logical name (key).
     * @param type Parameter logical value type.
     */
    ParamBase(std::string name, ValueType type);

    /**
     * @brief Virtual destructor.
     */
    virtual ~ParamBase() = default;

    /** @name Non-copyable / non-movable
     *  @brief ParamBase instances are not copyable or movable.
     *  @{
     */
    ParamBase(const ParamBase&)            = delete;
    ParamBase& operator=(const ParamBase&) = delete;
    ParamBase(ParamBase&&)                 = delete;
    ParamBase& operator=(ParamBase&&)      = delete;
    /** @} */

    /**
     * @brief Returns the parameter name.
     */
    const std::string& getName() const noexcept;

    /**
     * @brief Returns the parameter logical type.
     */
    ValueType getType() const noexcept;

    /**
     * @brief Returns the current value as a textual representation.
     *
     * @details
     * Used by generic serializers (compact string / JSON). The exact formatting is type-specific.
     */
    virtual std::string to_string() const = 0;

    /**
     * @brief Parses and assigns the value from its textual representation.
     *
     * @param repr Input textual representation.
     * @param[out] error Filled with a human-readable reason on failure.
     * @return true on success; false if parsing fails or constraints are violated.
     */
    virtual bool from_string(const std::string& repr, std::string& error) = 0;

    /******************************************************************/

   private:
    /** @brief Parameter logical name (key). */
    std::string name;

    /** @brief Parameter logical runtime type tag. */
    ValueType type;

    /******************************************************************/
};

}  // namespace jsonmp