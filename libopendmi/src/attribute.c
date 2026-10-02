//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/attribute.h>
#include <opendmi/internal.h>

#include <opendmi/utils.h>
#include <opendmi/utils/endian.h>
#include <opendmi/utils/string.h>
#include <opendmi/utils/datetime.h>
#include <opendmi/utils/uuid.h>
#include <opendmi/utils/version.h>

/**
 * @internal
 * @brief Name of the resource table holding the names shared by structures.
 */
#define DMI_ATTRIBUTE_TABLE "attribute"

/**
 * @internal
 * @brief Read an unsigned integer member of any supported width.
 *
 * @param[in] ptr  Member to read.
 * @param[in] size Size of the member in bytes.
 *
 * @return Value of the member, or zero if its size is not supported.
 */
static uintmax_t dmi_attribute_read_uint(const void *ptr, size_t size);

/**
 * @internal
 * @brief Read a signed integer member of any supported width.
 *
 * @details Selectors are usually enumerations, which may be signed.
 *
 * @param[in] ptr  Member to read.
 * @param[in] size Size of the member in bytes.
 *
 * @return Value of the member, or zero if its size is not supported.
 */
static intmax_t dmi_attribute_read_int(const void *ptr, size_t size);

/**
 * @internal
 * @brief Format a structure handle as a hexadecimal number.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_handle(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format a string, which is copied as it is.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated copy of the string, or `nullptr` if the string is
 * absent or on error.
 */
static char *dmi_attribute_format_string(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format a flag by the names of the attribute, or by the usual
 * names of boolean values if it has none for the value.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_bool(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format an integer as a signed, hexadecimal or unsigned number,
 * as the flags of the attribute say.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_ARGUMENT_INVALID Size of the value is not supported
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_integer(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format a fixed-point number scaled by a power of ten.
 *
 * @details The trailing zeros of the fraction are dropped, except the first
 * digit after the point. A value with no scale is formatted as an integer.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_ARGUMENT_INVALID Size of the value is not supported
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_decimal(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format a size in bytes.
 *
 * @details Pretty output gives the size in the largest binary unit it is a
 * whole number of.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_size(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format an address as a hexadecimal number as wide as the
 * addresses of the context.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_address(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format a value of an enumeration by the names of the attribute.
 *
 * @details A value the enumeration does not name is formatted as a
 * hexadecimal number.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_ARGUMENT_INVALID Attribute has no names of the values
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_enum(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format a set of flags as a hexadecimal number as wide as the
 * value.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_set(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format a version with as many components as the scale of the
 * attribute says.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_version(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format a date.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_date(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format a UUID in its canonical textual form.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_uuid(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

/**
 * @internal
 * @brief Format an IPv4 address in dotted decimal notation.
 *
 * @param[in] context DMI context.
 * @param[in] binary  Address of four bytes.
 *
 * @error DMI_ERROR_ARGUMENT_INVALID Address is not four bytes long
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_ipv4(dmi_context_t *context, const dmi_binary_t *binary);

/**
 * @internal
 * @brief Format an IPv6 address in its canonical textual form.
 *
 * @details The longest run of at least two zero groups is compressed, the
 * first one if there are several (RFC 5952, section 4.2).
 *
 * @param[in] context DMI context.
 * @param[in] binary  Address of sixteen bytes.
 *
 * @error DMI_ERROR_ARGUMENT_INVALID Address is not sixteen bytes long
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_ipv6(dmi_context_t *context, const dmi_binary_t *binary);

/**
 * @internal
 * @brief Format binary data as hexadecimal digits.
 *
 * @details Data flagged as an IP address of four or sixteen bytes is formatted
 * as one, and data flagged as a MAC address has its colon-separated bytes,
 * with the trailing zeros past the address dropped.
 *
 * @param[in] context   DMI context.
 * @param[in] attribute Attribute describing the value.
 * @param[in] value     Value to format.
 * @param[in] pretty    `true` for human-readable output, `false` for
 *                      machine-readable output.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory allocation failed
 *
 * @return Newly allocated string, or `nullptr` on error.
 */
static char *dmi_attribute_format_binary(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static const dmi_attribute_ops_t dmi_attribute_type_ops[] =
{
    [DMI_ATTRIBUTE_TYPE_HANDLE] = {
        .format = dmi_attribute_format_handle,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_STRING] = {
        .format = dmi_attribute_format_string,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_BOOL] = {
        .format = dmi_attribute_format_bool,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_INTEGER] = {
        .format = dmi_attribute_format_integer,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_DECIMAL] = {
        .format = dmi_attribute_format_decimal,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_SIZE] = {
        .format = dmi_attribute_format_size,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_ADDRESS] = {
        .format = dmi_attribute_format_address,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_ENUM] = {
        .format = dmi_attribute_format_enum,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_SET] = {
        .format = dmi_attribute_format_set,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_VERSION] = {
        .format = dmi_attribute_format_version,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_DATE] = {
        .format = dmi_attribute_format_date,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_UUID] = {
        .format = dmi_attribute_format_uuid,
        .parse  = nullptr
    },
    [DMI_ATTRIBUTE_TYPE_BINARY] = {
        .format = dmi_attribute_format_binary,
        .parse  = nullptr
    }
};

bool dmi_attribute_is_unspecified(const dmi_attribute_t *attr, const void *value)
{
    if (attr == nullptr)
        return dmi_trace_argument_null(nullptr, attr);
    if (value == nullptr)
        return dmi_trace_argument_null(nullptr, value);

    if (attr->params.unspec) {
        if (memcmp(value, attr->params.unspec, attr->value.size) == 0)
            return true;
    } else if (attr->type == DMI_ATTRIBUTE_TYPE_STRING) {
        if (dmi_deref(char *, value) == nullptr)
            return true;
    } else if (attr->type == DMI_ATTRIBUTE_TYPE_HANDLE) {
        if (dmi_deref(dmi_handle_t, value) == DMI_HANDLE_INVALID)
            return true;
    } else if (attr->type == DMI_ATTRIBUTE_TYPE_BINARY) {
        if (dmi_deref(dmi_binary_t, value).length == 0)
            return true;
    }

    return false;
}

bool dmi_attribute_is_unknown(const dmi_attribute_t *attr, const void *value)
{
    if (attr == nullptr)
        return dmi_trace_argument_null(nullptr, attr);
    if (value == nullptr)
        return dmi_trace_argument_null(nullptr, value);

    if (attr->params.unknown) {
        if (memcmp(value, attr->params.unknown, attr->value.size) == 0)
            return true;
    }

    return false;
}

bool dmi_attribute_get_bool(const dmi_attribute_t *attr, const void *value)
{
    dmi_unused(attr);

    if (value == nullptr)
        return dmi_trace_argument_null(nullptr, value);

    return dmi_deref(bool, value) ? true : false;
}

intmax_t dmi_attribute_get_int(const dmi_attribute_t *attr, const void *value)
{
    if (attr == nullptr)
        return dmi_trace_argument_null(nullptr, attr, 0);
    if (value == nullptr)
        return dmi_trace_argument_null(nullptr, value, 0);

    intmax_t rv;

    if (attr->value.size == sizeof(int8_t))
        rv = dmi_deref(int8_t, value);
    else if (attr->value.size == sizeof(int16_t))
        rv = dmi_deref(int16_t, value);
    else if (attr->value.size == sizeof(int32_t))
        rv = dmi_deref(int32_t, value);
    else if (attr->value.size == sizeof(int64_t))
        rv = dmi_deref(int64_t, value);
    else
        rv = INTMAX_MAX;

    return rv;
}

uintmax_t dmi_attribute_get_uint(const dmi_attribute_t *attr, const void *value)
{
    if (attr == nullptr)
        return dmi_trace_argument_null(nullptr, attr, 0);
    if (value == nullptr)
        return dmi_trace_argument_null(nullptr, value, 0);

    uintmax_t rv;

    if (attr->value.size == sizeof(uint8_t))
        rv = dmi_deref(uint8_t, value);
    else if (attr->value.size == sizeof(uint16_t))
        rv = dmi_deref(uint16_t, value);
    else if (attr->value.size == sizeof(uint32_t))
        rv = dmi_deref(uint32_t, value);
    else if (attr->value.size == sizeof(uint64_t))
        rv = dmi_deref(uint64_t, value);
    else
        rv = UINTMAX_MAX;

    return rv;
}

size_t dmi_attribute_get_count(const dmi_attribute_t *attr, const void *info)
{
    if (attr == nullptr)
        return dmi_trace_argument_null(nullptr, attr, 0);
    if (info == nullptr)
        return dmi_trace_argument_null(nullptr, info, 0);

    if (attr->count != 0)
        return attr->count;

    uintmax_t rv = dmi_attribute_read_uint(dmi_member_ptr(info, attr->counter, void), attr->counter.size);

    return (rv <= SIZE_MAX) ? (size_t)rv : 0;
}

bool dmi_attribute_is_array(const dmi_attribute_t *attr)
{
    if (attr == nullptr)
        return dmi_trace_argument_null(nullptr, attr);

    return dmi_member_is_present(attr->counter) or (attr->count != 0);
}

const void *dmi_attribute_get_elements(const dmi_attribute_t *attr, const void *value)
{
    if (attr == nullptr)
        return dmi_trace_argument_null(nullptr, attr, nullptr);
    if (value == nullptr)
        return dmi_trace_argument_null(nullptr, value, nullptr);

    return (attr->count != 0) ? value : dmi_deref(void *, value);
}

const char *dmi_attribute_name(const dmi_attribute_t *attr, const char *owner)
{
    if (attr == nullptr)
        return dmi_trace_argument_null(nullptr, attr, nullptr);

    // Printable names are translated, if the locale has a translation for the
    // attribute, while codes are machine-readable and are never translated
    if (attr->params.code != nullptr) {
        const char *translated = nullptr;

        // Names of the structure take precedence over the shared ones, so
        // that a common name can be overridden where it does not fit
        if (owner != nullptr) {
            char table[128];

            if (snprintf(table, sizeof(table), "%s/" DMI_ATTRIBUTE_TABLE, owner) < (int)sizeof(table))
                translated = dmi_locale_string(table, attr->params.code);
        }

        if (translated == nullptr)
            translated = dmi_locale_string(DMI_ATTRIBUTE_TABLE, attr->params.code);

        if (translated != nullptr)
            return translated;
    }

    return attr->params.name;
}

const dmi_attribute_t *dmi_attribute_resolve(const dmi_attribute_t *attr, const void *info)
{
    if (attr == nullptr)
        return dmi_trace_argument_null(nullptr, attr, nullptr);
    if (info == nullptr)
        return dmi_trace_argument_null(nullptr, info, nullptr);

    if (attr->type != DMI_ATTRIBUTE_TYPE_VARIANT)
        return attr;
    if (attr->params.variants == nullptr)
        return dmi_trace_argument_invalid(nullptr, attr, nullptr);

    intmax_t selector = dmi_attribute_read_int(dmi_member_ptr(info, attr->value, void), attr->value.size);
    const dmi_attribute_t *fallback = nullptr;

    for (const dmi_attribute_variant_t *variant = attr->params.variants;
         variant->attribute.type != DMI_ATTRIBUTE_TYPE_NONE;
         variant++)
    {
        if (variant->is_default)
            fallback = &variant->attribute;
        else if (variant->selector == selector)
            return &variant->attribute;
    }

    return fallback;
}

char *dmi_attribute_format(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context, nullptr);
    if (attribute == nullptr)
        return dmi_trace_argument_null(context, attribute, nullptr);
    if (value == nullptr)
        return dmi_trace_argument_null(context, value, nullptr);

    const dmi_attribute_ops_t *ops;

    if (attribute->type >= countof(dmi_attribute_type_ops))
        return nullptr;

    ops = &dmi_attribute_type_ops[attribute->type];
    if (ops->format == nullptr)
        return nullptr;

    return ops->format(context, attribute, value, pretty);
}

static char *dmi_attribute_format_handle(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    dmi_unused(attribute);
    dmi_unused(pretty);

    char *str = nullptr;

    if (dmi_asprintf(&str, "0x%04" PRIX16, dmi_deref(dmi_handle_t, value)) < 0)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_string(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    dmi_unused(attribute);
    dmi_unused(pretty);

    const char *str = *(const char **)value;

    if (str == nullptr)
        return nullptr;

    char *result = strdup(str);

    if (result == nullptr)
        return dmi_trace_out_of_memory(context, nullptr);

    return result;
}

static char *dmi_attribute_format_bool(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(attribute != nullptr);
    assert(value != nullptr);

    bool flag = dmi_deref(bool, value) ? true : false;
    const char *str  = nullptr;

    if (attribute->params.values) {
        str = pretty
            ? dmi_name_lookup(attribute->params.values, flag)
            : dmi_code_lookup(attribute->params.values, flag);
    }

    // Values the table does not name are named the usual way
    if (str == nullptr) {
        str = pretty
            ? dmi_bool_name(flag)
            : dmi_bool_code(flag);
    }

    char *result = strdup(str);

    if (result == nullptr)
        return dmi_trace_out_of_memory(context, nullptr);

    return result;
}

static char *dmi_attribute_format_integer(
        dmi_context_t        *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    dmi_unused(pretty);

    int   rv        = -1;
    bool  is_signed = attribute->params.flags & DMI_ATTRIBUTE_FLAG_SIGNED;
    bool  is_hex    = attribute->params.flags & DMI_ATTRIBUTE_FLAG_HEX;
    char *str       = nullptr;

    // Note: as printf() arguments of type integer with sizes less than
    // sizeof(int) passed by compiler using int type, we need different casts
    // to make sign extension work properly.

    switch (attribute->value.size) {
    case sizeof(int8_t):
        if (is_signed)
            rv = dmi_asprintf(&str, "%" PRId8, dmi_deref(int8_t, value));
        else if (is_hex)
            rv = dmi_asprintf(&str, "0x%" PRIX8, dmi_deref(uint8_t, value));
        else
            rv = dmi_asprintf(&str, "%" PRIu8, dmi_deref(uint8_t, value));
        break;

    case sizeof(int16_t):
        if (is_signed)
            rv = dmi_asprintf(&str, "%" PRId16, dmi_deref(int16_t, value));
        else if (is_hex)
            rv = dmi_asprintf(&str, "0x%" PRIX16, dmi_deref(uint16_t, value));
        else
            rv = dmi_asprintf(&str, "%" PRIu16, dmi_deref(uint16_t, value));
        break;

    case sizeof(int32_t):
        if (is_signed)
            rv = dmi_asprintf(&str, "%" PRId32, dmi_deref(int32_t, value));
        else if (is_hex)
            rv = dmi_asprintf(&str, "0x%" PRIX32, dmi_deref(uint32_t, value));
        else
            rv = dmi_asprintf(&str, "%" PRIu32, dmi_deref(uint32_t, value));
        break;

    case sizeof(int64_t):
        if (is_signed)
            rv = dmi_asprintf(&str, "%" PRId64, dmi_deref(int64_t, value));
        else if (is_hex)
            rv = dmi_asprintf(&str, "0x%" PRIX64, dmi_deref(uint64_t, value));
        else
            rv = dmi_asprintf(&str, "%" PRIu64, dmi_deref(uint64_t, value));
        break;

    default:
        dmi_error_raise(context, DMI_ERROR_ARGUMENT_INVALID);
        return nullptr;
    }

    if (rv < 0)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_decimal(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    dmi_unused(pretty);

    int       rv       = 0;
    char     *str      = nullptr;
    bool      negative = false;
    uintmax_t magnitude;

    if (attribute->params.scale == 0)
        return dmi_attribute_format_integer(context, attribute, value, pretty);

    if (attribute->params.flags & DMI_ATTRIBUTE_FLAG_SIGNED) {
        intmax_t src = dmi_attribute_get_int(attribute, value);

        negative = src < 0;
        magnitude = negative ? -(uintmax_t)src : (uintmax_t)src;
    } else {
        magnitude = dmi_attribute_get_uint(attribute, value);
    }

    unsigned int scale  = attribute->params.scale;
    uintmax_t    factor = dmi_ipow32(10, scale);

    // Adjust scale and factor
    while (scale > 1) {
        if (magnitude % 10 != 0)
            break;
        magnitude /= 10, factor /= 10;
        scale--;
    }

    rv = dmi_asprintf(&str, "%s%" PRIuMAX ".%0*" PRIuMAX,
                      negative ? "-" : "",
                      magnitude / factor, (int)scale, magnitude % factor);

    if (rv < 0)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_size(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    int rv;
    char *str = nullptr;
    uintmax_t size = dmi_attribute_get_uint(attribute, value);

    if (pretty) {
        unsigned int i;

        static const dmi_unit_t units[] = {
            DMI_UNIT_BYTE,
            DMI_UNIT_KIBIBYTE, DMI_UNIT_MEBIBYTE, DMI_UNIT_GIBIBYTE,
            DMI_UNIT_TEBIBYTE, DMI_UNIT_PEBIBYTE, DMI_UNIT_EXBIBYTE,
            DMI_UNIT_ZEBIBYTE, DMI_UNIT_YOBIBYTE, DMI_UNIT_ROBIBYTE,
            DMI_UNIT_QUEBIBYTE,
            DMI_UNIT_NONE
        };

        for (i = 0; units[i] != DMI_UNIT_NONE; i++) {
            if ((size < 1024) or (size % 1024 != 0))
                break;
            size >>= 10;
        }

        rv = dmi_asprintf(&str, "%" PRIuMAX " %s", size, dmi_unit_name(units[i]));
    } else {
        rv = dmi_asprintf(&str, "%" PRIuMAX, size);
    }

    if (rv < 0)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_address(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    dmi_unused(pretty);

    int rv;
    char *str = nullptr;
    uintmax_t addr = dmi_attribute_get_uint(attribute, value);

    if (context->state.address_size == sizeof(uint32_t))
        rv = dmi_asprintf(&str, "0x%08" PRIXMAX, addr);
    else
        rv = dmi_asprintf(&str, "0x%016" PRIXMAX, addr);

    if (rv < 0)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_enum(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    const char *name;
    char *str = nullptr;

    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    if (attribute->params.values == nullptr) {
        dmi_error_raise(context, DMI_ERROR_ARGUMENT_INVALID);
        return nullptr;
    }

    if (pretty)
        name = dmi_name_lookup(attribute->params.values, dmi_deref(int, value));
    else
        name = dmi_code_lookup(attribute->params.values, dmi_deref(int, value));

    if (name != nullptr)
        str = strdup(name);
    else if (pretty)
        dmi_asprintf(&str, "%s (0x%x)", dmi_value_text("invalid", "<invalid>"), dmi_deref(int, value));
    else
        dmi_asprintf(&str, "0x%x", dmi_deref(int, value));

    if (str == nullptr)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_set(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    dmi_unused(pretty);

    char fmt[32];
    char *str = nullptr;
    uintmax_t src = dmi_attribute_get_uint(attribute, value);

    snprintf(fmt, sizeof(fmt), "0x%%0%zu" PRIXMAX, attribute->value.size * 2);

    int rv = dmi_asprintf(&str, fmt, src);
    if (rv < 0)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_version(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    dmi_unused(pretty);

    int rv = 0;
    char *str = nullptr;

    dmi_version_t version = dmi_deref(dmi_version_t, value);

    unsigned major    = dmi_version_major(version);
    unsigned minor    = dmi_version_minor(version);
    unsigned revision = dmi_version_revision(version);

    unsigned scale = attribute->params.scale;

    if (scale == 1)
        rv = dmi_asprintf(&str, "%u", major);
    else if (scale == 2)
        rv = dmi_asprintf(&str, "%u.%u", major, minor);
    else
        rv = dmi_asprintf(&str, "%u.%u.%u", major, minor, revision);

    if (rv < 0)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_date(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    dmi_unused(attribute);
    dmi_unused(pretty);

    char *str = dmi_date_format(dmi_deref(dmi_date_t, value));

    if (str == nullptr)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_uuid(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    dmi_unused(attribute);
    dmi_unused(pretty);

    int rv = 0;
    char *str = nullptr;

    const dmi_uuid_t *uuid = dmi_cast(uuid, value);

    rv = dmi_asprintf(&str, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                      dmi_ntoh(uuid->time_low),
                      dmi_ntoh(uuid->time_mid),
                      dmi_ntoh(uuid->time_hi_and_version),
                      uuid->clock_seq_hi_and_reserved,
                      uuid->clock_seq_low,
                      uuid->node[0], uuid->node[1], uuid->node[2],
                      uuid->node[3], uuid->node[4], uuid->node[5]);

    if (rv < 0)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_ipv4(dmi_context_t *context, const dmi_binary_t *binary)
{
    const dmi_byte_t *data = binary->data;
    char *str = nullptr;

    if (binary->length != 4)
        return dmi_trace_argument_invalid(context, binary->length, nullptr);

    if (dmi_asprintf(&str, "%u.%u.%u.%u", data[0], data[1], data[2], data[3]) < 0)
        return dmi_trace_out_of_memory(context, nullptr);

    return str;
}

static char *dmi_attribute_format_ipv6(dmi_context_t *context, const dmi_binary_t *binary)
{
    const dmi_byte_t *data = binary->data;

    if (binary->length != 16)
        return dmi_trace_argument_invalid(context, binary->length, nullptr);

    uint16_t groups[8];
    for (size_t i = 0; i < countof(groups); i++)
        groups[i] = (uint16_t)((data[i * 2] << 8) | data[i * 2 + 1]);

    // The longest run of at least two zero groups is compressed, the first
    // one if there are several (RFC 5952, section 4.2)
    size_t zero_start  = countof(groups);
    size_t zero_length = 1;

    for (size_t i = 0; i < countof(groups);) {
        size_t j = i;
        while ((j < countof(groups)) and (groups[j] == 0))
            j++;

        if (j - i > zero_length) {
            zero_start  = i;
            zero_length = j - i;
        }

        i = (j > i) ? j : i + 1;
    }

    // The longest representation is "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"
    size_t size = sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");

    char *str = dmi_alloc(context, size);
    if (str == nullptr)
        return nullptr;

    char       *pos = str;
    const char *end = str + size;

    for (size_t i = 0; i < countof(groups); i++) {
        if (i == zero_start) {
            pos += snprintf(pos, end - pos, "::");
            i += zero_length - 1;
            continue;
        }

        // Separator is already written after the compressed run
        if ((i > 0) and (i != zero_start + zero_length))
            *pos++ = ':';

        pos += snprintf(pos, end - pos, "%x", groups[i]);
    }

    *pos = 0;

    return str;
}

static char *dmi_attribute_format_binary(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty)
{
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

    const dmi_binary_t *binary = dmi_cast(binary, value);

    if (attribute->params.flags & DMI_ATTRIBUTE_FLAG_IP) {
        if (binary->length == 4)
            return dmi_attribute_format_ipv4(context, binary);
        if (binary->length == 16)
            return dmi_attribute_format_ipv6(context, binary);
    }

    size_t length    = binary->length;
    char   separator = 0;

    if (attribute->params.flags & DMI_ATTRIBUTE_FLAG_MAC) {
        separator = ':';

        // MAC address fields may be longer than the address itself
        while ((length > DMI_MAC_ADDRESS_LENGTH) and (binary->data[length - 1] == 0))
            length--;
    } else if (pretty) {
        separator = ' ';
    }

    // Separators take the same space as the string terminator for the last
    // byte
    size_t size = (length > 0) ? length * (separator ? 3 : 2) + (separator ? 0 : 1) : 1;

    char *str = dmi_alloc(context, size);
    if (str == nullptr)
        return nullptr;

    const char *digits = pretty ? "0123456789ABCDEF" : "0123456789abcdef";
    char *pos = str;

    for (size_t i = 0; i < length; i++) {
        if (separator and (i > 0))
            *pos++ = separator;

        *pos++ = digits[binary->data[i] >> 4];
        *pos++ = digits[binary->data[i] & 0x0F];
    }

    *pos = 0;

    return str;
}

static uintmax_t dmi_attribute_read_uint(const void *ptr, size_t size)
{
    // Member width does not have to match `uintmax_t`
    if (size == sizeof(uint8_t))
        return dmi_deref(uint8_t, ptr);
    if (size == sizeof(uint16_t))
        return dmi_deref(uint16_t, ptr);
    if (size == sizeof(uint32_t))
        return dmi_deref(uint32_t, ptr);
    if (size == sizeof(uint64_t))
        return dmi_deref(uint64_t, ptr);

    return 0;
}

static intmax_t dmi_attribute_read_int(const void *ptr, size_t size)
{
    // Selectors are usually enumerations, which may be signed
    if (size == sizeof(int8_t))
        return dmi_deref(int8_t, ptr);
    if (size == sizeof(int16_t))
        return dmi_deref(int16_t, ptr);
    if (size == sizeof(int32_t))
        return dmi_deref(int32_t, ptr);
    if (size == sizeof(int64_t))
        return dmi_deref(int64_t, ptr);

    return 0;
}
