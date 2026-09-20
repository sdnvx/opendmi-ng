//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>
#include <opendmi/entity/intel/rsd-cabled-pcie.h>

static bool dmi_intel_rsd_cabled_pcie_decode(dmi_entity_t *entity);
static void dmi_intel_rsd_cabled_pcie_cleanup(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_intel_rsd_cabled_pcie_spec =
{
    .type            = DMI_TYPE(INTEL_RSD_CABLED_PCIE),
    .code            = "intel-rsd-cabled-pcie",
    .name            = "Intel RSD cabled PCIe port information",
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x0A,
    .decoded_length  = sizeof(dmi_intel_rsd_cabled_pcie_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_intel_rsd_cabled_pcie_t, pci_slot_id, INTEGER, {
            .code = "pci-slot-id",
            .name = "PCI Slot ID"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_cabled_pcie_t, link_width, INTEGER, {
            .code = "link-width",
            .name = "Link width"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_intel_rsd_cabled_pcie_t, ports, port_count, STRUCT, {
            .code  = "ports",
            .name  = "Ports",
            .attrs = (const dmi_attribute_t[]){
                DMI_ATTRIBUTE(dmi_intel_rsd_cabled_pcie_port_t, index, INTEGER, {
                    .code = "index",
                    .name = "Index"
                }),
                DMI_ATTRIBUTE(dmi_intel_rsd_cabled_pcie_port_t, start_lane, INTEGER, {
                    .code = "start-lane",
                    .name = "Start lane"
                }),
                {}
            }
        }),
        {}
    },
    .handlers = {
        .decode  = dmi_intel_rsd_cabled_pcie_decode,
        .cleanup = dmi_intel_rsd_cabled_pcie_cleanup
    }
};

static bool dmi_intel_rsd_cabled_pcie_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_cabled_pcie_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_CABLED_PCIE));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->pci_slot_id) and
        dmi_stream_decode(stream, dmi_byte_t, &info->link_width) and
        dmi_stream_decode(stream, dmi_byte_t, &info->port_count);
    if (not status or (info->port_count == 0))
        return false;

    info->ports = dmi_alloc_array(context,
                                  sizeof(dmi_intel_rsd_cabled_pcie_port_t),
                                  info->port_count);
    if (info->ports == nullptr)
        return false;

    for (unsigned i = 0; i < info->port_count; i++) {
        dmi_intel_rsd_cabled_pcie_port_t *port = &info->ports[i];

        status =
            dmi_stream_decode(stream, dmi_byte_t, &port->index) and
            dmi_stream_decode(stream, dmi_byte_t, &port->start_lane);
        if (not status)
            return false;
    }

    return true;
}

static void dmi_intel_rsd_cabled_pcie_cleanup(dmi_entity_t *entity)
{
    dmi_intel_rsd_cabled_pcie_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_CABLED_PCIE));
    if (info == nullptr)
        return;

    dmi_free(info->ports);
}
