//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <assert.h>

#include <opendmi/anonymize.h>
#include <opendmi/entity.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

#include <opendmi/backend/dump.h>

#include "context-internal.h"
#include "entry-internal.h"
#include "registry-internal.h"

/**
 * @internal
 * @brief Open a context with a backend, reading the entry point and the
 * table, and decoding the structures.
 *
 * @details
 * The context is closed again on failure, so that it is left as it was
 * before.
 */
static bool dmi_open_ex(
        dmi_context_t       *context,
        const dmi_backend_t *backend,
        const char          *device);

/**
 * @internal
 * @brief Read and decode the entry point, if the backend gives access to one.
 *
 * @details
 * Backends which have no access to the entry point leave the context without
 * one at all.
 */
static bool dmi_open_read_entry(dmi_context_t *context, const dmi_backend_t *backend);

/**
 * @internal
 * @brief Read the table, whose size the structures are checked against while
 * the table is scanned.
 */
static bool dmi_open_read_table(dmi_context_t *context, const dmi_backend_t *backend);

/**
 * @internal
 * @brief Scan the table for structures, set up the extensions of the platform,
 * and decode and link the structures.
 *
 * @details
 * Additional information is applied before any structure is decoded,
 * including the firmware information the vendor is told from.
 */
static bool dmi_open_read_structures(dmi_context_t *context);

/**
 * @internal
 * @brief Fixup DMI version number.
 */
static void dmi_version_fixup(dmi_context_t *context);

dmi_context_t *dmi_create(unsigned int flags)
{
    bool success = false;
    dmi_context_t *context = nullptr;

    // Allocate context descriptor
    context = dmi_alloc(nullptr, sizeof(dmi_context_t));
    if (context == nullptr)
        return nullptr;

    context->state.vendor = DMI_VENDOR_OTHER;
    context->flags        = flags;
    context->log_level    = DMI_LOG_DEBUG;

    dmi_vector_initialize(&context->modules, context, nullptr);
    dmi_vector_initialize(&context->state.modules, context, nullptr);

    do {
        // Allocate type map
        context->type_map = dmi_types_create(context);
        if (context->type_map == nullptr)
            break;

        // Initialize type map, which has no modules to conflict yet
        dmi_types_map(context, nullptr, false, context->type_map);

        success = true;
    } while (false);

    if (not success) {
        dmi_free(context->type_map);
        dmi_free(context);

        return nullptr;
    }

    return context;
}

void dmi_set_flags(dmi_context_t *context, unsigned flags)
{
    if (context == nullptr)
        return;

    context->flags = flags;
}

unsigned dmi_get_flags(const dmi_context_t *context)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context, 0);

    return context->flags;
}

bool dmi_open(dmi_context_t *context, const char *device)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);

    return dmi_open_ex(context, dmi_backend, device);
}

bool dmi_load(dmi_context_t *context, const char *path)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);

    if (path == nullptr)
        return dmi_trace_argument_null(context, path);

    dmi_log_info(context, "Loading DMI dump: %s...", path);

    return dmi_open_ex(context, &dmi_dump_backend, path);
}

dmi_log_t *dmi_get_logger(const dmi_context_t *context)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context, nullptr);

    return context->logger;
}

bool dmi_set_log_level(dmi_context_t *context, dmi_log_level_t level)
{
    if ((context == nullptr) or (level == DMI_LOG_INVALID))
        return false;

    context->log_level = level;

    return true;
}

dmi_log_level_t dmi_get_log_level(const dmi_context_t *context)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context, DMI_LOG_INVALID);

    return context->log_level;
}

bool dmi_log(dmi_context_t *context, dmi_log_level_t level, const char *format, ...)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);

    // Context level is checked before the level of the handler
    if (level > context->log_level)
        return false;

    va_list args;
    va_start(args, format);
    bool status = dmi_log_message_va(context->logger, level, format, args);
    va_end(args);

    return status;
}

bool dmi_set_logger(dmi_context_t *context, dmi_log_t *logger)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);

    context->logger = logger;

    return true;
}

dmi_registry_t *dmi_get_registry(dmi_context_t *context)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context, nullptr);

    return context->state.registry;
}

bool dmi_close(dmi_context_t *context)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);

    dmi_registry_destroy(context->state.registry);

    if ((context->state.backend != nullptr) and (context->state.session != nullptr))
        context->state.backend->close(context);

    // Data of the platform is held by the context rather than by the backend,
    // and nothing which refers to it outlives the context being closed
    dmi_buffer_destroy(context->state.entry);
    dmi_buffer_destroy(context->state.table);

    dmi_vector_clear(&context->state.modules);
    dmi_platform_destroy(context->state.platform);

    memset(&context->state, 0, sizeof(context->state));
    context->state.vendor = DMI_VENDOR_OTHER;
    dmi_vector_initialize(&context->state.modules, context, nullptr);

    // Modules enabled for the platform are gone, and so are specifications
    // of the modules enabled explicitly, which apply to its generations only.
    // What is left has been mapped without conflicts before, so the map is
    // updated in place.
    dmi_types_map(context, nullptr, false, context->type_map);

    return true;
}

void dmi_destroy(dmi_context_t *context)
{
    if (context == nullptr)
        return;

    // Modules are dropped before closing, since closing maps the modules
    // enabled explicitly again, and they need not outlive the context
    dmi_vector_clear(&context->modules);

    // Close and free context
    dmi_close(context);
    dmi_error_clear(context);

    dmi_platform_destroy(context->platform);
    dmi_free(context->type_map);
    dmi_free(context);
}

bool dmi_anonymize_context(dmi_context_t *context)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);

    dmi_buffer_t *table = dmi_buffer_create(context);
    if (table == nullptr)
        return false;

    if (not dmi_anonymize(context, table)) {
        dmi_buffer_destroy(table);
        return false;
    }

    // Structures are read anew from the copy, which has the very layout of
    // the table, so the platform and the modules told from it stay the same.
    // The old structures are kept until the new ones are read, so that the
    // context is left as it is on failure
    dmi_buffer_t   *old_table    = context->state.table;
    dmi_registry_t *old_registry = context->state.registry;
    const char     *old_vendor   = context->state.vendor_name;

    context->state.table    = table;
    context->state.registry = dmi_registry_create(context, 0);

    bool success = (context->state.registry != nullptr) and
                   dmi_registry_scan(context->state.registry) and
                   dmi_setup_vendor(context) and
                   dmi_registry_decode(context->state.registry) and
                   (((context->flags & DMI_CONTEXT_FLAG_LINK) == 0) or
                    dmi_registry_link(context->state.registry));

    if (not success) {
        if (context->state.registry != nullptr)
            dmi_registry_destroy(context->state.registry);
        dmi_buffer_destroy(table);

        context->state.table       = old_table;
        context->state.registry    = old_registry;
        context->state.vendor_name = old_vendor;
        return false;
    }

    dmi_registry_destroy(old_registry);
    dmi_buffer_destroy(old_table);

    return true;
}

static bool dmi_open_ex(
        dmi_context_t       *context,
        const dmi_backend_t *backend,
        const char          *device)
{
    assert(context != nullptr);

    if ((context->state.backend != nullptr) or (context->state.session != nullptr))
        return dmi_trace_state_invalid(context, "Context already initialized");

    dmi_log_info(context, "Opening DMI context...");
    dmi_log_info(context, "Using backend: %s", backend->name);

    bool success = false;
    do {
        context->state.backend = backend;

        // Data the backend reads belongs to the context, which holds it for
        // as long as it is open
        context->state.table = dmi_buffer_create(context);
        if (context->state.table == nullptr)
            break;

        if (not context->state.backend->open(context, device)) {
            dmi_error_raise_ex(context, DMI_ERROR_BACKEND_OPEN_FAILED, "%s", backend->name);
            break;
        }

        if (not dmi_open_read_entry(context, backend))
            break;

        dmi_version_fixup(context);
        dmi_log_info(context, "SMBIOS %u.%u.%u present",
                     dmi_version_major(context->state.smbios_version),
                     dmi_version_minor(context->state.smbios_version),
                     dmi_version_revision(context->state.smbios_version));

        if (not dmi_open_read_table(context, backend))
            break;

        if (not dmi_open_read_structures(context))
            break;

        success = true;
    } while (false);

    if (not success) {
        dmi_error_raise(context, DMI_ERROR_CONTEXT_OPEN_FAILED);
        dmi_close(context);
    }

    return success;
}

static bool dmi_open_read_entry(dmi_context_t *context, const dmi_backend_t *backend)
{
    if (backend->read_entry == nullptr)
        return true;

    dmi_log_info(context, "Reading DMI entry point...");

    context->state.entry = dmi_buffer_create(context);
    if (context->state.entry == nullptr)
        return false;

    if (not backend->read_entry(context, context->state.entry))
        return false;

    dmi_log_info(context, "Decoding DMI entry point...");

    return dmi_entry_decode(context, context->state.entry->data, context->state.entry->length);
}

static bool dmi_open_read_table(dmi_context_t *context, const dmi_backend_t *backend)
{
    dmi_log_info(context, "Reading DMI structures...");

    if (not backend->read_table(context, context->state.table))
        return false;

    if (context->state.table->length == 0) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_TRUNCATED, "SMBIOS table area is empty");
        return false;
    }

    return true;
}

static bool dmi_open_read_structures(dmi_context_t *context)
{
    context->state.registry = dmi_registry_create(context, 0);
    if (context->state.registry == nullptr)
        return false;

    if (not dmi_registry_scan(context->state.registry))
        return false;

    if ((context->flags & DMI_CONTEXT_FLAG_OVERLAY) and not dmi_registry_overlay(context->state.registry))
        return false;

    if (not dmi_setup_extensions(context))
        return false;

    if (not dmi_registry_decode(context->state.registry))
        return false;

    if ((context->flags & DMI_CONTEXT_FLAG_LINK) and not dmi_registry_link(context->state.registry))
        return false;

    return true;
}

static void dmi_version_fixup(dmi_context_t *context)
{
    unsigned int major    = dmi_version_major(context->state.smbios_version);
    unsigned int minor    = dmi_version_minor(context->state.smbios_version);
    unsigned int revision = dmi_version_revision(context->state.smbios_version);

    if (major != 2)
        return;

    // Some BIOS report weird SMBIOS version, fix that up
    switch (minor) {
    case 0x1F:
    case 0x21:
        minor = 3;
        break;

    case 0x33:
        minor = 6;
        break;

    default:
        return;
    }

    context->state.smbios_version = dmi_version(major, minor, revision);
}
