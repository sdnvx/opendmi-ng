//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/mgmt-device-threshold.h>

#include <opendmi/entity/mgmt-device-component-internal.h>

const dmi_entity_spec_t dmi_mgmt_device_component_spec =
{
    .code            = "mgmt-device-component",
    .name            = "Management device component",
    .description     = (const char *[]){
        "This structure associates a cooling device or environmental probe "
        "with structures that define the controlling hardware device and "
        "(optionally) the component’s thresholds.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MGMT_DEVICE_COMPONENT),
    .params = {
        .minimum_version = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x0B,
        .decoded_length  = sizeof(dmi_mgmt_device_component_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_mgmt_device_component_t, description),
        DMI_FIELD(dmi_mgmt_device_component_t, device_handle,    dmi_word_t),
        DMI_FIELD(dmi_mgmt_device_component_t, component_handle, dmi_word_t),
        DMI_FIELD(dmi_mgmt_device_component_t, threshold_handle, dmi_word_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_mgmt_device_component_t, description, STRING, {
            .code = "description",
            .name = "Description"
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_component_t, device_handle, HANDLE, {
            .code = "device-handle",
            .name = "Device handle",
            .targets = dmi_types(DMI_TYPE_MGMT_DEVICE),
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_component_t, component_handle, HANDLE, {
            .code = "component-handle",
            .name = "Component handle",
            .targets = dmi_types(DMI_TYPE_VOLTAGE_PROBE, DMI_TYPE_COOLING_DEVICE, DMI_TYPE_TEMPERATURE_PROBE, DMI_TYPE_CURRENT_PROBE),
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_component_t, threshold_handle, HANDLE, {
            .code = "threshold-handle",
            .name = "Threshold handle",
            .targets = dmi_types(DMI_TYPE_MGMT_DEVICE_THRESHOLD),
        }),
        {}
    }),

    .handlers = {
        .link   = dmi_mgmt_device_component_link
    }
};
