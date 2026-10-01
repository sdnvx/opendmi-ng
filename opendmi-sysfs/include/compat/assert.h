//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMPAT_ASSERT_H
#define OPENDMI_COMPAT_ASSERT_H

#pragma once

#include <linux/bug.h>

/**
 * @internal
 * @brief Check an assumption of the library.
 *
 * @details Broken assumptions are reported once with a stack trace, and the
 * kernel goes on, rather than being stopped.
 */
#define assert(expr) WARN_ON_ONCE(!(expr))

#endif // !OPENDMI_COMPAT_ASSERT_H
