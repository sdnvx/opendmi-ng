//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdlib.h>

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/entity.h>
#include <opendmi/utils.h>
#include <opendmi/backend/dump.h>

typedef struct dmi_dump_session dmi_dump_session_t;

struct dmi_dump_session
{
    dmi_data_t *data;
    size_t data_size;
};

static bool dmi_dump_open(dmi_context_t *context, const char *path);
static bool dmi_dump_check(dmi_context_t *context, const char *path, const dmi_data_t *data, size_t size);
static dmi_data_t *dmi_dump_read_entry(dmi_context_t *context, size_t *plength);
static dmi_data_t *dmi_dump_read_table(dmi_context_t *context, size_t *plength);
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

    if (path == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ARGUMENT, "path");
        return false;
    }

    dmi_dump_session_t *session = dmi_alloc(context, sizeof(dmi_dump_session_t));
    if (session == nullptr)
        return false;

    do {
        session->data_size = 0;
        session->data = dmi_file_get(context, (const char *)path, -1, &session->data_size);
        if (session->data == nullptr)
            break;
        if (not dmi_dump_check(context, path, session->data, session->data_size))
            break;

        success = true;
    } while (false);

    if (not success) {
        dmi_free(session->data);
        dmi_free(session);

        return false;
    }

    context->state.session = session;

    return true;
}

static dmi_data_t *dmi_dump_read_entry(dmi_context_t *context, size_t *plength)
{
    dmi_dump_session_t *session = dmi_cast(session, context->state.session);

    *plength = DMI_ENTRY_MAX_SIZE;

    return session->data;
}

static dmi_data_t *dmi_dump_read_table(dmi_context_t *context, size_t *plength)
{
    dmi_dump_session_t *session = dmi_cast(session, context->state.session);

    *plength = session->data_size - DMI_ENTRY_MAX_SIZE;

    return session->data + DMI_ENTRY_MAX_SIZE;
}

static bool dmi_dump_close(dmi_context_t *context)
{
    dmi_dump_session_t *session = dmi_cast(session, context->state.session);

    dmi_free(session->data);
    dmi_free(session);

    return true;
}

/**
 * @internal
 * @brief Dump file starts with entry point structure padded to its maximum
 * size, and is followed by the structure table.
 */
static bool dmi_dump_check(dmi_context_t *context, const char *path, const dmi_data_t *data, size_t size)
{
    static const char *anchors[] = {
        DMI_ANCHOR_V30,
        DMI_ANCHOR_V21,
        DMI_ANCHOR_LEGACY
    };

    if (size < DMI_ENTRY_MAX_SIZE + sizeof(dmi_header_t)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_DUMP,
                           "%s: File is too small (%zu bytes)", path, size);
        return false;
    }

    for (size_t i = 0; i < countof(anchors); i++) {
        if (memcmp(data, anchors[i], strlen(anchors[i])) == 0)
            return true;
    }

    dmi_error_raise_ex(context, DMI_ERROR_INVALID_DUMP,
                       "%s: No entry point structure found at the beginning of file", path);

    return false;
}
