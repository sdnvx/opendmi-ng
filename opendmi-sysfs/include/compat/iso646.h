//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_ISO646_H
#define OPENDMI_COMPAT_ISO646_H

#pragma once

// Kernel headers paste these words into names, so they must not be included
// once the macros are defined, see prelude.h

#define and    &&
#define and_eq &=
#define bitand &
#define bitor  |
#define compl  ~
#define not    !
#define not_eq !=
#define or     ||
#define or_eq  |=
#define xor    ^
#define xor_eq ^=

#endif // !OPENDMI_COMPAT_ISO646_H
