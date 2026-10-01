//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_MODULE_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_MODULE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-module.h>

/**
 * @internal
 * @brief Names of the memory module types.
 */
extern const dmi_name_set_t dmi_memory_module_type_names;

/**
 * @internal
 * @brief Names of the statuses of the size of a memory module.
 */
extern const dmi_name_set_t dmi_memory_module_size_status_names;

/**
 * @internal
 * @brief Names of the error status bits of a memory module.
 */
extern const dmi_name_set_t dmi_memory_module_error_names;

/**
 * @internal
 * @brief Decode the installed size of a memory module.
 *
 * @details Size is carried as the power of two it is a number of megabytes
 * of, or as the value saying why there is no size, along with the number of
 * the banks. Size out of range is logged as a warning.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the size in.
 *
 * @return Always `true`.
 */
bool dmi_memory_module_decode_installed_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Decode the enabled size of a memory module.
 *
 * @details See `dmi_memory_module_decode_installed_size()`.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the size in.
 *
 * @return Always `true`.
 */
bool dmi_memory_module_decode_enabled_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the installed or enabled size of a memory module.
 *
 * @details Size is written as the power of two it is a number of megabytes
 * of, or as the value saying why there is no size, along with the number of
 * the banks.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the size.
 * @param[out] data  Data the field is to carry.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_memory_module_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

#endif // !OPENDMI_ENTITY_MEMORY_MODULE_INTERNAL_H
