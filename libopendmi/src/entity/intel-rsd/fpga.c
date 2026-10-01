//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel-rsd.h>

#include "fpga-internal.h"

const dmi_entity_spec_t dmi_intel_rsd_fpga_spec =
{
    .code            = "intel-rsd-fpga",
    .name            = "Intel RSD FPGA information",
    .type            = DMI_TYPE(intel_rsd_fpga),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .decoded_length  = sizeof(dmi_intel_rsd_fpga_t),
        .minimum_length  = 0x24
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_rsd_fpga_t, index,  dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, type,   dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, status, dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, socket, dmi_byte_t),

        DMI_FIELD_STRING(dmi_intel_rsd_fpga_t, vendor),
        DMI_FIELD_STRING(dmi_intel_rsd_fpga_t, family),
        DMI_FIELD_STRING(dmi_intel_rsd_fpga_t, model),
        DMI_FIELD_STRING(dmi_intel_rsd_fpga_t, bit_stream_version),

        DMI_FIELD(dmi_intel_rsd_fpga_t, hps_core_count, dmi_byte_t),

        // Offset 0Dh is not defined by the specification
        DMI_FIELD_SKIP(sizeof(dmi_byte_t)),

        DMI_FIELD(dmi_intel_rsd_fpga_t, hps_isa,         dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, hssi_config,     dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, hssi_port_count, dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, hssi_port_speed, dmi_byte_t),
        DMI_FIELD_STRING(dmi_intel_rsd_fpga_t, hssi_side_band_config),
        DMI_FIELD(dmi_intel_rsd_fpga_t, reconfig_slots, dmi_byte_t),

        DMI_FIELD(dmi_intel_rsd_fpga_t, pci_slot_id,     dmi_word_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, pci_bus_number,      dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, pci_device_number,   dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, pci_function_number, dmi_byte_t),

        DMI_FIELD(dmi_intel_rsd_fpga_t, tdp,             dmi_dword_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, memory_tech,     dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, memory_capacity, dmi_dword_t),
        DMI_FIELD(dmi_intel_rsd_fpga_t, memory_speed,    dmi_word_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, index, INTEGER, {
            .code    = "index",
            .name    = "Index",
            .minimum = dmi_value_ptr((unsigned short)1)
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
        // Socket means something for an integrated FPGA only
        DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_fpga_t, type, {
            .code     = "socket",
            .name     = "Socket identifier",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_INTEL_RSD_FPGA_TYPE_INTEGRATED, dmi_intel_rsd_fpga_t, socket, INTEGER, {}),
                {}
            })
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
        // Rest of the interface is to be ignored when the configuration is
        // not available, and means nothing for the configurations the
        // specification does not define
        DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_fpga_t, hssi_config, {
            .code     = "hssi-port-count",
            .name     = "HSSI port count",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_INTEL_RSD_FPGA_HSSI_CONFIG_NETWORKING, dmi_intel_rsd_fpga_t, hssi_port_count, INTEGER, {}),
                DMI_VARIANT(DMI_INTEL_RSD_FPGA_HSSI_CONFIG_PCIE, dmi_intel_rsd_fpga_t, hssi_port_count, INTEGER, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_fpga_t, hssi_config, {
            .code     = "hssi-port-speed",
            .name     = "HSSI port speed",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_INTEL_RSD_FPGA_HSSI_CONFIG_NETWORKING, dmi_intel_rsd_fpga_t, hssi_port_speed, INTEGER, {}),
                DMI_VARIANT(DMI_INTEL_RSD_FPGA_HSSI_CONFIG_PCIE, dmi_intel_rsd_fpga_t, hssi_port_speed, INTEGER, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_fpga_t, hssi_config, {
            .code     = "hssi-side-band-config",
            .name     = "HSSI side-band configuration",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_INTEL_RSD_FPGA_HSSI_CONFIG_NETWORKING, dmi_intel_rsd_fpga_t, hssi_side_band_config, STRING, {}),
                DMI_VARIANT(DMI_INTEL_RSD_FPGA_HSSI_CONFIG_PCIE, dmi_intel_rsd_fpga_t, hssi_side_band_config, STRING, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, reconfig_slots, INTEGER, {
            .code   = "reconfig-slots",
            .name   = "Partial Reconfiguration (PR) slots"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, pci_slot_id, INTEGER, {
            .code   = "pci-slot-id",
            .name   = "PCI slot ID"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, pci_bus_number, INTEGER, {
            .code   = "pci-bus-number",
            .name   = "PCI bus number"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, pci_device_number, INTEGER, {
            .code   = "pci-device-number",
            .name   = "PCI device number"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, pci_function_number, INTEGER, {
            .code   = "pci-function-number",
            .name   = "PCI function number"
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
        // The specification gives the capacity in MB, which means 2^20 bytes
        // here, the way it does for the memory sizes of SMBIOS
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, memory_capacity, INTEGER, {
            .code   = "memory-capacity",
            .name   = "Memory capacity",
            .unit   = DMI_UNIT_MEBIBYTE
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_fpga_t, memory_speed, INTEGER, {
            .code   = "memory-speed",
            .name   = "Memory speed",
            .unit   = DMI_UNIT_MHZ
        }),
        {}
    })
};
