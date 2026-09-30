//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/nic-internal.h>

const dmi_entity_spec_t dmi_hpe_pxe_nic_spec =
{
    .type        = DMI_TYPE(HPE_PXE_NIC),
    .code        = "hpe-pxe-nic",
    .name        = "HP/HPE BIOS PXE NIC PCI and MAC information",
    .description = (const char *[]){
        "Lists the network ports the firmware boots from over PXE.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x04,
        .decoded_length = sizeof(dmi_hpe_nic_info_t)
    },

    // Ports run to the end of the structure, which carries no number of them
    // of its own
    .fields = DMI_FIELDS({
        DMI_FIELD_ARRAY(dmi_hpe_nic_info_t, ports, port_count,
            .stride = 8,
            .fields = DMI_FIELDS({
                DMI_FIELD(dmi_hpe_nic_port_t, devfn, dmi_byte_t),
                DMI_FIELD(dmi_hpe_nic_port_t, bus,   dmi_byte_t),
                DMI_FIELD_BINARY(dmi_hpe_nic_port_t, mac_address, 6),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_hpe_nic_info_t, ports, port_count, STRUCT, {
            .code  = "ports",
            .name  = "Ports",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_hpe_nic_port_t, state, ENUM, {
                    .code   = "state",
                    .name   = "State",
                    .values = &dmi_hpe_nic_state_names
                }),
                DMI_ATTRIBUTE(dmi_hpe_nic_port_t, bus, INTEGER, {
                    .code  = "bus",
                    .name  = "PCI bus",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_hpe_nic_port_t, devfn, INTEGER, {
                    .code  = "devfn",
                    .name  = "PCI device and function",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_hpe_nic_port_t, mac_address, BINARY, {
                    .code  = "mac-address",
                    .name  = "MAC address",
                    .flags = DMI_ATTRIBUTE_FLAG_MAC
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_hpe_nic_derive,
        .cleanup = dmi_hpe_nic_cleanup
    }
};

//
// Type 221 is given to the iSCSI ports up to G7, and is deprecated later
//
const dmi_entity_spec_t dmi_hpe_iscsi_nic_spec =
{
    .type        = DMI_TYPE(HPE_ISCSI_NIC),
    .code        = "hpe-iscsi-nic",
    .name        = "HP/HPE BIOS iSCSI NIC PCI and MAC information",
    .description = (const char *[]){
        "Lists the network ports the firmware boots from over iSCSI.",
        //
        nullptr
    },
    .params = {
        .generations    = { .maximum = DMI_HPE_GEN7 },
        .minimum_length = 0x04,
        .decoded_length = sizeof(dmi_hpe_nic_info_t)
    },

    // Ports run to the end of the structure, which carries no number of them
    // of its own
    .fields = DMI_FIELDS({
        DMI_FIELD_ARRAY(dmi_hpe_nic_info_t, ports, port_count,
            .stride = 8,
            .fields = DMI_FIELDS({
                DMI_FIELD(dmi_hpe_nic_port_t, devfn, dmi_byte_t),
                DMI_FIELD(dmi_hpe_nic_port_t, bus,   dmi_byte_t),
                DMI_FIELD_BINARY(dmi_hpe_nic_port_t, mac_address, 6),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_hpe_nic_info_t, ports, port_count, STRUCT, {
            .code  = "ports",
            .name  = "Ports",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_hpe_nic_port_t, state, ENUM, {
                    .code   = "state",
                    .name   = "State",
                    .values = &dmi_hpe_nic_state_names
                }),
                DMI_ATTRIBUTE(dmi_hpe_nic_port_t, bus, INTEGER, {
                    .code  = "bus",
                    .name  = "PCI bus",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_hpe_nic_port_t, devfn, INTEGER, {
                    .code  = "devfn",
                    .name  = "PCI device and function",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_hpe_nic_port_t, mac_address, BINARY, {
                    .code  = "mac-address",
                    .name  = "MAC address",
                    .flags = DMI_ATTRIBUTE_FLAG_MAC
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_hpe_nic_derive,
        .cleanup = dmi_hpe_nic_cleanup
    }
};

const dmi_name_set_t dmi_hpe_nic_state_names =
{
    .code  = "hpe-nic-state",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_NIC_STATE_INSTALLED,
            .code = "installed",
            .name = "Installed"
        },
        {
            .id   = DMI_HPE_NIC_STATE_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {
            .id   = DMI_HPE_NIC_STATE_NOT_INSTALLED,
            .code = "not-installed",
            .name = "Not installed"
        },
        {}
    })
};

const char *dmi_hpe_nic_state_name(dmi_hpe_nic_state_t value)
{
    return dmi_name_lookup(&dmi_hpe_nic_state_names, (int)value);
}

bool dmi_hpe_nic_derive(dmi_entity_t *entity)
{
    dmi_hpe_nic_info_t *info = dmi_entity_info(entity, DMI_TYPE_ANY);
    if (info == nullptr)
        return false;

    for (size_t i = 0; i < info->port_count; i++) {
        dmi_hpe_nic_port_t *port = &info->ports[i];

        if ((port->bus == 0x00) and (port->devfn == 0x00))
            port->state = DMI_HPE_NIC_STATE_DISABLED;
        else if ((port->bus == 0xFF) and (port->devfn == 0xFF))
            port->state = DMI_HPE_NIC_STATE_NOT_INSTALLED;
        else
            port->state = DMI_HPE_NIC_STATE_INSTALLED;
    }

    return true;
}

void dmi_hpe_nic_cleanup(dmi_entity_t *entity)
{
    dmi_hpe_nic_info_t *info = dmi_entity_info(entity, DMI_TYPE_ANY);
    if (info == nullptr)
        return;

    dmi_free(info->ports);
}
