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

#include <opendmi/entity/intel/asf-internal.h>

const dmi_entity_spec_t dmi_intel_asf_spec =
{
    .type        = DMI_TYPE(intel_asf),
    .code        = "intel-asf",
    .name        = "Intel ASF information",
    .description = (const char *[]){
        "Names the Alert Standard Format (ASF) support of the platform.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x08,
        .decoded_length = sizeof(dmi_intel_asf_t),
        // Version and the number of the first string tell the structure from
        // the ones other vendors give the same type, e.g. Gigabyte. The number
        // of the second string is not checked, since the structures whose
        // strings are the same are written back with a single one
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = (const uint8_t[]){ 0x01, 0x01 },
            .size   = 2
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_asf_t, version, dmi_byte_t),
        DMI_FIELD_STRING(dmi_intel_asf_t, name),
        DMI_FIELD_STRING(dmi_intel_asf_t, identifier),
        DMI_FIELD(dmi_intel_asf_t, parameter, dmi_byte_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD_VECTOR(dmi_intel_asf_t, extra,
            .fields = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_intel_asf_t, extra, dmi_byte_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_asf_t, version, INTEGER, {
            .code = "version",
            .name = "Version"
        }),
        DMI_ATTRIBUTE(dmi_intel_asf_t, name, STRING, {
            .code = "name",
            .name = "Name"
        }),
        DMI_ATTRIBUTE(dmi_intel_asf_t, identifier, STRING, {
            .code = "identifier",
            .name = "Identifier"
        }),
        DMI_ATTRIBUTE(dmi_intel_asf_t, parameter, INTEGER, {
            .code  = "parameter",
            .name  = "Parameter",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        // Longer structures hold more bytes of unknown meaning
        DMI_ATTRIBUTE_VARIANT(dmi_intel_asf_t, has_extra, {
            .code     = "extra",
            .name     = "Extra data",
            .variants = DMI_VARIANTS({
                {
                    .selector  = true,
                    .attribute = DMI_ATTRIBUTE_VECTOR(dmi_intel_asf_t, extra, INTEGER, {
                        .code  = "extra",
                        .name  = "Extra data",
                        .flags = DMI_ATTRIBUTE_FLAG_HEX
                    })
                },
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_intel_asf_derive
    }
};

bool dmi_intel_asf_derive(dmi_entity_t *entity)
{
    dmi_intel_asf_t *info = dmi_entity_info(entity, DMI_TYPE(intel_asf));
    if (info == nullptr)
        return false;

    info->has_extra = (entity->body_length >= 0x10);

    return true;
}
