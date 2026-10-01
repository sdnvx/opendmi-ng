//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_STDIO_H
#define OPENDMI_COMPAT_STDIO_H

#pragma once

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sprintf.h>

#include <stdarg.h>

// Strings are allocated the same way the C library allocates them, so that
// they are freed with free()

static inline __printf(2, 0) int vasprintf(char **strp, const char *format, va_list args)
{
    *strp = kvasprintf(GFP_KERNEL, format, args);

    return (*strp != NULL) ? (int)strlen(*strp) : -1;
}

static inline __printf(2, 3) int asprintf(char **strp, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    int length = vasprintf(strp, format, args);
    va_end(args);

    return length;
}

#endif // !OPENDMI_COMPAT_STDIO_H
