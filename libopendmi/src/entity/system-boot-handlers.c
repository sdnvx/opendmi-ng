//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system-boot-internal.h>

//
// Vendor- and product-specific codes carry data of their own.
//
bool dmi_system_boot_derive(dmi_entity_t *entity)
{
    dmi_system_boot_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_BOOT));
    if (info == nullptr)
        return false;

    info->has_status_data = (info->status >= __DMI_BOOT_STATUS_VENDOR_SPECIFIC_START);

    return true;
}
