//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/fvi-internal.h>

const dmi_entity_spec_t dmi_intel_fvi_spec =
{
    .type        = DMI_TYPE(INTEL_FVI),
    .code        = "intel-fvi",
    .name        = "Intel firmware version information",
    .description = (const char *[]){
        "Lists the versions of the firmware components of a platform, as the "
        "Intel reference code knows them: the reference code modules, the "
        "microcode, the Management Engine firmware, the option ROMs and "
        "others. Each module of the reference code adds a structure of its "
        "own, whose first item is the version of the module itself.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x05,
        .decoded_length = sizeof(dmi_intel_fvi_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_ARRAY(dmi_intel_fvi_t, items, item_count,
            .count_length = sizeof(dmi_byte_t),
            .fields       = DMI_FIELDS({
                DMI_FIELD_STRING(dmi_intel_fvi_item_t, component),
                DMI_FIELD_STRING(dmi_intel_fvi_item_t, version_string),
                DMI_FIELD(dmi_intel_fvi_item_t, major,    dmi_byte_t),
                DMI_FIELD(dmi_intel_fvi_item_t, minor,    dmi_byte_t),
                DMI_FIELD(dmi_intel_fvi_item_t, revision, dmi_byte_t),
                DMI_FIELD(dmi_intel_fvi_item_t, build,    dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_intel_fvi_t, items, item_count, STRUCT, {
            .code  = "items",
            .name  = "Items",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_intel_fvi_item_t, component, STRING, {
                    .code = "component",
                    .name = "Component"
                }),
                DMI_ATTRIBUTE(dmi_intel_fvi_item_t, version_string, STRING, {
                    .code = "version-string",
                    .name = "Version string"
                }),
                DMI_ATTRIBUTE(dmi_intel_fvi_item_t, major, INTEGER, {
                    .code   = "major",
                    .name   = "Major version",
                    .unspec = dmi_value_ptr((uint8_t)UINT8_MAX)
                }),
                DMI_ATTRIBUTE(dmi_intel_fvi_item_t, minor, INTEGER, {
                    .code   = "minor",
                    .name   = "Minor version",
                    .unspec = dmi_value_ptr((uint8_t)UINT8_MAX)
                }),
                DMI_ATTRIBUTE(dmi_intel_fvi_item_t, revision, INTEGER, {
                    .code   = "revision",
                    .name   = "Revision",
                    .unspec = dmi_value_ptr((uint8_t)UINT8_MAX)
                }),
                DMI_ATTRIBUTE(dmi_intel_fvi_item_t, build, INTEGER, {
                    .code   = "build",
                    .name   = "Build number",
                    .unspec = dmi_value_ptr((uint16_t)UINT16_MAX)
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_intel_fvi_cleanup
    }
};
