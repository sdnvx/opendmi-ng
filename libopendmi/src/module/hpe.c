//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <ctype.h>

#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/backplane.h>
#include <opendmi/entity/hpe/cru.h>
#include <opendmi/entity/hpe/device-correlation.h>
#include <opendmi/entity/hpe/dimm-attrs.h>
#include <opendmi/entity/hpe/dimm-config.h>
#include <opendmi/entity/hpe/dimm-location.h>
#include <opendmi/entity/hpe/dimm-vendor.h>
#include <opendmi/entity/hpe/drive.h>
#include <opendmi/entity/hpe/extension-board.h>
#include <opendmi/entity/hpe/inventory.h>
#include <opendmi/entity/hpe/microcode.h>
#include <opendmi/entity/hpe/nic.h>
#include <opendmi/entity/hpe/nic-mac.h>
#include <opendmi/entity/hpe/physical-attrs.h>
#include <opendmi/entity/hpe/power-supply.h>
#include <opendmi/entity/hpe/processor.h>
#include <opendmi/entity/hpe/proliant-info.h>
#include <opendmi/entity/hpe/rack-locator.h>
#include <opendmi/entity/hpe/reserved-memory.h>
#include <opendmi/entity/hpe/rom-info.h>
#include <opendmi/entity/hpe/super-io.h>
#include <opendmi/entity/hpe/system-id.h>
#include <opendmi/entity/hpe/tcontrol.h>
#include <opendmi/entity/hpe/trusted-module.h>
#include <opendmi/entity/hpe/usb-device.h>
#include <opendmi/entity/hpe/usb-port.h>
#include <opendmi/entity/hpe/version.h>
#include <opendmi/entity/intel/fvi.h>
#include <opendmi/entity/intel/mei.h>
#include <opendmi/entity/intel/svt.h>

// Structure types of the module
const dmi_type_t dmi_type_hpe_rom_info           = { .id = DMI_TYPE_ID(HPE_ROM_INFO)           };
const dmi_type_t dmi_type_hpe_super_io           = { .id = DMI_TYPE_ID(HPE_SUPER_IO)           };
const dmi_type_t dmi_type_hpe_system_id          = { .id = DMI_TYPE_ID(HPE_SYSTEM_ID)          };
const dmi_type_t dmi_type_hpe_processor          = { .id = DMI_TYPE_ID(HPE_PROCESSOR)          };
const dmi_type_t dmi_type_hpe_microcode          = { .id = DMI_TYPE_ID(HPE_MICROCODE)          };
const dmi_type_t dmi_type_hpe_dimm_location      = { .id = DMI_TYPE_ID(HPE_DIMM_LOCATION)      };
const dmi_type_t dmi_type_hpe_device_correlation = { .id = DMI_TYPE_ID(HPE_DEVICE_CORRELATION) };
const dmi_type_t dmi_type_hpe_rack_locator       = { .id = DMI_TYPE_ID(HPE_RACK_LOCATOR)       };
const dmi_type_t dmi_type_hpe_pxe_nic            = { .id = DMI_TYPE_ID(HPE_PXE_NIC)            };
const dmi_type_t dmi_type_hpe_tcontrol           = { .id = DMI_TYPE_ID(HPE_TCONTROL)           };
const dmi_type_t dmi_type_hpe_cru                = { .id = DMI_TYPE_ID(HPE_CRU)                };
const dmi_type_t dmi_type_hpe_version            = { .id = DMI_TYPE_ID(HPE_VERSION)            };
const dmi_type_t dmi_type_hpe_proliant_info      = { .id = DMI_TYPE_ID(HPE_PROLIANT_INFO)      };
const dmi_type_t dmi_type_hpe_iscsi_nic          = { .id = DMI_TYPE_ID(HPE_ISCSI_NIC)          };
const dmi_type_t dmi_type_hpe_trusted_module     = { .id = DMI_TYPE_ID(HPE_TRUSTED_MODULE)     };
const dmi_type_t dmi_type_hpe_physical_attrs     = { .id = DMI_TYPE_ID(HPE_PHYSICAL_ATTRS)     };
const dmi_type_t dmi_type_hpe_reserved_memory    = { .id = DMI_TYPE_ID(HPE_RESERVED_MEMORY)    };
const dmi_type_t dmi_type_hpe_power_supply       = { .id = DMI_TYPE_ID(HPE_POWER_SUPPLY)       };
const dmi_type_t dmi_type_hpe_dimm_attrs         = { .id = DMI_TYPE_ID(HPE_DIMM_ATTRS)         };
const dmi_type_t dmi_type_hpe_nic                = { .id = DMI_TYPE_ID(HPE_NIC)                };
const dmi_type_t dmi_type_hpe_backplane          = { .id = DMI_TYPE_ID(HPE_BACKPLANE)          };
const dmi_type_t dmi_type_hpe_dimm_vendor        = { .id = DMI_TYPE_ID(HPE_DIMM_VENDOR)        };
const dmi_type_t dmi_type_hpe_usb_port           = { .id = DMI_TYPE_ID(HPE_USB_PORT)           };
const dmi_type_t dmi_type_hpe_usb_device         = { .id = DMI_TYPE_ID(HPE_USB_DEVICE)         };
const dmi_type_t dmi_type_hpe_inventory          = { .id = DMI_TYPE_ID(HPE_INVENTORY)          };
const dmi_type_t dmi_type_hpe_drive              = { .id = DMI_TYPE_ID(HPE_DRIVE)              };
const dmi_type_t dmi_type_hpe_dimm_config        = { .id = DMI_TYPE_ID(HPE_DIMM_CONFIG)        };
const dmi_type_t dmi_type_hpe_riser              = { .id = DMI_TYPE_ID(HPE_EXTENSION_BOARD)    };
const dmi_type_t dmi_type_hpe_mhs_riser          = { .id = DMI_TYPE_ID(HPE_EXTENSION_BOARD)    };

/**
 * @internal
 * @brief Tell the generation from a word of the product name.
 *
 * @return Generation, or zero if the word does not name one.
 */
static unsigned dmi_hpe_generation_parse(const char *word, size_t length);

/**
 * @brief HPE module extension.
 */
const dmi_module_t dmi_hpe_module =
{
    .code      = "hpe",
    .name      = "HP/HPE extensions",
    .entities  = (const dmi_entity_spec_t *[]){
        &dmi_hpe_rom_info_spec,
        &dmi_hpe_super_io_spec,
        &dmi_hpe_system_id_spec,
        &dmi_hpe_processor_spec,
        &dmi_hpe_microcode_spec,
        &dmi_hpe_dimm_location_spec,
        &dmi_hpe_device_correlation_spec,
        &dmi_hpe_rack_locator_spec,
        &dmi_hpe_pxe_nic_spec,
        &dmi_hpe_tcontrol_spec,
        &dmi_hpe_cru_spec,
        &dmi_hpe_version_spec,
        &dmi_hpe_proliant_info_spec,
        &dmi_hpe_iscsi_nic_spec,
        &dmi_hpe_trusted_module_spec,
        &dmi_hpe_physical_attrs_legacy_spec,
        &dmi_hpe_physical_attrs_spec,
        &dmi_hpe_reserved_memory_spec,
        &dmi_hpe_power_supply_spec,
        &dmi_hpe_dimm_attrs_spec,
        &dmi_hpe_nic_mac_spec,
        &dmi_hpe_backplane_spec,
        &dmi_hpe_dimm_vendor_spec,
        &dmi_hpe_usb_port_spec,
        &dmi_hpe_usb_device_spec,
        &dmi_hpe_inventory_spec,
        &dmi_hpe_drive_spec,
        &dmi_hpe_dimm_config_spec,
        &dmi_hpe_riser_spec,
        &dmi_hpe_mhs_riser_spec,
        nullptr
    },
    .platforms = DMI_PLATFORMS({
        { .firmware_vendor = DMI_VENDOR_HP,  .family = DMI_HPE_FAMILY_SERVER },
        { .firmware_vendor = DMI_VENDOR_HPE, .family = DMI_HPE_FAMILY_SERVER },
        DMI_PLATFORM_NULL
    }),
    //
    // Servers give the types of the Intel reference code to structures of
    // their own, e.g. type 219 to the ProLiant information and type 221 to
    // the iSCSI NIC information up to G7
    //
    .relocations = DMI_RELOCATIONS({
        { &dmi_intel_mei_spec, DMI_TYPE_ID_INVALID },
        { &dmi_intel_fvi_spec, DMI_TYPE_ID_INVALID },
        { &dmi_intel_svt_spec, DMI_TYPE_ID_INVALID },
        { &dmi_intel_svt_aligned_spec, DMI_TYPE_ID_INVALID },
        {}
    })
};

bool dmi_hpe_platform_detect(dmi_platform_t *platform)
{
    static const char *lines[] = { "ProLiant", "Apollo", "Synergy", "Edgeline" };

    if ((platform == nullptr) or (platform->product == nullptr))
        return true;

    const char *product = platform->product;

    // Workstations and laptops of HP have structures of their own
    bool is_server = false;
    for (size_t i = 0; i < countof(lines); i++) {
        if (strstr(product, lines[i]) != nullptr)
            is_server = true;
    }

    if (not is_server)
        return true;

    if (not dmi_platform_set_family(platform, DMI_HPE_FAMILY_SERVER))
        return false;

    // Generation is a word of its own, e.g. "ProLiant DL360 Gen10 Plus"
    const char *ptr = product;
    while (*ptr != '\0') {
        while (*ptr == ' ')
            ptr++;

        const char *word = ptr;
        while ((*ptr != ' ') and (*ptr != '\0'))
            ptr++;

        unsigned generation = dmi_hpe_generation_parse(word, (size_t)(ptr - word));
        if (generation == 0)
            continue;

        while (*ptr == ' ')
            ptr++;
        if ((strncmp(ptr, "Plus", 4) == 0) and ((ptr[4] == ' ') or (ptr[4] == '\0')))
            generation += 5;

        platform->generation = generation;
        return true;
    }

    if (platform->firmware_vendor == DMI_VENDOR_HPE)
        platform->generation = DMI_HPE_GEN10_PLUS;

    return true;
}

static unsigned dmi_hpe_generation_parse(const char *word, size_t length)
{
    size_t prefix;

    // Up to G7 generations are named "G<n>", and "Gen<n>" from Gen8 onwards
    if ((length > 3) and (strncmp(word, "Gen", 3) == 0))
        prefix = 3;
    else if ((length > 1) and (word[0] == 'G'))
        prefix = 1;
    else
        return 0;

    unsigned number = 0;
    for (size_t i = prefix; i < length; i++) {
        if (not isdigit((unsigned char)word[i]))
            return 0;
        if (number > 99)
            return 0;

        number = number * 10 + (unsigned)(word[i] - '0');
    }

    return number * 10;
}
