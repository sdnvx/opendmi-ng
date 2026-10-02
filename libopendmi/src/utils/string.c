//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include <string.h>

#include <opendmi/error.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/utils/string.h>

int dmi_asprintf(char **strp, const char *format, ...)
{
    int rv;
    va_list args;

    if (strp == nullptr)
        return dmi_trace_argument_null(nullptr, strp, -1);
    if (format == nullptr) {
        *strp = nullptr;
        return dmi_trace_argument_null(nullptr, format, -1);
    }

    va_start(args, format);
    rv = dmi_vasprintf(strp, format, args);
    va_end(args);

    return rv;
}

int dmi_vasprintf(char **strp, const char *format, va_list args)
{
    int rv;

    if (strp == nullptr)
        return dmi_trace_argument_null(nullptr, strp, -1);
    if (format == nullptr) {
        *strp = nullptr;
        return dmi_trace_argument_null(nullptr, format, -1);
    }

#if defined(_WIN32)
    // _vscprintf tells you how big the buffer needs to be
    va_list args_copy;
    va_copy(args_copy, args);
    int len = _vscprintf(format, args_copy);
    va_end(args_copy);
    if (len < 0) {
        *strp = nullptr;
        return -1;
    }

    size_t size = (size_t)len + 1;
    char *str = malloc(size);
    if (str == nullptr) {
        *strp = nullptr;
        return -1;
    }

    // vsprintf_s is the "secure" version of vsprintf
    rv = vsprintf_s(str, size, format, args);
    if (rv < 0) {
        free(str);
        *strp = nullptr;
        return -1;
    }

    *strp = str;
#else // !defined(_WIN32)
    rv = vasprintf(strp, format, args);

    // glibc leaves the pointer undefined on failure, so do not pass it on
    if (rv < 0)
        *strp = nullptr;
#endif // !defined(_WIN32)

    return rv;
}

void dmi_string_tolower(char *str)
{
    if (str == nullptr) {
        (void)dmi_trace_argument_null(nullptr, str);
        return;
    }

    while (*str != 0) {
        *str = (char)tolower((unsigned char)*str);
        str++;
    }
}

void dmi_string_toupper(char *str)
{
    if (str == nullptr) {
        (void)dmi_trace_argument_null(nullptr, str);
        return;
    }

    while (*str != 0) {
        *str = (char)toupper((unsigned char)*str);
        str++;
    }
}

bool dmi_string_set(dmi_context_t *context, char **pstring, const char *value)
{
    if (pstring == nullptr)
        return dmi_trace_argument_null(context, pstring);

    char *copy = nullptr;

    if (value != nullptr) {
        copy = strdup(value);
        if (copy == nullptr)
            return dmi_trace_out_of_memory(context);
    }

    dmi_free(*pstring);
    *pstring = copy;

    return true;
}

/**
 * @internal
 * @brief Strings which firmware leaves in place of the data it has none of.
 *
 * @details They are valid strings, so they are worth a note rather than an
 * error, but nothing is to be made of them.
 */
static const char *const dmi_string_placeholders[] =
{
    "To Be Filled By O.E.M.",
    "Filled By OEM",
    "Default string",
    "Default",
    "System manufacturer",
    "System Product Name",
    "System Version",
    "System Serial Number",
    "System SKU Number",
    "Chassis Manufacture",
    "Chassis Version",
    "Chassis Serial Number",
    "Not Specified",
    "Not Applicable",
    "Not Available",
    "None",
    "Unknown",
    "N/A",
    "NA",
    "TBD",
    "OEM",
    "0123456789",
    "123456789",
    "XXXXXXXX",
    "Fill By OEM",
    "No Asset Tag",
    "Empty",
    "[Empty]",
    "NULL",
    "INVALID",
    "NO DIMM",
    nullptr
};

bool dmi_string_is_placeholder(const char *text)
{
    if (text == nullptr)
        return dmi_trace_argument_null(nullptr, text);

    for (const char *const *item = dmi_string_placeholders; *item != nullptr; item++) {
        if (strcasecmp(text, *item) == 0)
            return true;
    }

    return false;
}

const char *dmi_text_from_bytes(const uint8_t *data, size_t length, char *buffer, bool trim)
{
    size_t count = 0;

    if (data == nullptr)
        return dmi_trace_argument_null(nullptr, data, nullptr);

    for (size_t i = 0; i < length; i++) {
        unsigned char c = data[i];

        if (not isprint(c))
            return nullptr;
        if (trim and (c == ' '))
            continue;

        buffer[count++] = (char)c;
    }

    buffer[count] = 0;

    return (count > 0) ? buffer : nullptr;
}
