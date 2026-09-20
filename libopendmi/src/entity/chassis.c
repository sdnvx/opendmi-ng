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

#include <opendmi/entity/chassis.h>

static bool dmi_chassis_decode(dmi_entity_t *entity);
static void dmi_chassis_cleanup(dmi_entity_t *entity);

static const dmi_name_set_t dmi_chassis_type_names =
{
    .code  = "chassis-types",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_CHASSIS_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_CHASSIS_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_CHASSIS_TYPE_UNKNOWN),
        {
            .id   = DMI_CHASSIS_TYPE_DESKTOP,
            .code = "desktop",
            .name = "Desktop"
        },
        {
            .id   = DMI_CHASSIS_TYPE_LOW_PROFILE_DESKTOP,
            .code = "low-profile-desktop",
            .name = "Low-profile desktop"
        },
        {
            .id   = DMI_CHASSIS_TYPE_PIZZA_BOX,
            .code = "pizza-box",
            .name = "Pizza box"
        },
        {
            .id   = DMI_CHASSIS_TYPE_MINI_TOWER,
            .code = "mini-tower",
            .name = "Mini tower"
        },
        {
            .id   = DMI_CHASSIS_TYPE_TOWER,
            .code = "tower",
            .name = "Tower"
        },
        {
            .id   = DMI_CHASSIS_TYPE_PORTABLE,
            .code = "portable",
            .name = "Portable"
        },
        {
            .id   = DMI_CHASSIS_TYPE_LAPTOP,
            .code = "laptop",
            .name = "Laptop"
        },
        {
            .id   = DMI_CHASSIS_TYPE_NOTEBOOK,
            .code = "notebook",
            .name = "Notebook"
        },
        {
            .id   = DMI_CHASSIS_TYPE_HAND_HELD,
            .code = "hand-held",
            .name = "Hand held"
        },
        {
            .id   = DMI_CHASSIS_TYPE_DOCKING_STATION,
            .code = "docking-station",
            .name = "Docking station"
        },
        {
            .id   = DMI_CHASSIS_TYPE_ALL_IN_ONE,
            .code = "all-in-one",
            .name = "All-in-one"
        },
        {
            .id   = DMI_CHASSIS_TYPE_SUB_NOTEBOOK,
            .code = "sub-notebook",
            .name = "Sub-notebook"
        },
        {
            .id   = DMI_CHASSIS_TYPE_SPACE_SAVING,
            .code = "space-saving",
            .name = "Space-saving"
        },
        {
            .id   = DMI_CHASSIS_TYPE_LUNCH_BOX,
            .code = "lunch-box",
            .name = "Lunch box"
        },
        {
            .id   = DMI_CHASSIS_TYPE_MAIN_SERVER,
            .code = "main-server",
            .name = "Main server chassis"
        },
        {
            .id   = DMI_CHASSIS_TYPE_EXPANSION,
            .code = "expansion",
            .name = "Expansion chassis"
        },
        {
            .id   = DMI_CHASSIS_TYPE_SUB_CHASSIS,
            .code = "sub-chassis",
            .name = "Sub-chassis"
        },
        {
            .id   = DMI_CHASSIS_TYPE_BUS_EXPANSION,
            .code = "bus-expansion",
            .name = "Bus expansion chassis"
        },
        {
            .id   = DMI_CHASSIS_TYPE_PERIPHERAL,
            .code = "peripheral",
            .name = "Peripheral chassis"
        },
        {
            .id   = DMI_CHASSIS_TYPE_RAID,
            .code = "raid",
            .name = "RAID chassis"
        },
        {
            .id   = DMI_CHASSIS_TYPE_RACK_MOUNT,
            .code = "rack-mount",
            .name = "Rack-mount chassis"
        },
        {
            .id   = DMI_CHASSIS_TYPE_SEALED_CASE_PC,
            .code = "sealed-case-pc",
            .name = "Sealed-case PC"
        },
        {
            .id   = DMI_CHASSIS_TYPE_MULTI_SYSTEM,
            .code = "multy-system",
            .name = "Multi-system chassis"
        },
        {
            .id   = DMI_CHASSIS_TYPE_COMPACT_PCI,
            .code = "compact-pci",
            .name = "Compact PCI"
        },
        {
            .id   = DMI_CHASSIS_TYPE_ADVANCED_TCA,
            .code = "advanced-tca",
            .name = "Advanced TCA"
        },
        {
            .id   = DMI_CHASSIS_TYPE_BLADE,
            .code = "blade",
            .name = "Blade"
        },
        {
            .id   = DMI_CHASSIS_TYPE_BLADE_ENCLOSURE,
            .code = "blade-enclosure",
            .name = "Blade enclosure"
        },
        {
            .id   = DMI_CHASSIS_TYPE_TABLET,
            .code = "tablet",
            .name = "Tablet"
        },
        {
            .id   = DMI_CHASSIS_TYPE_CONVERTIBLE,
            .code = "convertible",
            .name = "Convertible"
        },
        {
            .id   = DMI_CHASSIS_TYPE_DETACHABLE,
            .code = "detachable",
            .name = "Detachable"
        },
        {
            .id   = DMI_CHASSIS_TYPE_IOT_GATEWAY,
            .code = "iot-gateway",
            .name = "IoT gateway"
        },
        {
            .id   = DMI_CHASSIS_TYPE_EMBEDDED_PC,
            .code = "embedded-pc",
            .name = "Embedded PC"
        },
        {
            .id   = DMI_CHASSIS_TYPE_MINI_PC,
            .code = "mini-pc",
            .name = "Mini PC"
        },
        {
            .id   = DMI_CHASSIS_TYPE_STICK_PC,
            .code = "stick-pc",
            .name = "Stick PC"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_chassis_security_status_names =
{
    .code  = "chassis-security-statuses",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_CHASSIS_SECURITY_STATUS_UNSPEC),
        DMI_NAME_OTHER(DMI_CHASSIS_SECURITY_STATUS_OTHER),
        DMI_NAME_UNKNOWN(DMI_CHASSIS_SECURITY_STATUS_UNKNOWN),
        DMI_NAME_NONE(DMI_CHASSIS_SECURITY_STATUS_NONE),
        {
            .id   = DMI_CHASSIS_SECURITY_STATUS_EXT_IF_LOCKED,
            .code = "ext-if-locked",
            .name = "External interface locked"
        },
        {
            .id   = DMI_CHASSIS_SECURITY_STATUS_EXT_IF_ENABLED,
            .code = "ext-if-enabled",
            .name = "External interface enabled"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_rack_type_names =
{
    .code  = "rack-types",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_RACK_TYPE_UNSPEC),
        {
            .id   = DMI_RACK_TYPE_OPEN,
            .code = "open-rack",
            .name = "Open Rack"
        },
        DMI_NAME_NULL
    }
};

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
    .minimum_version = DMI_VERSION(2, 0, 0),
    .required_from   = DMI_VERSION(2, 3, 0),
    .required_till   = DMI_VERSION_NONE,
    .unique          = false,
    .minimum_length  = 0x09,
    .decoded_length  = sizeof(dmi_chassis_t),
    .attributes      = (const dmi_attribute_t[]) {
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
            .attrs   = (const dmi_attribute_t[]){
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
                DMI_ATTRIBUTE_NULL
            }
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
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_chassis_decode,
        .cleanup = dmi_chassis_cleanup
    }
};

const char *dmi_chassis_type_name(dmi_chassis_type_t value)
{
    return dmi_name_lookup(&dmi_chassis_type_names, (int)value);
}

const char *dmi_chassis_security_status_name(dmi_chassis_security_status_t value)
{
    return dmi_name_lookup(&dmi_chassis_security_status_names, (int)value);
}

static bool dmi_chassis_decode(dmi_entity_t *entity)
{
    dmi_chassis_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(CHASSIS));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    // SMBIOS 2.0 fields
    dmi_byte_t type_value = 0;

    bool status =
        dmi_stream_decode_str(stream, &info->vendor) and
        dmi_stream_decode(stream, dmi_byte_t, &type_value) and
        dmi_stream_decode_str(stream, &info->version) and
        dmi_stream_decode_str(stream, &info->serial_number) and
        dmi_stream_decode_str(stream, &info->asset_tag);
    if (not status)
        return false;

    dmi_chassis_type_data_t type = {
        .__value = type_value
    };

    info->type            = type.type;
    info->is_lock_present = type.is_lock_present;

    // SMBIOS 2.1 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 1, 0);

    status =
        dmi_stream_decode(stream, dmi_byte_t, &info->bootup_state) and
        dmi_stream_decode(stream, dmi_byte_t, &info->power_supply_state) and
        dmi_stream_decode(stream, dmi_byte_t, &info->thermal_state) and
        dmi_stream_decode(stream, dmi_byte_t, &info->security_status);
    if (not status)
        return dmi_entity_incomplete(entity);

    // SMBIOS 2.3 fields, grouped as dmidecode does
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 3, 0);

    if (not dmi_stream_decode(stream, dmi_dword_t, &info->oem_defined))
        return dmi_entity_incomplete(entity);

    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    status =
        dmi_stream_decode(stream, dmi_byte_t, &info->height) and
        dmi_stream_decode(stream, dmi_byte_t, &info->power_cord_count);
    if (not status)
        return dmi_entity_incomplete(entity);

    // Contained elements
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    dmi_byte_t element_count = 0;
    dmi_byte_t element_size = 0;

    status =
        dmi_stream_decode(stream, dmi_byte_t, &element_count) and
        dmi_stream_decode(stream, dmi_byte_t, &element_size);
    if (not status)
        return dmi_entity_incomplete(entity);

    info->element_size = element_size;

    // Element record consists of type, minimum and maximum counts. Records
    // shorter than defined by specification are skipped.
    const size_t element_data_size = 3 * sizeof(dmi_byte_t);
    bool decode_elements = (element_size >= element_data_size);

    if ((element_count > 0) and decode_elements) {
        info->elements = dmi_alloc_array(context, sizeof(dmi_chassis_element_t), element_count);
        if (info->elements == nullptr)
            return false;
    }

    // Only completely present elements are counted, and the following fields
    // are not decoded if elements do not fit into the structure
    for (size_t i = 0; i < element_count; i++) {
        if (not dmi_stream_has(stream, element_size))
            return dmi_entity_incomplete(entity);

        if (not decode_elements) {
            if (not dmi_stream_skip(stream, element_size))
                return false;
            continue;
        }

        dmi_chassis_element_t *element = &info->elements[i];
        dmi_byte_t element_type = 0;

        status =
            dmi_stream_decode(stream, dmi_byte_t, &element_type) and
            dmi_stream_decode(stream, dmi_byte_t, &element->minimum_count) and
            dmi_stream_decode(stream, dmi_byte_t, &element->maximum_count) and
            dmi_stream_skip(stream, element_size - element_data_size);
        if (not status)
            return false;

        // Reserved values
        if (element->minimum_count == 0xFFu)
            element->minimum_count = SIZE_MAX;
        if (element->maximum_count == 0x00u)
            element->maximum_count = SIZE_MAX;

        // Element type is either SMBIOS structure type or baseboard type
        if (element_type & 0x80u) {
            element->type       = element_type & 0x7Fu;
            element->board_type = DMI_BASEBOARD_TYPE_UNSPEC;
        } else {
            element->type       = DMI_TYPE_INVALID;
            element->board_type = element_type;
        }

        info->element_count++;
    }

    // SMBIOS 2.7 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 7, 0);

    if (not dmi_stream_decode_str(stream, &info->sku_number))
        return dmi_entity_incomplete(entity);

    // SMBIOS 3.9 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(3, 9, 0);

    status =
        dmi_stream_decode(stream, dmi_byte_t, &info->rack_type) and
        dmi_stream_decode(stream, dmi_byte_t, &info->rack_height);
    if (not status)
        return dmi_entity_incomplete(entity);

    return true;
}

static void dmi_chassis_cleanup(dmi_entity_t *entity)
{
    dmi_chassis_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(CHASSIS));
    if (info == nullptr)
        return;

    dmi_free(info->elements);
}
