//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_DEFS_H
#define OPENDMI_DEFS_H

#pragma once

#if __has_include(<sys/cdefs.h>)
#   include <sys/cdefs.h>
#endif

#ifndef __BEGIN_DECLS
#   if defined(__cplusplus)
#       define __BEGIN_DECLS extern "C" {
#       define __END_DECLS   }
#   else
#       define __BEGIN_DECLS
#       define __END_DECLS
#   endif
#endif // __BEGIN_DECLS

#include <stddef.h>
#include <iso646.h>

// Modern features support on MSVC
#ifdef _MSC_VER

// Zero-sized array in struct/union
#pragma warning(disable: 4200)

// Nameless struct/union
#pragma warning(disable: 4201)

#endif // _MSC_VER

// Nullptr emulation before C23
#if !defined(__cplusplus) && (!defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202311L))
#   define nullptr ((void *)0)
#endif

// Type inference, typeof is a keyword since C23
#if !defined(__cplusplus) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
#   define __dmi_typeof typeof
#else
#   define __dmi_typeof __typeof__
#endif

// Function attributes: result of const functions depends only on their
// arguments, and pure functions have no side effects
#if defined(__GNUC__) || defined(__clang__)
#   define __dmi_const __attribute__((const))
#   define __dmi_pure  __attribute__((pure))
#else
#   define __dmi_const
#   define __dmi_pure
#endif

// C2y countof() macro
#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif // !countof

// Type-cast macros
#define dmi_cast(dst, expr) ((__dmi_typeof(dst))(expr))
#define dmi_deref(type, expr) (*(const type *)(expr))

// Value pointer macro
#define dmi_value_ptr(x) &(__dmi_typeof(x)){ (x) }

// Cross-platform attribute unused macro
#define dmi_unused(x) (void)(x)

// Public API symbols export and import. The library itself is built with
// DMI_BUILD defined, and users of static library define DMI_STATIC.
#if defined(_WIN32) || defined(__CYGWIN__)
#   if defined(DMI_BUILD)
#       define __dmi_api __declspec(dllexport)
#   elif defined(DMI_STATIC)
#       define __dmi_api
#   else
#       define __dmi_api __declspec(dllimport)
#   endif
#elif defined(__GNUC__) || defined(__clang__)
#   define __dmi_api __attribute__((visibility("default")))
#else
#   define __dmi_api
#endif

// Cross-compiler packed structures support
#ifdef _MSC_VER
#   define dmi_packed_struct(...) __pragma(pack(push, 1)) struct __VA_ARGS__ __pragma(pack(pop))
#   define dmi_packed_union(...) __pragma(pack(push, 1)) union __VA_ARGS__ __pragma(pack(pop))
#else
#   define dmi_packed_struct(...) struct __attribute__((packed)) __VA_ARGS__
#   define dmi_packed_union(...) union __attribute__((packed)) __VA_ARGS__
#endif

// Cross-compiler thread-local specifier support, thread_local is a keyword
// since C23
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

#define dmi_member_size(__type, __member)   sizeof(((__type *)0)->__member)
#define dmi_member_offset(__type, __member) offsetof(__type, __member)
#define dmi_element_size(__type, __member)  sizeof(*((__type *)0)->__member)

/**
 * @brief Check that value union has the same size as its raw value.
 *
 * Bit field layout of value unions must not depend on the compiler. For
 * example, MinGW uses Microsoft layout by default, which does not merge
 * adjacent bit fields of different types, so all bit fields of a value union
 * must have the same type as its `__value` member.
 */
#ifdef __cplusplus
#   define dmi_static_assert_value_union(__name) \
        static_assert(sizeof(union __name) == sizeof(((union __name *)0)->__value), \
                      #__name " size does not match its value size")
#else
#   define dmi_static_assert_value_union(__name) \
        _Static_assert(sizeof(union __name) == sizeof(((union __name *)0)->__value), \
                       #__name " size does not match its value size")
#endif

/**
 * @brief Size of consecutive members from @p __first to @p __last inclusive.
 */
#define dmi_member_span(__type, __first, __last)          \
        (dmi_member_offset(__type, __last) +              \
         dmi_member_size(__type, __last) -                \
         dmi_member_offset(__type, __first))

typedef struct dmi_member_ref
{
    size_t offset;
    size_t size;
} dmi_member_ref_t;

#define dmi_member(__type, __member)                       \
        ((dmi_member_ref_t){                               \
            .offset = dmi_member_offset(__type, __member), \
            .size   = dmi_member_size(__type, __member)    \
        })
#define dmi_member_array(__type, __member)                 \
        ((dmi_member_ref_t){                               \
            .offset = dmi_member_offset(__type, __member), \
            .size   = dmi_element_size(__type, __member)   \
        })

#define DMI_MEMBER_NULL ((dmi_member_ref_t){ 0, 0 })

#define dmi_member_is_present(__member) ((__member).size != 0)

#define dmi_member_ptr(__object, __member, __type) \
        ((const __type *)(dmi_data(__object) + __member.offset))
#define dmi_member_value(__object, __member, __type) \
        (*dmi_member_ptr(__object, __member, __type))

#endif // !OPENDMI_DEFS_H
