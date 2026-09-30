//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/mei-internal.h>

const dmi_entity_spec_t dmi_intel_mei_spec =
{
    .type        = DMI_TYPE(intel_mei),
    .code        = "intel-mei",
    .name        = "Intel Management Engine interface information",
    .description = (const char *[]){
        "Keeps the firmware status registers of the interfaces of the Intel "
        "Management Engine, as the firmware has read them at boot, which tell "
        "the state, the operation mode and the SKU of the Management Engine "
        "firmware.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x06,
        .decoded_length = sizeof(dmi_intel_mei_t),
        // Version 1 and the name of the first interface tell the structure
        // from the ones of the vendors at the same types, e.g. of Dell
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = (const uint8_t[]){ 0x01 },
            .size   = 1,
            .string = 1,
            .text   = "MEI1"
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_mei_t, version, dmi_byte_t),
        DMI_FIELD_ARRAY(dmi_intel_mei_t, devices, device_count,
            .count_length = sizeof(dmi_byte_t),
            .fields       = DMI_FIELDS({
                DMI_FIELD_STRING(dmi_intel_mei_device_t, name),
                DMI_FIELD_VECTOR(dmi_intel_mei_device_t, hfsts,
                    .fields = DMI_FIELDS({
                        DMI_FIELD_ELEMENT(dmi_intel_mei_device_t, hfsts, dmi_dword_t),
                        {}
                    })),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_mei_t, version, INTEGER, {
            .code = "version",
            .name = "Version"
        }),
        DMI_ATTRIBUTE(dmi_intel_mei_t, state, ENUM, {
            .code    = "state",
            .name    = "Firmware state",
            .unspec  = dmi_value_ptr(DMI_INTEL_ME_STATE_UNSPEC),
            .values  = &dmi_intel_me_state_names
        }),
        DMI_ATTRIBUTE(dmi_intel_mei_t, mode, ENUM, {
            .code    = "mode",
            .name    = "Operation mode",
            .unspec  = dmi_value_ptr(DMI_INTEL_ME_MODE_UNSPEC),
            .values  = &dmi_intel_me_mode_names
        }),
        DMI_ATTRIBUTE(dmi_intel_mei_t, error_code, ENUM, {
            .code    = "error-code",
            .name    = "Error code",
            .unspec  = dmi_value_ptr(DMI_INTEL_ME_ERROR_UNSPEC),
            .values  = &dmi_intel_me_error_names
        }),
        DMI_ATTRIBUTE(dmi_intel_mei_t, is_init_complete, BOOL, {
            .code    = "is-init-complete",
            .name    = "Initialization complete"
        }),
        DMI_ATTRIBUTE(dmi_intel_mei_t, is_manufacturing, BOOL, {
            .code    = "is-manufacturing",
            .name    = "Manufacturing mode"
        }),
        DMI_ATTRIBUTE(dmi_intel_mei_t, sku, ENUM, {
            .code    = "sku",
            .name    = "Firmware SKU",
            .unspec  = dmi_value_ptr(DMI_INTEL_ME_SKU_UNSPEC),
            .values  = &dmi_intel_me_sku_names
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_intel_mei_t, devices, device_count, STRUCT, {
            .code  = "devices",
            .name  = "Interfaces",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_intel_mei_device_t, name, STRING, {
                    .code = "name",
                    .name = "Name"
                }),
                DMI_ATTRIBUTE(dmi_intel_mei_device_t, is_reported, BOOL, {
                    .code = "is-reported",
                    .name = "Reported"
                }),
                // Interfaces whose registers are all zeroes tell nothing
                DMI_ATTRIBUTE_VARIANT(dmi_intel_mei_device_t, is_reported, {
                    .code     = "is-present",
                    .name     = "Present",
                    .variants = DMI_VARIANTS({
                        DMI_VARIANT(true, dmi_intel_mei_device_t, is_present, BOOL, {}),
                        {}
                    })
                }),
                DMI_ATTRIBUTE_VARIANT(dmi_intel_mei_device_t, is_reported, {
                    .code     = "hfsts",
                    .name     = "Firmware status",
                    .variants = DMI_VARIANTS({
                        {
                            .selector  = true,
                            .attribute = DMI_ATTRIBUTE_VECTOR(dmi_intel_mei_device_t, hfsts, INTEGER, {
                                .code  = "hfsts",
                                .name  = "Firmware status",
                                .flags = DMI_ATTRIBUTE_FLAG_HEX
                            })
                        },
                        {}
                    })
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_intel_mei_derive,
        .cleanup = dmi_intel_mei_cleanup
    }
};
