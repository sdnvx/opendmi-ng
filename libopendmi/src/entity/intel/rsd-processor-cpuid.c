//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-processor-cpuid-internal.h>

const dmi_entity_spec_t dmi_intel_rsd_processor_cpuid_spec =
{
    .type            = DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID),
    .code            = "intel-rsd-processor-cpuid",
    .name            = "Intel RSD processor CPUID information",
    .description     = (const char *[]){
        "Declares CPUID leaves of the processors in the system, as returned by "
        "the CPUID instruction. Leaves 00h to 10h and 14h to 80000008h are "
        "stored in structures of different subtypes.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x06,
        .decoded_length  = sizeof(dmi_intel_rsd_processor_cpuid_t)
    },

    .attributes      = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_rsd_processor_cpuid_t, socket_designation, STRING, {
            .code     = "socket-designation",
            .name     = "Socket designation"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_processor_cpuid_t, subtype, ENUM, {
            .code     = "subtype",
            .name     = "Subtype",
            .values   = &dmi_intel_rsd_cpuid_subtype_names
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_intel_rsd_processor_cpuid_t, leaves, leaf_count, STRUCT, {
            .code     = "leaves",
            .name     = "Leaves",
            .attrs    = dmi_intel_rsd_cpuid_leaf_attrs
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_processor_cpuid_t, is_raw, {
            .code     = "data",
            .name     = "CPUID data",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_intel_rsd_processor_cpuid_t, data, BINARY, {}),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .decode  = dmi_intel_rsd_processor_cpuid_decode,
        .cleanup = dmi_intel_rsd_processor_cpuid_cleanup
    }
};
