//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/ami.h>

#include <opendmi/entity/ami/type-221-internal.h>

void dmi_ami_type_221_cleanup(dmi_entity_t *entity)
{
    dmi_ami_type_221_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(AMI_221));
    if (info == nullptr)
        return;

    dmi_free(info->items);
}
