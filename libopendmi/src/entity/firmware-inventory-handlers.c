//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-inventory-internal.h>

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

const dmi_attribute_t dmi_firmware_version_number_attrs[] =
{
    DMI_ATTRIBUTE(dmi_firmware_version_number_t, major, INTEGER, {
        .code = "major",
        .name = "Major"
    }),
    DMI_ATTRIBUTE(dmi_firmware_version_number_t, minor, INTEGER, {
        .code = "minor",
        .name = "Minor"
    }),
    {}
};

//
// Versions and identifiers are written as strings, which are read according
// to the formats the structure declares for them.
//
bool dmi_firmware_inventory_derive(dmi_entity_t *entity)
{
    dmi_firmware_inventory_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
    if (info == nullptr)
        return false;

    dmi_firmware_version_parse(info->version, info->version_format, &info->parsed_version);
    dmi_firmware_version_parse(info->lowest_version, info->version_format,
                               &info->parsed_lowest_version);
    dmi_firmware_ident_parse(info->ident, info->ident_format, &info->parsed_ident);

    return true;
}

bool dmi_firmware_inventory_link(dmi_entity_t *entity)
{
    dmi_firmware_inventory_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    bool success = true;
    for (size_t i = 0; i < info->component_count; i++) {
        dmi_firmware_inventory_component_t *component = &info->components[i];

        if (not dmi_registry_resolve(registry, component->handle, DMI_TYPE_ANY, &component->entity))
            success = false;
    }

    return success;
}

void dmi_firmware_inventory_cleanup(dmi_entity_t *entity)
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
