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

static uintmax_t dmi_attribute_read_uint(const void *ptr, size_t size);
static intmax_t dmi_attribute_read_int(const void *ptr, size_t size);

static char *dmi_attribute_format_handle(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_string(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_bool(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_integer(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_decimal(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_size(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_address(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_enum(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_set(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_version(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_date(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

static char *dmi_attribute_format_uuid(
        dmi_context_t         *context,
        const dmi_attribute_t *attribute,
        const void            *value,
        bool                   pretty);

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
    assert(attr != nullptr);
    assert(value != nullptr);

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
    assert(attr != nullptr);
    assert(value != nullptr);

    if (attr->params.unknown) {
        if (memcmp(value, attr->params.unknown, attr->value.size) == 0)
            return true;
    }

    return false;
}

bool dmi_attribute_get_bool(const dmi_attribute_t *attr, const void *value)
{
    dmi_unused(attr);

    return dmi_deref(bool, value) ? true : false;
}

intmax_t dmi_attribute_get_int(const dmi_attribute_t *attr, const void *value)
{
    assert(attr != nullptr);
    assert(value != nullptr);

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
    assert(attr != nullptr);
    assert(value != nullptr);

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
    assert(attr != nullptr);
    assert(info != nullptr);

    uintmax_t rv = dmi_attribute_read_uint(dmi_member_ptr(info, attr->counter, void), attr->counter.size);

    return (rv <= SIZE_MAX) ? (size_t)rv : 0;
}

const dmi_attribute_t *dmi_attribute_resolve(const dmi_attribute_t *attr, const void *info)
{
    assert(attr != nullptr);
    assert(info != nullptr);

    if (attr->type != DMI_ATTRIBUTE_TYPE_VARIANT)
        return attr;

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
    assert(context != nullptr);
    assert(attribute != nullptr);
    assert(value != nullptr);

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

    if (dmi_asprintf(&str, "0x%04" PRIX16, dmi_deref(dmi_handle_t, value)) < 0) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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
    char *result = nullptr;

    if (str != nullptr) {
        result = strdup(str);

        if (result == nullptr) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            return nullptr;
        }
    }

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

    bool flag =  dmi_deref(bool, value) ? true : false;
    const char *str = nullptr;

    if (attribute->params.values) {
        if (pretty)
            str = dmi_name_lookup(attribute->params.values, flag);
        else
            str = dmi_code_lookup(attribute->params.values, flag);
    } else {
        if (pretty)
            str = flag ? "yes" : "no";
        else
            str = flag ? "true" : "false";
    }

    char *result = strdup(str);

    if (result == nullptr) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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
        dmi_error_raise(context, DMI_ERROR_INVALID_ARGUMENT);
        return nullptr;
    }

    if (rv < 0) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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

    if (rv < 0) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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

        static const char *units[] = {
            "bytes",
            "KiB", "MiB", "GiB", "TiB", "PiB",
            "EiB", "ZiB", "YiB", "RiB", "QiB",
            nullptr
        };

        for (i = 0; units[i]; i++) {
            if ((size < 1024) or (size % 1024 != 0))
                break;
            size >>= 10;
        }

        rv = dmi_asprintf(&str, "%" PRIu64 " %s", size, units[i]);
    } else {
        rv = dmi_asprintf(&str, "%" PRIu64, size);
    }

    if (rv < 0) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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

    if (rv < 0) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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
        dmi_error_raise(context, DMI_ERROR_INVALID_ARGUMENT);
        return nullptr;
    }

    if (pretty)
        name = dmi_name_lookup(attribute->params.values, dmi_deref(int, value));
    else
        name = dmi_code_lookup(attribute->params.values, dmi_deref(int, value));

    if (name != nullptr)
        str = strdup(name);
    else if (pretty)
        dmi_asprintf(&str, "<invalid> (0x%x)", dmi_deref(int, value));
    else
        dmi_asprintf(&str, "0x%x", dmi_deref(int, value));

    if (str == nullptr) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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
    if (rv < 0) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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

    if (rv < 0) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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

    if (str == nullptr) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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

    if (rv < 0) {
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
        return nullptr;
    }

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

