//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <errno.h>
#include <string.h>

int dmi_compat_errno;

char *strtok_r(char *str, const char *delim, char **saveptr)
{
    if (str == NULL)
        str = *saveptr;

    // Leading delimiters are skipped, and nothing is left if there is
    // nothing but them
    str += strspn(str, delim);
    if (*str == '\0') {
        *saveptr = str;
        return NULL;
    }

    char *end = str + strcspn(str, delim);
    if (*end != '\0')
        *end++ = '\0';

    *saveptr = end;

    return str;
}
