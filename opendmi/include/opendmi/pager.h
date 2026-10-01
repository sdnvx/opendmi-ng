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
