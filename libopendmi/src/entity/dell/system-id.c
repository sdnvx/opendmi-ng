//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/system-id.h>

const dmi_entity_spec_t dmi_dell_system_id_spec =
{
    .type        = DMI_TYPE(DELL_SYSTEM_ID),
    .code        = "dell-system-id",
    .name        = "Dell system ID record",
    .description = (const char *[]){
        "Holds the system ID of the platform in hexadecimal digits, along "
        "with an identifier whose meaning is not established.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x06,
        .decoded_length = sizeof(dmi_dell_system_id_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_dell_system_id_t, identifier),
        DMI_FIELD_STRING(dmi_dell_system_id_t, system_id),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_system_id_t, identifier, STRING, {
            .code  = "identifier",
            .name  = "Identifier",
            .flags = DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        DMI_ATTRIBUTE(dmi_dell_system_id_t, system_id, STRING, {
            .code = "system-id",
            .name = "System ID"
        }),
        {}
    })
};
