//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_MEMORY_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-device.h>

/**
 * @internal
 * @brief Names of the types of the memory devices.
 */
extern const dmi_name_set_t dmi_memory_device_type_names;

/**
 * @internal
 * @brief Names of the type details of the memory devices.
 */
extern const dmi_name_set_t dmi_memory_device_type_detail_names;

/**
 * @internal
 * @brief Names of the form factors of the memory devices.
 */
extern const dmi_name_set_t dmi_memory_device_form_factor_names;

/**
 * @internal
 * @brief Names of the technologies of the memory devices.
 */
extern const dmi_name_set_t dmi_memory_device_tech_names;

/**
 * @internal
 * @brief Decode the size of the device.
 *
 * @details Sizes are carried in granules of their own, which the most
 * significant bit of the field tells apart, see `dmi_memory_device_size()`.
 * The conversions the field engine applies take the values the way the data
 * carries them.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the size in bytes in.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_memory_device_decode_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Decode the extended size of the device, which is carried in a field
 * of the width the plain one is too narrow for.
 *
 * @details See `dmi_memory_device_size_ex()`.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the size in bytes in.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_memory_device_decode_size_ex(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the size of the device, which undoes
 * `dmi_memory_device_decode_size()`.
 *
 * @details Sizes are written in megabytes whenever they fit, and in kilobytes
 * otherwise, the way the most significant bit of the field tells them apart.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the size in bytes.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_memory_device_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Encode the extended size of the device, which undoes
 * `dmi_memory_device_decode_size_ex()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the size in bytes.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_memory_device_encode_size_ex(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Check that a size referring to the extended one comes with the
 * extended field.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_device_lint_extended_size(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the total width is not less than the data width.
 *
 * @details Total width covers the data bits and the ones used for error
 * correction.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_device_lint_width(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the configured speed does not exceed the maximum one.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_device_lint_speed(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the configured voltage is within the minimum and the
 * maximum ones.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_device_lint_voltage(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that the volatile and non-volatile sizes do not add up to
 * more than the size of the device.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_device_lint_sizes(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that no reserved bits of the attributes are set.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_device_lint_attributes(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @internal
 * @brief Check that a device disabled because of an error refers to the
 * information on that error.
 *
 * @details The flag is defined by SMBIOS 3.10. Error information is
 * optional, while the handle telling that no error has been detected, or an
 * error of no kind, contradicts the flag.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_memory_device_lint_disabled(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_MEMORY_DEVICE_INTERNAL_H
