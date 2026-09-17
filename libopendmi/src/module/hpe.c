//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/hpe.h>

/**
 * @brief HPE module extension.
 */
const dmi_module_t dmi_hpe_module =
{
    .code     = "hpe",
    .name     = "HP/HPE extensions",
    .entities = (const dmi_entity_spec_t *[]){
        nullptr
    }
};
