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
#include "dimm-attrs-internal.h"

/**
 * @internal
 * @brief Value of a flag held as a bit telling whether it is defined and a
 * bit telling its value.
 *
 * @param[in] attributes Attributes of the module.
 * @param[in] bit        Bit telling whether the flag is defined, which the
 *                       bit of its value follows.
 *
 * @return Value of the flag.
 */
static dmi_hpe_flag_t dmi_hpe_dimm_attrs_flag(uint32_t attributes, unsigned bit);

bool dmi_hpe_dimm_attrs_derive(dmi_entity_t *entity)
{
    dmi_hpe_dimm_attrs_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_dimm_attrs));
    if (info == nullptr)
        return false;

    // SmartMemory is told by both bits, of which values 2 and 3 are unknown
    switch (info->attributes & 0x03) {
    case 0:
        info->smart_memory = DMI_HPE_FLAG_NO;
        break;
    case 1:
        info->smart_memory = DMI_HPE_FLAG_YES;
        break;
    default:
        info->smart_memory = DMI_HPE_FLAG_UNSPEC;
        break;
    }

    info->load_reduced    = dmi_hpe_dimm_attrs_flag(info->attributes, 2);
    info->standard_memory = dmi_hpe_dimm_attrs_flag(info->attributes, 4);

    return true;
}

static dmi_hpe_flag_t dmi_hpe_dimm_attrs_flag(uint32_t attributes, unsigned bit)
{
    if (not (attributes & (1u << bit)))
        return DMI_HPE_FLAG_UNSPEC;

    return (attributes & (1u << (bit + 1))) ? DMI_HPE_FLAG_YES : DMI_HPE_FLAG_NO;
}
