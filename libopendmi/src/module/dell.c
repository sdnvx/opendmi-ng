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
#include <opendmi/entity/dell/device-bay.h>
#include <opendmi/entity/dell/device-names.h>
#include <opendmi/entity/dell/hotkeys.h>
#include <opendmi/entity/dell/memory-ids.h>
#include <opendmi/entity/dell/system-id.h>
#include <opendmi/entity/dell/token-refs.h>
#include <opendmi/entity/dell/video-rom.h>

// Structure types of the module
const dmi_type_t dmi_type_dell_bios_flags       = { .id = DMI_TYPE_ID(DELL_BIOS_FLAGS)       };
const dmi_type_t dmi_type_dell_hotkeys          = { .id = DMI_TYPE_ID(DELL_HOTKEYS)          };
const dmi_type_t dmi_type_dell_revisions        = { .id = DMI_TYPE_ID(DELL_REVISIONS)        };
const dmi_type_t dmi_type_dell_parallel_port    = { .id = DMI_TYPE_ID(DELL_PARALLEL_PORT)    };
const dmi_type_t dmi_type_dell_serial_port      = { .id = DMI_TYPE_ID(DELL_SERIAL_PORT)      };
const dmi_type_t dmi_type_dell_infrared_port    = { .id = DMI_TYPE_ID(DELL_INFRARED_PORT)    };
const dmi_type_t dmi_type_dell_indexed_io       = { .id = DMI_TYPE_ID(DELL_INDEXED_IO)       };
const dmi_type_t dmi_type_dell_protected_area_1 = { .id = DMI_TYPE_ID(DELL_PROTECTED_AREA_1) };
const dmi_type_t dmi_type_dell_protected_area_2 = { .id = DMI_TYPE_ID(DELL_PROTECTED_AREA_2) };
const dmi_type_t dmi_type_dell_video_rom        = { .id = DMI_TYPE_ID(DELL_VIDEO_ROM)        };
const dmi_type_t dmi_type_dell_calling_iface    = { .id = DMI_TYPE_ID(DELL_CALLING_IFACE)    };
const dmi_type_t dmi_type_dell_device_bay       = { .id = DMI_TYPE_ID(DELL_DEVICE_BAY)       };
const dmi_type_t dmi_type_dell_token_refs_1     = { .id = DMI_TYPE_ID(DELL_TOKEN_REFS_1)     };
const dmi_type_t dmi_type_dell_token_refs_2     = { .id = DMI_TYPE_ID(DELL_TOKEN_REFS_2)     };
const dmi_type_t dmi_type_dell_memory_ids       = { .id = DMI_TYPE_ID(DELL_MEMORY_IDS)       };
const dmi_type_t dmi_type_dell_device_names     = { .id = DMI_TYPE_ID(DELL_DEVICE_NAMES)     };
const dmi_type_t dmi_type_dell_system_id        = { .id = DMI_TYPE_ID(DELL_SYSTEM_ID)        };

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
        &dmi_dell_video_rom_spec,
        &dmi_dell_calling_iface_spec,
        &dmi_dell_device_bay_spec,
        &dmi_dell_token_refs_1_spec,
        &dmi_dell_token_refs_2_spec,
        &dmi_dell_memory_ids_spec,
        &dmi_dell_device_names_spec,
        &dmi_dell_system_id_spec,
        nullptr
    },
    //
    // Systems of Dell may carry the firmware of other vendors, e.g. PowerEdge
    // 8450 the one of Intel, and Unisys ES servers, which Dell makes, carry
    // the firmware of Dell under the name of Unisys
    //
    .platforms = DMI_PLATFORMS({
        { .firmware_vendor = DMI_VENDOR_DELL   },
        { .system_vendor   = DMI_VENDOR_DELL   },
        { .firmware_vendor = DMI_VENDOR_UNISYS },
        DMI_PLATFORM_NULL
    }),
    //
    // Dell gives types 221 and 222 to structures of its own, and moves the
    // structures of the Intel reference code 16 types down, or 16 types up
    // on some platforms, e.g. Precision Tower 3620. Some systems keep them at
    // their own types, e.g. XPS 13 9350, where the signatures of the Dell
    // structures tell them apart
    //
    .relocations = DMI_RELOCATIONS({
        { &dmi_intel_mei_spec, 203 },
        { &dmi_intel_mei_spec, 219 },
        { &dmi_intel_mei_spec, 235 },
        { &dmi_intel_fvi_spec, 205 },
        { &dmi_intel_fvi_spec, 221 },
        { &dmi_intel_fvi_spec, 237 },
        { &dmi_intel_svt_spec, 206 },
        { &dmi_intel_svt_aligned_spec, 206 },
        {}
    })
};
