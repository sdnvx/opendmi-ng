//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_STDINT_H
#define OPENDMI_COMPAT_STDINT_H

#pragma once

#include <linux/types.h>
#include <linux/limits.h>

// Widest integer types are 64-bit, the same as in the C library
typedef long long          intmax_t;
typedef unsigned long long uintmax_t;

#ifndef INT8_MAX
#   define INT8_MIN   S8_MIN
#   define INT8_MAX   S8_MAX
#   define INT16_MIN  S16_MIN
#   define INT16_MAX  S16_MAX
#   define INT32_MIN  S32_MIN
#   define INT32_MAX  S32_MAX
#   define INT64_MIN  S64_MIN
#   define INT64_MAX  S64_MAX
#   define UINT8_MAX  U8_MAX
#   define UINT16_MAX U16_MAX
#   define UINT32_MAX U32_MAX
#   define UINT64_MAX U64_MAX
#endif

#define INTMAX_MIN  LLONG_MIN
#define INTMAX_MAX  LLONG_MAX
#define UINTMAX_MAX ULLONG_MAX

#ifndef UINTPTR_MAX
#   define UINTPTR_MAX ULONG_MAX
#endif

#define INT8_C(value)   value
#define INT16_C(value)  value
#define INT32_C(value)  value
#define INT64_C(value)  value ## LL
#define INTMAX_C(value) value ## LL

#define UINT8_C(value)   value
#define UINT16_C(value)  value
#define UINT32_C(value)  value ## U
#define UINT64_C(value)  value ## ULL
#define UINTMAX_C(value) value ## ULL

#endif // !OPENDMI_COMPAT_STDINT_H
