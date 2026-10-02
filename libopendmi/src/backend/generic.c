//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include <opendmi/backend/generic.h>

#if defined(__i386__) or defined(__x86_64__)

/**
 * @internal
 * @brief Tell whether the data found at an anchor is a valid entry point.
 *
 * @details The check is made the way dmidecode does it: anchor strings occur
 * in firmware code and data as well, so a match of the anchor alone proves
 * nothing.
 *
 * @param[in] data      Data starting with the anchor.
 * @param[in] available Number of bytes available at `data`.
 * @param[in] anchor    Anchor string found at `data`.
 *
 * @return `true` if the entry point is valid, `false` otherwise.
 */
static bool dmi_generic_entry_valid(const dmi_data_t *data, size_t available, const char *anchor);

#endif

bool dmi_generic_parse_entry_addr(dmi_context_t *context, const char *str, uint64_t *paddr)
{
    unsigned long long addr;
    char *ep;

    if (str == nullptr)
        return dmi_trace_argument_null(context, str);
    if (paddr == nullptr)
        return dmi_trace_argument_null(context, paddr);

    errno = 0;
    addr  = strtoull(str, &ep, 0);

    if ((ep == str) or (*ep != 0)) {
        dmi_error_raise_ex(context, DMI_ERROR_ARGUMENT_INVALID, "Invalid SMBIOS address: %s", str);
        return false;
    }
    if ((errno == ERANGE) and (addr == ULLONG_MAX)) {
        dmi_error_raise_ex(context, DMI_ERROR_ARGUMENT_INVALID, "SMBIOS address is out of range: %s", str);
        return false;
    }

    *paddr = (uint64_t)addr;
    dmi_log_debug(context, "Found SMBIOS address: 0x%llx", addr);

    return true;
}

#if defined(__i386__) or defined(__x86_64__)

static bool dmi_generic_entry_valid(const dmi_data_t *data, size_t available, const char *anchor)
{
    size_t length;

    if (strcmp(anchor, DMI_ANCHOR_V30) == 0) {
        length = data[offsetof(dmi_entry_v30_t, length)];

        if ((length < sizeof(dmi_entry_v30_t)) or (length > available))
            return false;

        return dmi_checksum_test(data, length);
    }

    if (strcmp(anchor, DMI_ANCHOR_V21) == 0) {
        const dmi_data_t *ieps = data + offsetof(dmi_entry_v21_t, ieps);

        // Length 0x1E is accepted due to a mistake in SMBIOS 2.1
        length = data[offsetof(dmi_entry_v21_t, length)];

        if ((length < sizeof(dmi_entry_v21_t) - 1) or (length > available))
            return false;
        if (not dmi_checksum_test(data, length))
            return false;

        // Intermediate entry point must be present and valid as well. The
        // one of an entry point of length 0x1E lacks its last byte, and is
        // covered by the checksum of the enclosing entry point instead, see
        // dmi_entry_decode_v21().
        if (memcmp(ieps, DMI_ANCHOR_LEGACY, strlen(DMI_ANCHOR_LEGACY)) != 0)
            return false;
        if (length < sizeof(dmi_entry_v21_t))
            return true;

        return dmi_checksum_test(ieps, sizeof(dmi_entry_legacy_t));
    }

    if (sizeof(dmi_entry_legacy_t) > available)
        return false;

    return dmi_checksum_test(data, sizeof(dmi_entry_legacy_t));
}

bool dmi_generic_find_entry_addr(
        dmi_context_t *context,
        const char    *device,
        uint64_t      *paddr)
{
    const uint64_t base_addr = 0xF0000;
    const size_t area_size = 0x10000;

    bool found = false;

    dmi_log_debug(context, "Running memory scan...");

    dmi_buffer_t *area = dmi_buffer_create(context);
    if (area == nullptr)
        return false;

    if (not dmi_memory_load(area, device, base_addr, area_size)) {
        dmi_buffer_destroy(area);
        return false;
    }

    found =
        dmi_generic_find_anchor(context, area->data, base_addr, area_size, DMI_ANCHOR_V30, paddr) or
        dmi_generic_find_anchor(context, area->data, base_addr, area_size, DMI_ANCHOR_V21, paddr) or
        dmi_generic_find_anchor(context, area->data, base_addr, area_size, DMI_ANCHOR_LEGACY, paddr);

    if (not found)
        dmi_log_debug(context, "No SMBIOS entry point found");

    dmi_buffer_destroy(area);

    return found;
}

bool dmi_generic_find_anchor(
        dmi_context_t *context,
        dmi_data_t    *buffer,
        uint64_t       base_addr,
        size_t         area_size,
        const char    *anchor,
        uint64_t      *paddr)
{
    size_t length;
    size_t offset;

    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);
    if (buffer == nullptr)
        return dmi_trace_argument_null(context, buffer);
    if (base_addr % 16 != 0)
        return dmi_trace_argument_invalid(context, base_addr);
    if ((area_size < 16) or (area_size % 16 != 0))
        return dmi_trace_argument_invalid(context, area_size);
    if (anchor == nullptr)
        return dmi_trace_argument_null(context, anchor);
    if (paddr == nullptr)
        return dmi_trace_argument_null(context, paddr);

    length = strlen(anchor);
    assert(length <= 16);

    dmi_log_debug(context, "Scanning for SMBIOS anchor: '%s'...", anchor);

    for (offset = 0; offset <= area_size - DMI_ENTRY_MAX_SIZE; offset += 16) {
        if (memcmp(buffer + offset, anchor, length) != 0)
            continue;

        // Scanning goes on past a damaged or false entry point
        if (not dmi_generic_entry_valid(buffer + offset, area_size - offset, anchor)) {
            dmi_log_debug(context, "Invalid SMBIOS entry point at 0x%llx, skipping",
                          (unsigned long long)(base_addr + offset));
            continue;
        }

        *paddr = base_addr + offset;
        dmi_log_debug(context, "Found SMBIOS address: 0x%llx", (unsigned long long)*paddr);
        return true;
    }

    dmi_log_debug(context, "No SMBIOS anchor found");

    return false;
}
#endif
