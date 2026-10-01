//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ARGUMENT_H
#define OPENDMI_ARGUMENT_H

#pragma once

#include <opendmi/types.h>

typedef struct dmi_argument dmi_argument_t;

/**
 * @brief Argument handler function type.
 */
typedef bool dmi_argument_fn(const char *value);

/**
 * @brief Type of the value of an argument or of an option.
 */
typedef enum dmi_argument_type
{
    DMI_ARGUMENT_TYPE_NONE,  ///< No value, e.g. of an option which is a flag
    DMI_ARGUMENT_TYPE_STRING ///< String value
} dmi_argument_type_t;

/**
 * @brief Command line argument descriptor, which describes the argument of a
 * command or the value of an option.
 */
struct dmi_argument
{
    /**
     * @brief Name of the argument, which usage is printed with.
     */
    const char *name;

    /**
     * @brief Type of the value.
     */
    dmi_argument_type_t type;

    /**
     * @brief Whether the value is required.
     */
    bool required;

    /**
     * @brief Variable to store the value in.
     */
    void *data;

    /**
     * @brief Handler of the value.
     */
    dmi_argument_fn *handler;
};

#endif // !OPENDMI_ARGUMENT_H
