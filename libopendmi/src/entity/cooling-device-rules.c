//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cooling-device-internal.h>

void dmi_cooling_device_lint_probe(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_cooling_device_t *info = dmi_entity_info(entity, DMI_TYPE(COOLING_DEVICE));
    if (info == nullptr)
        return;

    if ((info->probe_handle == DMI_HANDLE_INVALID) or
        (info->probe_handle == DMI_HANDLE_UNSUPPORTED))
        return;

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    const dmi_entity_t *probe =
            dmi_registry_lookup(registry, info->probe_handle, DMI_TYPE_ANY, true);
    if ((probe == nullptr) or (dmi_entity_type(probe) == DMI_TYPE_TEMPERATURE_PROBE))
        return;

    dmi_lint_issue(lint, entity, "probe-handle", dmi_lint_entity_offset(lint, entity),
                   "handle 0x%04X refers to a structure of type %d, expected a temperature probe",
                   (unsigned)info->probe_handle, (int)dmi_entity_type(probe));
}
