//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/system-id-internal.h>

const dmi_entity_spec_t dmi_hpe_system_id_spec =
{
    .type        = DMI_TYPE(hpe_system_id),
    .code        = "hpe-system-id",
    .name        = "HP/HPE server system ID",
    .description = (const char *[]){
        "Gives the unique ID of the system, which replaces the EISA ID of the "
        "older systems, and the platform ID iLO tells the platform by.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x05,
        .decoded_length = sizeof(dmi_hpe_system_id_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_hpe_system_id_t, system_id),

        DMI_FIELD_GROUP(),
        DMI_FIELD_VECTOR(dmi_hpe_system_id_t, platform_id,
            .fields = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_hpe_system_id_t, platform_id, dmi_byte_t),
                {}
            })),

        DMI_FIELD_GROUP(),
        DMI_FIELD_UUID(dmi_hpe_system_id_t, guid),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_system_id_t, system_id, STRING, {
            .code = "system-id",
            .name = "Server system ID"
        }),
        // Platform ID is not shown when the structure is too short to hold
        // it
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_system_id_t, has_platform_id, {
            .code     = "platform-id",
            .name     = "Platform ID",
            .variants = DMI_VARIANTS({
                {
                    .selector  = true,
                    .attribute = DMI_ATTRIBUTE_VECTOR(dmi_hpe_system_id_t, platform_id, INTEGER, {
                        .flags = DMI_ATTRIBUTE_FLAG_HEX
                    })
                },
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_hpe_system_id_t, guid, UUID, {
            .code   = "guid",
            .name   = "GUID",
            .unspec = &(const dmi_uuid_t){},
            .flags  = DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_system_id_derive
    }
};

bool dmi_hpe_system_id_derive(dmi_entity_t *entity)
{
    dmi_hpe_system_id_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_system_id));
    if (info == nullptr)
        return false;

    info->has_platform_id = (entity->body_length >= 0x07);

    return true;
}
