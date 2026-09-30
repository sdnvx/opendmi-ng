//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/dell.h>
#include <opendmi/entity/intel/fvi.h>
#include <opendmi/entity/intel/mei.h>
#include <opendmi/entity/intel/svt.h>

#include <opendmi/entity/dell/bios-flags.h>
#include <opendmi/entity/dell/revisions.h>
#include <opendmi/entity/dell/parallel-port.h>
#include <opendmi/entity/dell/serial-port.h>
#include <opendmi/entity/dell/infrared-port.h>
#include <opendmi/entity/dell/indexed-io.h>
#include <opendmi/entity/dell/protected-area-1.h>
#include <opendmi/entity/dell/protected-area-2.h>
#include <opendmi/entity/dell/calling-iface.h>
#include <opendmi/entity/dell/hotkeys.h>

/**
 * @brief Dell extension module.
 */
const dmi_module_t dmi_dell_module =
{
    .code     = "dell",
    .name     = "Dell extensions",
    .entities = (const dmi_entity_spec_t *[]){
        &dmi_dell_bios_flags_spec,
        &dmi_dell_hotkeys_spec,
        &dmi_dell_revisions_spec,
        &dmi_dell_parallel_port_spec,
        &dmi_dell_serial_port_spec,
        &dmi_dell_infrared_port_spec,
        &dmi_dell_indexed_io_spec,
        &dmi_dell_protected_area_1_spec,
        &dmi_dell_protected_area_2_spec,
        &dmi_dell_calling_iface_spec,
        nullptr
    },
    .platforms = DMI_PLATFORMS({
        { .firmware_vendor = DMI_VENDOR_DELL },
        DMI_PLATFORM_NULL
    }),
    //
    // Dell gives types 221 and 222 to structures of its own, and moves the
    // structures of the Intel reference code 16 types down
    //
    .relocations = DMI_RELOCATIONS({
        { &dmi_intel_mei_spec, 203 },
        { &dmi_intel_fvi_spec, 205 },
        { &dmi_intel_svt_spec, 206 },
        {}
    })
};
