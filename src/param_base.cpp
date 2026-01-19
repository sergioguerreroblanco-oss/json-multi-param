/**
 * @file        param_base.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-11-25>
 * @version     1.0.0
 *
 * @brief       ParamBase implementation.
 *
 * @details
 * Contains the non-templated common logic shared by all parameters.
 */

/*****************************************************************************/

/* Project libraries */
#include "param_base.h"

/*****************************************************************************/

/* Namespace */
namespace jsonmp
{

/*****************************************************************************/

/* Public Methods */

ParamBase::ParamBase(std::string name, ValueType type) : name(std::move(name)), type(type) {}

const std::string& ParamBase::getName() const noexcept
{
    return name;
}

ValueType ParamBase::getType() const noexcept
{
    return type;
}

/*****************************************************************************/

}  // namespace jsonmp
