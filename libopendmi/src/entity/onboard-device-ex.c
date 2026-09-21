//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/onboard-device-ex-internal.h>

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
    .params = {
        .minimum_version = DMI_VERSION(2, 6, 0),
        .minimum_length  = 0x0B,
        .decoded_length  = sizeof(dmi_onboard_device_ex_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_onboard_device_ex_t, designator, STRING),

        // One byte holding the type of the device and whether it is enabled
        DMI_FIELD_BITS(dmi_onboard_device_ex_t, type,       7),
        DMI_FIELD_BITS(dmi_onboard_device_ex_t, is_enabled, 1),
        DMI_FIELD_PAD(BYTE),

        DMI_FIELD(dmi_onboard_device_ex_t, instance, BYTE),
        DMI_FIELD_CUSTOM(dmi_onboard_device_ex_t, address,
                         .decode = dmi_onboard_device_ex_decode_pci_addr),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
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
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("onboard-device-ex.instance", dmi_onboard_device_ex_lint_instance, {
            .name              = "Instances of the devices of a type are unique",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
