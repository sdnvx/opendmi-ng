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

#include <opendmi/entity/oem-strings-internal.h>

//
// Strings of the structure are its values, so the array points at the ones
// the structure carries rather than at anything read from the data.
//
bool dmi_oem_strings_derive(dmi_entity_t *entity)
{
    dmi_oem_strings_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(OEM_STRINGS));
    if (info == nullptr)
        return false;

    if (info->string_count == 0)
        return true;

    info->strings = dmi_alloc_array(dmi_entity_context(entity),
                                       sizeof(const char *), info->string_count);
    if (info->strings == nullptr)
        return false;

    for (size_t i = 0; i < info->string_count; i++)
        info->strings[i] = dmi_entity_string(entity, (dmi_string_t)(i + 1));

    return true;
}

void dmi_oem_strings_cleanup(dmi_entity_t *entity)
{
    dmi_oem_strings_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(OEM_STRINGS));
    if (info == nullptr)
        return;

    dmi_free(info->strings);
}
