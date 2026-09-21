//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/stream.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/system-config-internal.h>

const dmi_entity_spec_t dmi_system_config_opts_spec =
{
    .code            = "system-config-options",
    .name            = "System configuration options",
    .description     = (const char *[]){
        "This structure contains information required to configure the "
        "baseboard\'s jumpers and switches.",
        //
        nullptr
    },
    .type            = DMI_TYPE(SYSTEM_CONFIG_OPTIONS),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x05,
        .decoded_length  = sizeof(dmi_system_config_opts_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_system_config_opts_t, option_count, dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_system_config_opts_t, options, option_count, STRING, {
            .code = "options",
            .name = "Options"
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_system_config_opts_derive,
        .cleanup = dmi_system_config_opts_cleanup
    }
};
