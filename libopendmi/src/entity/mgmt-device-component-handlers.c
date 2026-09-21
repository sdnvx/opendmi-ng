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

bool dmi_mgmt_device_component_link(dmi_entity_t *entity)
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

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    bool success = true;

    // Management device and component are required, threshold is optional
    if (not dmi_registry_resolve(registry, info->device_handle, DMI_TYPE(MGMT_DEVICE), &info->device)) {
        success = false;
    } else if (info->device == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND,
                           "Management device component 0x%04x: management device is not specified",
                           dmi_entity_handle(entity));
        success = false;
    }

    if (not dmi_registry_resolve_any(registry, info->component_handle, dmi_component_types, &info->component)) {
        success = false;
    } else if (info->component == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND,
                           "Management device component 0x%04x: component is not specified",
                           dmi_entity_handle(entity));
        success = false;
    }

    if (not dmi_registry_resolve(registry, info->threshold_handle, DMI_TYPE(MGMT_DEVICE_THRESHOLD), &info->threshold))
        success = false;

    // Units of threshold values are defined by the component
    if ((info->threshold != nullptr) and (info->component != nullptr))
        dmi_mgmt_device_threshold_set_component(info->threshold, info->component->type);

    return success;
}
