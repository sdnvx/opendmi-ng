//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/mgmt-device-internal.h>

const dmi_entity_spec_t dmi_mgmt_device_spec =
{
    .code            = "mgmt-device",
    .name            = "Management device",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of a "
        "Management Device. A Management Device might control one or more "
        "fans or voltage, current, or temperature probes as defined by one "
        "or more Management Device Component structures.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MGMT_DEVICE),
    .params = {
        .minimum_version = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x0B,
        .decoded_length  = sizeof(dmi_mgmt_device_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_mgmt_device_t, description),
        DMI_FIELD(dmi_mgmt_device_t, type,      dmi_byte_t),
        DMI_FIELD(dmi_mgmt_device_t, addr,      dmi_dword_t),
        DMI_FIELD(dmi_mgmt_device_t, addr_type, dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_mgmt_device_t, description, STRING, {
            .code    = "description",
            .name    = "Description"
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_MGMT_DEVICE_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MGMT_DEVICE_TYPE_UNKNOWN),
            .values  = &dmi_mgmt_device_type_names
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_t, addr, ADDRESS, {
            .code    = "address",
            .name    = "Address",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_t, addr_type, ENUM, {
            .code    = "address-type",
            .name    = "Address type",
            .unspec  = dmi_value_ptr(DMI_MGMT_DEVICE_ADDR_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MGMT_DEVICE_ADDR_TYPE_UNKNOWN),
            .values  = &dmi_mgmt_device_addr_type_names
        }),
        {}
    }),
};
