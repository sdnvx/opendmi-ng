//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/cisco.h>

#include <opendmi/entity/cisco/slot-buses.h>

const dmi_entity_spec_t dmi_cisco_slot_buses_spec =
{
    .type        = DMI_TYPE(cisco_slot_buses),
    .code        = "cisco-slot-buses",
    .name        = "Cisco PCI slot buses",
    .description = (const char *[]){
        "Lists the PCI buses behind a slot, presumably.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x06,
        .decoded_length = sizeof(dmi_cisco_slot_buses_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_cisco_slot_buses_t, slot),
        DMI_FIELD_ARRAY(dmi_cisco_slot_buses_t, buses, bus_count,
            .count_length = sizeof(dmi_byte_t),
            .fields       = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_cisco_slot_buses_t, buses, dmi_byte_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_cisco_slot_buses_t, slot, STRING, {
            .code = "slot",
            .name = "Slot"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_cisco_slot_buses_t, buses, bus_count, INTEGER, {
            .code  = "buses",
            .name  = "PCI buses",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};
