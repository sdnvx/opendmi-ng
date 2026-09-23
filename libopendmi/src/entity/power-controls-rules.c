//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/lint.h>
#include <opendmi/reader.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/power-controls-internal.h>

/**
 * @internal
 * @brief Fields of the next scheduled power-on, at the offsets the
 * specification gives them, along with the attributes they are decoded into.
 */
static const struct
{
    size_t      offset;
    const char *code;
} dmi_power_controls_bcd_fields[] =
{
    { 0x04, "poweron-month"  },
    { 0x05, "poweron-day"    },
    { 0x06, "poweron-hour"   },
    { 0x07, "poweron-minute" },
    { 0x08, "poweron-second" }
};

//
// Values are decoded into plain numbers, and an invalid pair of digits is
// silently turned into one of them, so the raw data is read instead.
//
void dmi_power_controls_lint_bcd(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_reader_t reader;

    if (not dmi_reader_initialize(&reader, entity))
        return;

    for (size_t i = 0; i < countof(dmi_power_controls_bcd_fields); i++) {
        size_t offset = dmi_power_controls_bcd_fields[i].offset;
        dmi_byte_t value;

        if (not dmi_reader_get_bytes_at(&reader, &value, offset, sizeof(value)))
            break;

        // Fields which are not set hold 0xFF, which is no decimal either
        if (value == 0xFF)
            continue;

        if (((value & 0x0F) <= 9) and (((value >> 4) & 0x0F) <= 9))
            continue;

        dmi_lint_issue(lint, entity, dmi_power_controls_bcd_fields[i].code,
                       dmi_lint_entity_offset(lint, entity) + offset,
                       "value 0x%02X is not a binary-coded decimal", (unsigned)value);
    }
}
