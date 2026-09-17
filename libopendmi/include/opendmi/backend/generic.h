//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_BACKEND_GENERIC_H
#define OPENDMI_BACKEND_GENERIC_H

#pragma once

#include <opendmi/types.h>

__BEGIN_DECLS

__dmi_api bool dmi_generic_parse_entry_addr(dmi_context_t *context, const char *str, size_t *paddr);

#if defined(__i386__) || defined(__x86_64__)
    __dmi_api bool dmi_generic_find_entry_addr(
            dmi_context_t *context,
            const char    *device,
            size_t        *paddr);

    __dmi_api bool dmi_generic_find_anchor(
            dmi_context_t *context,
            dmi_data_t    *buffer,
            size_t         base_addr,
            size_t         area_size,
            const char    *anchor,
            size_t        *paddr);
#endif

__END_DECLS

#endif // !OPENDMI_BACKEND_GENERIC_H
