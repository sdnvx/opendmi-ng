//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ERROR_H
#define OPENDMI_ERROR_H

#pragma once

#include <opendmi/types.h>
#include <opendmi/utils/name.h>

#define DMI_ERROR_MAX_DEPTH 64

/**
 * @brief DMI error codes.
 */
typedef enum dmi_error_code
{
    DMI_ERROR_NONE,                    ///< Success
    DMI_ERROR_INTERNAL,                ///< Internal error
    DMI_ERROR_SYSTEM,                  ///< System error
    DMI_ERROR_OUT_OF_MEMORY,           ///< Out of memory
    DMI_ERROR_ARGUMENT_NULL,           ///< Argument is NULL
    DMI_ERROR_ARGUMENT_INVALID,        ///< Invalid argument
    DMI_ERROR_STATE_INVALID,           ///< Invalid state
    DMI_ERROR_FILE_OPEN_FAILED,        ///< Unable to open file
    DMI_ERROR_FILE_STAT_FAILED,        ///< Unable to stat file
    DMI_ERROR_FILE_READ_FAILED,        ///< Unable to read file
    DMI_ERROR_FILE_WRITE_FAILED,       ///< Unable to write file
    DMI_ERROR_FILE_DUP_FAILED,         ///< Unable to clone file handle
    DMI_ERROR_FILE_MAP_FAILED,         ///< Unable to map file
    DMI_ERROR_ENTRY_NOT_FOUND,         ///< Entry point structure not found
    DMI_ERROR_ENTRY_ANCHOR_UNKNOWN,    ///< Unknown entry point structure anchor
    DMI_ERROR_ENTRY_LENGTH_INVALID,    ///< Invalid entry point structure length
    DMI_ERROR_ENTRY_CHECKSUM_INVALID,  ///< Invalid entry point structure checksum
    DMI_ERROR_ENTITY_LENGTH_INVALID,   ///< Invalid structure length
    DMI_ERROR_ENTITY_TYPE_INVALID,     ///< Invalid structure type
    DMI_ERROR_ENTITY_TRUNCATED,        ///< Truncated structure
    DMI_ERROR_ENTITY_DECODE_FAILED,    ///< Unable to decode structure
    DMI_ERROR_ENTITY_REGISTER_FAILED,  ///< Unable to register structure
    DMI_ERROR_ENTITY_LINK_FAILED,      ///< Unable to link structure
    DMI_ERROR_ENTITY_NOT_FOUND,        ///< Structure not found
    DMI_ERROR_STRING_NOT_FOUND,        ///< String not found
    DMI_ERROR_ENTITY_DUPLICATE,        ///< Duplicate structure
    DMI_ERROR_HANDLE_DUPLICATE,        ///< Duplicate handle
    DMI_ERROR_FIRMWARE_INFO_NOT_FOUND, ///< No platform firmware information structure is present
    DMI_ERROR_MODULE_CONFLICT,         ///< Module has conflicts
    DMI_ERROR_SERVICE_UNAVAILABLE,     ///< Service unavailable
    DMI_ERROR_BACKEND_OPEN_FAILED,     ///< Unable to open backend
    DMI_ERROR_CONTEXT_OPEN_FAILED,     ///< Unable to open context
    DMI_ERROR_DUMP_INVALID,            ///< Invalid SMBIOS dump
    DMI_ERROR_OVERLAY_INVALID,         ///< Invalid additional information entry
    __DMI_ERROR_COUNT
} dmi_error_code_t;

/**
 * @brief Error descriptor.
 */
typedef struct dmi_error
{
    /**
     * @brief Path to source file.
     */
    const char *file;

    /**
     * @brief Function name.
     */
    const char *function;

    /**
     * @brief Line number.
     */
    int line;

    /**
     * @brief Error reason code.
     */
    dmi_error_code_t reason;

    /**
     * @brief Additional error message (optional).
     */
    char *message;
} dmi_error_t;

/**
 * @brief Errors ring queue.
 */
typedef struct dmi_error_queue
{
    dmi_error_t errors[DMI_ERROR_MAX_DEPTH];
    size_t first;
    size_t count;
} dmi_error_queue_t;

/**
 * @brief Raises an error and adds it to the error queue of @p context.
 *
 * The source file, function name, and line number are captured automatically.
 *
 * @param context DMI context.
 * @param reason  Error reason code.
 * @return @c true on success, @c false if @p context is @c nullptr.
 */
#define dmi_error_raise(context, reason) \
        __dmi_error_raise(context, __FILE__, __func__, __LINE__, reason, nullptr)

/**
 * @brief Raises an error with an additional formatted message and adds it to
 * the error queue of @p context.
 *
 * The source file, function name, and line number are captured automatically.
 *
 * @param context  DMI context.
 * @param reason   Error reason code.
 * @param message  printf-style format string for the additional message.
 * @param ...      Format arguments.
 * @return @c true on success, @c false if @p context is @c nullptr or memory
 *         allocation for the message fails. In the latter case the error is
 *         still added to the queue, without a message.
 */
#define dmi_error_raise_ex(context, reason, message, ...) \
        __dmi_error_raise(context, __FILE__, __func__, __LINE__, reason, message, ##__VA_ARGS__)

__BEGIN_DECLS

/**
 * @brief Names of error reason codes.
 *
 * Codes of errors are machine-readable, while the messages are printable
 * descriptions, which are translated to the locale.
 */
extern __dmi_api const dmi_name_set_t dmi_error_names;

/**
 * @brief Returns the human-readable message for an error reason code.
 *
 * @param reason Error reason code.
 * @return A pointer to a static string describing the error.  Returns
 *         @c "Unknown error" if @p reason is out of range or has no message.
 */
__dmi_api const char *dmi_error_message(dmi_error_code_t reason);

/**
 * @internal
 * @brief Adds an error entry to the error queue of @p context.
 *
 * Prefer the @c dmi_error_raise() and @c dmi_error_raise_ex() macros, which
 * fill @p file, @p function, and @p line automatically.
 *
 * @param context  DMI context.
 * @param file     Path to the source file where the error occurred.
 * @param function Name of the function where the error occurred.
 * @param line     Line number where the error occurred.
 * @param reason   Error reason code.
 * @param message  printf-style format string for an additional message, or
 *                 @c nullptr if no additional message is needed.
 * @param ...      Format arguments.
 * @return @c true on success, @c false if @p context is @c nullptr or memory
 *         allocation for the message fails. In the latter case the error is
 *         still added to the queue, without a message.
 */
__dmi_api bool __dmi_error_raise(
        dmi_context_t    *context,
        const char       *file,
        const char       *function,
        unsigned int      line,
        dmi_error_code_t  reason,
        const char       *message,
        ...);

/**
 * @internal
 * @brief @c va_list variant of @c __dmi_error_raise().
 *
 * @param context  DMI context.
 * @param file     Path to the source file where the error occurred.
 * @param function Name of the function where the error occurred.
 * @param line     Line number where the error occurred.
 * @param reason   Error reason code.
 * @param message  printf-style format string for an additional message, or
 *                 @c nullptr if no additional message is needed.
 * @param args     Format arguments.
 * @return @c true on success, @c false if @p context is @c nullptr or memory
 *         allocation for the message fails. In the latter case the error is
 *         still added to the queue, without a message.
 */
__dmi_api bool __dmi_error_vraise(
        dmi_context_t    *context,
        const char       *file,
        const char       *function,
        unsigned int      line,
        dmi_error_code_t  reason,
        const char       *message,
        va_list           args);

/**
 * @brief Returns the oldest error descriptor from the error queue without
 * removing it.
 *
 * @param context DMI context.
 * @return Pointer to the oldest error descriptor, or @c nullptr if @p context is
 *         @c nullptr or the error queue is empty.
 */
__dmi_api dmi_error_t *dmi_error_peek_first(dmi_context_t *context);

/**
 * @brief Returns the newest error descriptor from the error queue without
 * removing it.
 *
 * @param context DMI context.
 * @return Pointer to the newest error descriptor, or @c nullptr if @p context is
 *         @c nullptr or the error queue is empty.
 */
__dmi_api dmi_error_t *dmi_error_peek_last(dmi_context_t *context);

/**
 * @brief Removes and returns the oldest error descriptor from the error queue.
 *
 * Can be called repeatedly until there are no more entries.
 *
 * @param context DMI context.
 * @return Pointer to the oldest error descriptor, or @c nullptr if @p context is
 *         @c nullptr or the error queue is empty.
 *
 * @note The returned pointer is valid only until the error queue is next
 *       modified by @c dmi_error_raise(), @c dmi_error_raise_ex(),
 *       @c dmi_error_get_first(), @c dmi_error_get_last(), or
 *       @c dmi_error_clear(). The descriptor is no longer part of the queue,
 *       so its slot, including the message, is reused by the next error
 *       raised for @p context, and any library function called with
 *       @p context may raise one. Copy whatever is needed before calling
 *       into the library again.
 */
__dmi_api dmi_error_t *dmi_error_get_first(dmi_context_t *context);

/**
 * @brief Removes and returns the newest error descriptor from the error queue.
 *
 * Can be called repeatedly until there are no more entries.
 *
 * @param context DMI context.
 * @return Pointer to the newest error descriptor, or @c nullptr if @p context is
 *         @c nullptr or the error queue is empty.
 *
 * @note The returned pointer is valid only until the error queue is next
 *       modified by @c dmi_error_raise(), @c dmi_error_raise_ex(),
 *       @c dmi_error_get_first(), @c dmi_error_get_last(), or
 *       @c dmi_error_clear(). The descriptor is no longer part of the queue,
 *       so its slot, including the message, is reused by the next error
 *       raised for @p context, and any library function called with
 *       @p context may raise one. Copy whatever is needed before calling
 *       into the library again.
 */
__dmi_api dmi_error_t *dmi_error_get_last(dmi_context_t *context);

/**
 * @brief Removes all error descriptors from the error queue.
 *
 * @param context DMI context.
 */
__dmi_api void dmi_error_clear(dmi_context_t *context);

__END_DECLS

#endif // !OPENDMI_ERROR_H
