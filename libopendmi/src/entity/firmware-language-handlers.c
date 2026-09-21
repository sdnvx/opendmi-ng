//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-language-internal.h>

//
// Languages available are the strings of the structure, so the array points
// at the ones the structure carries rather than at anything read from the
// data.
//
bool dmi_firmware_language_derive(dmi_entity_t *entity)
{
    dmi_firmware_language_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_LANGUAGE));
    if (info == nullptr)
        return false;

    if (info->language_count == 0)
        return true;

    info->languages = dmi_alloc_array(dmi_entity_context(entity),
                                      sizeof(const char *), info->language_count);
    if (info->languages == nullptr)
        return false;

    for (size_t i = 0; i < info->language_count; i++)
        info->languages[i] = dmi_entity_string(entity, (dmi_string_t)(i + 1));

    return true;
}

void dmi_firmware_language_cleanup(dmi_entity_t *entity)
{
    dmi_firmware_language_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_LANGUAGE));
    if (info == nullptr)
        return;

    dmi_free(info->languages);
}
