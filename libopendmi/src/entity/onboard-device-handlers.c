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
#include <opendmi/utils/codec.h>

#include <opendmi/entity/onboard-device-internal.h>

void dmi_onboard_device_cleanup(dmi_entity_t *entity)
{
    dmi_onboard_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(ONBOARD_DEVICE));
    if (info == nullptr)
        return;

    dmi_free(info->instances);
}
