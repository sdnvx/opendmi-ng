//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/sun.h>

#include <opendmi/entity/sun/processor-ex.h>
#include <opendmi/entity/sun/memory-array-ex.h>
#include <opendmi/entity/sun/memory-device-ex.h>
#include <opendmi/entity/sun/port-ex.h>
#include <opendmi/entity/sun/pcie-root-complex.h>

/**
 * @brief Sun extension module.
 */
const dmi_module_t dmi_sun_module =
{
    .code     = "sun",
    .name     = "Sun extensions",
    .entities = (const dmi_entity_spec_t *[]){
        &dmi_sun_processor_ex_spec,
        &dmi_sun_memory_array_ex_spec,
        &dmi_sun_memory_device_ex_spec,
        &dmi_sun_port_ex_spec,
        &dmi_sun_pcie_root_complex_spec,
        nullptr
    }
};
