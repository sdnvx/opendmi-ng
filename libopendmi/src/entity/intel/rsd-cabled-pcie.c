//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-cabled-pcie-internal.h>

const dmi_entity_spec_t dmi_intel_rsd_cabled_pcie_spec =
{
    .type            = DMI_TYPE(INTEL_RSD_CABLED_PCIE),
    .code            = "intel-rsd-cabled-pcie",
    .name            = "Intel RSD cabled PCIe port information",
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x0A,
        .decoded_length  = sizeof(dmi_intel_rsd_cabled_pcie_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_rsd_cabled_pcie_t, pci_slot_id, WORD),
        DMI_FIELD(dmi_intel_rsd_cabled_pcie_t, link_width,  BYTE),

        DMI_FIELD_ARRAY(dmi_intel_rsd_cabled_pcie_t, ports, port_count,
            .count_type = DMI_FIELD_TYPE_BYTE,
            .fields     = DMI_FIELDS({
                DMI_FIELD(dmi_intel_rsd_cabled_pcie_port_t, index,      BYTE),
                DMI_FIELD(dmi_intel_rsd_cabled_pcie_port_t, start_lane, BYTE),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_rsd_cabled_pcie_t, pci_slot_id, INTEGER, {
            .code = "pci-slot-id",
            .name = "PCI slot ID"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_cabled_pcie_t, link_width, INTEGER, {
            .code = "link-width",
            .name = "Link width"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_intel_rsd_cabled_pcie_t, ports, port_count, STRUCT, {
            .code  = "ports",
            .name  = "Ports",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_intel_rsd_cabled_pcie_port_t, index, INTEGER, {
                    .code = "index",
                    .name = "Index"
                }),
                DMI_ATTRIBUTE(dmi_intel_rsd_cabled_pcie_port_t, start_lane, INTEGER, {
                    .code = "start-lane",
                    .name = "Start lane"
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_intel_rsd_cabled_pcie_cleanup
    }
};
