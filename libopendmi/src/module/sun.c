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

// Structure types of the module
const dmi_type_t dmi_type_sun_processor_ex      = { .id = DMI_TYPE_ID(SUN_PROCESSOR_EX)      };
const dmi_type_t dmi_type_sun_port_ex           = { .id = DMI_TYPE_ID(SUN_PORT_EX)           };
const dmi_type_t dmi_type_sun_pcie_root_complex = { .id = DMI_TYPE_ID(SUN_PCIE_ROOT_COMPLEX) };
const dmi_type_t dmi_type_sun_memory_array_ex   = { .id = DMI_TYPE_ID(SUN_MEMORY_ARRAY_EX)   };
const dmi_type_t dmi_type_sun_memory_device_ex  = { .id = DMI_TYPE_ID(SUN_MEMORY_DEVICE_EX)  };

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
