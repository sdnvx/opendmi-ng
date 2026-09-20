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

#include <opendmi/entity/system-config.h>

static bool dmi_system_config_opts_decode(dmi_entity_t *entity);
static void dmi_system_config_opts_cleanup(dmi_entity_t *entity);

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
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x05,
    .decoded_length  = sizeof(dmi_system_config_opts_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE_ARRAY(dmi_system_config_opts_t, options, option_count, STRING, {
            .code = "options",
            .name = "Options"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_system_config_opts_decode,
        .cleanup = dmi_system_config_opts_cleanup
    }
};

static bool dmi_system_config_opts_decode(dmi_entity_t *entity)
{
    dmi_system_config_opts_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_CONFIG_OPTIONS));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    if (not dmi_stream_decode(stream, dmi_byte_t, &info->option_count))
        return false;

    info->options = dmi_alloc_array(context, sizeof(const char *), info->option_count);
    if (info->options == nullptr)
        return false;

    for (size_t i = 0; i < info->option_count; i++) {
        info->options[i] = dmi_entity_string(entity, i + 1);
    }

    return true;
}

static void dmi_system_config_opts_cleanup(dmi_entity_t *entity)
{
    dmi_system_config_opts_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_CONFIG_OPTIONS));
    if (info == nullptr)
        return;

    dmi_free(info->options);
}
