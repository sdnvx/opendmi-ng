//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_INTTYPES_H
#define OPENDMI_COMPAT_INTTYPES_H

#pragma once

#include <stdint.h>

// Types narrower than int are promoted to it, and 64-bit types are long long
// on all the architectures, see include/asm-generic/int-ll64.h

#define PRId8   "d"
#define PRId16  "d"
#define PRId32  "d"
#define PRId64  "lld"
#define PRIdMAX "lld"

#define PRIi8   "i"
#define PRIi16  "i"
#define PRIi32  "i"
#define PRIi64  "lli"
#define PRIiMAX "lli"

#define PRIu8   "u"
#define PRIu16  "u"
#define PRIu32  "u"
#define PRIu64  "llu"
#define PRIuMAX "llu"

#define PRIx8   "x"
#define PRIx16  "x"
#define PRIx32  "x"
#define PRIx64  "llx"
#define PRIxMAX "llx"

#define PRIX8   "X"
#define PRIX16  "X"
#define PRIX32  "X"
#define PRIX64  "llX"
#define PRIXMAX "llX"

#define PRIuPTR "lu"
#define PRIxPTR "lx"
#define PRIXPTR "lX"

#endif // !OPENDMI_COMPAT_INTTYPES_H
