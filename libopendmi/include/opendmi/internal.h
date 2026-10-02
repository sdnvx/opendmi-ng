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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <opendmi/defs.h>
#include <opendmi/trace.h>

#ifndef countof
/**
 * @internal
 * @brief Number of elements in an array.
 */
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif // !countof

/**
 * @internal
 * @brief Cast an expression to the type of a variable.
 */
#define dmi_cast(dst, expr) ((__dmi_typeof(dst))(expr))

/**
 * @internal
 * @brief Read a value of a type through a pointer of another type.
 */
#define dmi_deref(type, expr) (*(const type *)(expr))

/**
 * @internal
 * @brief Pointer to a copy of a value, held by a compound literal.
 */
#define dmi_value_ptr(x) &(__dmi_typeof(x)){ (x) }

/**
 * @internal
 * @brief List of structure types, e.g. the ones a handle may refer to, see
 * `dmi_attribute_params_t::targets`.
 */
#define dmi_types(...) \
        (const dmi_type_t *const[]){ __VA_ARGS__, nullptr }

/**
 * @internal
 * @brief Look up a string of the library resources.
 *
 * @param[in] table Table of the resources to look in.
 * @param[in] key   Key of the string.
 *
 * @return String, or `nullptr` if there is none.
 */
const char *dmi_locale_string(const char *table, const char *key);

/**
 * @internal
 * @brief Look up the text of a value which has no text of its own, e.g. of
 * an unknown enumeration identifier.
 *
 * @param[in] key      Key of the text.
 * @param[in] fallback English text, which is returned if there is no
 *                     translation.
 *
 * @return Text of the value.
 */
const char *dmi_value_text(const char *key, const char *fallback);

/**
 * @internal
 * @brief Copy the text the bytes of a structure spell, e.g. a signature,
 * into a buffer.
 *
 * @param[in]  data   Bytes of the structure.
 * @param[in]  length Number of the bytes.
 * @param[out] buffer Buffer at least one byte longer than the bytes, which
 *                    the text is copied into and terminated.
 * @param[in]  trim   Whether the spaces are left out.
 *
 * @return Text in the buffer, or `nullptr` if the bytes are not all printable
 *         or leave nothing but spaces.
 */
const char *dmi_text_from_bytes(const uint8_t *data, size_t length, char *buffer, bool trim);

/**
 * @internal
 * @brief Mark a variable or a parameter as deliberately unused.
 */
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
