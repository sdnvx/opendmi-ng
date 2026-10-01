//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_STDLIB_H
#define OPENDMI_COMPAT_STDLIB_H

#pragma once

#include <linux/kernel.h>
#include <linux/slab.h>

// Memory of the library is never used from atomic context, so the callers
// may sleep while it is allocated

static inline void *malloc(size_t size)
{
    return kmalloc(size, GFP_KERNEL);
}

static inline void *calloc(size_t count, size_t size)
{
    return kcalloc(count, size, GFP_KERNEL);
}

static inline void *realloc(void *ptr, size_t size)
{
    // Memory is freed the way glibc does, rather than being replaced with
    // the zero-size pointer krealloc() returns
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }

    return krealloc(ptr, size, GFP_KERNEL);
}

static inline void free(void *ptr)
{
    kfree(ptr);
}

// Numbers are parsed as the C library does: the functions of the kernel
// either do not tell where parsing has stopped (kstrto*()), or do not detect
// overflow (simple_strto*()). On overflow, errno is set to ERANGE and the
// maximum value is returned.

unsigned long long strtoull(const char *str, char **end, int base);

unsigned long strtoul(const char *str, char **end, int base);

#endif // !OPENDMI_COMPAT_STDLIB_H
