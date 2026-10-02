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

#include <opendmi/entity/intel/amt.h>

/**
 * @internal
 * @brief Names of the bits of the OEM capabilities, as the Intel AMT
 * implementation guide names them.
 */
static const dmi_name_set_t dmi_intel_amt_oem_caps_1_names =
{
    .code  = "intel-amt-oem-caps-1",
    .names = DMI_NAMES({
        { .id = 0, .code = "storage-redirection", .name = "Storage redirection" },
        { .id = 1, .code = "sol",                 .name = "Serial over LAN" },
        { .id = 2, .code = "bios-reflash",        .name = "BIOS reflash" },
        { .id = 3, .code = "bios-setup",          .name = "BIOS setup" },
        { .id = 4, .code = "bios-pause",          .name = "BIOS pause" },
        { .id = 5, .code = "floppy-boot-block",   .name = "Blocking floppy boot" },
        { .id = 6, .code = "cd-boot-block",       .name = "Blocking CD boot" },
        {}
    })
};

static const dmi_name_set_t dmi_intel_amt_oem_caps_3_names =
{
    .code  = "intel-amt-oem-caps-3",
    .names = DMI_NAMES({
        { .id = 6, .code = "secure-erase", .name = "Remote Secure Erase" },
        { .id = 7, .code = "secure-boot",  .name = "BIOS Secure Boot" },
        {}
    })
};

static const dmi_name_set_t dmi_intel_amt_oem_caps_4_names =
{
    .code  = "intel-amt-oem-caps-4",
    .names = DMI_NAMES({
        { .id = 0, .code = "thunderbolt-dock", .name = "Thunderbolt dock" },
        { .id = 1, .code = "https-boot",       .name = "HTTPS boot" },
        { .id = 2, .code = "pba-boot",         .name = "Local PBA boot" },
        { .id = 3, .code = "winre-boot",       .name = "WinRE boot" },
        { .id = 4, .code = "nonsecure-boot",   .name = "Non-secure boot" },
        { .id = 5, .code = "wifi-coexistence", .name = "Wi-Fi coexistence" },
        {}
    })
};

static const dmi_name_set_t dmi_intel_amt_terminal_names =
{
    .code  = "intel-amt-terminal",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_INTEL_AMT_TERMINAL_UNSPEC),
        {
            .id   = DMI_INTEL_AMT_TERMINAL_VT52,
            .code = "vt52",
            .name = "VT52"
        },
        {
            .id   = DMI_INTEL_AMT_TERMINAL_VT100_PLUS,
            .code = "vt100-plus",
            .name = "VT100+"
        },
        {
            .id   = DMI_INTEL_AMT_TERMINAL_VT_UTF8,
            .code = "vt-utf8",
            .name = "VT-UTF8"
        },
        {
            .id   = DMI_INTEL_AMT_TERMINAL_PC_ANSI,
            .code = "pc-ansi",
            .name = "PC-ANSI"
        },
        {}
    })
};

const dmi_entity_spec_t dmi_intel_amt_spec =
{
    .type        = DMI_TYPE(intel_amt),
    .code        = "intel-amt",
    .name        = "Intel Active Management Technology information",
    .description = (const char *[]){
        "Tells whether the platform supports Intel Active Management "
        "Technology (AMT), and which of its features are enabled.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x14,
        .decoded_length = sizeof(dmi_intel_amt_t),
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = "$AMT",
            .size   = 4
        })
    },

    .fields = DMI_FIELDS({
        // Signature is kept, so that the structure is written back with it
        DMI_FIELD_BINARY(dmi_intel_amt_t, signature, 4),
        DMI_FIELD(dmi_intel_amt_t, is_supported,       dmi_byte_t),
        DMI_FIELD(dmi_intel_amt_t, is_enabled,         dmi_byte_t),
        DMI_FIELD(dmi_intel_amt_t, is_ider_enabled,    dmi_byte_t),
        DMI_FIELD(dmi_intel_amt_t, is_sol_enabled,     dmi_byte_t),
        DMI_FIELD(dmi_intel_amt_t, is_network_enabled, dmi_byte_t),
        DMI_FIELD(dmi_intel_amt_t, extended_data,      dmi_byte_t),
        DMI_FIELD(dmi_intel_amt_t, oem_capabilities_1, dmi_byte_t),
        DMI_FIELD_BITS(dmi_intel_amt_t, terminal, 4),
        DMI_FIELD_PAD(dmi_byte_t),
        DMI_FIELD(dmi_intel_amt_t, oem_capabilities_3, dmi_byte_t),
        DMI_FIELD(dmi_intel_amt_t, oem_capabilities_4, dmi_byte_t),
        DMI_FIELD(dmi_intel_amt_t, is_kvm_enabled, dmi_byte_t),
        DMI_FIELD_SKIP(1),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_amt_t, is_supported, BOOL, {
            .code = "is-supported",
            .name = "Supported"
        }),
        DMI_ATTRIBUTE(dmi_intel_amt_t, is_enabled, BOOL, {
            .code = "is-enabled",
            .name = "Enabled"
        }),
        DMI_ATTRIBUTE(dmi_intel_amt_t, is_ider_enabled, BOOL, {
            .code = "is-ider-enabled",
            .name = "Storage redirection enabled"
        }),
        DMI_ATTRIBUTE(dmi_intel_amt_t, is_sol_enabled, BOOL, {
            .code = "is-sol-enabled",
            .name = "Serial over LAN enabled"
        }),
        DMI_ATTRIBUTE(dmi_intel_amt_t, is_network_enabled, BOOL, {
            .code = "is-network-enabled",
            .name = "Network enabled"
        }),
        DMI_ATTRIBUTE(dmi_intel_amt_t, is_kvm_enabled, BOOL, {
            .code = "is-kvm-enabled",
            .name = "KVM redirection enabled"
        }),
        DMI_ATTRIBUTE(dmi_intel_amt_t, extended_data, INTEGER, {
            .code  = "extended-data",
            .name  = "Extended data",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_amt_t, oem_capabilities_1, SET, {
            .code   = "oem-capabilities-1",
            .name   = "OEM capabilities",
            .values = &dmi_intel_amt_oem_caps_1_names
        }),
        DMI_ATTRIBUTE(dmi_intel_amt_t, terminal, ENUM, {
            .code   = "terminal",
            .name   = "Serial over LAN terminal emulation",
            .unspec = dmi_value_ptr(DMI_INTEL_AMT_TERMINAL_UNSPEC),
            .values = &dmi_intel_amt_terminal_names
        }),
        DMI_ATTRIBUTE(dmi_intel_amt_t, oem_capabilities_3, SET, {
            .code   = "oem-capabilities-3",
            .name   = "Security capabilities",
            .values = &dmi_intel_amt_oem_caps_3_names
        }),
        DMI_ATTRIBUTE(dmi_intel_amt_t, oem_capabilities_4, SET, {
            .code   = "oem-capabilities-4",
            .name   = "Remote boot capabilities",
            .values = &dmi_intel_amt_oem_caps_4_names
        }),
        {}
    })
};

DMI_NAME_FUNCTION(dmi_intel_amt_terminal)
