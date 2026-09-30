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
        DMI_FIELD_VECTOR(dmi_intel_amt_t, oem_capabilities,
            .fields = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_intel_amt_t, oem_capabilities, dmi_byte_t),
                {}
            })),
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
            .name = "IDE redirection enabled"
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
            .name  = "Extended data marker",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_VECTOR(dmi_intel_amt_t, oem_capabilities, INTEGER, {
            .code  = "oem-capabilities",
            .name  = "OEM capabilities",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};
