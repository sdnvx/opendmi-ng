//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include <opendmi/backend/generic.h>

bool dmi_generic_parse_entry_addr(dmi_context_t *context, const char *str, size_t *paddr)
{
    unsigned long long addr;
    char *ep;

    assert(str != nullptr);
    assert(paddr != nullptr);

    errno = 0;
    addr  = strtoull(str, &ep, 0);

    if ((ep == str) or (*ep != 0)) {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Invalid SMBIOS address: %s", str);
        return false;
    }
    if (((errno == ERANGE) and (addr == ULLONG_MAX)) or (addr > SIZE_MAX)) {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "SMBIOS address is out of range: %s", str);
        return false;
    }

    *paddr = (size_t)addr;
    dmi_log_debug(context, "Found SMBIOS address: 0x%zx", *paddr);

    return true;
}

#if defined(__i386__) or defined(__x86_64__)
bool dmi_generic_find_entry_addr(
        dmi_context_t *context,
        const char    *device,
        size_t        *paddr)
{
    const size_t base_addr = 0xF0000;
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
        size_t         base_addr,
        size_t         area_size,
        const char    *anchor,
        size_t        *paddr)
{
    size_t length;
    size_t offset;

    assert(context != nullptr);
    assert(buffer != nullptr);
    assert(base_addr % 16 == 0);
    assert((area_size >= 16) and (area_size % 16 == 0));
    assert(anchor != nullptr);
    assert(paddr != nullptr);

    length = strlen(anchor);
    assert(length <= 16);

    dmi_log_debug(context, "Scanning for SMBIOS anchor: '%s'...", anchor);

    for (offset = 0; offset <= area_size - DMI_ENTRY_MAX_SIZE; offset += 16) {
        if (memcmp(buffer + offset, anchor, length) == 0) {
            *paddr = base_addr + offset;
            dmi_log_debug(context, "Found SMBIOS address: 0x%zx", *paddr);
            return true;
        }
    }

    dmi_log_debug(context, "No SMBIOS anchor found");

    return false;
}
#endif
