//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "common-internal.h"
#include "dimm-attrs-internal.h"

const dmi_entity_spec_t dmi_hpe_dimm_attrs_spec =
{
    .type        = DMI_TYPE(hpe_dimm_attrs),
    .code        = "hpe-dimm-attrs",
    .name        = "HP/HPE DIMM attributes record",
    .description = (const char *[]){
        "Tells what the memory device structure (type 17) of a DIMM socket "
        "does not: whether the module is an HPE one, and the voltages it runs "
        "at.",
        //
        nullptr
    },
    .params = {
        .generations    = { .minimum = DMI_HPE_GEN9 },
        .minimum_length = 0x0E,
        .decoded_length = sizeof(dmi_hpe_dimm_attrs_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_dimm_attrs_t, device_handle,      dmi_word_t),
        DMI_FIELD(dmi_hpe_dimm_attrs_t, attributes,         dmi_dword_t),
        DMI_FIELD(dmi_hpe_dimm_attrs_t, minimum_voltage,    dmi_word_t),
        DMI_FIELD(dmi_hpe_dimm_attrs_t, configured_voltage, dmi_word_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD_SKIP(20),
        DMI_FIELD_BITS(dmi_hpe_dimm_attrs_t, is_config_error,   1),
        DMI_FIELD_BITS(dmi_hpe_dimm_attrs_t, is_training_error, 1),
        DMI_FIELD_PAD(dmi_byte_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_dimm_attrs_t, encryption, dmi_byte_t,
                  .absent = dmi_value_ptr((dmi_hpe_encryption_t)UINT8_MAX)),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_dimm_attrs_t, device_handle, HANDLE, {
            .code    = "device-handle",
            .name    = "Memory device handle",
            .targets = dmi_types(DMI_TYPE(memory_device))
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_attrs_t, attributes, INTEGER, {
            .code  = "attributes",
            .name  = "Attributes",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_attrs_t, smart_memory, ENUM, {
            .code   = "smart-memory",
            .name   = "HPE SmartMemory",
            .unspec = dmi_value_ptr(DMI_HPE_FLAG_UNSPEC),
            .values = &dmi_hpe_flag_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_attrs_t, load_reduced, ENUM, {
            .code   = "load-reduced",
            .name   = "Load reduced DIMM",
            .unspec = dmi_value_ptr(DMI_HPE_FLAG_UNSPEC),
            .values = &dmi_hpe_flag_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_attrs_t, standard_memory, ENUM, {
            .code   = "standard-memory",
            .name   = "HPE standard memory",
            .unspec = dmi_value_ptr(DMI_HPE_FLAG_UNSPEC),
            .values = &dmi_hpe_flag_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_attrs_t, minimum_voltage, INTEGER, {
            .code   = "minimum-voltage",
            .name   = "Minimum voltage",
            .unit   = DMI_UNIT_MILLIVOLT,
            .unspec = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_attrs_t, configured_voltage, INTEGER, {
            .code   = "configured-voltage",
            .name   = "Configured voltage",
            .unit   = DMI_UNIT_MILLIVOLT,
            .unspec = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_attrs_t, is_config_error, BOOL, {
            .code = "is-config-error",
            .name = "Mapped out by a configuration error"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_attrs_t, is_training_error, BOOL, {
            .code = "is-training-error",
            .name = "Mapped out by a training error"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_attrs_t, encryption, ENUM, {
            .code   = "encryption",
            .name   = "Encryption status",
            .unspec = dmi_value_ptr((dmi_hpe_encryption_t)UINT8_MAX),
            .values = &dmi_hpe_encryption_names
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_dimm_attrs_derive
    }
};

/**
 * @internal
 * @brief Value of a flag held as a bit telling whether it is defined and a
 * bit telling its value.
 */
static dmi_hpe_flag_t dmi_hpe_dimm_attrs_flag(uint32_t attributes, unsigned bit)
{
    if (not (attributes & (1u << bit)))
        return DMI_HPE_FLAG_UNSPEC;

    return (attributes & (1u << (bit + 1))) ? DMI_HPE_FLAG_YES : DMI_HPE_FLAG_NO;
}

bool dmi_hpe_dimm_attrs_derive(dmi_entity_t *entity)
{
    dmi_hpe_dimm_attrs_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_dimm_attrs));
    if (info == nullptr)
        return false;

    // SmartMemory is told by both bits, of which values 2 and 3 are unknown
    switch (info->attributes & 0x03) {
    case 0:
        info->smart_memory = DMI_HPE_FLAG_NO;
        break;
    case 1:
        info->smart_memory = DMI_HPE_FLAG_YES;
        break;
    default:
        info->smart_memory = DMI_HPE_FLAG_UNSPEC;
        break;
    }

    info->load_reduced    = dmi_hpe_dimm_attrs_flag(info->attributes, 2);
    info->standard_memory = dmi_hpe_dimm_attrs_flag(info->attributes, 4);

    return true;
}
