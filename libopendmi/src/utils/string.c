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

    assert(strp != nullptr);
    assert(format != nullptr);

    va_start(args, format);
    rv = dmi_vasprintf(strp, format, args);
    va_end(args);

    return rv;
}

int dmi_vasprintf(char **strp, const char *format, va_list args)
{
    int rv;

#if defined(_WIN32)
    // _vscprintf tells you how big the buffer needs to be
    va_list args_copy;
    va_copy(args_copy, args);
    int len = _vscprintf(format, args_copy);
    va_end(args_copy);
    if (len < 0)
        return -1;

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
#endif // !defined(_WIN32)

    return rv;
}

void dmi_string_tolower(char *str)
{
    assert(str != nullptr);

    while (*str != 0) {
        *str = tolower((int)*str);
        str++;
    }
}

void dmi_string_toupper(char *str)
{
    assert(str != nullptr);

    while (*str != 0) {
        *str = toupper((int)*str);
        str++;
    }
}

bool dmi_string_set(dmi_context_t *context, char **pstring, const char *value)
{
    assert(pstring != nullptr);

    char *copy = nullptr;

    if (value != nullptr) {
        copy = strdup(value);
        if (copy == nullptr) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }
    }

    dmi_free(*pstring);
    *pstring = copy;

    return true;
}
