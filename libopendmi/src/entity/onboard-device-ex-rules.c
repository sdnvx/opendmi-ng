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

//
// Devices of the same type are told apart by their instances, so no two of
// them share one.
//
void dmi_onboard_device_ex_lint_instance(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_onboard_device_ex_t *info = dmi_entity_info(entity, DMI_TYPE(ONBOARD_DEVICE_EX));
    if (info == nullptr)
        return;

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));
    dmi_registry_iter_t iter;
    dmi_entity_t *other;

    if (not dmi_registry_iter_init(&iter, registry, nullptr))
        return;

    while ((other = dmi_registry_iter_next(&iter)) != nullptr) {
        if ((other == entity) or (dmi_entity_type(other) != DMI_TYPE_ONBOARD_DEVICE_EX))
            continue;

        // Every pair is reported once, by the structure which comes later
        if (dmi_entity_handle(other) >= dmi_entity_handle(entity))
            continue;

        const dmi_onboard_device_ex_t *peer =
                dmi_entity_info(other, DMI_TYPE(ONBOARD_DEVICE_EX));

        if ((peer == nullptr) or (peer->type != info->type) or (peer->instance != info->instance))
            continue;

        dmi_lint_issue(lint, entity, "instance", dmi_lint_entity_offset(lint, entity),
                       "instance %u of the device type is taken by handle 0x%04X",
                       info->instance, (unsigned)dmi_entity_handle(other));
    }
}
