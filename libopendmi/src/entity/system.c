//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/stream.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system-internal.h>

const dmi_entity_spec_t dmi_system_spec =
{
    .code            = "system",
    .name            = "System information",
    .description     = (const char *[]){
        "The information in this structure defines attributes of the overall "
        "system and is intended to be associated with the Component ID group "
        "of the system\'s MIF. An SMBIOS implementation is associated with a "
        "single system instance and contains one and only one System "
        "Information (Type 1) structure.",
        //
        nullptr
    },
    .type            = DMI_TYPE(SYSTEM),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .required_from   = DMI_VERSION(2, 3, 0),
        .required_till   = DMI_VERSION_NONE,
        .unique          = true,
        .minimum_length  = 0x8,
        .decoded_length  = sizeof(dmi_system_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_system_t, vendor,        STRING),
        DMI_FIELD(dmi_system_t, product,       STRING),
        DMI_FIELD(dmi_system_t, version,       STRING),
        DMI_FIELD(dmi_system_t, serial_number, STRING),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 1, 0)),
        DMI_FIELD(dmi_system_t, uuid,        UUID),
        DMI_FIELD(dmi_system_t, wakeup_type, BYTE),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 4, 0)),
        DMI_FIELD(dmi_system_t, sku_number, STRING),
        DMI_FIELD(dmi_system_t, family,     STRING),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_system_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Manufacturer"
        }),
        DMI_ATTRIBUTE(dmi_system_t, product, STRING, {
            .code    = "product",
            .name    = "Product"
        }),
        DMI_ATTRIBUTE(dmi_system_t, version, STRING, {
            .code    = "version",
            .name    = "Version"
        }),
        DMI_ATTRIBUTE(dmi_system_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number"
        }),
        DMI_ATTRIBUTE(dmi_system_t, uuid, UUID, {
            .code    = "uuid",
            .name    = "UUID",
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_system_t, wakeup_type, ENUM, {
            .code    = "wakeup-type",
            .name    = "Wakeup type",
            .values  = &dmi_system_wakeup_type_names,
            .unknown = dmi_value_ptr(DMI_SYSTEM_WAKEUP_TYPE_UNKNOWN),
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_system_t, sku_number, STRING, {
            .code    = "sku-number",
            .name    = "SKU number",
            .level   = DMI_VERSION(2, 4, 0)
        }),
        DMI_ATTRIBUTE(dmi_system_t, family, STRING, {
            .code    = "family",
            .name    = "Family",
            .level   = DMI_VERSION(2, 4, 0)
        }),
        {}
    })
};
