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

//
// Register-related fields are defined only for interfaces in I/O or memory
// space, and are not shown for SSIF interface
//
#define dmi_ipmi_register_variants(__member, ...)                                                  \
    DMI_VARIANTS({                                                                                 \
        DMI_VARIANT(DMI_IPMI_ADDR_TYPE_IO, dmi_ipmi_device_t, __member, INTEGER, __VA_ARGS__),     \
        DMI_VARIANT(DMI_IPMI_ADDR_TYPE_MEMORY, dmi_ipmi_device_t, __member, INTEGER, __VA_ARGS__), \
        {}                                                                                         \
    })

// Value names, see ipmi-device-names.c
extern const dmi_name_set_t dmi_ipmi_interface_names;
extern const dmi_name_set_t dmi_ipmi_addr_type_names;
extern const dmi_name_set_t dmi_ipmi_intr_trigger_names;
extern const dmi_name_set_t dmi_ipmi_intr_polarity_names;

// Operation handlers, see ipmi-device-handlers.c
bool dmi_ipmi_device_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_ipmi_device_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);
bool dmi_ipmi_device_decode_address(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_ipmi_device_encode_address(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

// Checks the lint rules of the specification perform, see ipmi-device-rules.c
void dmi_ipmi_device_lint_revision(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_IPMI_DEVICE_INTERNAL_H
