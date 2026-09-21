//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include <opendmi/entity/additional-info-internal.h>

const dmi_entity_spec_t dmi_additional_info_spec =
{
    .code = "additional-info",
    .name = "Additional information",
    .type = DMI_TYPE(ADDITIONAL_INFO),

    .params = {
        .minimum_version = DMI_VERSION(2, 6, 0),
        .minimum_length  = 0x0B,
        .decoded_length  = sizeof(dmi_additional_info_t)
    },

    .attributes = (const dmi_attribute_t[]) {
        DMI_ATTRIBUTE_ARRAY(dmi_additional_info_t, entries, entry_count, STRUCT, {
            .code  = "entries",
            .name  = "Entries",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_additional_info_entry_t, ref_handle, HANDLE, {
                    .code  = "referenced-handle",
                    .name  = "Referenced handle"
                }),
                DMI_ATTRIBUTE(dmi_additional_info_entry_t, ref_offset, INTEGER, {
                    .code  = "referenced-offset",
                    .name  = "Referenced offset",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_additional_info_entry_t, string, STRING, {
                    .code = "string",
                    .name = "String value"
                }),
                DMI_ATTRIBUTE(dmi_additional_info_entry_t, value, BINARY, {
                    .code = "value",
                    .name = "Value"
                }),
                {}
            })
        }),
        {}
    },

    .handlers = {
        .decode  = dmi_additional_info_decode,
        .encode  = dmi_additional_info_encode,
        .cleanup = dmi_additional_info_cleanup
    }
};
