//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LOG_H
#define OPENDMI_LOG_H

#pragma once

#include <stdarg.h>
#include <opendmi/types.h>

typedef struct dmi_log dmi_log_t;

/**
 * @brief Logging levels.
 */
typedef enum dmi_log_level
{
    DMI_LOG_INVALID = -1, ///< Invalid
    DMI_LOG_ERROR,        ///< Error
    DMI_LOG_WARNING,      ///< Warning
    DMI_LOG_NOTICE,       ///< Notice
    DMI_LOG_INFO,         ///< Info
    DMI_LOG_DEBUG,        ///< Debug
    __DMI_LOG_LEVEL_COUNT
} dmi_log_level_t;

typedef void dmi_log_handler_fn(
        dmi_log_t       *target,
        dmi_log_level_t  level,
        const char      *format,
        va_list          args);

struct dmi_log
{
    dmi_log_level_t level;
    dmi_log_handler_fn *handler;
};

__BEGIN_DECLS

__dmi_api bool dmi_log_set_level(dmi_log_t *target, dmi_log_level_t level);

__dmi_api bool dmi_log_message(
        dmi_log_t       *target,
        dmi_log_level_t  level,
        const char      *format,
        ...);

__dmi_api const char *dmi_log_level_name(dmi_log_level_t value);
__dmi_api dmi_log_level_t dmi_log_level_find(const char *code);

__END_DECLS

#define dmi_log_error(target, format, ...) \
        dmi_log_message(target, DMI_LOG_ERROR, format, ##__VA_ARGS__)
#define dmi_log_warning(target, format, ...) \
        dmi_log_message(target, DMI_LOG_WARNING, format, ##__VA_ARGS__)
#define dmi_log_notice(target, format, ...) \
        dmi_log_message(target, DMI_LOG_NOTICE, format, ##__VA_ARGS__)
#define dmi_log_info(target, format, ...) \
        dmi_log_message(target, DMI_LOG_INFO, format, ##__VA_ARGS__)
#define dmi_log_debug(target, format, ...) \
        dmi_log_message(target, DMI_LOG_DEBUG, format, ##__VA_ARGS__)

#endif // !OPENDMI_LOG_H
