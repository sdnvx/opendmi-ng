//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-inventory.h>

static bool dmi_firmware_inventory_decode(dmi_entity_t *entity);

static void dmi_firmware_version_parse(
        const char             *str,
        dmi_version_format_t    format,
        dmi_firmware_version_t *version);
static void dmi_firmware_ident_parse(
        const char                  *str,
        dmi_firmware_ident_format_t  format,
        dmi_firmware_ident_t        *ident);

static bool dmi_firmware_parse_decimal(const char **pstr, uint32_t *value);
static bool dmi_firmware_parse_hex(const char *str, size_t max_digits, uint64_t *value);
static int dmi_firmware_hex_digit(char c);
static bool dmi_firmware_inventory_link(dmi_entity_t *entity);
static void dmi_firmware_inventory_cleanup(dmi_entity_t *entity);

static const dmi_name_set_t dmi_version_format_names =
{
    .code  = "version-formats",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_VERSION_FORMAT_FREE,
            .code = "free-form",
            .name = "Free form"
        },
        {
            .id   = DMI_VERSION_FORMAT_SEMANTIC,
            .code = "semantic",
            .name = "Semantic"
        },
        {
            .id   = DMI_VERSION_FORMAT_HEX_32,
            .code = "hexadecimal-32",
            .name = "Hexadecimal (32-bit)"
        },
        {
            .id   = DMI_VERSION_FORMAT_HEX_64,
            .code = "hexadecimal-64",
            .name = "Hexadecimal (64-bit)"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_firmware_ident_format_names =
{
    .code  = "firmware-ident-formats",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_FIRMWARE_IDENT_FORMAT_FREE,
            .code = "free-form",
            .name = "Free form"
        },
        {
            .id   = DMI_FIRMWARE_IDENT_FORMAT_GUID,
            .code = "guid",
            .name = "GUID"
        },
        DMI_NAME_NULL
    },
    .ranges = (dmi_name_range_t[]){
        {
            .start_id = __DMI_FIRMWARE_IDENT_FORMAT_RESERVED_START,
            .end_id   = __DMI_FIRMWARE_IDENT_FORMAT_RESERVED_END,
            .code     = "reserved",
            .name     = "Reserved"
        },
        {
            .start_id = __DMI_FIRMWARE_IDENT_FORMAT_VENDOR_SPECIFIC_START,
            .end_id   = __DMI_FIRMWARE_IDENT_FORMAT_VENDOR_SPECIFIC_END,
            .code     = "vendor-specific",
            .name     = "Vendor/OEM-specific"
        },
        DMI_NAME_RANGE_NULL
    }
};

static const dmi_name_set_t dmi_firmware_inventory_feature_names =
{
    .code  = "firmware-inventory-features",
    .names = (dmi_name_t[]){
        {
            .id   = 0,
            .code = "is-updatable",
            .name = "Updatable"
        },
        {
            .id   = 1,
            .code = "is-write-protected",
            .name = "Write-protected"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_firmware_inventory_state_names =
{
    .code  = "firmware-inventory-states",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_FIRMWARE_INVENTORY_STATE_UNSPEC),
        DMI_NAME_OTHER(DMI_FIRMWARE_INVENTORY_STATE_OTHER),
        DMI_NAME_UNKNOWN(DMI_FIRMWARE_INVENTORY_STATE_UNKNOWN),
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_ABSENT,
            .code = "absent",
            .name = "Absent"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_STANDBY_OFFLINE,
            .code = "standby-offline",
            .name = "Standby offline"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_STANDBY_SPARE,
            .code = "standby-spare",
            .name = "Standby spare"
        },
        {
            .id   = DMI_FIRMWARE_INVENTORY_STATE_UNAVAIL_OFFLINE,
            .code = "unavail-offline",
            .name = "Unavailable offline"
        },
        DMI_NAME_NULL
    }
};

static const dmi_attribute_t dmi_firmware_version_number_attrs[] =
{
    DMI_ATTRIBUTE(dmi_firmware_version_number_t, major, INTEGER, {
        .code = "major",
        .name = "Major"
    }),
    DMI_ATTRIBUTE(dmi_firmware_version_number_t, minor, INTEGER, {
        .code = "minor",
        .name = "Minor"
    }),
    DMI_ATTRIBUTE_NULL
};

//
// Version is shown as parsed according to the version format, or as the
// original string, if it does not conform to the format
//
#define dmi_firmware_version_variants(__string, __parsed)                                        \
    (const dmi_attribute_variant_t[]){                                                         \
        DMI_VARIANT(DMI_VERSION_FORMAT_SEMANTIC, dmi_firmware_inventory_t, __parsed.number,     \
                    STRUCT, { .attrs = dmi_firmware_version_number_attrs }),                   \
        DMI_VARIANT(DMI_VERSION_FORMAT_HEX_32, dmi_firmware_inventory_t, __parsed.value,        \
                    INTEGER, { .flags = DMI_ATTRIBUTE_FLAG_HEX }),                             \
        DMI_VARIANT(DMI_VERSION_FORMAT_HEX_64, dmi_firmware_inventory_t, __parsed.value,        \
                    INTEGER, { .flags = DMI_ATTRIBUTE_FLAG_HEX }),                             \
        DMI_VARIANT_DEFAULT(dmi_firmware_inventory_t, __string, STRING, {}),                   \
        DMI_VARIANT_NULL                                                                       \
    }

const dmi_entity_spec_t dmi_firmware_inventory_spec =
{
    .code            = "firmware-inventory",
    .name            = "Firmware inventory information",
    .type            = DMI_TYPE(FIRMWARE_INVENTORY),
    .description     = (const char *[]){
        "The information in this structure defines an inventory of firmware "
        "components in the system. This can include firmware components such "
        "as platform firmware, BMC, as well as firmware for other devices in "
        "the system.",
        //
        "The information can be used by software to display the firmware "
        "inventory in a uniform manner. It can also be used by a management "
        "controller, such as a BMC, for remote system management. This "
        "structure is not intended to replace other standard programmatic "
        "interfaces for firmware updates.",
        //
        "One Type 45 structure is provided for each firmware component.",
        //
        nullptr
    },
    .minimum_version = DMI_VERSION(3, 5, 0),
    .minimum_length  = 0x17,
    .decoded_length  = sizeof(dmi_firmware_inventory_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, name, STRING, {
            .code    = "name",
            .name    = "Name"
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_firmware_inventory_t, parsed_version.format, {
            .code     = "version",
            .name     = "Version",
            .variants = dmi_firmware_version_variants(version, parsed_version)
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, version_format, ENUM, {
            .code    = "version-format",
            .name    = "Version format",
            .values  = &dmi_version_format_names
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_firmware_inventory_t, parsed_ident.format, {
            .code     = "ident",
            .name     = "Identifier",
            .variants = (const dmi_attribute_variant_t[]){
                DMI_VARIANT(DMI_FIRMWARE_IDENT_FORMAT_GUID, dmi_firmware_inventory_t, parsed_ident.guid, UUID, {}),
                DMI_VARIANT_DEFAULT(dmi_firmware_inventory_t, ident, STRING, {}),
                DMI_VARIANT_NULL
            }
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, ident_format, ENUM, {
            .code    = "ident-format",
            .name    = "Identifier format",
            .values  = &dmi_firmware_ident_format_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, release_date, STRING, {
            .code    = "release-date",
            .name    = "Release date"
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Vendor"
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_firmware_inventory_t, parsed_lowest_version.format, {
            .code     = "lowest-version",
            .name     = "Lowest version",
            .variants = dmi_firmware_version_variants(lowest_version, parsed_lowest_version)
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, image_size, SIZE, {
            .code    = "image-size",
            .name    = "Image size",
            .unknown = dmi_value_ptr(DMI_SIZE_MAX)
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, features, SET, {
            .code    = "characteristics",
            .name    = "Characteristics",
            .values  = &dmi_firmware_inventory_feature_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, state, ENUM, {
            .code    = "state",
            .name    = "State",
            .unspec  = dmi_value_ptr(DMI_FIRMWARE_INVENTORY_STATE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_FIRMWARE_INVENTORY_STATE_UNSPEC),
            .values  = &dmi_firmware_inventory_state_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, component_count, INTEGER, {
            .code    = "component-count",
            .name    = "Associated components count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_firmware_inventory_t, components, component_count, STRUCT, {
            .code    = "components",
            .name    = "Associated components",
            .attrs   = (const dmi_attribute_t[]){
                DMI_ATTRIBUTE(dmi_firmware_inventory_component_t, handle, HANDLE, {
                    .code = "handle",
                    .name = "Handle"
                }),
                {}
            }
        }),
        {}
    },
    .handlers = {
        .decode  = dmi_firmware_inventory_decode,
        .link    = dmi_firmware_inventory_link,
        .cleanup = dmi_firmware_inventory_cleanup
    }
};

const char *dmi_version_format_name(dmi_version_format_t value)
{
    return dmi_name_lookup(&dmi_version_format_names, (int)value);
}

const char *dmi_firmware_ident_format_name(dmi_firmware_ident_format_t value)
{
    return dmi_name_lookup(&dmi_firmware_ident_format_names, (int)value);
}

const char *dmi_firmware_inventory_state_name(dmi_firmware_inventory_state_t value)
{
    return dmi_name_lookup(&dmi_firmware_inventory_state_names, (int)value);
}

static bool dmi_firmware_inventory_decode(dmi_entity_t *entity)
{
    dmi_firmware_inventory_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    bool status =
        dmi_stream_decode_str(stream, &info->name) and
        dmi_stream_decode_str(stream, &info->version) and
        dmi_stream_decode(stream, dmi_byte_t, &info->version_format) and
        dmi_stream_decode_str(stream, &info->ident) and
        dmi_stream_decode(stream, dmi_byte_t, &info->ident_format) and
        dmi_stream_decode_str(stream, &info->release_date) and
        dmi_stream_decode_str(stream, &info->vendor) and
        dmi_stream_decode_str(stream, &info->lowest_version) and
        dmi_stream_decode(stream, dmi_qword_t, &info->image_size) and
        dmi_stream_decode(stream, dmi_word_t, &info->features.__value) and
        dmi_stream_decode(stream, dmi_byte_t, &info->state);

    if (not status)
        return false;

    dmi_firmware_version_parse(info->version, info->version_format, &info->parsed_version);
    dmi_firmware_version_parse(info->lowest_version, info->version_format, &info->parsed_lowest_version);
    dmi_firmware_ident_parse(info->ident, info->ident_format, &info->parsed_ident);

    // Associated components
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    dmi_byte_t component_count = 0;
    if (not dmi_stream_decode(stream, dmi_byte_t, &component_count))
        return false;

    if (component_count == 0)
        return true;

    info->components = dmi_alloc_array(entity->context,
                                       sizeof(dmi_firmware_inventory_component_t),
                                       component_count);
    if (info->components == nullptr)
        return false;

    // Only completely present component handles are counted
    for (size_t i = 0; i < component_count; i++) {
        dmi_firmware_inventory_component_t *component = &info->components[i];

        if (not dmi_stream_decode(stream, dmi_word_t, &component->handle))
            return dmi_entity_incomplete(entity);

        info->component_count++;
    }

    return true;
}

static bool dmi_firmware_inventory_link(dmi_entity_t *entity)
{
    dmi_firmware_inventory_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
    if (info == nullptr)
        return false;

    dmi_registry_t *registry = entity->context->state.registry;
    bool success = true;

    for (size_t i = 0; i < info->component_count; i++) {
        dmi_firmware_inventory_component_t *component = &info->components[i];

        if (not dmi_registry_resolve(registry, component->handle, DMI_TYPE_INVALID, &component->entity))
            success = false;
    }

    return success;
}

static void dmi_firmware_inventory_cleanup(dmi_entity_t *entity)
{
    dmi_firmware_inventory_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
    if (info == nullptr)
        return;

    dmi_free(info->components);
}

static void dmi_firmware_version_parse(
        const char             *str,
        dmi_version_format_t    format,
        dmi_firmware_version_t *version)
{
    version->format = DMI_VERSION_FORMAT_FREE;

    if (str == nullptr)
        return;

    // Strings not conforming to the format are kept as free-form ones
    switch (format) {
    case DMI_VERSION_FORMAT_SEMANTIC:
        if (dmi_firmware_parse_decimal(&str, &version->number.major) and (*str++ == '.') and
            dmi_firmware_parse_decimal(&str, &version->number.minor) and (*str == 0))
            version->format = format;
        break;

    case DMI_VERSION_FORMAT_HEX_32:
        if (dmi_firmware_parse_hex(str, 8, &version->value))
            version->format = format;
        break;

    case DMI_VERSION_FORMAT_HEX_64:
        if (dmi_firmware_parse_hex(str, 16, &version->value))
            version->format = format;
        break;

    default:
        break;
    }
}

static void dmi_firmware_ident_parse(
        const char                  *str,
        dmi_firmware_ident_format_t  format,
        dmi_firmware_ident_t        *ident)
{
    ident->format = DMI_FIRMWARE_IDENT_FORMAT_FREE;

    if ((str == nullptr) or (format != DMI_FIRMWARE_IDENT_FORMAT_GUID))
        return;

    // GUID string uses RFC 4122 format, in which bytes are in the same order
    // as in the UUID value
    size_t count = 0;

    for (size_t i = 0; str[i] != 0; i++) {
        if ((i == 8) or (i == 13) or (i == 18) or (i == 23)) {
            if (str[i] != '-')
                return;
            continue;
        }

        int digit = dmi_firmware_hex_digit(str[i]);
        if ((digit < 0) or (count >= 2 * sizeof(ident->guid.__value)))
            return;

        if (count % 2 == 0)
            ident->guid.__value[count / 2] = (dmi_byte_t)(digit << 4);
        else
            ident->guid.__value[count / 2] |= (dmi_byte_t)digit;

        count++;
    }

    if (count == 2 * sizeof(ident->guid.__value))
        ident->format = format;
}

static bool dmi_firmware_parse_decimal(const char **pstr, uint32_t *value)
{
    const char *str = *pstr;
    uint64_t rv = 0;

    if ((*str < '0') or (*str > '9'))
        return false;

    for (; (*str >= '0') and (*str <= '9'); str++) {
        rv = rv * 10 + (uint64_t)(*str - '0');
        if (rv > UINT32_MAX)
            return false;
    }

    *pstr  = str;
    *value = (uint32_t)rv;

    return true;
}

static bool dmi_firmware_parse_hex(const char *str, size_t max_digits, uint64_t *value)
{
    uint64_t rv = 0;
    size_t count = 0;

    if ((str[0] != '0') or ((str[1] != 'x') and (str[1] != 'X')))
        return false;

    for (str += 2; *str != 0; str++, count++) {
        int digit = dmi_firmware_hex_digit(*str);
        if ((digit < 0) or (count >= max_digits))
            return false;

        rv = (rv << 4) | (uint64_t)digit;
    }

    if (count == 0)
        return false;

    *value = rv;

    return true;
}

static int dmi_firmware_hex_digit(char c)
{
    if ((c >= '0') and (c <= '9'))
        return c - '0';
    if ((c >= 'a') and (c <= 'f'))
        return c - 'a' + 10;
    if ((c >= 'A') and (c <= 'F'))
        return c - 'A' + 10;

    return -1;
}

