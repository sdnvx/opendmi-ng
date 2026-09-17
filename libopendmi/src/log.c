//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/log.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/internal.h>

static const dmi_name_set_t dmi_log_level_names =
{
    .code  = "log-levels",
    .names = (const dmi_name_t[]) {
        { .id = DMI_LOG_ERROR,   .code = "error",   .name = "ERROR"   },
        { .id = DMI_LOG_WARNING, .code = "warning", .name = "WARNING" },
        { .id = DMI_LOG_NOTICE,  .code = "notice",  .name = "NOTICE"  },
        { .id = DMI_LOG_INFO,    .code = "info",    .name = "INFO"    },
        { .id = DMI_LOG_DEBUG,   .code = "debug",   .name = "DEBUG"   },
        DMI_NAME_NULL
    }
};


bool dmi_log_set_level(dmi_log_t *target, dmi_log_level_t level)
{
    if (target == nullptr)
        return false;

    target->level = level;

    return true;
}

bool dmi_log_message(dmi_log_t *target, dmi_log_level_t level, const char *format, ...)
{
    va_list args;

    if ((target == nullptr) or (target->handler == nullptr))
        return false;

    if (level > target->level)
        return false;

    va_start(args, format);
    target->handler(target, level, format, args);
    va_end(args);

    return true;
}

const char *dmi_log_level_name(dmi_log_level_t value)
{
    const char *name;

    name = dmi_name_lookup(&dmi_log_level_names, (int)value);
    if (name == nullptr)
        return "UNKNOWN";

    return name;
}

dmi_log_level_t dmi_log_level_find(const char *code)
{
    dmi_log_level_t level;

    if (code == nullptr)
        return DMI_LOG_INVALID;

    level = dmi_code_lookup_rev(&dmi_log_level_names, code);
    if (level < 0)
        return DMI_LOG_INVALID;

    return level;
}
