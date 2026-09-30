//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/ami.h>

#include <opendmi/entity/ami/firewire-guid.h>

const dmi_entity_spec_t dmi_ami_firewire_guid_spec =
{
    .type        = DMI_TYPE(ami_firewire_guid),
    .code        = "ami-firewire-guid",
    .name        = "AMI FireWire GUID",
    .description = (const char *[]){
        "Keeps the data holding the GUID of the IEEE 1394 (FireWire) "
        "controller of the board.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x36,
        .decoded_length = sizeof(dmi_ami_firewire_guid_t),
        // Signature and the length of the only known structure tell it, since
        // the layout of the data is not established
        .signature      = DMI_SIGNATURE({
            .length = 0x36,
            .offset = 0x04,
            .bytes  = (const uint8_t[]){ 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10 },
            .size   = 8
        })
    },

    .fields = DMI_FIELDS({
        // Signature is kept, so that the structure is written back with it
        DMI_FIELD_BINARY(dmi_ami_firewire_guid_t, signature, 8),
        DMI_FIELD_BINARY(dmi_ami_firewire_guid_t, data, 40),
        DMI_FIELD(dmi_ami_firewire_guid_t, unknown, dmi_byte_t),
        DMI_FIELD_STRING(dmi_ami_firewire_guid_t, name),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_ami_firewire_guid_t, name, STRING, {
            .code = "name",
            .name = "Name"
        }),
        DMI_ATTRIBUTE(dmi_ami_firewire_guid_t, data, BINARY, {
            .code  = "data",
            .name  = "Data",
            .flags = DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        DMI_ATTRIBUTE(dmi_ami_firewire_guid_t, unknown, INTEGER, {
            .code  = "unknown",
            .name  = "Unknown",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    })
};
