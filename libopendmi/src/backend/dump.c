//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/entity.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/backend/dump.h>

// Files are not accessible from the kernel
#if !defined(__KERNEL__)
#   include <opendmi/utils/file.h>
#endif

/**
 * @internal
 * @brief Maximum size of dump file: the entry point padded to its maximum
 * size, followed by a table of the maximum size.
 */
#define DMI_DUMP_MAX_SIZE (DMI_ENTRY_MAX_SIZE + DMI_TABLE_MAX_SIZE)

typedef struct dmi_dump_session dmi_dump_session_t;

struct dmi_dump_session
{
    dmi_buffer_t *data;
};

/**
 * @internal
 * @brief Open a binary dump file.
 *
 * @details
 * Loads the whole of the file into the session, so that it is not held open
 * while the entry point and the table are read, and checks that it begins
 * with an entry point structure. The session is stored in the state of the
 * context.
 *
 * @param[in] context Context descriptor.
 * @param[in] path    Path to the dump file.
 *
 * @return `true` if the file is loaded, `false` otherwise, with the reason left
 * as the last error in the queue.
 *
 * @error DMI_ERROR_ARGUMENT_INVALID No path is given.
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 * @error DMI_ERROR_FILE_OPEN_FAILED The file cannot be opened or its status cannot be read.
 * @error DMI_ERROR_FILE_READ_FAILED The file cannot be read.
 * @error DMI_ERROR_DUMP_INVALID The file is not a regular file, is too large or
 * too small, or does not begin with an entry point structure.
 */
static bool dmi_dump_open(dmi_context_t *context, const char *path);

/**
 * @internal
 * @brief Load the whole of the dump file, which must be a regular file of sane
 * size: it is read at once, and its size is not to be taken from whatever it is.
 */
static bool dmi_dump_load(dmi_context_t *context, const char *path, dmi_buffer_t *buffer);

/**
 * @internal
 * @brief Check that the data of a dump file looks like a dump.
 *
 * @details
 * Dump file starts with entry point structure padded to its maximum size, and
 * is followed by the structure table, so the data must hold the padded entry
 * point and a structure header at least, and begin with one of the anchors.
 * Only the anchor is checked here: the entry point itself is validated when
 * it is decoded.
 *
 * @param[in] context Context descriptor.
 * @param[in] path    Path to the dump file, used in error messages only.
 * @param[in] data    Data of the dump file.
 * @param[in] size    Size of the data in bytes.
 *
 * @return `true` if the data looks like a dump, `false` otherwise.
 *
 * @error DMI_ERROR_DUMP_INVALID The data is too small, or does not begin with
 * an entry point structure.
 */
static bool dmi_dump_check(dmi_context_t *context, const char *path, const dmi_data_t *data, size_t size);

/**
 * @internal
 * @brief Read the entry point structure of the dump.
 *
 * @details
 * Copies the entry point area, padding included, into the buffer: the length
 * of the entry point is told when it is decoded, not here.
 *
 * @param[in]  context Context descriptor, holding the open session.
 * @param[out] buffer  Buffer receiving the entry point area.
 *
 * @return `true` on success, `false` if memory is exhausted.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 */
static bool dmi_dump_read_entry(dmi_context_t *context, dmi_buffer_t *buffer);

/**
 * @internal
 * @brief Read the structure table of the dump.
 *
 * @details
 * Copies everything after the entry point area into the buffer. The table
 * always follows the entry point in a dump, so the table address given by the
 * entry point is not used, and a warning is logged if it says otherwise.
 *
 * @param[in]  context Context descriptor, holding the open session and the
 *                     decoded entry point.
 * @param[out] buffer  Buffer receiving the structure table.
 *
 * @return `true` on success, `false` if memory is exhausted.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 */
static bool dmi_dump_read_table(dmi_context_t *context, dmi_buffer_t *buffer);

/**
 * @internal
 * @brief Close the dump, releasing the session and the data loaded from it.
 */
static bool dmi_dump_close(dmi_context_t *context);

dmi_backend_t dmi_dump_backend =
{
    .name       = "Binary dump",
    .open       = dmi_dump_open,
    .read_entry = dmi_dump_read_entry,
    .read_table = dmi_dump_read_table,
    .close      = dmi_dump_close
};

static bool dmi_dump_open(dmi_context_t *context, const char *path)
{
    bool success = false;

    if (context == nullptr)
        return false;
    if (path == nullptr)
        return dmi_trace_argument_null(context, path);

    dmi_dump_session_t *session = dmi_alloc(context, sizeof(dmi_dump_session_t));
    if (session == nullptr)
        return false;

    do {
        session->data = dmi_buffer_create(context);
        if (session->data == nullptr)
            break;
        if (not dmi_dump_load(context, path, session->data))
            break;
        if (not dmi_dump_check(context, path, session->data->data, session->data->length))
            break;

        success = true;
    } while (false);

    if (not success) {
        dmi_buffer_destroy(session->data);
        dmi_free(session);

        return false;
    }

    context->state.session = session;

    return true;
}

#if defined(__KERNEL__)
static bool dmi_dump_load(dmi_context_t *context, const char *path, dmi_buffer_t *buffer)
{
    // There is no file to check before loading it, see dmi_file_load()
    dmi_unused(context);

    return dmi_file_load(buffer, path, -1, 0);
}
#else
static bool dmi_dump_load(dmi_context_t *context, const char *path, dmi_buffer_t *buffer)
{
    dmi_file_stat_t st;

#if defined(_WIN32)
    if (_stat(path, &st) != 0) {
#else
    if (stat(path, &st) != 0) {
#endif
        dmi_error_raise_ex(context, DMI_ERROR_FILE_STAT_FAILED, "%s: %s", path, strerror(errno));
        return false;
    }

    if (not S_ISREG(st.st_mode)) {
        dmi_error_raise_ex(context, DMI_ERROR_DUMP_INVALID, "%s: Not a regular file", path);
        return false;
    }
    if (st.st_size > DMI_DUMP_MAX_SIZE) {
        dmi_error_raise_ex(context, DMI_ERROR_DUMP_INVALID,
                           "%s: File is too large (%lld bytes, at most %zu expected)",
                           path, (long long)st.st_size, (size_t)DMI_DUMP_MAX_SIZE);
        return false;
    }

    // Empty file is left for the check of its contents to report, since
    // zero length would mean reading the whole of it. Otherwise no more
    // than its size is read, even if the file grows in the meantime.
    if (st.st_size == 0)
        return true;

    return dmi_file_load(buffer, path, -1, (size_t)st.st_size);
}
#endif // defined(__KERNEL__)

static bool dmi_dump_read_entry(dmi_context_t *context, dmi_buffer_t *buffer)
{
    dmi_dump_session_t *session = dmi_cast(session, context->state.session);

    return dmi_buffer_assign(buffer, session->data->data, DMI_ENTRY_MAX_SIZE);
}

static bool dmi_dump_read_table(dmi_context_t *context, dmi_buffer_t *buffer)
{
    dmi_dump_session_t *session = dmi_cast(session, context->state.session);

    // Table always follows the entry point, but dmidecode reads it at the
    // address specified in the entry point
    if (context->state.table_area_addr != DMI_ENTRY_MAX_SIZE) {
        dmi_log_warning(context, "Unexpected table address in dump entry point: 0x%llX",
                        (unsigned long long)context->state.table_area_addr);
    }

    return dmi_buffer_assign(buffer, session->data->data + DMI_ENTRY_MAX_SIZE,
                             session->data->length - DMI_ENTRY_MAX_SIZE);
}

static bool dmi_dump_close(dmi_context_t *context)
{
    dmi_dump_session_t *session = dmi_cast(session, context->state.session);

    dmi_buffer_destroy(session->data);
    dmi_free(session);

    return true;
}

static bool dmi_dump_check(dmi_context_t *context, const char *path, const dmi_data_t *data, size_t size)
{
    static const char *anchors[] = {
        DMI_ANCHOR_V30,
        DMI_ANCHOR_V21,
        DMI_ANCHOR_LEGACY
    };

    if (size < DMI_ENTRY_MAX_SIZE + sizeof(dmi_header_t)) {
        dmi_error_raise_ex(context, DMI_ERROR_DUMP_INVALID,
                           "%s: File is too small (%zu bytes)", path, size);
        return false;
    }

    for (size_t i = 0; i < countof(anchors); i++) {
        if (memcmp(data, anchors[i], strlen(anchors[i])) == 0)
            return true;
    }

    dmi_error_raise_ex(context, DMI_ERROR_DUMP_INVALID,
                       "%s: No entry point structure found at the beginning of file", path);

    return false;
}
