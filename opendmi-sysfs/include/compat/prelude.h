//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_PRELUDE_H
#define OPENDMI_COMPAT_PRELUDE_H

#pragma once

/**
 * @file
 * @brief Kernel headers included into every source before anything else.
 *
 * The library spells logical operators as the macros of `<iso646.h>`, e.g.
 * `and` and `or`, while kernel headers paste these very words into names,
 * e.g. `__lse_atomic_fetch_##op`. Kernel headers are therefore included here,
 * before the macros are defined, so that they are skipped by their include
 * guards once the macros are. Every kernel header the compatibility headers
 * and the kernel-specific parts of the library and the module include must be
 * included here too.
 */

#include <linux/bug.h>
#include <linux/ctype.h>
#include <linux/efi.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/kstrtox.h>
#include <linux/limits.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/sprintf.h>
#include <linux/stdarg.h>
#include <linux/stddef.h>
#include <linux/string.h>
#include <linux/types.h>

#endif // !OPENDMI_COMPAT_PRELUDE_H
