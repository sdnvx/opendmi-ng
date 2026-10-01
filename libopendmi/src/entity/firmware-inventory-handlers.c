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

#include "firmware-inventory-internal.h"

/**
 * @internal
 * @brief Parse a version string according to the format the structure
 * declares for it.
 *
 * @details Strings not conforming to the format are kept as free-form ones.
 *
 * @param[in]  str     Version string, or `nullptr` if there is none.
 * @param[in]  format  Format declared for the version.
 * @param[out] version Variable to store the parsed version in.
 */
static void dmi_firmware_version_parse(
        const char                    *str,
        dmi_firmware_version_format_t  format,
        dmi_firmware_version_t        *version);

/**
 * @internal
 * @brief Parse an identifier string according to the format the structure
 * declares for it.
 *
 * @details Only GUIDs are parsed, written in the RFC 4122 format. Other
 * identifiers, and the strings not conforming to the format, are kept as
 * free-form ones.
 *
 * @param[in]  str    Identifier string, or `nullptr` if there is none.
 * @param[in]  format Format declared for the identifier.
 * @param[out] ident  Variable to store the parsed identifier in.
 */
static void dmi_firmware_ident_parse(
        const char                  *str,
        dmi_firmware_ident_format_t  format,
        dmi_firmware_ident_t        *ident);

/**
 * @internal
 * @brief Parse a decimal number which fits into 32 bits.
 *
 * @param[in,out] pstr  Pointer to the string, which is advanced past the
 *                      digits on success.
 * @param[out]    value Variable to store the number in.
 *
 * @return `true` if at least one digit is read and the number fits,
 *         `false` otherwise.
 */
static bool dmi_firmware_parse_decimal(const char **pstr, uint32_t *value);

/**
 * @internal
 * @brief Parse a whole string as a hexadecimal number prefixed with `0x`.
 *
 * @param[in]  str        String to parse.
 * @param[in]  max_digits Maximum number of digits.
 * @param[out] value      Variable to store the number in.
 *
 * @return `true` if the string is a valid number, `false` otherwise.
 */
static bool dmi_firmware_parse_hex(const char *str, size_t max_digits, uint64_t *value);

/**
 * @internal
 * @brief Get the value of a hexadecimal digit.
 *
 * @param[in] c Character to convert.
 *
 * @return Value of the digit, or `-1` if the character is not a digit.
 */
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

bool dmi_firmware_inventory_derive(dmi_entity_t *entity)
{
    dmi_firmware_inventory_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(firmware_inventory));
    if (info == nullptr)
        return false;

    dmi_firmware_version_parse(info->version, info->version_format, &info->parsed_version);
    dmi_firmware_version_parse(info->lowest_version, info->version_format,
                               &info->parsed_lowest_version);
    dmi_firmware_ident_parse(info->ident, info->ident_format, &info->parsed_ident);

    return true;
}

static void dmi_firmware_version_parse(
        const char                    *str,
        dmi_firmware_version_format_t  format,
        dmi_firmware_version_t        *version)
{
    version->format = DMI_FIRMWARE_VERSION_FORMAT_FREE;

    if (str == nullptr)
        return;

    // Strings not conforming to the format are kept as free-form ones
    switch (format) {
    case DMI_FIRMWARE_VERSION_FORMAT_SEMANTIC:
        if (dmi_firmware_parse_decimal(&str, &version->number.major) and (*str++ == '.') and
            dmi_firmware_parse_decimal(&str, &version->number.minor) and (*str == 0))
            version->format = format;
        break;

    case DMI_FIRMWARE_VERSION_FORMAT_HEX_32:
        if (dmi_firmware_parse_hex(str, 8, &version->value))
            version->format = format;
        break;

    case DMI_FIRMWARE_VERSION_FORMAT_HEX_64:
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
