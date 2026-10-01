//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_STRING_H
#define OPENDMI_COMPAT_STRING_H

#pragma once

#include <linux/string.h>
#include <linux/slab.h>

// Strings are duplicated the same way the C library duplicates them, so that
// they are freed with free()

static inline char *strdup(const char *str)
{
    return kstrdup(str, GFP_KERNEL);
}

static inline char *strndup(const char *str, size_t length)
{
    return kstrndup(str, length, GFP_KERNEL);
}

char *strtok_r(char *str, const char *delim, char **saveptr);

#endif // !OPENDMI_COMPAT_STRING_H
