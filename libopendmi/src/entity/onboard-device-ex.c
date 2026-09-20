//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/onboard-device-ex.h>

static bool dmi_onboard_device_ex_decode(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_onboard_device_ex_spec =
{
    .code            = "onboard-device-ex",
    .name            = "Onboard devices extended information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of devices "
        "that are onboard (soldered onto) a system element, usually the "
        "baseboard.",
        //
        "In general, an entry in this table implies that the firmware has "
        "some level of control over the enablement of the associated device "
        "for use by the system.",
        //
        "To describe multi-function devices, use one type 41 structure per "
        "function, and one type 14 (Group Association) structure referencing "
        "all the function handles.",
        //
        nullptr
    },
    .type            = DMI_TYPE(ONBOARD_DEVICE_EX),
    .minimum_version = DMI_VERSION(2, 6, 0),
    .minimum_length  = 0x0B,
    .decoded_length  = sizeof(dmi_onboard_device_ex_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_onboard_device_ex_t, designator, STRING, {
            .code    = "designator",
            .name    = "Designator"
        }),
        DMI_ATTRIBUTE(dmi_onboard_device_ex_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_ONBOARD_DEVICE_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_ONBOARD_DEVICE_TYPE_UNKNOWN),
            .values  = &dmi_onboard_device_type_names
        }),
        DMI_ATTRIBUTE(dmi_onboard_device_ex_t, is_enabled, BOOL, {
            .code    = "is-enabled",
            .name    = "Enabled"
        }),
        DMI_ATTRIBUTE(dmi_onboard_device_ex_t, instance, INTEGER, {
            .code    = "instance",
            .name    = "Instance"
        }),
        DMI_ATTRIBUTE(dmi_onboard_device_ex_t, address, STRUCT, {
            .code    = "address",
            .name    = "Address",
            .attrs   = dmi_pci_addr_attrs
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_onboard_device_ex_decode
    }
};

static bool dmi_onboard_device_ex_decode(dmi_entity_t *entity)
{
    dmi_onboard_device_ex_t *info;
    dmi_onboard_device_instance_details_t details;

    info = dmi_entity_info(entity, DMI_TYPE(ONBOARD_DEVICE_EX));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    bool status =
        dmi_stream_decode_str(stream, &info->designator) and
        dmi_stream_decode(stream, dmi_byte_t, &details.__value) and
        dmi_stream_decode(stream, dmi_byte_t, &info->instance) and
        dmi_pci_addr_decode(stream, &info->address);

    if (not status)
        return false;

    info->type       = details.type;
    info->is_enabled = details.is_enabled;

    return true;
}
