//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-fpga-internal.h>

const dmi_name_set_t dmi_intel_rsd_fpga_type_names =
{
    .code  = "intel-rsd-fpga-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_INTEL_RSD_FPGA_TYPE_INTEGRATED,
            .code = "integrated",
            .name = "Integrated"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_TYPE_DISCRETE,
            .code = "discrete",
            .name = "Discrete"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_TYPE_DISCRETE_SOC,
            .code = "discrete-soc",
            .name = "Discrete with SoC/Hard Processor Subsystem (HPS)"
        },
        {}
    })
};

const dmi_name_set_t dmi_intel_rsd_fpga_status_names =
{
    .code  = "intel-rsd-fpga-status",
    .names = DMI_NAMES({
        {
            .id   = DMI_INTEL_RSD_FPGA_STATUS_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_STATUS_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {}
    })
};

const dmi_name_set_t dmi_intel_rsd_fpga_hps_isa_names =
{
    .code  = "intel-rsd-fpga-isa",
    .names = DMI_NAMES({
        {
            .id   = DMI_INTEL_RSD_FPGA_HPS_ISA_X86,
            .code = "x86",
            .name = "x86"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_HPS_ISA_X86_64,
            .code = "x86-64",
            .name = "x86-64"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_HPS_ISA_IA_64,
            .code = "ia-64",
            .name = "IA-64"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_HPS_ISA_ARM_A32,
            .code = "arm-a32",
            .name = "ARM-A32"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_HPS_ISA_ARM_A64,
            .code = "arm-a64",
            .name = "ARM-A64"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_HPS_ISA_MIPS32,
            .code = "mips32",
            .name = "MIPS32"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_HPS_ISA_MIPS64,
            .code = "mips64",
            .name = "MIPS64"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_HPS_ISA_OEM,
            .code = "oem",
            .name = "OEM"
        },
        {}
    })
};

const dmi_name_set_t dmi_intel_rsd_fpga_hssi_config_names =
{
    .code  = "intel-rsd-fpga-config",
    .names = DMI_NAMES({
        {
            .id   = DMI_INTEL_RSD_FPGA_HSSI_CONFIG_NETWORKING,
            .code = "networking",
            .name = "Networking"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_HSSI_CONFIG_PCIE,
            .code = "pcie",
            .name = "PCIe"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_HSSI_CONFIG_UNAVAILABLE,
            .code = "unavailable",
            .name = "Information not available"
        },
        {}
    })
};

const dmi_name_set_t dmi_intel_rsd_fpga_memory_tech_names =
{
    .code  = "intel-rsd-fpga-memory-tech",
    .names = DMI_NAMES({
        {
            .id   = DMI_INTEL_RSD_FPGA_MEMORY_TECH_NONE,
            .code = "none",
            .name = "None"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_MEMORY_TECH_EDRAM,
            .code = "edram",
            .name = "EDRAM"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_MEMORY_TECH_HBM,
            .code = "hbm",
            .name = "HBM"
        },
        {
            .id   = DMI_INTEL_RSD_FPGA_MEMORY_TECH_HBM2,
            .code = "hbm2",
            .name = "HBM2"
        },
        {}
    })
};
