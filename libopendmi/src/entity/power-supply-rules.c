//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/registry.h>
#include <opendmi/utils.h>

#include <opendmi/entity/power-supply-internal.h>

void dmi_power_supply_lint_probes(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_power_supply_t *info = dmi_entity_info(entity, DMI_TYPE(POWER_SUPPLY));
    if (info == nullptr)
        return;

    const struct
    {
        dmi_handle_t handle;
        dmi_type_t   type;
        const char  *code;
    } probes[] =
    {
        { info->voltage_probe_handle,  DMI_TYPE_VOLTAGE_PROBE,  "voltage-probe-handle"  },
        { info->cooling_device_handle, DMI_TYPE_COOLING_DEVICE, "cooling-device-handle" },
        { info->current_probe_handle,  DMI_TYPE_CURRENT_PROBE,  "current-probe-handle"  }
    };

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    for (size_t i = 0; i < countof(probes); i++) {
        if ((probes[i].handle == DMI_HANDLE_INVALID) or
            (probes[i].handle == DMI_HANDLE_UNSUPPORTED))
            continue;

        const dmi_entity_t *probe =
                dmi_registry_lookup(registry, probes[i].handle, DMI_TYPE_ANY, true);

        if ((probe == nullptr) or (dmi_entity_type(probe) == probes[i].type))
            continue;

        dmi_lint_issue(lint, entity, probes[i].code, dmi_lint_entity_offset(lint, entity),
                       "handle 0x%04X refers to a structure of type %d, expected type %d",
                       (unsigned)probes[i].handle, (int)dmi_entity_type(probe),
                       (int)probes[i].type);
    }
}
