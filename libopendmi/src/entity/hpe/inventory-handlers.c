//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "common-internal.h"
#include "inventory-internal.h"

/**
 * @internal
 * @brief Value of an attribute, which is undefined unless the mask of the
 * defined attributes has it.
 *
 * @param[in] info Decoded structure.
 * @param[in] attr Attribute in question.
 *
 * @return Value of the attribute.
 */
static dmi_hpe_flag_t dmi_hpe_inventory_flag(const dmi_hpe_inventory_t *info, dmi_hpe_inventory_attr_t attr);

bool dmi_hpe_inventory_derive(dmi_entity_t *entity)
{
    dmi_hpe_inventory_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_inventory));
    if (info == nullptr)
        return false;

    info->is_updatable      = dmi_hpe_inventory_flag(info, DMI_HPE_INVENTORY_ATTR_UPDATABLE);
    info->is_reset_required = dmi_hpe_inventory_flag(info, DMI_HPE_INVENTORY_ATTR_RESET);
    info->is_auth_required  = dmi_hpe_inventory_flag(info, DMI_HPE_INVENTORY_ATTR_AUTHENTICATED);
    info->is_in_use         = dmi_hpe_inventory_flag(info, DMI_HPE_INVENTORY_ATTR_IN_USE);
    info->is_uefi_image     = dmi_hpe_inventory_flag(info, DMI_HPE_INVENTORY_ATTR_UEFI_IMAGE);

    return true;
}

static dmi_hpe_flag_t dmi_hpe_inventory_flag(const dmi_hpe_inventory_t *info, dmi_hpe_inventory_attr_t attr)
{
    uint64_t bit = UINT64_C(1) << attr;

    if (not (info->attrs_defined & bit))
        return DMI_HPE_FLAG_UNSPEC;

    return (info->attrs_set & bit) ? DMI_HPE_FLAG_YES : DMI_HPE_FLAG_NO;
}
