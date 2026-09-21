//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/stream.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <limits.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/system-reset.h>

static bool dmi_system_reset_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_boot_option_names =
{
    .code  = "boot-option",
    .names = (dmi_name_t[]){
        DMI_NAME_RESERVED(DMI_BOOT_OPTION_RESERVED),
        {
            .id   = DMI_BOOT_OPTION_OPERATING_SYSTEM,
            .code = "operating-system",
            .name = "Operating system"
        },
        {
            .id   = DMI_BOOT_OPTION_SYSTEM_UTILS,
            .code = "system-utils",
            .name = "System utilities"
        },
        {
            .id   = DMI_BOOT_OPTION_AVOID_REBOOT,
            .code = "avoid-reboot",
            .name = "Do not reboot"
        },
        DMI_NAME_NULL
    }
};

static void dmi_system_reset_lint_limit(dmi_lint_t *lint, const dmi_entity_t *entity);

static const dmi_lint_rule_t dmi_system_reset_limit_rule =
{
    .code              = "system-reset.limit",
    .name              = "Number of the resets fits the limit of them",
    .severity          = DMI_LINT_SEVERITY_WARNING,
    .producer_severity = DMI_LINT_SEVERITY_ERROR,
    .scope             = DMI_LINT_SCOPE_ENTITY,
    .check             = dmi_system_reset_lint_limit
};

const dmi_entity_spec_t dmi_system_reset_spec =
{
    .code            = "system-reset",
    .name            = "System reset",
    .description     = (const char *[]){
        "This structure describes whether Automatic System Reset functions "
        "are enabled (Status).",
        //
        "If the system has a watchdog timer and the timer is not reset (Timer "
        "Reset) before the Interval elapses, an automatic system reset occurs. "
        "The system re-boots according to the Boot Option. This function may "
        "repeat until the Limit is reached, at which time the system re-boots "
        "according to the Boot Option at Limit.",
        //
        nullptr
    },
    .type            = DMI_TYPE(SYSTEM_RESET),
    .minimum_version = DMI_VERSION(2, 2, 0),
    .minimum_length  = 0x0D,
    .decoded_length  = sizeof(dmi_system_reset_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_system_reset_t, is_enabled, BOOL, {
            .code    = "is-enabled",
            .name    = "Reset enabled"
        }),
        DMI_ATTRIBUTE(dmi_system_reset_t, boot_on_watchdog, ENUM, {
            .code    = "boot-on-watchdog",
            .name    = "Boot option on watchdog",
            .values  = &dmi_boot_option_names
        }),
        DMI_ATTRIBUTE(dmi_system_reset_t, boot_on_limit, ENUM, {
            .code    = "boot-on-limit",
            .name    = "Boot option on reset limit",
            .values  = &dmi_boot_option_names
        }),
        DMI_ATTRIBUTE(dmi_system_reset_t, has_watchdog, BOOL, {
            .code    = "has-watchdog",
            .name    = "Watchdog present"
        }),
        DMI_ATTRIBUTE(dmi_system_reset_t, reset_count, INTEGER, {
            .code    = "reset-count",
            .name    = "Reset count",
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_system_reset_t, reset_limit, INTEGER, {
            .code    = "reset-limit",
            .name    = "Reset limit",
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_system_reset_t, timer_interval, INTEGER, {
            .code    = "timer-interval",
            .name    = "Timer interval",
            .unit    = DMI_UNIT_MINUTE,
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_system_reset_t, timeout, INTEGER, {
            .code    = "timeout",
            .name    = "Timeout",
            .unit    = DMI_UNIT_MINUTE,
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX),
        }),
        DMI_ATTRIBUTE_NULL
    },
    .lint_rules      = (const dmi_lint_rule_t *const[]){
        &dmi_system_reset_limit_rule,
        nullptr
    },

    .handlers = {
        .decode = dmi_system_reset_decode
    }
};

const char *dmi_boot_option_name(dmi_boot_option_t value)
{
    return dmi_name_lookup(&dmi_boot_option_names, (int)value);
}

static bool dmi_system_reset_decode(dmi_entity_t *entity)
{
    dmi_system_reset_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_RESET));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    dmi_system_reset_caps_t capabilities;

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &capabilities.__value) and
        dmi_stream_decode(stream, dmi_word_t, &info->reset_count) and
        dmi_stream_decode(stream, dmi_word_t, &info->reset_limit) and
        dmi_stream_decode(stream, dmi_word_t, &info->timer_interval) and
        dmi_stream_decode(stream, dmi_word_t, &info->timeout);
    if (not status)
        return false;

    info->is_enabled       = capabilities.is_enabled;
    info->boot_on_watchdog = capabilities.boot_on_watchdog;
    info->boot_on_limit    = capabilities.boot_on_limit;
    info->has_watchdog     = capabilities.has_watchdog;

    return true;
}

static void dmi_system_reset_lint_limit(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_system_reset_t *info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_RESET));
    if (info == nullptr)
        return;

    // Both counters hold 0xFFFF when they are unknown
    if ((info->reset_count == USHRT_MAX) or (info->reset_limit == USHRT_MAX))
        return;

    if (info->reset_count <= info->reset_limit)
        return;

    dmi_lint_issue(lint, entity, "reset-count", dmi_lint_entity_offset(lint, entity),
                   "system has been reset %u times, while the limit is %u",
                   info->reset_count, info->reset_limit);
}
