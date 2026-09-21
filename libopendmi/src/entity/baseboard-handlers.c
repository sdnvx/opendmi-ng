//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/baseboard-internal.h>

void dmi_baseboard_cleanup(dmi_entity_t *entity)
{
    dmi_baseboard_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    if (info == nullptr)
        return;

    dmi_free(info->object_handles);
    dmi_free(info->objects);
}
