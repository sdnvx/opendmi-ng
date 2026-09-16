//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/intel.h>
#include <opendmi/entity/intel/rsd-fpga.h>
#include <opendmi/value.h>

static bool dmi_intel_rsd_fpga_decode(dmi_entity_t *entity);

const dmi_name_set_t dmi_intel_rsd_fpga_type_names =
{
    .code  = "intel-rsd-fpga-types",
    .names = (dmi_name_t[]){
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
    }
};

const dmi_name_set_t dmi_intel_rsd_fpga_status_names =
{
    .code  = "intel-rsd-fpga-statuses",
    .names = (dmi_name_t[]){
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
    }
};

const dmi_name_set_t dmi_intel_rsd_fpga_hps_isa_names =
{
    .code  = "intel-rsd-fpga-isas",
    .names = (dmi_name_t[]){
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
    }
};

const dmi_name_set_t dmi_intel_rsd_fpga_hssi_config_names =
{
    .code  = "intel-rsd-fpga-configs",
    .names = (dmi_name_t[]){
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
    }
};

const dmi_name_set_t dmi_intel_rsd_fpga_memory_tech_names =
{
    .code  = "intel-rsd-fpga-memory-techs",
    .names = (dmi_name_t[]){
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
    }
};

const dmi_entity_spec_t dmi_intel_rsd_fpga_spec =
{
    .code            = "intel-rsd-fpga",
    .name            = "Intel RSD FPGA information",
    .type            = DMI_TYPE(INTEL_RSD_FPGA),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .decoded_length  = sizeof(dmi_intel_rsd_fpga_t),
    .minimum_length  = 0x24,
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, index, INTEGER, {
            .code   = "index",
            .name   = "Index"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, type, ENUM, {
            .code   = "type",
            .name   = "Type",
            .values = &dmi_intel_rsd_fpga_type_names
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, status, ENUM, {
            .code   = "status",
            .name   = "Status",
            .values = &dmi_intel_rsd_fpga_status_names
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, socket, INTEGER, {
            .code   = "socket",
            .name   = "Socket identifier"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, vendor, STRING, {
            .code   = "vendor",
            .name   = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, family, STRING, {
            .code   = "family",
            .name   = "Family"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, model, STRING, {
            .code   = "model",
            .name   = "Model"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, bit_stream_version, STRING, {
            .code   = "bit-stream-version",
            .name   = "Bit stream version"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, hps_core_count, INTEGER, {
            .code   = "hps-core-count",
            .name   = "HPS core count"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, hps_isa, ENUM, {
            .code   = "hps-isa",
            .name   = "HPS ISA",
            .values = &dmi_intel_rsd_fpga_hps_isa_names
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, hssi_config, ENUM, {
            .code   = "hssi-config",
            .name   = "HSSI configuration",
            .values = &dmi_intel_rsd_fpga_hssi_config_names
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, hssi_port_count, INTEGER, {
            .code   = "hssi-port-count",
            .name   = "HSSI port count"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, hssi_port_speed, INTEGER, {
            .code   = "hssi-port-speed",
            .name   = "HSSI port speed"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, hssi_side_band_config, STRING, {
            .code   = "hssi-side-band-config",
            .name   = "HSSI side-band configuration"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, reconfig_slots, INTEGER, {
            .code   = "reconfig-slots",
            .name   = "Partial Reconfiguration (PR) slots"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, pci_slot_id, INTEGER, {
            .code   = "pcie-slot-id",
            .name   = "PCIe slot ID"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, pci_bus_number, INTEGER, {
            .code   = "pcie-bus-number",
            .name   = "PCIe bus number"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, pci_device_id, INTEGER, {
            .code   = "pcie-device-id",
            .name   = "PCIe device ID"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, pci_function_id, INTEGER, {
            .code   = "pcie-function-id",
            .name   = "PCIe function ID"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, tdp, DECIMAL, {
            .code   = "tdp",
            .name   = "TDP",
            .scale  = 3,
            .unit   = DMI_UNIT_WATT
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, memory_tech, ENUM, {
            .code   = "memory-tech",
            .name   = "Memory technology",
            .values = &dmi_intel_rsd_fpga_memory_tech_names
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, memory_capacity, INTEGER, {
            .code   = "memory-capacity",
            .name   = "Memory capacity",
            .unit   = DMI_UNIT_MEGABYTE
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, memory_speed, INTEGER, {
            .code   = "memory-speed",
            .name   = "Memory speed",
            .unit   = DMI_UNIT_MHZ
        }),
        {}
    },
    .handlers = {
        .decode = dmi_intel_rsd_fpga_decode
    }
};

static bool dmi_intel_rsd_fpga_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_fpga_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_FPGA));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    return
        dmi_stream_decode(stream, dmi_byte_t, &info->index) and
        dmi_stream_decode(stream, dmi_byte_t, &info->type) and
        dmi_stream_decode(stream, dmi_byte_t, &info->status) and
        dmi_stream_decode(stream, dmi_byte_t, &info->socket) and
        dmi_stream_decode_str(stream, &info->vendor) and
        dmi_stream_decode_str(stream, &info->family) and
        dmi_stream_decode_str(stream, &info->model) and
        dmi_stream_decode_str(stream, &info->bit_stream_version) and
        dmi_stream_decode(stream, dmi_byte_t, &info->hps_core_count) and
        dmi_stream_skip(stream, 1) and // Offset 0Dh is not defined by specification
        dmi_stream_decode(stream, dmi_byte_t, &info->hps_isa) and
        dmi_stream_decode(stream, dmi_byte_t, &info->hssi_config) and
        dmi_stream_decode(stream, dmi_byte_t, &info->hssi_port_count) and
        dmi_stream_decode(stream, dmi_byte_t, &info->hssi_port_speed) and
        dmi_stream_decode_str(stream, &info->hssi_side_band_config) and
        dmi_stream_decode(stream, dmi_byte_t, &info->reconfig_slots) and
        dmi_stream_decode(stream, dmi_word_t, &info->pci_slot_id) and
        dmi_stream_decode(stream, dmi_byte_t, &info->pci_bus_number) and
        dmi_stream_decode(stream, dmi_byte_t, &info->pci_device_id) and
        dmi_stream_decode(stream, dmi_byte_t, &info->pci_function_id) and
        dmi_stream_decode(stream, dmi_dword_t, &info->tdp) and
        dmi_stream_decode(stream, dmi_byte_t, &info->memory_tech) and
        dmi_stream_decode(stream, dmi_dword_t, &info->memory_capacity) and
        dmi_stream_decode(stream, dmi_word_t, &info->memory_speed);
}
