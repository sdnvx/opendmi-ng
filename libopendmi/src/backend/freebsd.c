//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef __FreeBSD__
#   error "Unsupported OS type"
#endif // !__FreeBSD__

#include <string.h>
#include <stdlib.h>
#include <kenv.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include <opendmi/backend/generic.h>
#include <opendmi/backend/freebsd.h>

typedef struct dmi_freebsd_session dmi_freebsd_session_t;

struct dmi_freebsd_session
{
    const char *device;
};

static bool dmi_freebsd_open(dmi_context_t *context, const char *path);
static bool dmi_freebsd_read_entry(dmi_context_t *context, dmi_buffer_t *buffer);
static bool dmi_freebsd_read_table(dmi_context_t *context, dmi_buffer_t *buffer);
static bool dmi_freebsd_close(dmi_context_t *context);
static void dmi_freebsd_session_free(dmi_freebsd_session_t *session);

static bool dmi_freebsd_get_entry_addr(dmi_context_t *context, size_t *paddr);

dmi_backend_t dmi_freebsd_backend =
{
    .name       = "FreeBSD",
    .open       = dmi_freebsd_open,
    .read_entry = dmi_freebsd_read_entry,
    .read_table = dmi_freebsd_read_table,
    .close      = dmi_freebsd_close
};

static bool dmi_freebsd_open(dmi_context_t *context, const char *path)
{
    dmi_freebsd_session_t *session = nullptr;

    assert(context != nullptr);
    assert(context->state.session == nullptr);

    dmi_unused(path);

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr)
        return false;

    session->device = DMI_FREEBSD_DEV_MEMORY;

    context->state.session = session;

    return true;
}

static bool dmi_freebsd_read_entry(dmi_context_t *context, dmi_buffer_t *buffer)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(buffer != nullptr);

    dmi_freebsd_session_t *session = dmi_cast(session, context->state.session);

    size_t addr  = 0;
    bool   found = false;

    found = dmi_freebsd_get_entry_addr(context, &addr);
#   if defined(__i386__) || defined(__x86_64__)
        if (not found)
            found = dmi_generic_find_entry_addr(context, session->device, &addr);
#   endif

    if (not found) {
        dmi_error_raise(context, DMI_ERROR_EPS_NOT_FOUND);
        return false;
    }

    return dmi_memory_load(buffer, session->device, addr, DMI_ENTRY_MAX_SIZE);
}

static bool dmi_freebsd_read_table(dmi_context_t *context, dmi_buffer_t *buffer)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(buffer != nullptr);

    dmi_freebsd_session_t *session = dmi_cast(session, context->state.session);

    return dmi_memory_load(buffer, session->device,
                           context->state.table_area_addr, context->state.table_area_max_size);
}

static bool dmi_freebsd_close(dmi_context_t *context)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);

    dmi_freebsd_session_free(context->state.session);

    return true;
}

static void dmi_freebsd_session_free(dmi_freebsd_session_t *session)
{
    dmi_free(session);
}

static bool dmi_freebsd_get_entry_addr(dmi_context_t *context, size_t *paddr)
{
    char str[KENV_MVALLEN + 1];

    assert(context != nullptr);
    assert(paddr   != nullptr);

    dmi_log_debug(context, "Getting SMBIOS address from EFI...");

    if (kenv(KENV_GET, DMI_FREEBSD_KENV_SMBIOS, str, sizeof(str)) < 0) {
        if (errno != ENOENT)
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "kenv() failed: %s", strerror(errno));
        else
            dmi_log_debug(context, "No SMBIOS address found");

        return false;
    }

    return dmi_generic_parse_entry_addr(context, str, paddr);
}
