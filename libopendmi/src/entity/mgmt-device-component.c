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

#include <opendmi/entity/mgmt-device-component.h>
#include <opendmi/entity/mgmt-device-threshold.h>

static bool dmi_mgmt_device_component_decode(dmi_entity_t *entity);
static bool dmi_mgmt_device_component_link(dmi_entity_t *entity);

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
    .minimum_version = DMI_VERSION(2, 3, 0),
    .minimum_length  = 0x0B,
    .decoded_length  = sizeof(dmi_mgmt_device_component_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_mgmt_device_component_t, description, STRING, {
            .code = "description",
            .name = "Description"
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_component_t, device_handle, HANDLE, {
            .code = "device-handle",
            .name = "Device handle"
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_component_t, component_handle, HANDLE, {
            .code = "component-handle",
            .name = "Component handle"
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_component_t, threshold_handle, HANDLE, {
            .code = "threshold-handle",
            .name = "Threshold handle"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_mgmt_device_component_decode,
        .link   = dmi_mgmt_device_component_link
    }
};

static bool dmi_mgmt_device_component_decode(dmi_entity_t *entity)
{
    dmi_mgmt_device_component_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_COMPONENT));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    return
        dmi_stream_decode_str(stream, &info->description) and
        dmi_stream_decode(stream, dmi_handle_t, &info->device_handle) and
        dmi_stream_decode(stream, dmi_handle_t, &info->component_handle) and
        dmi_stream_decode(stream, dmi_handle_t, &info->threshold_handle);
}

static bool dmi_mgmt_device_component_link(dmi_entity_t *entity)
{
    static const dmi_type_t dmi_component_types[] = {
        DMI_TYPE(COOLING_DEVICE),
        DMI_TYPE(TEMPERATURE_PROBE),
        DMI_TYPE(VOLTAGE_PROBE),
        DMI_TYPE(CURRENT_PROBE),
        DMI_TYPE_INVALID
    };

    dmi_mgmt_device_component_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_COMPONENT));
    if (info == nullptr)
        return false;

    dmi_context_t *context = entity->context;
    dmi_registry_t *registry = context->state.registry;
    bool success = true;

    // Management device and component are required, threshold is optional
    if (not dmi_registry_resolve(registry, info->device_handle, DMI_TYPE(MGMT_DEVICE), &info->device)) {
        success = false;
    } else if (info->device == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND,
                           "Management device component 0x%04x: management device is not specified",
                           entity->handle);
        success = false;
    }

    if (not dmi_registry_resolve_any(registry, info->component_handle, dmi_component_types, &info->component)) {
        success = false;
    } else if (info->component == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND,
                           "Management device component 0x%04x: component is not specified",
                           entity->handle);
        success = false;
    }

    if (not dmi_registry_resolve(registry, info->threshold_handle, DMI_TYPE(MGMT_DEVICE_THRESHOLD), &info->threshold))
        success = false;

    // Units of threshold values are defined by the component
    if ((info->threshold != nullptr) and (info->component != nullptr))
        dmi_mgmt_device_threshold_set_component(info->threshold, info->component->type);

    return success;
}
