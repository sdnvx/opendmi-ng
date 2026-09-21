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

//
// Strings of the structure are its values, so the array points at the ones
// the structure carries rather than at anything read from the data.
//
bool dmi_system_config_opts_derive(dmi_entity_t *entity)
{
    dmi_system_config_opts_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_CONFIG_OPTIONS));
    if (info == nullptr)
        return false;

    if (info->option_count == 0)
        return true;

    info->options = dmi_alloc_array(dmi_entity_context(entity),
                                       sizeof(const char *), info->option_count);
    if (info->options == nullptr)
        return false;

    for (size_t i = 0; i < info->option_count; i++)
        info->options[i] = dmi_entity_string(entity, (dmi_string_t)(i + 1));

    return true;
}

void dmi_system_config_opts_cleanup(dmi_entity_t *entity)
{
    dmi_system_config_opts_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_CONFIG_OPTIONS));
    if (info == nullptr)
        return;

    dmi_free(info->options);
}
