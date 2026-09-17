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
#include <opendmi/utils/string.h>

static size_t dmi_error_slot_get(dmi_error_queue_t *queue);
static void dmi_error_slot_clear(dmi_error_queue_t *queue, size_t idx);

static inline size_t dmi_error_queue_last_slot(const dmi_error_queue_t *queue)
{
    return (queue->first + queue->count - 1) % DMI_ERROR_MAX_DEPTH;
}

static const char *dmi_error_messages[__DMI_ERROR_COUNT] =
{
    [DMI_ERROR_NONE]                  = "Success",
    [DMI_ERROR_NULL_ARGUMENT]         = "Argument is NULL",
    [DMI_ERROR_INVALID_ARGUMENT]      = "Invalid argument",
    [DMI_ERROR_INVALID_STATE]         = "Invalid state",
    [DMI_ERROR_FILE_OPEN]             = "Unable to open file",
    [DMI_ERROR_FILE_STAT]             = "Unable to stat file",
    [DMI_ERROR_FILE_READ]             = "Unable to read file",
    [DMI_ERROR_FILE_WRITE]            = "Unable to write file",
    [DMI_ERROR_FILE_DUP]              = "Unable to clone file handle",
    [DMI_ERROR_FILE_MAP]              = "Unable to map file",
    [DMI_ERROR_EPS_NOT_FOUND]         = "Entry point structure not found",
    [DMI_ERROR_UNKNOWN_EPS_ANCHOR]    = "Unknown entry point structure anchor",
    [DMI_ERROR_INVALID_EPS_LENGTH]    = "Invalid entry point structure length",
    [DMI_ERROR_INVALID_EPS_CHECKSUM]  = "Invalid entry point structure checksum",
    [DMI_ERROR_INVALID_ENTITY_ADDR]   = "Invalid structure address",
    [DMI_ERROR_INVALID_ENTITY_LENGTH] = "Invalid structure length",
    [DMI_ERROR_INVALID_ENTITY_TYPE]   = "Invalid structure type",
    [DMI_ERROR_ENTITY_TRUNCATED]      = "Truncated structure",
    [DMI_ERROR_ENTITY_DECODE]         = "Unable to decode structure",
    [DMI_ERROR_ENTITY_REGISTER]       = "Unable to register structure",
    [DMI_ERROR_ENTITY_LINK]           = "Unable to link structure",
    [DMI_ERROR_ENTITY_NOT_FOUND]      = "Structure not found",
    [DMI_ERROR_STRING_NOT_FOUND]      = "String not found",
    [DMI_ERROR_DUPLICATE_ENTRY]       = "Duplicate entry",
    [DMI_ERROR_DUPLICATE_HANDLE]      = "Duplicate handle",
    [DMI_ERROR_NO_MORE_ENTRIES]       = "No more entries",
    [DMI_ERROR_MISSING_FIRMWARE_INFO] = "No platform firmware information structure is present",
    [DMI_ERROR_MODULE_CONFLICT]       = "Module has conflicts",
    [DMI_ERROR_SERVICE_UNAVAILABLE]   = "Service unavailable",
    [DMI_ERROR_OUT_OF_MEMORY]         = "Out of memory",
    [DMI_ERROR_SYSTEM]                = "System error",
    [DMI_ERROR_INTERNAL]              = "Internal error",
    [DMI_ERROR_BACKEND_INIT]          = "Unable to initialize backend",
    [DMI_ERROR_CONTEXT_OPEN]          = "Unable to open context",
    [DMI_ERROR_INVALID_DUMP]          = "Invalid SMBIOS dump"
};

static const dmi_error_t dmi_error_null =
{
    .file     = nullptr,
    .function = nullptr,
    .line     = 0,
    .reason   = DMI_ERROR_NONE,
    .message  = nullptr
};

const char *dmi_error_message(dmi_error_code_t reason)
{
    if ((reason < 0) or (reason >= __DMI_ERROR_COUNT) or (dmi_error_messages[reason] == nullptr))
        return "Unknown error";

    return dmi_error_messages[reason];
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

    if (context == nullptr)
        return false;

    slot = dmi_error_slot_get(&context->error_queue);
    error = &context->error_queue.errors[slot];

    error->file     = file;
    error->function = function;
    error->line     = line;
    error->reason   = reason;

    if (message != nullptr) {
        int rv = dmi_vasprintf(&error->message, message, args);

        if ((rv < 0) or (error->message == nullptr)) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }
    }

    if (error->message != nullptr)
        dmi_log_error(context->logger, "%s: %s", dmi_error_message(error->reason), error->message);
    else
        dmi_log_error(context->logger, "%s", dmi_error_message(error->reason));

    return true;
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
    queue->errors[idx] = dmi_error_null;
}
