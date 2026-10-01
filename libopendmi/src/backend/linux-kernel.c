//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef __KERNEL__
#   error "Linux kernel backend is built into kernel modules only"
#endif // !__KERNEL__

#include <assert.h>

#include <linux/efi.h>

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include <opendmi/backend/generic.h>
#include <opendmi/backend/linux-kernel.h>

static bool dmi_linux_kernel_open(dmi_context_t *context, const char *path);
static bool dmi_linux_kernel_read_entry(dmi_context_t *context, dmi_buffer_t *buffer);
static bool dmi_linux_kernel_read_table(dmi_context_t *context, dmi_buffer_t *buffer);
static bool dmi_linux_kernel_close(dmi_context_t *context);

/**
 * @internal
 * @brief Get the entry point address from the EFI configuration tables.
 *
 * @return `true` if the address is found, `false` otherwise.
 */
static bool dmi_linux_kernel_get_entry_addr(dmi_context_t *context, size_t *paddr);

dmi_backend_t dmi_linux_kernel_backend =
{
    .name       = "Linux kernel",
    .open       = dmi_linux_kernel_open,
    .read_entry = dmi_linux_kernel_read_entry,
    .read_table = dmi_linux_kernel_read_table,
    .close      = dmi_linux_kernel_close
};

static bool dmi_linux_kernel_open(dmi_context_t *context, const char *path)
{
    assert(context != nullptr);

    // Physical memory is mapped directly, so there is neither a device to
    // open nor a session to keep
    dmi_unused(path);

    return true;
}

static bool dmi_linux_kernel_read_entry(dmi_context_t *context, dmi_buffer_t *buffer)
{
    assert(context != nullptr);
    assert(buffer != nullptr);

    size_t addr  = 0;
    bool   found = false;

    found = dmi_linux_kernel_get_entry_addr(context, &addr);
#   if defined(__i386__) || defined(__x86_64__)
        // Legacy firmware has no EFI configuration tables, and the entry
        // point is found in its memory area
        if (not found)
            found = dmi_generic_find_entry_addr(context, nullptr, &addr);
#   endif

    if (not found) {
        dmi_error_raise(context, DMI_ERROR_EPS_NOT_FOUND);
        return false;
    }

    return dmi_memory_load(buffer, nullptr, addr, DMI_ENTRY_MAX_SIZE);
}

static bool dmi_linux_kernel_read_table(dmi_context_t *context, dmi_buffer_t *buffer)
{
    assert(context != nullptr);
    assert(buffer != nullptr);

    uint64_t addr = context->state.table_area_addr;

    // Address of SMBIOS 3.0 table is 64-bit, which does not fit the address
    // space of a 32-bit kernel
    if ((size_t)addr != addr) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_MAP, "Table address 0x%llx is out of range",
                           (unsigned long long)addr);
        return false;
    }

    return dmi_memory_load(buffer, nullptr, (size_t)addr, context->state.table_area_max_size);
}

static bool dmi_linux_kernel_close(dmi_context_t *context)
{
    assert(context != nullptr);

    return true;
}

static bool dmi_linux_kernel_get_entry_addr(dmi_context_t *context, size_t *paddr)
{
    assert(context != nullptr);
    assert(paddr   != nullptr);

#if defined(CONFIG_EFI)
    dmi_log_debug(context, "Getting SMBIOS address from EFI...");

    if (not efi_enabled(EFI_CONFIG_TABLES)) {
        dmi_log_debug(context, "No EFI configuration tables found");
        return false;
    }

    // SMBIOS 3.0 entry point is preferred, the same way the kernel does
    if (efi.smbios3 != EFI_INVALID_TABLE_ADDR) {
        *paddr = efi.smbios3;
    } else if (efi.smbios != EFI_INVALID_TABLE_ADDR) {
        *paddr = efi.smbios;
    } else {
        dmi_log_debug(context, "No SMBIOS address found");
        return false;
    }

    dmi_log_debug(context, "Found SMBIOS address: 0x%zx", *paddr);

    return true;
#else
    dmi_unused(paddr);

    dmi_log_debug(context, "No EFI support in the kernel");

    return false;
#endif // CONFIG_EFI
}
