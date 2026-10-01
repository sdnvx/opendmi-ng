//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_CTYPE_H
#define OPENDMI_COMPAT_CTYPE_H

#pragma once

#include <linux/ctype.h>

// Character classes of the kernel follow Latin-1, so that bytes above 0x7F
// are letters or printable there. The library expects them to be classified
// the way the C library does, which knows nothing but ASCII in single bytes.

#undef isalnum
#undef isalpha
#undef isdigit
#undef islower
#undef isprint
#undef isspace
#undef isupper
#undef isxdigit
#undef tolower
#undef toupper

static inline int dmi_compat_isdigit(int c)
{
    return (c >= '0') && (c <= '9');
}

static inline int dmi_compat_islower(int c)
{
    return (c >= 'a') && (c <= 'z');
}

static inline int dmi_compat_isupper(int c)
{
    return (c >= 'A') && (c <= 'Z');
}

static inline int dmi_compat_isalpha(int c)
{
    return dmi_compat_islower(c) || dmi_compat_isupper(c);
}

static inline int dmi_compat_isalnum(int c)
{
    return dmi_compat_isalpha(c) || dmi_compat_isdigit(c);
}

static inline int dmi_compat_isxdigit(int c)
{
    return dmi_compat_isdigit(c) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'));
}

static inline int dmi_compat_isprint(int c)
{
    return (c >= 0x20) && (c <= 0x7E);
}

static inline int dmi_compat_isspace(int c)
{
    return (c == ' ') || ((c >= '\t') && (c <= '\r'));
}

static inline int dmi_compat_tolower(int c)
{
    return dmi_compat_isupper(c) ? (c - 'A' + 'a') : c;
}

static inline int dmi_compat_toupper(int c)
{
    return dmi_compat_islower(c) ? (c - 'a' + 'A') : c;
}

#define isalnum(c)  dmi_compat_isalnum(c)
#define isalpha(c)  dmi_compat_isalpha(c)
#define isdigit(c)  dmi_compat_isdigit(c)
#define islower(c)  dmi_compat_islower(c)
#define isprint(c)  dmi_compat_isprint(c)
#define isspace(c)  dmi_compat_isspace(c)
#define isupper(c)  dmi_compat_isupper(c)
#define isxdigit(c) dmi_compat_isxdigit(c)
#define tolower(c)  dmi_compat_tolower(c)
#define toupper(c)  dmi_compat_toupper(c)

#endif // !OPENDMI_COMPAT_CTYPE_H
