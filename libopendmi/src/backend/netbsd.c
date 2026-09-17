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
    dmi_data_t *entry;
    size_t entry_size;
    dmi_data_t *table;
    size_t table_size;
};

static bool dmi_netbsd_open(dmi_context_t *context, const char *path);
static dmi_data_t *dmi_netbsd_read_entry(dmi_context_t *context, size_t *plength);
static dmi_data_t *dmi_netbsd_read_table(dmi_context_t *context, size_t *plength);
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

static dmi_data_t *dmi_netbsd_read_entry(dmi_context_t *context, size_t *plength)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(plength != nullptr);

    dmi_netbsd_session_t *session = dmi_cast(session, context->state.session);

    if (session->entry == nullptr) {
        const char *device = DMI_NETBSD_DEV_SMBIOS;
        size_t      addr   = 0;
        bool        found  = false;

        found = dmi_netbsd_get_entry_addr(context, &addr);
#       if defined(__i386__) || defined(__x86_64__)
            if (not found) {
                device = DMI_NETBSD_DEV_MEMORY;
                found  = dmi_generic_find_entry_addr(context, DMI_NETBSD_DEV_MEMORY, &addr);
            }
#       endif

        if (not found) {
            dmi_error_raise(context, DMI_ERROR_EPS_NOT_FOUND);
            return nullptr;
        }

        session->device = device;
        session->entry_size = DMI_ENTRY_MAX_SIZE;
        session->entry = dmi_file_get(context, device, addr, &session->entry_size);

        if (session->entry == nullptr)
            return nullptr;
    }

    *plength = session->entry_size;

    return session->entry;
}

static dmi_data_t *dmi_netbsd_read_table(dmi_context_t *context, size_t *plength)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(plength != nullptr);

    dmi_netbsd_session_t *session = dmi_cast(session, context->state.session);

    if (session->table == nullptr) {
        session->table_size = context->state.table_area_max_size;
        session->table = dmi_file_get(context, session->device,
                                      context->state.table_area_addr, &session->table_size);
        if (session->table == nullptr)
            return nullptr;

    }

    *plength = session->table_size;

    return session->table;
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
    if (session == nullptr)
        return;

    dmi_free(session->entry);
    dmi_free(session->table);

    dmi_free(session);
}

static bool dmi_netbsd_get_entry_addr(dmi_context_t *context, size_t *paddr)
{
    void *addr;

    assert(context != nullptr);
    assert(paddr   != nullptr);

    dmi_log_debug(context->logger, "Getting SMBIOS address from EFI...");

    size_t length = sizeof(addr);
    if (sysctlbyname(DMI_NETBSD_SYSCTL_SMBIOS, &addr, &length, nullptr, 0) < 0) {
        if (errno != ENOENT)
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "sysctlbyname() failed: %s", strerror(errno));
        else
            dmi_log_debug(context->logger, "No SMBIOS address found");

        return false;
    }

    *paddr = (size_t)addr;

    return true;
}
