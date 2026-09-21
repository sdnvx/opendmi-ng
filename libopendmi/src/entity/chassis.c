//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/chassis-internal.h>

const dmi_entity_spec_t dmi_chassis_spec =
{
    .code            = "chassis",
    .name            = "System enclosure or chassis",
    .type            = DMI_TYPE(CHASSIS),
    .description     = (const char *[]){
        "The information in this structure defines attributes of the system’s "
        "mechanical enclosure(s). For example, if a system included a separate "
        "enclosure for its peripheral devices, two structures would be "
        "returned: one for the main system enclosure and the second for the "
        "peripheral device enclosure. The additions to this structure in "
        "version 2.1 of this specification support the population of the "
        "CIM_Chassis class.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .required_from   = DMI_VERSION(2, 3, 0),
        .required_till   = DMI_VERSION_NONE,
        .unique          = false,
        .minimum_length  = 0x09,
        .decoded_length  = sizeof(dmi_chassis_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_chassis_t, vendor, STRING),

        // One byte holding the type of the chassis and whether it is locked
        DMI_FIELD_BITS(dmi_chassis_t, type,            7),
        DMI_FIELD_BITS(dmi_chassis_t, is_lock_present, 1),
        DMI_FIELD_PAD(BYTE),

        DMI_FIELD(dmi_chassis_t, version,       STRING),
        DMI_FIELD(dmi_chassis_t, serial_number, STRING),
        DMI_FIELD(dmi_chassis_t, asset_tag,     STRING),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 1, 0)),
        DMI_FIELD(dmi_chassis_t, bootup_state,       BYTE),
        DMI_FIELD(dmi_chassis_t, power_supply_state, BYTE),
        DMI_FIELD(dmi_chassis_t, thermal_state,      BYTE),
        DMI_FIELD(dmi_chassis_t, security_status,    BYTE),

        // Fields of SMBIOS 2.3 are grouped the way dmidecode groups them
        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 3, 0)),
        DMI_FIELD(dmi_chassis_t, oem_defined, DWORD),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_chassis_t, height,           BYTE),
        DMI_FIELD(dmi_chassis_t, power_cord_count, BYTE),

        DMI_FIELD_GROUP(),
        DMI_FIELD_ARRAY(dmi_chassis_t, elements, element_count,
            .count_type     = DMI_FIELD_TYPE_BYTE,
            .stride_type    = DMI_FIELD_TYPE_BYTE,
            .stride_minimum = 3 * sizeof(dmi_byte_t),
            .stride_member  = dmi_member(dmi_chassis_t, element_size),
            .fields         = DMI_FIELDS({
                // Element is named by either a structure type or a baseboard
                // type, which the most significant bit tells apart
                DMI_FIELD_SPLIT(dmi_chassis_element_t, BYTE,
                                .decode = dmi_chassis_decode_element_type,
                                .encode = dmi_chassis_encode_element_type),

                DMI_FIELD(dmi_chassis_element_t, minimum_count, BYTE, .unknown_raw = 0xFFu),
                DMI_FIELD(dmi_chassis_element_t, maximum_count, BYTE,
                          .decode = dmi_chassis_decode_maximum_count,
                          .encode = dmi_chassis_encode_maximum_count),
                {}
            })),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 7, 0)),
        DMI_FIELD(dmi_chassis_t, sku_number, STRING),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 9, 0)),
        DMI_FIELD(dmi_chassis_t, rack_type,   BYTE),
        DMI_FIELD(dmi_chassis_t, rack_height, BYTE),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_chassis_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_CHASSIS_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_CHASSIS_TYPE_UNKNOWN),
            .values  = &dmi_chassis_type_names
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, is_lock_present, BOOL, {
            .code    = "is-lock-present",
            .name    = "Lock present"
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, version, STRING, {
            .code    = "version",
            .name    = "Version"
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number"
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, asset_tag, STRING, {
            .code    = "asset-tag",
            .name    = "Asset tag"
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, bootup_state, ENUM, {
            .code    = "bootup-state",
            .name    = "Boot-up state",
            .unspec  = dmi_value_ptr(DMI_STATUS_UNSPEC),
            .unknown = dmi_value_ptr(DMI_STATUS_UNKNOWN),
            .values  = &dmi_status_names,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, power_supply_state, ENUM, {
            .code    = "power-supply-state",
            .name    = "Power supply state",
            .unspec  = dmi_value_ptr(DMI_STATUS_UNSPEC),
            .unknown = dmi_value_ptr(DMI_STATUS_UNKNOWN),
            .values  = &dmi_status_names,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, thermal_state, ENUM, {
            .code    = "thermal-state",
            .name    = "Thermal state",
            .unspec  = dmi_value_ptr(DMI_STATUS_UNSPEC),
            .unknown = dmi_value_ptr(DMI_STATUS_UNKNOWN),
            .values  = &dmi_status_names,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, security_status, ENUM, {
            .code    = "security-status",
            .name    = "Security status",
            .unspec  = dmi_value_ptr(DMI_CHASSIS_SECURITY_STATUS_UNSPEC),
            .unknown = dmi_value_ptr(DMI_CHASSIS_SECURITY_STATUS_UNKNOWN),
            .values  = &dmi_chassis_security_status_names,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, oem_defined, INTEGER, {
            .code    = "oem-defined",
            .name    = "OEM-defined",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, height, INTEGER, {
            .code    = "height",
            .name    = "Height",
            .unit    = DMI_UNIT_RACK,
            .unspec  = dmi_value_ptr((unsigned short)0),
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, power_cord_count, INTEGER, {
            .code    = "power-cord-count",
            .name    = "Power cord count",
            .unspec  = dmi_value_ptr((unsigned short)0),
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, element_count, INTEGER, {
            .code    = "element-count",
            .name    = "Contained element count",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, element_size, SIZE, {
            .code    = "element-size",
            .name    = "Contained element size",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_chassis_t, elements, element_count, STRUCT, {
            .code    = "elements",
            .name    = "Contained elements",
            .level   = DMI_VERSION(2, 3, 0),
            .attrs   = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_chassis_element_t, type, INTEGER, {
                    .code    = "type",
                    .name    = "Structure type",
                    .unspec  = dmi_value_ptr((dmi_type_t)DMI_TYPE_INVALID),
                    .flags   = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_chassis_element_t, board_type, ENUM, {
                    .code    = "board-type",
                    .name    = "Board type",
                    .unspec  = dmi_value_ptr(DMI_BASEBOARD_TYPE_UNSPEC),
                    .unknown = dmi_value_ptr(DMI_BASEBOARD_TYPE_UNKNOWN),
                    .values  = &dmi_baseboard_type_names
                }),
                DMI_ATTRIBUTE(dmi_chassis_element_t, minimum_count, INTEGER, {
                    .code    = "minimum-count",
                    .name    = "Minimum count",
                    .unknown = dmi_value_ptr((size_t)SIZE_MAX)
                }),
                DMI_ATTRIBUTE(dmi_chassis_element_t, maximum_count, INTEGER, {
                    .code    = "maximum-count",
                    .name    = "Maximum count",
                    .unknown = dmi_value_ptr((size_t)SIZE_MAX)
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, sku_number, STRING, {
            .code    = "sku-number",
            .name    = "SKU number",
            .level   = DMI_VERSION(2, 7, 0)
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, rack_type, ENUM, {
            .code    = "rack-type",
            .name    = "Rack type",
            .unspec  = dmi_value_ptr(DMI_RACK_TYPE_UNSPEC),
            .values  = &dmi_rack_type_names,
            .level   = DMI_VERSION(3, 9, 0)
        }),
        DMI_ATTRIBUTE(dmi_chassis_t, rack_height, INTEGER, {
            .code    = "rack-height",
            .name    = "Rack height",
            .level   = DMI_VERSION(3, 9, 0)
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_chassis_cleanup
    }
};
