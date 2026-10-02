//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_HPE_H
#define OPENDMI_MODULE_HPE_H

#pragma once

#include <opendmi/module.h>
#include <opendmi/platform.h>

/**
 * @brief Code of the family of HP and HPE servers: ProLiant, Apollo, Synergy
 * and Edgeline, which share the vendor-specific structures.
 */
#define DMI_HPE_FAMILY_SERVER "server"

/**
 * @brief HP and HPE structure type identifiers.
 */
typedef enum dmi_hpe_type_id
{
    DMI_TYPE_ID_HPE_ROM_INFO           = 193, ///< Other ROM information
    DMI_TYPE_ID_HPE_SUPER_IO           = 194, ///< Super I/O enable/disable indicator
    DMI_TYPE_ID_HPE_SYSTEM_ID          = 195, ///< Server system ID
    DMI_TYPE_ID_HPE_PROCESSOR          = 197, ///< Processor specific information
    DMI_TYPE_ID_HPE_MICROCODE          = 199, ///< CPU microcode patch support information
    DMI_TYPE_ID_HPE_DIMM_LOCATION      = 202, ///< DIMM location record
    DMI_TYPE_ID_HPE_DEVICE_CORRELATION = 203, ///< Device correlation record
    DMI_TYPE_ID_HPE_RACK_LOCATOR       = 204, ///< System/rack locator
    DMI_TYPE_ID_HPE_PXE_NIC            = 209, ///< BIOS PXE NIC PCI and MAC information
    DMI_TYPE_ID_HPE_TCONTROL           = 211, ///< Processor TControl information
    DMI_TYPE_ID_HPE_CRU                = 212, ///< 64-bit CRU information
    DMI_TYPE_ID_HPE_VERSION            = 216, ///< Version indicator record
    DMI_TYPE_ID_HPE_PROLIANT_INFO      = 219, ///< ProLiant information
    DMI_TYPE_ID_HPE_ISCSI_NIC          = 221, ///< BIOS iSCSI NIC PCI and MAC information
    DMI_TYPE_ID_HPE_TRUSTED_MODULE     = 224, ///< Trusted module (TPM or TCM) status
    DMI_TYPE_ID_HPE_PHYSICAL_ATTRS     = 226, ///< Physical attribute information
    DMI_TYPE_ID_HPE_RESERVED_MEMORY    = 229, ///< Reserved memory location
    DMI_TYPE_ID_HPE_POWER_SUPPLY       = 230, ///< Power supply information
    DMI_TYPE_ID_HPE_DIMM_ATTRS         = 232, ///< DIMM attributes record
    DMI_TYPE_ID_HPE_NIC                = 233, ///< NIC PCI and MAC information
    DMI_TYPE_ID_HPE_BACKPLANE          = 236, ///< HDD backplane FRU information
    DMI_TYPE_ID_HPE_DIMM_VENDOR        = 237, ///< DIMM vendor information
    DMI_TYPE_ID_HPE_USB_PORT           = 238, ///< USB port connector correlation record
    DMI_TYPE_ID_HPE_USB_DEVICE         = 239, ///< USB device correlation record
    DMI_TYPE_ID_HPE_INVENTORY          = 240, ///< Firmware inventory record
    DMI_TYPE_ID_HPE_DRIVE              = 242, ///< Hard drive inventory record
    DMI_TYPE_ID_HPE_DIMM_CONFIG        = 244, ///< DIMM current configuration record
    DMI_TYPE_ID_HPE_EXTENSION_BOARD    = 245  ///< Extension board inventory record
} dmi_hpe_type_id_t;

/**
 * @brief Generations of HP and HPE servers.
 *
 * A generation is numbered ten times its number, so that the ones between,
 * e.g. Gen10 Plus, fit in.
 */
typedef enum dmi_hpe_generation
{
    DMI_HPE_GEN1       = 10,  ///< Gen1
    DMI_HPE_GEN2       = 20,  ///< Gen2
    DMI_HPE_GEN3       = 30,  ///< Gen3
    DMI_HPE_GEN4       = 40,  ///< Gen4
    DMI_HPE_GEN5       = 50,  ///< Gen5
    DMI_HPE_GEN6       = 60,  ///< Gen6
    DMI_HPE_GEN7       = 70,  ///< Gen7
    DMI_HPE_GEN8       = 80,  ///< Gen8
    DMI_HPE_GEN9       = 90,  ///< Gen9
    DMI_HPE_GEN10      = 100, ///< Gen10
    DMI_HPE_GEN10_PLUS = 105, ///< Gen10 Plus
    DMI_HPE_GEN11      = 110, ///< Gen11
    DMI_HPE_GEN12      = 120  ///< Gen12
} dmi_hpe_generation_t;

__BEGIN_DECLS

/** @brief Other ROM information */
extern __dmi_api const dmi_type_t dmi_type_hpe_rom_info;

/** @brief Super I/O enable/disable indicator */
extern __dmi_api const dmi_type_t dmi_type_hpe_super_io;

/** @brief Server system ID */
extern __dmi_api const dmi_type_t dmi_type_hpe_system_id;

/** @brief Processor specific information */
extern __dmi_api const dmi_type_t dmi_type_hpe_processor;

/** @brief CPU microcode patch support information */
extern __dmi_api const dmi_type_t dmi_type_hpe_microcode;

/** @brief DIMM location record */
extern __dmi_api const dmi_type_t dmi_type_hpe_dimm_location;

/** @brief Device correlation record */
extern __dmi_api const dmi_type_t dmi_type_hpe_device_correlation;

/** @brief System/rack locator */
extern __dmi_api const dmi_type_t dmi_type_hpe_rack_locator;

/** @brief BIOS PXE NIC PCI and MAC information */
extern __dmi_api const dmi_type_t dmi_type_hpe_pxe_nic;

/** @brief Processor TControl information */
extern __dmi_api const dmi_type_t dmi_type_hpe_tcontrol;

/** @brief 64-bit CRU information */
extern __dmi_api const dmi_type_t dmi_type_hpe_cru;

/** @brief Version indicator record */
extern __dmi_api const dmi_type_t dmi_type_hpe_version;

/** @brief ProLiant information */
extern __dmi_api const dmi_type_t dmi_type_hpe_proliant_info;

/** @brief BIOS iSCSI NIC PCI and MAC information */
extern __dmi_api const dmi_type_t dmi_type_hpe_iscsi_nic;

/** @brief Trusted module (TPM or TCM) status */
extern __dmi_api const dmi_type_t dmi_type_hpe_trusted_module;

/** @brief Physical attribute information */
extern __dmi_api const dmi_type_t dmi_type_hpe_physical_attrs;

/** @brief Reserved memory location */
extern __dmi_api const dmi_type_t dmi_type_hpe_reserved_memory;

/** @brief Power supply information */
extern __dmi_api const dmi_type_t dmi_type_hpe_power_supply;

/** @brief DIMM attributes record */
extern __dmi_api const dmi_type_t dmi_type_hpe_dimm_attrs;

/** @brief NIC PCI and MAC information */
extern __dmi_api const dmi_type_t dmi_type_hpe_nic;

/** @brief HDD backplane FRU information */
extern __dmi_api const dmi_type_t dmi_type_hpe_backplane;

/** @brief DIMM vendor information */
extern __dmi_api const dmi_type_t dmi_type_hpe_dimm_vendor;

/** @brief USB port connector correlation record */
extern __dmi_api const dmi_type_t dmi_type_hpe_usb_port;

/** @brief USB device correlation record */
extern __dmi_api const dmi_type_t dmi_type_hpe_usb_device;

/** @brief Firmware inventory record */
extern __dmi_api const dmi_type_t dmi_type_hpe_inventory;

/** @brief Hard drive inventory record */
extern __dmi_api const dmi_type_t dmi_type_hpe_drive;

/** @brief DIMM current configuration record */
extern __dmi_api const dmi_type_t dmi_type_hpe_dimm_config;

/** @brief PCIe riser inventory record */
extern __dmi_api const dmi_type_t dmi_type_hpe_riser;

/** @brief PCIe riser inventory record of MHS platforms */
extern __dmi_api const dmi_type_t dmi_type_hpe_mhs_riser;

extern __dmi_api const dmi_module_t dmi_hpe_module;

/**
 * @brief Tell the family and the generation of an HP or HPE platform.
 *
 * Servers are told by the name of the product line in the product name:
 * ProLiant, Apollo, Synergy or Edgeline. The generation is told by a word
 * of the product name, `G1` to `G7` or `Gen8` onwards, followed by `Plus` for
 * the generations between. Product names of HPE servers without generation
 * belong to Gen10 Plus or later, since the firmware of earlier generations
 * names HP as its vendor, and are taken for Gen10 Plus; the ones of HP
 * servers are taken for Gen6, the way dmidecode takes them. Platforms which
 * are not servers are left as they are.
 *
 * @param[in,out] platform Platform with the vendor of the firmware and the
 *                          product name set.
 *
 * @return `false` if memory is exhausted, `true` otherwise, including the
 *         case when @p platform is @c nullptr or has no product name.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 */
__dmi_api bool dmi_hpe_platform_detect(dmi_platform_t *platform);

__END_DECLS

#endif // !OPENDMI_MODULE_HPE_H
