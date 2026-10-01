//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_CONTROLLER_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_CONTROLLER_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-controller.h>

/**
 * @internal
 * @brief Names of the error detecting methods.
 */
extern const dmi_name_set_t dmi_error_detect_method_names;

/**
 * @internal
 * @brief Names of the error correcting capabilities.
 */
extern const dmi_name_set_t dmi_error_correct_caps_names;

/**
 * @internal
 * @brief Names of the supported speeds of memory modules.
 */
extern const dmi_name_set_t dmi_memory_module_speed_names;

/**
 * @internal
 * @brief Names of the memory interleave modes.
 */
extern const dmi_name_set_t dmi_memory_interleave_names;

/**
 * @internal
 * @brief Names of the supported voltages of memory modules.
 */
extern const dmi_name_set_t dmi_memory_module_voltage_names;

/**
 * @internal
 * @brief Decode the maximum size of a memory module.
 *
 * @details Module size is carried as the power of two it is a number of
 * megabytes of.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the size in.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_memory_controller_decode_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the maximum size of a memory module.
 *
 * @details Module size is written as the power of two it is a number of
 * megabytes of, which the widest size the member holds bounds.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the size.
 * @param[out] data  Data the field is to carry.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_memory_controller_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Derive the maximum size of the memory a controller supports.
 *
 * @details Memory the controller supports is what its slots hold when every
 * one of them carries a module of the largest size. A module size too large
 * for any module, and a total too large for the member, leave the size
 * unknown rather than wrapped around.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_memory_controller_derive(dmi_entity_t *entity);

/**
 * @internal
 * @brief Bind the memory modules of a controller to the controller.
 *
 * @details Modules of a controller are linked by the attributes, and learn
 * the controller they belong to here. The sizes they declare are checked
 * against the largest one the controller supports by the lint rules, since
 * linking leaves the members the data decodes into as they are.
 *
 * @param[in,out] entity Structure being linked.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_memory_controller_link(dmi_entity_t *entity);

/**
 * @internal
 * @brief Check the sizes of the modules of a controller.
 *
 * @details Modules of a controller are no larger than the largest module the
 * controller supports, whether installed or enabled.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_controller_lint_module_size(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_CONTROLLER_INTERNAL_H
