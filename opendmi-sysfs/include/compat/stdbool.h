//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_STDBOOL_H
#define OPENDMI_COMPAT_STDBOOL_H

#pragma once

// Boolean type and its values are defined by the kernel itself, and the
// definitions of the C library would clash with them
#include <linux/types.h>
#include <linux/stddef.h>

#endif // !OPENDMI_COMPAT_STDBOOL_H
