//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "usb-device-internal.h"

bool dmi_hpe_usb_device_derive(dmi_entity_t *entity)
{
    dmi_hpe_usb_device_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_usb_device));
    if (info == nullptr)
        return false;

    info->capacity = (uint64_t)info->raw_capacity * 1024 * 1024;

    return true;
}
