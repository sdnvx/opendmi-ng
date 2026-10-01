//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_IPMI_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_IPMI_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/ipmi-device.h>

/**
 * @internal
 * @brief Offset of the revision of the IPMI specification, which holds the
 * major version in the high nibble and the minor one in the low nibble.
 */
#define DMI_IPMI_DEVICE_REVISION_OFFSET 0x05

/**
 * @internal
 * @brief Variants of a register-related field.
 *
 * @details Register-related fields are defined only for interfaces in I/O or
 * memory space, and are not shown for SSIF interface.
 */
#define dmi_ipmi_register_variants(__member, ...)                                                  \
    DMI_VARIANTS({                                                                                 \
        DMI_VARIANT(DMI_IPMI_ADDR_TYPE_IO, dmi_ipmi_device_t, __member, INTEGER, __VA_ARGS__),     \
        DMI_VARIANT(DMI_IPMI_ADDR_TYPE_MEMORY, dmi_ipmi_device_t, __member, INTEGER, __VA_ARGS__), \
        {}                                                                                         \
    })

/**
 * @internal
 * @brief Names of the IPMI interface types.
 */
extern const dmi_name_set_t dmi_ipmi_interface_names;

/**
 * @internal
 * @brief Names of the types of the base address.
 */
extern const dmi_name_set_t dmi_ipmi_addr_type_names;

/**
 * @internal
 * @brief Names of the trigger modes of the interrupt.
 */
extern const dmi_name_set_t dmi_ipmi_intr_trigger_names;

/**
 * @internal
 * @brief Names of the polarities of the interrupt.
 */
extern const dmi_name_set_t dmi_ipmi_intr_polarity_names;

/**
 * @internal
 * @brief Decode the revision of the IPMI specification into a version.
 *
 * @details Revision is one nibble of major and one of minor.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the version in.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_ipmi_device_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the version of the IPMI specification into a revision, which
 * undoes `dmi_ipmi_device_decode_version()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the version.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_ipmi_device_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Decode the base address along with its type.
 *
 * @details Base address means what the interface type says it does: an SMBus
 * target address shifted left by one bit for the SSIF interface, and a memory
 * or an I/O address for the rest, whose least significant bit tells the two
 * apart and is carried by the modifier instead, see
 * `dmi_ipmi_device_decode_modifier()`.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Decoded structure to store the address in.
 *
 * @return Always `true`.
 */
bool dmi_ipmi_device_decode_address(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the base address along with its type, which undoes
 * `dmi_ipmi_device_decode_address()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Decoded structure holding the address.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_ipmi_device_encode_address(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Decode the base address modifier and the interrupt information.
 *
 * @details Base address modifier holds the least significant bit of a memory
 * or an I/O address along with the interrupt information. The structure is
 * allowed to end before it, which leaves the bit clear and the rest
 * unspecified.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Decoded structure to store the information in.
 *
 * @return Always `true`.
 */
bool dmi_ipmi_device_decode_modifier(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the base address modifier and the interrupt information,
 * which undoes `dmi_ipmi_device_decode_modifier()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Decoded structure holding the information.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_ipmi_device_encode_modifier(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @internal
 * @brief Check that the revision of the IPMI specification is a binary-coded
 * decimal.
 *
 * @details Revision is decoded into a version, which keeps no trace of the
 * digits it was made of, so the raw data is read instead.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_ipmi_device_lint_revision(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_IPMI_DEVICE_INTERNAL_H
