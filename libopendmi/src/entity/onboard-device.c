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

#include <opendmi/entity/onboard-device-internal.h>

const dmi_entity_spec_t dmi_onboard_device_spec =
{
    .code            = "onboard-device",
    .name            = "Onboard devices information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of devices "
        "that are onboard (soldered onto) a system element, usually the "
        "baseboard. In general, an entry in this table implies that the "
        "firmware has some level of control over the enabling of the "
        "associated device for use by the system.",
        //
        "Note: This structure is obsolete starting with version 2.6 of SMBIOS "
        "specification; the Onboard Devices Extended Information (Type 41) "
        "structure should be used instead. Firmware providers can choose to "
        "implement both types to allow existing SMBIOS browsers to properly "
        "display the system’s onboard devices information.",
        nullptr
    },
    .type            = DMI_TYPE(ONBOARD_DEVICE),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x06,
        .decoded_length  = sizeof(dmi_onboard_device_t)
    },

    .fields = DMI_FIELDS({
        // Instances run to the end of the structure, which carries no number
        // of them of its own
        DMI_FIELD_ARRAY(dmi_onboard_device_t, instances, instance_count,
            .stride = 2 * sizeof(dmi_byte_t),
            .fields = DMI_FIELDS({
                DMI_FIELD_BITS(dmi_onboard_device_instance_t, type,       7),
                DMI_FIELD_BITS(dmi_onboard_device_instance_t, is_enabled, 1),
                DMI_FIELD_PAD(dmi_byte_t),

                DMI_FIELD_STRING(dmi_onboard_device_instance_t, description),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_onboard_device_t, instance_count, INTEGER, {
            .code  = "instance-count",
            .name  = "Instance count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_onboard_device_t, instances, instance_count, STRUCT, {
            .code  = "instances",
            .name  = "Instances",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_onboard_device_instance_t, type, ENUM, {
                    .code    = "type",
                    .name    = "Type",
                    .unspec  = dmi_value_ptr(DMI_ONBOARD_DEVICE_TYPE_UNSPEC),
                    .unknown = dmi_value_ptr(DMI_ONBOARD_DEVICE_TYPE_UNKNOWN),
                    .values  = &dmi_onboard_device_type_names
                }),
                DMI_ATTRIBUTE(dmi_onboard_device_instance_t, is_enabled, BOOL, {
                    .code    = "is-enabled",
                    .name    = "Enabled"
                }),
                DMI_ATTRIBUTE(dmi_onboard_device_instance_t, description, STRING, {
                    .code    = "description",
                    .name    = "Description"
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_onboard_device_cleanup
    }
};
