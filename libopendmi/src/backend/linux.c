//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef __linux__
#   error "Unsupported OS type"
#endif // !__linux__

#include <sys/mman.h>

#include <stdio.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include <opendmi/backend/linux.h>

typedef struct dmi_linux_session dmi_linux_session_t;

typedef struct dmi_linux_session
{
    dmi_data_t *entry;
    size_t entry_size;
    dmi_data_t *table;
    size_t table_size;
} dmi_linux_session_t;

static bool dmi_linux_open(dmi_context_t *context, const char *path);
static dmi_data_t *dmi_linux_read_entry(dmi_context_t *context, size_t *plength);
static dmi_data_t *dmi_linux_read_table(dmi_context_t *context, size_t *plength);
static bool dmi_linux_close(dmi_context_t *context);
static void dmi_linux_session_free(dmi_linux_session_t *session);

/**
 * @internal
 * @brief SMBIOS data directory path (SYSFS).
 */
static const char *dmi_linux_sysfs_path = "/sys/firmware/dmi/tables";

/**
 * @internal
 * @brief SMBIOS entry point file name (SYSFS).
 */
static const char *dmi_linux_entry_file = "smbios_entry_point";

/**
 * @brief SMBIOS table file name (SYSFS).
 */
static const char *dmi_linux_table_file = "DMI";

dmi_backend_t dmi_linux_backend =
{
    .name       = "Linux SysFS",
    .open       = dmi_linux_open,
    .read_entry = dmi_linux_read_entry,
    .read_table = dmi_linux_read_table,
    .close      = dmi_linux_close
};

static bool dmi_linux_open(dmi_context_t *context, const char *path)
{
    bool success = false;
    dmi_linux_session_t *session = nullptr;
    char *entry_path = nullptr;
    char *table_path = nullptr;

    assert(context != nullptr);
    assert(context->state.session == nullptr);

    dmi_unused(path);

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr)
        return false;

    do {
        if (asprintf(&entry_path, "%s/%s", dmi_linux_sysfs_path, dmi_linux_entry_file) < 0)
            break;
        if (asprintf(&table_path, "%s/%s", dmi_linux_sysfs_path, dmi_linux_table_file) < 0)
            break;

        session->entry_size = 0;
        session->entry = dmi_file_get(context, entry_path, -1, &session->entry_size);
        if (session->entry == nullptr)
            break;

        session->table_size = 0;
        session->table = dmi_file_get(context, table_path, -1, &session->table_size);
        if (session->table == nullptr)
            break;

        context->state.session = session;
        success = true;
    } while (false);

    dmi_free(entry_path);
    dmi_free(table_path);

    if (not success)
        dmi_linux_session_free(session);

    return success;
}

static dmi_data_t *dmi_linux_read_entry(dmi_context_t *context, size_t *plength)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(plength != nullptr);

    dmi_linux_session_t *session = dmi_cast(session, context->state.session);

    *plength = session->entry_size;

    return session->entry;
}

static dmi_data_t *dmi_linux_read_table(dmi_context_t *context, size_t *plength)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(plength != nullptr);

    dmi_linux_session_t *session = dmi_cast(session, context->state.session);

    *plength = session->table_size;

    return session->table;
}

static bool dmi_linux_close(dmi_context_t *context)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);

    dmi_linux_session_free(context->state.session);

    return true;
}

static void dmi_linux_session_free(dmi_linux_session_t *session)
{
    if (session == nullptr)
        return;

    dmi_free(session->entry);
    dmi_free(session->table);

    dmi_free(session);
}
