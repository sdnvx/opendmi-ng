//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/reader.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <limits.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/system-reset-internal.h>

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
    .params = {
        .minimum_version = DMI_VERSION(2, 2, 0),
        .minimum_length  = 0x0D,
        .decoded_length  = sizeof(dmi_system_reset_t)
    },

    .fields = DMI_FIELDS({
        // One byte holding the capabilities of the controller
        DMI_FIELD_BITS(dmi_system_reset_t, is_enabled,       1),
        DMI_FIELD_BITS(dmi_system_reset_t, boot_on_watchdog, 2),
        DMI_FIELD_BITS(dmi_system_reset_t, boot_on_limit,    2),
        DMI_FIELD_BITS(dmi_system_reset_t, has_watchdog,     1),
        DMI_FIELD_PAD(dmi_byte_t),

        DMI_FIELD(dmi_system_reset_t, reset_count,    dmi_word_t),
        DMI_FIELD(dmi_system_reset_t, reset_limit,    dmi_word_t),
        DMI_FIELD(dmi_system_reset_t, timer_interval, dmi_word_t),
        DMI_FIELD(dmi_system_reset_t, timeout,        dmi_word_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
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
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("system-reset.limit", dmi_system_reset_lint_limit, {
            .name              = "Number of the resets fits the limit of them",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
