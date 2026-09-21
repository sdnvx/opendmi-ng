//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/indexed-io-internal.h>

const dmi_entity_spec_t dmi_dell_indexed_io_spec =
{
    .type            = DMI_TYPE(DELL_INDEXED_IO),
    .code            = "dell-indexed-io",
    .name            = "Dell indexed IO",
    .description     = (const char *[]){
        "Describes tokens, which are stored in CMOS and accessed through the "
        "indexed I/O ports, and the checksum of the CMOS area.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x0C,
        .decoded_length  = sizeof(dmi_dell_indexed_io_t)
    },

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, index_port, INTEGER, {
            .code   = "index-port",
            .name   = "Index port",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, data_port, INTEGER, {
            .code   = "data-port",
            .name   = "Data port",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, check_type, ENUM, {
            .code   = "check-type",
            .name   = "Checksum type",
            .values = &dmi_dell_check_type_names
        }),
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, check_start, INTEGER, {
            .code   = "check-start",
            .name   = "Checked range start",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, check_end, INTEGER, {
            .code   = "check-end",
            .name   = "Checked range end",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_indexed_io_t, check_index, INTEGER, {
            .code   = "check-index",
            .name   = "Checksum index",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_dell_indexed_io_t, tokens, token_count, STRUCT, {
            .code   = "tokens",
            .name   = "Tokens",
            .attrs  = dmi_dell_indexed_io_token_attrs
        }),
        {}
    }),

    .handlers = {
        .decode  = dmi_dell_indexed_io_decode,
        .cleanup = dmi_dell_indexed_io_cleanup
    }
};
