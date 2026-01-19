/**
 * @file        value_type_traits.h
 * @author      Sergio Guerrero Blanco
 * @date        <2025-11-25>
 * @version     1.0.0
 *
 * @brief       Compile-time mapping between C++ types and ValueType.
 *
 * @details
 * Provides a lightweight type trait used to associate a C++ type T
 * with its corresponding runtime ValueType.
 */

/*****************************************************************************/

#pragma once

/*****************************************************************************/

/* Project libraries */
#include "param_base.h"

/*****************************************************************************/

/* Namespace */
namespace jsonmp
{

/*****************************************************************************/
/* ValueTypeOf */
/*****************************************************************************/

/**
 * @struct ValueTypeOf
 * @brief Maps a C++ type to its corresponding ValueType.
 *
 * @tparam T C++ value type.
 *
 * @details
 * This trait must be specialized for every supported parameter type.
 * The primary template triggers a compile-time error for unsupported types.
 */
template <typename T>
struct ValueTypeOf
{
    static_assert(sizeof(T) == 0,
                  "Unsupported type T for ValueTypeOf<T>. "
                  "You must provide a specialization for this type.");
};

/** @brief ValueType mapping for int. */
template <>
struct ValueTypeOf<int>
{
    static constexpr ValueType value = ValueType::Integer;
};

/** @brief ValueType mapping for unsigned integer types. */
template <>
struct ValueTypeOf<unsigned int>
{
    static constexpr ValueType value = ValueType::UnsignedInteger;
};

/** @brief ValueType mapping for double. */
template <>
struct ValueTypeOf<double>
{
    static constexpr ValueType value = ValueType::FloatingPoint;
};

/** @brief ValueType mapping for std::string. */
template <>
struct ValueTypeOf<std::string>
{
    static constexpr ValueType value = ValueType::String;
};

/** @brief ValueType mapping for bool. */
template <>
struct ValueTypeOf<bool>
{
    static constexpr ValueType value = ValueType::Boolean;
};

/*****************************************************************************/

}  // namespace jsonmp