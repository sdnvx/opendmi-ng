//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_PAGER_H
#define OPENDMI_PAGER_H

#pragma once

#include <stdio.h>

#include <opendmi/types.h>

__BEGIN_DECLS

/**
 * @brief Start the pager and redirect standard output to it.
 *
 * The pager is taken from `PAGER` environment variable. If the variable is
 * not set, it is `less` on POSIX systems, unless it is not installed, and
 * output is not paged on Windows. Empty value disables the pager. Calling the
 * function again once the pager has been started does nothing.
 *
 * @param[in] context Context to raise errors on.
 *
 * @error DMI_ERROR_SYSTEM Pager command is invalid, or the pager cannot be started
 * @error DMI_ERROR_OUT_OF_MEMORY Pager command cannot be expanded
 * @error DMI_ERROR_FILE_DUP_FAILED Standard output cannot be redirected
 *
 * @return `true` on success or if output is not paged, `false` otherwise.
 */
bool dmi_pager_start(dmi_context_t *context);

/**
 * @brief Check if output has failed since the pager has been quit before
 * reading the whole output.
 *
 * Such failure is not an error, since the user has seen as much of the output
 * as needed.
 *
 * @param[in] stream Output stream, which has failed.
 * @param[in] error  Error number of the failure.
 *
 * @return `true` if the pager is active, @p stream is the standard output and
 *         @p error tells that the pipe to the pager is broken, `false`
 *         otherwise.
 */
bool dmi_pager_has_quit(const FILE *stream, int error);

__END_DECLS

#endif // !OPENDMI_PAGER_H
