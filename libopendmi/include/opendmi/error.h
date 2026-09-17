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

#define DMI_ERROR_MAX_DEPTH 32

/**
 * @brief DMI error codes.
 */
typedef enum dmi_error_code
{
    DMI_ERROR_NONE,                  ///< Success
    DMI_ERROR_NULL_ARGUMENT,         ///< Argument is NULL
    DMI_ERROR_INVALID_ARGUMENT,      ///< Invalid argument
    DMI_ERROR_INVALID_STATE,         ///< Invalid state
    DMI_ERROR_FILE_OPEN,             ///< Unable to open file
    DMI_ERROR_FILE_STAT,             ///< Unable to stat file
    DMI_ERROR_FILE_READ,             ///< Unable to read file
    DMI_ERROR_FILE_WRITE,            ///< Unable to write file
    DMI_ERROR_FILE_DUP,              ///< Unable to clone file handle
    DMI_ERROR_FILE_MAP,              ///< Unable to map file
    DMI_ERROR_EPS_NOT_FOUND,         ///< Entry point structure not found
    DMI_ERROR_UNKNOWN_EPS_ANCHOR,    ///< Unknown entry point structure anchor
    DMI_ERROR_INVALID_EPS_LENGTH,    ///< Invalid entry point structure length
    DMI_ERROR_INVALID_EPS_CHECKSUM,  ///< Invalid entry point structure checksum
    DMI_ERROR_INVALID_ENTITY_ADDR,   ///< Invalid structure address
    DMI_ERROR_INVALID_ENTITY_LENGTH, ///< Invalid structure length
    DMI_ERROR_INVALID_ENTITY_TYPE,   ///< Invalid structure type
    DMI_ERROR_ENTITY_TRUNCATED,      ///< Truncated entity
    DMI_ERROR_ENTITY_DECODE,         ///< Unable to decode structure
    DMI_ERROR_ENTITY_REGISTER,       ///< Unable to register structure
    DMI_ERROR_ENTITY_LINK,           ///< Unable to link structure
    DMI_ERROR_ENTITY_NOT_FOUND,      ///< Structure not found
    DMI_ERROR_STRING_NOT_FOUND,      ///< String not found
    DMI_ERROR_DUPLICATE_ENTRY,       ///< Duplicate entry
    DMI_ERROR_DUPLICATE_HANDLE,      ///< Duplicate handle
    DMI_ERROR_NO_MORE_ENTRIES,       ///< No more entries
    DMI_ERROR_MISSING_FIRMWARE_INFO, ///< No platform firmware information structure is present
    DMI_ERROR_MODULE_CONFLICT,       ///< Extension has conflicts
    DMI_ERROR_OUT_OF_MEMORY,         ///< Out of memory
    DMI_ERROR_SERVICE_UNAVAILABLE,   ///< Service unavailable
    DMI_ERROR_SYSTEM,                ///< System error
    DMI_ERROR_INTERNAL,              ///< Internal error
    DMI_ERROR_BACKEND_INIT,          ///< Unable to open backend
    DMI_ERROR_CONTEXT_OPEN,          ///< Unable to open context
    DMI_ERROR_INVALID_DUMP,          ///< Invalid SMBIOS dump
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
 * @return @c true on success, @c false if @p context is @c nullptr or memory
 *         allocation fails.
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
 *         allocation fails.
 */
#define dmi_error_raise_ex(context, reason, message, ...) \
        __dmi_error_raise(context, __FILE__, __func__, __LINE__, reason, message, ##__VA_ARGS__)

__BEGIN_DECLS

/**
 * @brief Returns the human-readable message for an error reason code.
 *
 * @param reason Error reason code.
 * @return A pointer to a static string describing the error.  Returns
 *         @c "Unknown error" if @p reason is out of range or has no message.
 */
const char *dmi_error_message(dmi_error_code_t reason);

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
 *         allocation for the message fails.
 */
bool __dmi_error_raise(
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
 *         allocation for the message fails.
 */
bool __dmi_error_vraise(
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
dmi_error_t *dmi_error_peek_first(dmi_context_t *context);

/**
 * @brief Returns the newest error descriptor from the error queue without
 * removing it.
 *
 * @param context DMI context.
 * @return Pointer to the newest error descriptor, or @c nullptr if @p context is
 *         @c nullptr or the error queue is empty.
 */
dmi_error_t *dmi_error_peek_last(dmi_context_t *context);

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
 *       @c dmi_error_clear().
 */
dmi_error_t *dmi_error_get_first(dmi_context_t *context);

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
 *       @c dmi_error_clear().
 */
dmi_error_t *dmi_error_get_last(dmi_context_t *context);

/**
 * @brief Removes all error descriptors from the error queue.
 *
 * @param context DMI context.
 */
void dmi_error_clear(dmi_context_t *context);

__END_DECLS

#endif // !OPENDMI_ERROR_H
