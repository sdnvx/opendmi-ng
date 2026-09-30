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

#include <opendmi/entity/intel/platform.h>

const dmi_entity_spec_t dmi_intel_platform_spec =
{
    .type        = DMI_TYPE(intel_platform),
    .code        = "intel-platform",
    .name        = "Intel platform information",
    .description = (const char *[]){
        "Gives the versions of the firmware components and some settings of "
        "the platform.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x1A,
        .decoded_length = sizeof(dmi_intel_platform_t),
        // Length tells the structure from the ones Intel server boards give
        // the same type, e.g. S2600WTT
        .signature      = DMI_SIGNATURE({
            .length = 0x1A
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_intel_platform_t, gop_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, microcode_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, mrc_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, sec_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, ulpmc_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, pmc_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, punit_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, soc_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, board_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, fab_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, cpu_flavor),
        DMI_FIELD_STRING(dmi_intel_platform_t, bios_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, pmic_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, touch_version),
        DMI_FIELD_STRING(dmi_intel_platform_t, secure_boot),
        DMI_FIELD_STRING(dmi_intel_platform_t, boot_mode),
        DMI_FIELD_STRING(dmi_intel_platform_t, speedstep_mode),
        DMI_FIELD_STRING(dmi_intel_platform_t, turbo_mode),
        DMI_FIELD_STRING(dmi_intel_platform_t, max_cstate),
        DMI_FIELD_STRING(dmi_intel_platform_t, gfx_turbo),
        DMI_FIELD_STRING(dmi_intel_platform_t, idle_reserve),
        DMI_FIELD_STRING(dmi_intel_platform_t, rc6),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_platform_t, gop_version, STRING, {
            .code = "gop-version",
            .name = "GOP version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, microcode_version, STRING, {
            .code = "microcode-version",
            .name = "Microcode version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, mrc_version, STRING, {
            .code = "mrc-version",
            .name = "MRC version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, sec_version, STRING, {
            .code = "sec-version",
            .name = "SEC version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, ulpmc_version, STRING, {
            .code = "ulpmc-version",
            .name = "ULPMC version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, pmc_version, STRING, {
            .code = "pmc-version",
            .name = "PMC version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, punit_version, STRING, {
            .code = "punit-version",
            .name = "P-unit version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, soc_version, STRING, {
            .code = "soc-version",
            .name = "SoC version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, board_version, STRING, {
            .code = "board-version",
            .name = "Board version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, fab_version, STRING, {
            .code = "fab-version",
            .name = "Fab version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, cpu_flavor, STRING, {
            .code = "cpu-flavor",
            .name = "CPU flavor"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, bios_version, STRING, {
            .code = "bios-version",
            .name = "BIOS version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, pmic_version, STRING, {
            .code = "pmic-version",
            .name = "PMIC version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, touch_version, STRING, {
            .code = "touch-version",
            .name = "Touch version"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, secure_boot, STRING, {
            .code = "secure-boot",
            .name = "Secure Boot"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, boot_mode, STRING, {
            .code = "boot-mode",
            .name = "Boot mode"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, speedstep_mode, STRING, {
            .code = "speedstep-mode",
            .name = "SpeedStep mode"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, turbo_mode, STRING, {
            .code = "turbo-mode",
            .name = "Turbo mode"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, max_cstate, STRING, {
            .code = "max-cstate",
            .name = "Maximum C-state"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, gfx_turbo, STRING, {
            .code = "gfx-turbo",
            .name = "Graphics turbo"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, idle_reserve, STRING, {
            .code = "idle-reserve",
            .name = "Idle reserve"
        }),
        DMI_ATTRIBUTE(dmi_intel_platform_t, rc6, STRING, {
            .code = "rc6",
            .name = "RC6"
        }),
        {}
    })
};
