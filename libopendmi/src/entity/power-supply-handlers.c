//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/utils.h>

#include <opendmi/entity/power-supply-internal.h>

bool dmi_power_supply_link(dmi_entity_t *entity)
{
    dmi_power_supply_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(POWER_SUPPLY));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    bool success = true;
    if (not dmi_registry_resolve(registry, info->voltage_probe_handle, DMI_TYPE(VOLTAGE_PROBE), &info->voltage_probe))
        success = false;
    if (not dmi_registry_resolve(registry, info->cooling_device_handle, DMI_TYPE(COOLING_DEVICE), &info->cooling_device))
        success = false;
    if (not dmi_registry_resolve(registry, info->current_probe_handle, DMI_TYPE(CURRENT_PROBE), &info->current_probe))
        success = false;

    return success;
}
