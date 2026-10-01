//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/context.h>
#include <opendmi/platform.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "rom-info-internal.h"

bool dmi_hpe_rom_info_derive(dmi_entity_t *entity)
{
    dmi_hpe_rom_info_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_rom_info));
    if (info == nullptr)
        return false;

    const dmi_platform_t *platform = dmi_get_platform(dmi_entity_context(entity));
    unsigned generation = (platform != nullptr) ? platform->generation : 0;

    info->has_redundant_rom_version = info->is_redundant_rom and (generation < DMI_HPE_GEN12);

    // Firmware with no OEM ROM image fills the name with blanks, which the
    // decoded string is trimmed of, so the raw one is checked
    const dmi_data_t *data = dmi_entity_data(entity, DMI_TYPE_ANY);
    const char *filename = (entity->body_length > 0x07)
                         ? dmi_entity_string_ex(entity, data[0x07], true)
                         : nullptr;

    info->has_oem_rom = (filename != nullptr) and (strncmp(filename, "  ", 2) != 0);

    return true;
}
