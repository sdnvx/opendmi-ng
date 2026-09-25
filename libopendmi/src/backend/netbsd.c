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

static bool dmi_netbsd_open(dmi_context_t *context, const char *path);
static bool dmi_netbsd_read_entry(dmi_context_t *context, dmi_buffer_t *buffer);
static bool dmi_netbsd_read_table(dmi_context_t *context, dmi_buffer_t *buffer);
static bool dmi_netbsd_close(dmi_context_t *context);
static void dmi_netbsd_session_free(dmi_netbsd_session_t *session);

static bool dmi_netbsd_get_entry_addr(dmi_context_t *context, size_t *paddr);

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
    size_t      addr   = 0;
    bool        found  = false;

    found = dmi_netbsd_get_entry_addr(context, &addr);
#   if defined(__i386__) || defined(__x86_64__)
        if (not found) {
            device = DMI_NETBSD_DEV_MEMORY;
            found  = dmi_generic_find_entry_addr(context, DMI_NETBSD_DEV_MEMORY, &addr);
        }
#   endif

    if (not found) {
        dmi_error_raise(context, DMI_ERROR_EPS_NOT_FOUND);
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

static bool dmi_netbsd_get_entry_addr(dmi_context_t *context, size_t *paddr)
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

    *paddr = (size_t)addr;

    return true;
}
