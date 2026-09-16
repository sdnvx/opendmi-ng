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

#include <opendmi/backend/generic.h>
#include <opendmi/backend/freebsd.h>

typedef struct dmi_freebsd_session dmi_freebsd_session_t;

struct dmi_freebsd_session
{
    dmi_data_t *entry;
    size_t entry_size;
    dmi_data_t *table;
    size_t table_size;
};

static bool dmi_freebsd_open(dmi_context_t *context, const char *path);
static dmi_data_t *dmi_freebsd_read_entry(dmi_context_t *context, size_t *plength);
static dmi_data_t *dmi_freebsd_read_table(dmi_context_t *context, size_t *plength);
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

    context->state.session = session;

    return true;
}

static dmi_data_t *dmi_freebsd_read_entry(dmi_context_t *context, size_t *plength)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(plength != nullptr);

    dmi_freebsd_session_t *session = dmi_cast(session, context->state.session);

    if (session->entry == nullptr) {
        size_t addr  = 0;
        bool   found = false;

        found = dmi_freebsd_get_entry_addr(context, &addr);
#       if defined(__i386__) || defined(__x86_64__)
            if (not found)
                found = dmi_generic_find_entry_addr(context, DMI_FREEBSD_DEV_MEMORY, &addr);
#       endif

        if (not found) {
            dmi_error_raise(context, DMI_ERROR_EPS_NOT_FOUND);
            return nullptr;
        }

        session->entry = dmi_memory_get(context, DMI_FREEBSD_DEV_MEMORY, addr, DMI_ENTRY_MAX_SIZE);
        if (session->entry == nullptr)
            return nullptr;

        session->entry_size = DMI_ENTRY_MAX_SIZE;
    }

    *plength = session->entry_size;

    return session->entry;
}

static dmi_data_t *dmi_freebsd_read_table(dmi_context_t *context, size_t *plength)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(plength != nullptr);

    dmi_freebsd_session_t *session = dmi_cast(session, context->state.session);

    if (session->table == nullptr) {
        session->table = dmi_memory_get(context, DMI_FREEBSD_DEV_MEMORY,
                                        context->state.table_area_addr, context->state.table_area_max_size);
        if (session->table == nullptr)
            return nullptr;

        session->table_size = context->state.table_area_max_size;
    }

    *plength = session->table_size;

    return session->table;
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
    if (session == nullptr)
        return;

    dmi_free(session->entry);
    dmi_free(session->table);

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
