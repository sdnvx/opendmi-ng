//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/processor-ex-internal.h>

const dmi_entity_spec_t dmi_processor_ex_spec =
{
    .code            = "processor-ex",
    .name            = "Processor additional information",
    .description     = (const char *[]){
        "The information in this structure defines the processor additional "
        "information in case SMBIOS type 4 is not sufficient to describe "
        "processor characteristics.",
        //
        "The SMBIOS type 44 structure has a reference handle field to link "
        "back to the related SMBIOS type 4 structure. There may be multiple "
        "SMBIOS type 44 structures linked to the same SMBIOS type 4 "
        "structure. For example, when cores are not identical in a processor, "
        "SMBIOS type 44 structures describe different core-specific "
        "information.",
        //
        "SMBIOS type 44 defines the standard header for the "
        "processor-specific block (see 7.45.1), while the contents of "
        "processor-specific data are maintained by processor architecture "
        "workgroups or vendors in separate documents.",
        //
        nullptr
    },
    .type            = DMI_TYPE(PROCESSOR_EX),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x06,
        .decoded_length  = sizeof(dmi_processor_ex_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_processor_ex_t, processor_handle, WORD),

        // Processor-specific block declares a length of its own, and the
        // minimum length of the structure does not include it
        DMI_FIELD_SPLIT(dmi_processor_ex_t, .decode = dmi_processor_ex_decode_data),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_processor_ex_t, processor_handle, HANDLE, {
            .code   = "processor-handle",
            .name   = "Processor handle",
            .targets = dmi_types(DMI_TYPE_PROCESSOR),
            .link   = dmi_member(dmi_processor_ex_t, processor)
        }),
        DMI_ATTRIBUTE(dmi_processor_ex_t, arch, ENUM, {
            .code   = "architecture",
            .name   = "Architecture",
            .values = &dmi_processor_arch_names
        }),
        DMI_ATTRIBUTE(dmi_processor_ex_t, data, BINARY, {
            .code   = "data",
            .name   = "Processor-specific data"
        }),
        {}
    })
};
