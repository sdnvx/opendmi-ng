//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/svt-internal.h>

// Attributes of both layouts, which decode into the same structure
static const dmi_attribute_t dmi_intel_svt_attrs[] =
{
    DMI_ATTRIBUTE(dmi_intel_svt_t, version, INTEGER, {
        .code = "version",
        .name = "Version"
    }),
    DMI_ATTRIBUTE(dmi_intel_svt_t, parameter, INTEGER, {
        .code  = "parameter",
        .name  = "Parameter",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE_ARRAY(dmi_intel_svt_t, milestones, milestone_count, STRUCT, {
        .code  = "milestones",
        .name  = "Milestones",
        .attrs = DMI_ATTRIBUTES({
            DMI_ATTRIBUTE(dmi_intel_svt_milestone_t, code, INTEGER, {
                .code  = "code",
                .name  = "Code",
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            DMI_ATTRIBUTE(dmi_intel_svt_milestone_t, name, STRING, {
                .code = "name",
                .name = "Name"
            }),
            {}
        })
    }),
    {}
};

const dmi_entity_spec_t dmi_intel_svt_spec =
{
    .type        = DMI_TYPE(intel_svt),
    .code        = "intel-svt",
    .name        = "Intel Silicon View Technology milestones",
    .description = (const char *[]){
        "Lists the milestones of the boot the firmware reports to the debug "
        "and validation tools of Intel Silicon View Technology.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x08,
        .decoded_length = sizeof(dmi_intel_svt_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_svt_t, version,   dmi_byte_t),
        DMI_FIELD(dmi_intel_svt_t, parameter, dmi_word_t),
        DMI_FIELD_ARRAY(dmi_intel_svt_t, milestones, milestone_count,
            .count_length = sizeof(dmi_byte_t),
            .fields       = DMI_FIELDS({
                DMI_FIELD(dmi_intel_svt_milestone_t, code, dmi_byte_t),
                DMI_FIELD_STRING(dmi_intel_svt_milestone_t, name),
                {}
            })),
        {}
    }),

    .attributes = dmi_intel_svt_attrs,

    .handlers = {
        .cleanup = dmi_intel_svt_cleanup
    }
};

const dmi_entity_spec_t dmi_intel_svt_aligned_spec =
{
    .type        = DMI_TYPE(intel_svt),
    .code        = "intel-svt-aligned",
    .name        = "Intel Silicon View Technology milestones",
    .description = (const char *[]){
        "Lists the milestones of the boot the firmware reports to the debug "
        "and validation tools of Intel Silicon View Technology, laid out with "
        "the parameter aligned to two bytes.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0A,
        .decoded_length = sizeof(dmi_intel_svt_t),
        // Byte which aligns the parameter, where the other layout holds the
        // low byte of the parameter
        .signature      = DMI_SIGNATURE({
            .offset = 0x05,
            .bytes  = (const uint8_t[]){ 0x00 },
            .size   = 1
        })
    },

    // Layout of a structure the firmware does not pack: the parameter is
    // aligned to two bytes, and the milestones are followed by a byte which
    // makes the length of the structure even
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_svt_t, version, dmi_byte_t),
        DMI_FIELD_SKIP(sizeof(dmi_byte_t)),
        DMI_FIELD(dmi_intel_svt_t, parameter, dmi_word_t),
        DMI_FIELD_ARRAY(dmi_intel_svt_t, milestones, milestone_count,
            .count_length = sizeof(dmi_byte_t),
            .fields       = DMI_FIELDS({
                DMI_FIELD(dmi_intel_svt_milestone_t, code, dmi_byte_t),
                DMI_FIELD_STRING(dmi_intel_svt_milestone_t, name),
                {}
            })),
        DMI_FIELD_SKIP(sizeof(dmi_byte_t)),
        {}
    }),

    .attributes = dmi_intel_svt_attrs,

    .handlers = {
        .cleanup = dmi_intel_svt_cleanup
    }
};

void dmi_intel_svt_cleanup(dmi_entity_t *entity)
{
    dmi_intel_svt_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(intel_svt));
    if (info == nullptr)
        return;

    dmi_free(info->milestones);
}
