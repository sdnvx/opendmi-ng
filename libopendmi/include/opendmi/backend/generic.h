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

/**
 * @brief Parse the physical address of the SMBIOS entry point, as a system
 * tells it in text.
 *
 * The address is read the way `strtoull`(3) reads it with the base of zero,
 * and the whole string must be the address.
 *
 * @param[in]  context Context to raise errors on.
 * @param[in]  str     Address in text.
 * @param[out] paddr   Variable to store the address in.
 *
 * @error DMI_ERROR_ARGUMENT_NULL String or variable is `nullptr`
 * @error DMI_ERROR_ARGUMENT_INVALID String is not an address, or the address does not
 * fit in 64 bits
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_generic_parse_entry_addr(dmi_context_t *context, const char *str, uint64_t *paddr);

#if defined(__i386__) || defined(__x86_64__)

    /**
     * @brief Find the SMBIOS entry point by scanning the memory between
     * `0xF0000` and `0xFFFFF`.
     *
     * The entry points of SMBIOS 3.0, SMBIOS 2.1 and the legacy one are looked
     * for in this order, and only the valid ones are taken.
     *
     * @param[in]  context Context to raise errors on.
     * @param[in]  device  Path to the memory device, or @c nullptr in the
     *                     kernel.
     * @param[out] paddr   Variable to store the address of the entry point in.
     *
     * @return `true` if a valid entry point has been found, `false` otherwise.
     */
    __dmi_api bool dmi_generic_find_entry_addr(
            dmi_context_t *context,
            const char    *device,
            uint64_t      *paddr);

    /**
     * @brief Find a valid entry point of a single anchor in a memory area,
     * on 16-byte boundaries.
     *
     * @param[in]  context   Context to log the progress on.
     * @param[in]  buffer    Memory area.
     * @param[in]  base_addr Physical address of the area, aligned on 16 bytes.
     * @param[in]  area_size Size of the area, a multiple of 16 bytes.
     * @param[in]  anchor    Anchor string, e.g. `DMI_ANCHOR_V30`.
     * @param[out] paddr     Variable to store the address of the entry point
     *                       in.
     *
     * @return `true` if a valid entry point has been found, `false` otherwise.
     */
    __dmi_api bool dmi_generic_find_anchor(
            dmi_context_t *context,
            dmi_data_t    *buffer,
            uint64_t       base_addr,
            size_t         area_size,
            const char    *anchor,
            uint64_t      *paddr);
#endif

__END_DECLS

#endif // !OPENDMI_BACKEND_GENERIC_H
