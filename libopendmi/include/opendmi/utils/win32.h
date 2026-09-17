//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_WIN32_H
#define OPENDMI_UTILS_WIN32_H

#pragma once

#include <windef.h>
#include <ntdef.h>

#include <opendmi/defs.h>

__BEGIN_DECLS

/**
 * @brief Convert a Win32 error code to a human-readable string.
 *
 * Formats the system error message for a Win32 error code using
 * `FormatMessageA`. The returned string is stored in a static buffer and must
 * not be freed by the caller. Subsequent calls overwrite the previous result.
 *
 * @param[in] error_code Win32 error code (e.g., from `GetLastError()`).
 *
 * @return A pointer to a static buffer containing the error message.
 */
__dmi_api const char *dmi_win32err_to_string(DWORD error_code);

/**
 * @brief Convert an NTSTATUS code to a human-readable string.
 *
 * Translates the NTSTATUS value to a Win32 error code via `RtlNtStatusToDosError`
 * and then formats it with `dmi_win32err_to_string`. The returned string is
 * stored in a static buffer and must not be freed by the caller. Subsequent
 * calls overwrite the previous result.
 *
 * @param[in] status NTSTATUS code.
 *
 * @return A pointer to a static buffer containing the error message.
 */
__dmi_api const char *dmi_ntstatus_to_string(NTSTATUS status);

__END_DECLS

#endif // !OPENDMI_UTILS_WIN32_H
