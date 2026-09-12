//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DMI_ENTITY_INTEL_RSD_FPGA_H
#define DMI_ENTITY_INTEL_RSD_FPGA_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/entity/common.h>

typedef struct dmi_intel_rsd_fpga dmi_intel_rsd_fpga_t;

/**
 * @brief Intel RSD FPGA types.
 */
typedef enum dmi_intel_rsd_fpga_type
{
    DMI_INTEL_RSD_FPGA_TYPE_INTEGRATED   = 0x00, ///< Integrated
    DMI_INTEL_RSD_FPGA_TYPE_DISCRETE     = 0x01, ///< Discrete
    DMI_INTEL_RSD_FPGA_TYPE_DISCRETE_SOC = 0x02, ///< Discrete with SoC/Hard Processor Subsystem (HPS)
    __DMI_INTEL_RSD_FPGA_TYPE_COUNT
} dmi_intel_rsd_fpga_type_t;

/**
 * @brief Intel RSD FPGA status values.
 */
typedef enum dmi_intel_rsd_fpga_status
{
    DMI_INTEL_RSD_FPGA_STATUS_DISABLED = 0x00, ///< Disabled
    DMI_INTEL_RSD_FPGA_STATUS_ENABLED  = 0x01, ///< Enabled
} dmi_intel_rsd_fpga_status_t;

typedef enum dmi_intel_rsd_fpga_hps_isa
{
    DMI_INTEL_RSD_FPGA_HPS_ISA_X86     = 0x00, ///< x86
    DMI_INTEL_RSD_FPGA_HPS_ISA_X86_64  = 0x01, ///< x86-64
    DMI_INTEL_RSD_FPGA_HPS_ISA_IA_64   = 0x02, ///< IA-64
    DMI_INTEL_RSD_FPGA_HPS_ISA_ARM_A32 = 0x03, ///< ARM-A32
    DMI_INTEL_RSD_FPGA_HPS_ISA_ARM_A64 = 0x04, ///< ARM-A64
    DMI_INTEL_RSD_FPGA_HPS_ISA_MIPS32  = 0x05, ///< MIPS32
    DMI_INTEL_RSD_FPGA_HPS_ISA_MIPS64  = 0x06, ///< MIPS64
    DMI_INTEL_RSD_FPGA_HPS_ISA_OEM     = 0x07, ///< OEM
    __DMI_INTEL_RSD_FPGA_HPS_ISA_COUNT
} dmi_intel_rsd_fpga_hps_isa_t;

/**
 * @brief FPGA High-Speed Serial Interface (HSSI) configurations.
 */
typedef enum  dmi_intel_rsd_fpga_hssi_config
{
    DMI_INTEL_RSD_FPGA_HSS_CONFIG_NETWORKING  = 0x00, ///< Networking
    DMI_INTEL_RSD_FPGA_HSS_CONFIG_PCIE        = 0x01, ///< PCIe
    DMI_INTEL_RSD_FPGA_HSS_CONFIG_UNAVAILABLE = 0xFF, ///< Information ot available
} dmi_intel_rsd_fpga_hssi_config_t;

typedef enum dmi_intel_rsd_fpga_memory_tech
{
    DMI_INTEL_RSD_FPGA_MEMORY_TECH_NONE  = 0x00,
    DMI_INTEL_RSD_FPGA_MEMORY_TECH_EDRAM = 0x01,
    DMI_INTEL_RSD_FPGA_MEMORY_TECH_HBM   = 0x02,
    DMI_INTEL_RSD_FPGA_MEMORY_TECH_HBM2  = 0x03
} dmi_intel_rsd_fpga_memory_tech_t;

/**
 * @brief Intel RSD FPGA information (type 198).
 *
 * SMBIOS OEM type 198 is used to declare Field-Programmable Gate Array (FPGA)
 * information on the system. UEFI FW shall implement this structure only if
 * FPGA capability is present on the platform.
 */
struct dmi_intel_rsd_fpga
{
    /**
     * @brief 1-based index to identify specific FPGA instance on the platform.
     */
    unsigned short index;

    /**
     * @brief Indicates the type of this FPGA.
     */
    dmi_intel_rsd_fpga_type_t type;

    /**
     * @brief Current status of this FPGA.
     */
    dmi_intel_rsd_fpga_status_t status;

    /**
     * @brief Identifies the specific socket the FPGA is integrated into.
     * Applicable only for integrated FPGA type. Otherwise, this field is not
     * applicable.
     */
    unsigned short socket;

    /**
     * @brief The string that identifies FPGA vendor (either a descriptive
     * string or Vendor ID identified in the PCI configuration space for the
     * discrete FPGA).
     */
    const char *vendor;

    /**
     * @brief The string that identifies FPGA family.
     */
    const char *family;

    /**
     * @brief The string that identifies FPGA model.
     */
    const char *model;

    /**
     * @brief The string that identifies bit stream version.
     */
    const char *bit_stream_version;

    /**
     * @brief Identifies the number of cores supported in the HPS cores if
     * available.
     */
    unsigned short hps_core_count;

    /**
     * @brief Identifies the Instruction Set Architecture (ISA) for the HPS
     * cores on this FPGA.
     */
    dmi_intel_rsd_fpga_hps_isa_t hps_isa;

    /**
     * @brief Identifies the supported High-Speed Serial Interface (HSSI)
     * configuration in this FPGA.
     */
    dmi_intel_rsd_fpga_hssi_config_t hssi_config;

    /**
     * @brief Number of ports supported for the available HSSI configuration.
     * E.g. `2` for "2x10G", `4` for "4x10G", `2` for "2x40G", `2` for "2 x16
     * PCIe" etc.
     */
    unsigned short hssi_port_count;

    /**
     * @brief Speed in Gbps or number of lanes supported by the HSSI
     * configuration. E.g. `10` for "2x10G", `40 for "2x40G", `16` for "x16
     * PCIe" etc.
     */
    unsigned short hssi_port_speed;

    /**
     * @brief The string that identifies the HSSI side band configuration
     * mechanism supported by this FPGA. E.g.: "SPI", "I2C-0", "SMBus" etc.
     */
    const char *hssi_side_band_config;

    /**
     * @brief Number of Partial Reconfiguration (PR) slots available in this
     * FPGA.
     */
    unsigned short reconfig_slots;

    /**
     * @brief Physical slot number of the slot connected to the PCIe port where
     * this FPGA can be accessed.
     */
    dmi_pci_slot_t pci_slot_id;

    /**
     * @brief PCIe bus number where this FPGA can be accessed.
     */
    uint8_t pci_bus_number;

    /**
     * @brief PCIe device number where this FPGA can be accessed.
     */
    uint8_t pci_device_id;

    /**
     * @brief PCIe function number where this FPGA can be accessed.
     */
    uint8_t pci_function_id;

    /**
     * @brief Thermal design power of the FPGA device in mW.
     */
    uint32_t tdp;

    /**
     * @brief Identifies the technology of attached memory if any.
     */
    dmi_intel_rsd_fpga_memory_tech_t memory_tech;

    /**
     * @brief Capacity of on-package memory in MiB.
     */
    uint32_t memory_capacity;

    /**
     * @brief Speed of memory in MHz.
     */
    uint16_t memory_speed;
};

/**
 * @brief Intel RSD FPGA information entity specification.
 */
extern const dmi_entity_spec_t dmi_intel_rsd_fpga_spec;

#endif // !DMI_ENTITY_INTEL_RSD_FPGA_H
