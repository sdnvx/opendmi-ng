//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/vpro.h>

// Attributes of the versions of the firmware components
static const dmi_attribute_t dmi_intel_vpro_version_attrs[] =
{
    DMI_ATTRIBUTE(dmi_intel_vpro_version_t, major, INTEGER, {
        .code = "major",
        .name = "Major version"
    }),
    DMI_ATTRIBUTE(dmi_intel_vpro_version_t, minor, INTEGER, {
        .code = "minor",
        .name = "Minor version"
    }),
    DMI_ATTRIBUTE(dmi_intel_vpro_version_t, hotfix, INTEGER, {
        .code = "hotfix",
        .name = "Hotfix"
    }),
    DMI_ATTRIBUTE(dmi_intel_vpro_version_t, build, INTEGER, {
        .code = "build",
        .name = "Build number"
    }),
    {}
};

const dmi_entity_spec_t dmi_intel_vpro_spec =
{
    .type        = DMI_TYPE(INTEL_VPRO),
    .code        = "intel-vpro",
    .name        = "Intel vPro information",
    .description = (const char *[]){
        "Describes the parts of the platform Intel vPro technology relies "
        "on: the versions of the Management Engine firmware and of its BIOS "
        "extension, and the PCI functions of the chipset and of the network "
        "controller the Management Engine uses.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x40,
        .decoded_length = sizeof(dmi_intel_vpro_t),
        .signature      = DMI_SIGNATURE({
            .offset = 0x38,
            .bytes  = "vPro",
            .size   = 4
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_vpro_t, flags_1, dmi_dword_t),

        // Version of the BIOS extension is laid out from the major part down
        DMI_FIELD(dmi_intel_vpro_t, mebx_version.major,  dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, mebx_version.minor,  dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, mebx_version.hotfix, dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, mebx_version.build,  dmi_word_t),

        DMI_FIELD(dmi_intel_vpro_t, lpc_devfn,     dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, lpc_device_id, dmi_word_t),
        DMI_FIELD_SKIP(4),
        DMI_FIELD(dmi_intel_vpro_t, flags_2, dmi_dword_t),

        // Version of the firmware is laid out as the firmware reports it
        DMI_FIELD(dmi_intel_vpro_t, me_version.minor,  dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, me_version.major,  dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, me_version.build,  dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, me_version.hotfix, dmi_word_t),
        DMI_FIELD_SKIP(4),

        DMI_FIELD(dmi_intel_vpro_t, gbe_devfn,     dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, gbe_device_id, dmi_word_t),
        DMI_FIELD_SKIP(8),
        DMI_FIELD(dmi_intel_vpro_t, flags_3, dmi_dword_t),

        // Signature is kept, so that the structure is written back with it
        DMI_FIELD_BINARY(dmi_intel_vpro_t, signature, 4),
        DMI_FIELD_SKIP(4),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_vpro_t, flags_1, INTEGER, {
            .code  = "flags-1",
            .name  = "Flags 1",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, mebx_version, STRUCT, {
            .code  = "mebx-version",
            .name  = "MEBx version",
            .attrs = dmi_intel_vpro_version_attrs
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, lpc_devfn, INTEGER, {
            .code  = "lpc-devfn",
            .name  = "LPC bridge device and function",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, lpc_device_id, INTEGER, {
            .code  = "lpc-device-id",
            .name  = "LPC bridge device ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, flags_2, INTEGER, {
            .code  = "flags-2",
            .name  = "Flags 2",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, me_version, STRUCT, {
            .code  = "me-version",
            .name  = "ME firmware version",
            .attrs = dmi_intel_vpro_version_attrs
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, gbe_devfn, INTEGER, {
            .code  = "gbe-devfn",
            .name  = "Gigabit Ethernet device and function",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, gbe_device_id, INTEGER, {
            .code   = "gbe-device-id",
            .name   = "Gigabit Ethernet device ID",
            .unspec = dmi_value_ptr((uint16_t)UINT16_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, flags_3, INTEGER, {
            .code  = "flags-3",
            .name  = "Flags 3",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};
