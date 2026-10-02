//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/string.h>

/**
 * @internal
 * @brief Take a slot of the error queue for a new error.
 *
 * @details If the queue is full, the oldest error is overwritten. The slot is
 * cleared before it is returned.
 *
 * @param[in,out] queue Error queue.
 *
 * @return Index of the slot.
 */
static size_t dmi_error_slot_get(dmi_error_queue_t *queue);

/**
 * @internal
 * @brief Clear a slot of the error queue, freeing its message.
 *
 * @param[in,out] queue Error queue.
 * @param[in]     idx   Index of the slot.
 */
static void dmi_error_slot_clear(dmi_error_queue_t *queue, size_t idx);

/**
 * @internal
 * @brief Get the index of the slot holding the latest error of the queue.
 *
 * @param[in] queue Error queue, which is not empty.
 *
 * @return Index of the slot.
 */
static inline size_t dmi_error_queue_last_slot(const dmi_error_queue_t *queue);

const dmi_name_set_t dmi_error_names =
{
    .code  = "error",
    .name  = "Error reasons",
    .names = DMI_NAMES({
        { DMI_ERROR_NONE,                    "none",                    "Success" },
        { DMI_ERROR_INTERNAL,                "internal",                "Internal error" },
        { DMI_ERROR_SYSTEM,                  "system",                  "System error" },
        { DMI_ERROR_OUT_OF_MEMORY,           "out-of-memory",           "Out of memory" },
        { DMI_ERROR_ARGUMENT_NULL,           "argument-null",           "Argument is NULL" },
        { DMI_ERROR_ARGUMENT_INVALID,        "argument-invalid",        "Invalid argument" },
        { DMI_ERROR_STATE_INVALID,           "state-invalid",           "Invalid state" },
        { DMI_ERROR_FILE_OPEN_FAILED,        "file-open-failed",        "Unable to open file" },
        { DMI_ERROR_FILE_STAT_FAILED,        "file-stat-failed",        "Unable to stat file" },
        { DMI_ERROR_FILE_READ_FAILED,        "file-read-failed",        "Unable to read file" },
        { DMI_ERROR_FILE_WRITE_FAILED,       "file-write-failed",       "Unable to write file" },
        { DMI_ERROR_FILE_DUP_FAILED,         "file-dup-failed",         "Unable to clone file handle" },
        { DMI_ERROR_FILE_MAP_FAILED,         "file-map-failed",         "Unable to map file" },
        { DMI_ERROR_ENTRY_NOT_FOUND,         "entry-not-found",         "Entry point structure not found" },
        { DMI_ERROR_ENTRY_ANCHOR_UNKNOWN,    "entry-anchor-unknown",    "Unknown entry point structure anchor" },
        { DMI_ERROR_ENTRY_LENGTH_INVALID,    "entry-length-invalid",    "Invalid entry point structure length" },
        { DMI_ERROR_ENTRY_CHECKSUM_INVALID,  "entry-checksum-invalid",  "Invalid entry point structure checksum" },
        { DMI_ERROR_ENTITY_LENGTH_INVALID,   "entity-length-invalid",   "Invalid structure length" },
        { DMI_ERROR_ENTITY_TYPE_INVALID,     "entity-type-invalid",     "Invalid structure type" },
        { DMI_ERROR_ENTITY_TRUNCATED,        "entity-truncated",        "Truncated structure" },
        { DMI_ERROR_ENTITY_DECODE_FAILED,    "entity-decode-failed",    "Unable to decode structure" },
        { DMI_ERROR_ENTITY_REGISTER_FAILED,  "entity-register-failed",  "Unable to register structure" },
        { DMI_ERROR_ENTITY_LINK_FAILED,      "entity-link-failed",      "Unable to link structure" },
        { DMI_ERROR_ENTITY_NOT_FOUND,        "entity-not-found",        "Structure not found" },
        { DMI_ERROR_STRING_NOT_FOUND,        "string-not-found",        "String not found" },
        { DMI_ERROR_ENTITY_DUPLICATE,        "entity-duplicate",        "Duplicate structure" },
        { DMI_ERROR_HANDLE_DUPLICATE,        "handle-duplicate",        "Duplicate handle" },
        { DMI_ERROR_FIRMWARE_INFO_NOT_FOUND, "firmware-info-not-found", "No platform firmware information structure is present" },
        { DMI_ERROR_MODULE_CONFLICT,         "module-conflict",         "Module has conflicts" },
        { DMI_ERROR_SERVICE_UNAVAILABLE,     "service-unavailable",     "Service unavailable" },
        { DMI_ERROR_BACKEND_OPEN_FAILED,     "backend-open-failed",     "Unable to open backend" },
        { DMI_ERROR_CONTEXT_OPEN_FAILED,     "context-open-failed",     "Unable to open context" },
        { DMI_ERROR_DUMP_INVALID,            "dump-invalid",            "Invalid SMBIOS dump" },
        { DMI_ERROR_OVERLAY_INVALID,         "overlay-invalid",         "Invalid additional information entry" },
        {}
    })
};

const char *dmi_error_message(dmi_error_code_t reason)
{
    const char *message = dmi_name_lookup(&dmi_error_names, reason);

    if (message == nullptr)
        return dmi_value_text("unknown-error", "Unknown error");

    return message;
}

bool __dmi_error_raise(
        dmi_context_t    *context,
        const char       *file,
        const char       *function,
        unsigned int      line,
        dmi_error_code_t  reason,
        const char       *message,
        ...)
{
    va_list args;
    bool rv;

    va_start(args, message);
    rv = __dmi_error_vraise(context, file, function, line, reason, message, args);
    va_end(args);

    return rv;
}

bool __dmi_error_vraise(
        dmi_context_t    *context,
        const char       *file,
        const char       *function,
        unsigned int      line,
        dmi_error_code_t  reason,
        const char       *message,
        va_list           args)
{
    size_t slot;
    dmi_error_t *error;
    char *text = nullptr;

    if (context == nullptr)
        return false;

    //
    // Format the message before taking a slot: a failure here must neither
    // leave a half-filled slot behind nor raise another error, which would
    // take one more slot and push out an older error. The reason is kept
    // on its own then, which is still worth reporting.
    //
    if (message != nullptr)
        (void)dmi_vasprintf(&text, message, args);

    slot = dmi_error_slot_get(&context->error_queue);
    error = &context->error_queue.errors[slot];

    error->file     = file;
    error->function = function;
    error->line     = line;
    error->reason   = reason;
    error->message  = text;

    if (error->message != nullptr)
        dmi_log_error(context, "%s: %s", dmi_error_message(error->reason), error->message);
    else
        dmi_log_error(context, "%s", dmi_error_message(error->reason));

    return (message == nullptr) or (text != nullptr);
}

dmi_error_t *dmi_error_peek_first(dmi_context_t *context)
{
    if (context == nullptr)
        return nullptr;

    dmi_error_queue_t *queue = &context->error_queue;

    if (queue->count == 0)
        return nullptr;

    return &queue->errors[queue->first];
}

dmi_error_t *dmi_error_peek_last(dmi_context_t *context)
{
    if (context == nullptr)
        return nullptr;

    dmi_error_queue_t *queue = &context->error_queue;

    if (queue->count == 0)
        return nullptr;

    size_t last = dmi_error_queue_last_slot(queue);
    return &queue->errors[last];
}

dmi_error_t *dmi_error_get_first(dmi_context_t *context)
{
    if (context == nullptr)
        return nullptr;

    dmi_error_queue_t *queue = &context->error_queue;

    if (queue->count == 0)
        return nullptr;

    dmi_error_t *error = &queue->errors[queue->first];

    queue->first = (queue->first + 1) % DMI_ERROR_MAX_DEPTH;
    queue->count--;

    return error;
}

dmi_error_t *dmi_error_get_last(dmi_context_t *context)
{
    if (context == nullptr)
        return nullptr;

    dmi_error_queue_t *queue = &context->error_queue;

    if (queue->count == 0)
        return nullptr;

    size_t       last  = dmi_error_queue_last_slot(queue);
    dmi_error_t *error = &queue->errors[last];

    queue->count--;

    return error;
}

void dmi_error_clear(dmi_context_t *context)
{
    if (context == nullptr)
        return;

    dmi_error_queue_t *queue = &context->error_queue;

    for (size_t i = 0; i < DMI_ERROR_MAX_DEPTH; i++) {
        dmi_error_slot_clear(queue, i);
    }

    queue->first = 0;
    queue->count = 0;
}

static size_t dmi_error_slot_get(dmi_error_queue_t *queue)
{
    size_t slot;

    if (queue->count == DMI_ERROR_MAX_DEPTH) {
        // Queue is full, overwrite the oldest error
        slot = queue->first;
        queue->first = (queue->first + 1) % DMI_ERROR_MAX_DEPTH;
    } else {
        queue->count++;
        slot = dmi_error_queue_last_slot(queue);
    }

    dmi_error_slot_clear(queue, slot);

    return slot;
}

static void dmi_error_slot_clear(dmi_error_queue_t *queue, size_t idx)
{
    dmi_free(queue->errors[idx].message);
    queue->errors[idx] = (dmi_error_t){};
}

static inline size_t dmi_error_queue_last_slot(const dmi_error_queue_t *queue)
{
    return (queue->first + queue->count - 1) % DMI_ERROR_MAX_DEPTH;
}
