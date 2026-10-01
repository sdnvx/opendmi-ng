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
    char *entry_path;
    char *table_path;
} dmi_linux_session_t;

/**
 * @internal
 * @brief Open the SMBIOS tables exported by the kernel through SysFS.
 *
 * @details The session holds the paths of the entry point and of the table
 * files, which are fixed, so @p path is ignored.
 *
 * @param[in] context Context being opened.
 * @param[in] path    Path to the data, unused.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_linux_open(dmi_context_t *context, const char *path);

/**
 * @internal
 * @brief Read the SMBIOS entry point from its SysFS file.
 *
 * @param[in]  context Context being opened.
 * @param[out] buffer  Buffer to read the entry point into.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_linux_read_entry(dmi_context_t *context, dmi_buffer_t *buffer);

/**
 * @internal
 * @brief Read the SMBIOS structure table from its SysFS file.
 *
 * @param[in]  context Context being opened.
 * @param[out] buffer  Buffer to read the table into.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_linux_read_table(dmi_context_t *context, dmi_buffer_t *buffer);

/**
 * @internal
 * @brief Close the backend and dispose of its session.
 *
 * @param[in] context Context being closed.
 *
 * @return Always `true`.
 */
static bool dmi_linux_close(dmi_context_t *context);

/**
 * @internal
 * @brief Free a session along with the paths it holds.
 *
 * @param[in] session Session to free, or `nullptr`.
 */
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
 * @internal
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

    assert(context != nullptr);
    assert(context->state.session == nullptr);

    dmi_unused(path);

    session = dmi_alloc(context, sizeof(*session));
    if (session == nullptr)
        return false;

    do {
        // Path is left as it was when it cannot be made up, so that the
        // session is disposed of by what it holds
        if (asprintf(&session->entry_path, "%s/%s", dmi_linux_sysfs_path, dmi_linux_entry_file) < 0) {
            session->entry_path = nullptr;
            break;
        }
        if (asprintf(&session->table_path, "%s/%s", dmi_linux_sysfs_path, dmi_linux_table_file) < 0) {
            session->table_path = nullptr;
            break;
        }

        context->state.session = session;
        success = true;
    } while (false);

    if (not success)
        dmi_linux_session_free(session);

    return success;
}

static bool dmi_linux_read_entry(dmi_context_t *context, dmi_buffer_t *buffer)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(buffer != nullptr);

    dmi_linux_session_t *session = dmi_cast(session, context->state.session);

    return dmi_file_load(buffer, session->entry_path, -1, 0);
}

static bool dmi_linux_read_table(dmi_context_t *context, dmi_buffer_t *buffer)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(buffer != nullptr);

    dmi_linux_session_t *session = dmi_cast(session, context->state.session);

    return dmi_file_load(buffer, session->table_path, -1, 0);
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

    dmi_free(session->entry_path);
    dmi_free(session->table_path);

    dmi_free(session);
}
