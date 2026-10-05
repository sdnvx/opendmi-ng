//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTRY_INTERNAL_H
#define OPENDMI_ENTRY_INTERNAL_H

#pragma once

#include <opendmi/entry.h>

__BEGIN_DECLS

/**
 * @internal
 * @brief Decode DMI entry point and initialize related context properties.
 *
 * @param[in] context Context handle to initialize.
 * @param[in] data Pointer to entry point data.
 * @param[in] length Entry point data length.
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
bool dmi_entry_decode(dmi_context_t *context, const void *data, size_t length);

__END_DECLS

#endif // !OPENDMI_ENTRY_INTERNAL_H
