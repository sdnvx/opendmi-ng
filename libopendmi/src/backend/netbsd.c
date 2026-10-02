//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef __NetBSD__
#   error "Unsupported OS type"
#endif // !__NetBSD__

#include <sys/param.h>
#include <sys/sysctl.h>

#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include <opendmi/backend/generic.h>
#include <opendmi/backend/netbsd.h>

typedef struct dmi_netbsd_session dmi_netbsd_session_t;

struct dmi_netbsd_session
{
    const char *device;
};

/**
 * @internal
 * @brief Open the backend, creating a session in the state of the context.
 *
 * @param[in] context Context descriptor.
 * @param[in] path    Path to the data source, not used by this backend.
 *
 * @return `true` on success, `false` if memory is exhausted.
 */
static bool dmi_netbsd_open(dmi_context_t *context, const char *path);

/**
 * @internal
 * @brief Read the entry point structure.
 *
 * @details The address of the entry point is taken from the kernel, which
 * gets it from EFI. On x86 the legacy memory area is scanned for it if the
 * kernel does not know it. The device the entry point is read from is
 * remembered in the session, so that the table is read from it as well.
 *
 * @param[in]  context Context descriptor, holding the open session.
 * @param[out] buffer  Buffer receiving the entry point.
 *
 * @error DMI_ERROR_ENTRY_NOT_FOUND Entry point cannot be found
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_netbsd_read_entry(dmi_context_t *context, dmi_buffer_t *buffer);

/**
 * @internal
 * @brief Read the structure table from the device the entry point has been
 * read from.
 *
 * @param[in]  context Context descriptor, holding the open session and the
 *                     decoded entry point.
 * @param[out] buffer  Buffer receiving the structure table.
 *
 * @error DMI_ERROR_FILE_READ_FAILED Table is too large, or its address is out of
 *                            range
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_netbsd_read_table(dmi_context_t *context, dmi_buffer_t *buffer);

/**
 * @internal
 * @brief Close the backend, releasing the session.
 *
 * @param[in] context Context descriptor, holding the open session.
 *
 * @return Always `true`.
 */
static bool dmi_netbsd_close(dmi_context_t *context);

/**
 * @internal
 * @brief Release a session.
 *
 * @param[in] session Session to release.
 */
static void dmi_netbsd_session_free(dmi_netbsd_session_t *session);

/**
 * @internal
 * @brief Get the address of the entry point the kernel has got from EFI.
 *
 * @param[in]  context Context descriptor.
 * @param[out] paddr   Variable to store the address in.
 *
 * @error DMI_ERROR_SYSTEM Kernel cannot be queried for the address
 *
 * @return `true` if the address is found, `false` otherwise.
 */
static bool dmi_netbsd_get_entry_addr(dmi_context_t *context, uint64_t *paddr);

dmi_backend_t dmi_netbsd_backend =
{
    .name       = "NetBSD",
    .open       = dmi_netbsd_open,
    .read_entry = dmi_netbsd_read_entry,
    .read_table = dmi_netbsd_read_table,
    .close      = dmi_netbsd_close
};

static bool dmi_netbsd_open(dmi_context_t *context, const char *path)
{
    dmi_netbsd_session_t *session = nullptr;

    assert(context != nullptr);
    assert(context->state.session == nullptr);

    dmi_unused(path);

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr)
        return false;

    context->state.session = session;

    return true;
}

static bool dmi_netbsd_read_entry(dmi_context_t *context, dmi_buffer_t *buffer)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(buffer != nullptr);

    dmi_netbsd_session_t *session = dmi_cast(session, context->state.session);

    const char *device = DMI_NETBSD_DEV_SMBIOS;
    uint64_t    addr   = 0;
    bool        found  = false;

    found = dmi_netbsd_get_entry_addr(context, &addr);
#   if defined(__i386__) || defined(__x86_64__)
        if (not found) {
            device = DMI_NETBSD_DEV_MEMORY;
            found  = dmi_generic_find_entry_addr(context, DMI_NETBSD_DEV_MEMORY, &addr);
        }
#   endif

    if (not found) {
        dmi_error_raise(context, DMI_ERROR_ENTRY_NOT_FOUND);
        return false;
    }

    // Device the entry point has been read from is the one the table is read
    // from as well
    session->device = device;

    return dmi_file_load(buffer, device, (off_t)addr, DMI_ENTRY_MAX_SIZE);
}

static bool dmi_netbsd_read_table(dmi_context_t *context, dmi_buffer_t *buffer)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(buffer != nullptr);

    dmi_netbsd_session_t *session = dmi_cast(session, context->state.session);

    // Size comes from firmware, which is not trusted to have the library
    // read whatever it likes
    if (context->state.table_area_max_size > DMI_TABLE_MAX_SIZE) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_READ_FAILED, "%s: table of %zu bytes exceeds the limit of %zu bytes",
                           session->device, context->state.table_area_max_size, (size_t)DMI_TABLE_MAX_SIZE);
        return false;
    }

    // Offset of the device is signed
    if (context->state.table_area_addr > INT64_MAX) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_READ_FAILED, "%s: table address 0x%" PRIx64 " is out of range",
                           session->device, context->state.table_area_addr);
        return false;
    }

    return dmi_file_load(buffer, session->device, (off_t)context->state.table_area_addr,
                         context->state.table_area_max_size);
}

static bool dmi_netbsd_close(dmi_context_t *context)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);

    dmi_netbsd_session_free(context->state.session);

    return true;
}

static void dmi_netbsd_session_free(dmi_netbsd_session_t *session)
{
    dmi_free(session);
}

static bool dmi_netbsd_get_entry_addr(dmi_context_t *context, uint64_t *paddr)
{
    void *addr;

    assert(context != nullptr);
    assert(paddr   != nullptr);

    dmi_log_debug(context, "Getting SMBIOS address from EFI...");

    size_t length = sizeof(addr);
    if (sysctlbyname(DMI_NETBSD_SYSCTL_SMBIOS, &addr, &length, nullptr, 0) < 0) {
        if (errno != ENOENT)
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "sysctlbyname() failed: %s", strerror(errno));
        else
            dmi_log_debug(context, "No SMBIOS address found");

        return false;
    }

    *paddr = (uintptr_t)addr;

    return true;
}
