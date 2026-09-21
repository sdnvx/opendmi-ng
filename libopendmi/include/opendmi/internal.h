//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_INTERNAL_H
#define OPENDMI_INTERNAL_H

#pragma once

/**
 * @file
 * @brief Definitions shared by the library, the command line tool and tests.
 *
 * This header is not installed and is not a part of the public API. Anything
 * which is not required by users of the library belongs here instead of
 * `<opendmi/defs.h>`, so that it is not imposed on them.
 */

#include <iso646.h>

#include <opendmi/defs.h>

// Number of elements in an array
#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif // !countof

// Type-cast macros
#define dmi_cast(dst, expr) ((__dmi_typeof(dst))(expr))
#define dmi_deref(type, expr) (*(const type *)(expr))

// Value pointer macro
#define dmi_value_ptr(x) &(__dmi_typeof(x)){ (x) }

// List of the types a handle may refer to, see dmi_attribute_params_t::targets
#define dmi_targets(...) \
        (const dmi_type_t[]){ __VA_ARGS__, DMI_TYPE_INVALID }

// Look up a string of the library resources, nullptr if there is none
const char *dmi_locale_string(const char *table, const char *key);

// Look up the text of a value which has no text of its own, e.g. an unknown
// enumeration identifier, falling back to the English one
const char *dmi_value_text(const char *key, const char *fallback);

// Cross-platform attribute unused macro
#define dmi_unused(x) (void)(x)

// Cross-compiler thread-local specifier support, thread_local is a
// keyword since C23
#if !defined(thread_local) && !defined(__cplusplus) && (__STDC_VERSION__ < 202311L)
#   if (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_THREADS__)
#       define thread_local _Thread_local
#   elif defined(_WIN32) && (defined(_MSC_VER) || defined(__ICL))
#       define thread_local __declspec(thread)
#   elif defined(__GNUC__) || defined(__clang__)
#       define thread_local __thread
#   else
#       error "Cannot define thread_local"
#   endif
#endif

#endif // !OPENDMI_INTERNAL_H
