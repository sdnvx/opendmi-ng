//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_TRACE_H
#define OPENDMI_TRACE_H

#pragma once

/**
 * @file
 * @brief Raising of the standard errors by the functions of the library.
 *
 * The macros are used by the library, and by the modules built out of its
 * tree, to report the errors the functions of the API report the same way.
 * Each macro raises an error on a context and evaluates to the value the
 * failing function returns, `false` unless another one is given, so that a
 * check reads as a condition and its failure:
 *
 * @code
 * if (path == nullptr)
 *     return dmi_trace_argument_null(context, path);
 *
 * if (type == nullptr)
 *     return dmi_trace_argument_null(context, type, nullptr);
 * @endcode
 *
 * A function returning nothing raises the error with the value of the macro
 * cast to `void`, and returns on its own.
 *
 * The argument is checked to be a valid expression, which is not evaluated,
 * so that its name cannot go stale. Macros are named after the error codes
 * they raise, e.g.
 * `dmi_trace_argument_null()` raises `DMI_ERROR_ARGUMENT_NULL`. The error is
 * raised whether or not it can be recorded, e.g. with no context to record it
 * on, and the value is returned either way.
 */

#include <opendmi/error.h>

/**
 * @internal
 * @brief Value a trace macro evaluates to: the one given, or `false`.
 */
#define __dmi_trace_return(...) __dmi_trace_first(__VA_ARGS__ __VA_OPT__(,) false)

/**
 * @internal
 * @brief First of the arguments.
 */
#define __dmi_trace_first(value, ...) (value)

/**
 * @brief Raise `DMI_ERROR_ARGUMENT_NULL` for an argument, which is named by
 * its expression.
 *
 * @param context Context to raise the error on, which may be `nullptr`.
 * @param arg     Argument, which is `nullptr`.
 * @param ...     Value to evaluate to, `false` if none is given.
 *
 * @return The value given, or `false`.
 */
#define dmi_trace_argument_null(context, arg, ...)                         \
    ((void)sizeof(arg),                                                    \
     dmi_error_raise_ex((context), DMI_ERROR_ARGUMENT_NULL, "%s", #arg), \
     __dmi_trace_return(__VA_ARGS__))

/**
 * @brief Raise `DMI_ERROR_ARGUMENT_INVALID` for an argument, which is named by
 * its expression.
 *
 * @param context Context to raise the error on, which may be `nullptr`.
 * @param arg     Argument, the value of which is invalid.
 * @param ...     Value to evaluate to, `false` if none is given.
 *
 * @return The value given, or `false`.
 */
#define dmi_trace_argument_invalid(context, arg, ...)                         \
    ((void)sizeof(arg),                                                    \
     dmi_error_raise_ex((context), DMI_ERROR_ARGUMENT_INVALID, "%s", #arg), \
     __dmi_trace_return(__VA_ARGS__))

/**
 * @brief Raise `DMI_ERROR_STATE_INVALID` for a context, an entity or another
 * object the function cannot work on in its current state.
 *
 * @param context Context to raise the error on, which may be `nullptr`.
 * @param message Message telling what is wrong with the state, e.g.
 *                `"Context is not open"`, which is not a format string.
 * @param ...     Value to evaluate to, `false` if none is given.
 *
 * @return The value given, or `false`.
 */
#define dmi_trace_state_invalid(context, message, ...)                     \
    (dmi_error_raise_ex((context), DMI_ERROR_STATE_INVALID, "%s", (message)), \
     __dmi_trace_return(__VA_ARGS__))

/**
 * @brief Raise `DMI_ERROR_OUT_OF_MEMORY`, e.g. once an allocation has failed.
 *
 * @param context Context to raise the error on, which may be `nullptr`.
 * @param ...     Value to evaluate to, `false` if none is given.
 *
 * @return The value given, or `false`.
 */
#define dmi_trace_out_of_memory(context, ...)                    \
    (dmi_error_raise((context), DMI_ERROR_OUT_OF_MEMORY),         \
     __dmi_trace_return(__VA_ARGS__))

#endif // !OPENDMI_TRACE_H
