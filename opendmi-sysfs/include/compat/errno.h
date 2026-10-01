//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_ERRNO_H
#define OPENDMI_COMPAT_ERRNO_H

#pragma once

#include <linux/errno.h>

// The kernel reports errors by return values and has no errno. The library
// only sets it on invalid arguments, and nothing in the kernel reads it, so
// one variable shared by all the threads does.
extern int dmi_compat_errno;

#define errno dmi_compat_errno

#ifndef ENOTSUP
#   define ENOTSUP EOPNOTSUPP
#endif

#endif // !OPENDMI_COMPAT_ERRNO_H
