//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/baseboard-internal.h>

const dmi_entity_spec_t dmi_baseboard_spec =
{
    .code            = "baseboard",
    .name            = "Baseboard or module information",
    .description     = (const char *[]){
        "The information in this structure defines attributes of a system "
        "baseboard (for example, a motherboard, planar, server blade, or other "
        "standard system module).",
        //
        "Note: If more than one Type 2 structure is provided by an SMBIOS "
        "implementation, each structure shall include the Number of Contained "
        "Object Handles and Contained Object Handles fields to specify which "
        "system elements are contained on which boards. If a single Type 2 "
        "structure is provided and the contained object information is not "
        "present, or if no Type 2 structure is provided, all system elements "
        "identified by the SMBIOS implementation are associated with a single "
        "motherboard.",
        //
        nullptr
    },
    .type            = DMI_TYPE(BASEBOARD),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x08,
        .decoded_length  = sizeof(dmi_baseboard_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_baseboard_t, vendor,        STRING),
        DMI_FIELD(dmi_baseboard_t, product,       STRING),
        DMI_FIELD(dmi_baseboard_t, version,       STRING),
        DMI_FIELD(dmi_baseboard_t, serial_number, STRING),

        // Optional fields are grouped the way dmidecode groups them
        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_baseboard_t, asset_tag, STRING),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_baseboard_t, features, BYTE),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_baseboard_t, location, STRING),
        DMI_FIELD_PRESET(dmi_baseboard_t, chassis_handle, DMI_HANDLE_INVALID),
        DMI_FIELD(dmi_baseboard_t, chassis_handle, WORD),
        DMI_FIELD(dmi_baseboard_t, type,           BYTE),

        DMI_FIELD_GROUP(),
        DMI_FIELD_ARRAY(dmi_baseboard_t, object_handles, object_count,
            .count_type = DMI_FIELD_TYPE_BYTE,
            .fields     = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_baseboard_t, object_handles, WORD),
                {}
            })),
        {}
    }),

    .attributes      = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_baseboard_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, product, STRING, {
            .code    = "product",
            .name    = "Product"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, version, STRING, {
            .code    = "version",
            .name    = "Version"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, asset_tag, STRING, {
            .code    = "asset-tag",
            .name    = "Asset tag"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, features, SET, {
            .code    = "features",
            .name    = "Features",
            .values  = &dmi_baseboard_feature_names
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, location, STRING, {
            .code    = "location",
            .name    = "Location"
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, chassis_handle, HANDLE, {
            .code    = "chassis-handle",
            .name    = "Chassis handle",
            .targets = dmi_types(DMI_TYPE_CHASSIS),
            .unspec  = dmi_value_ptr(DMI_HANDLE_INVALID),
            .link    = dmi_member(dmi_baseboard_t, chassis)
        }),
        DMI_ATTRIBUTE(dmi_baseboard_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_BASEBOARD_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_BASEBOARD_TYPE_UNKNOWN),
            .values  = &dmi_baseboard_type_names
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_baseboard_t, object_handles, object_count, HANDLE, {
            .code    = "contained-objects",
            .name    = "Contained objects",
            .link    = dmi_member(dmi_baseboard_t, objects)
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_baseboard_cleanup
    }
};
