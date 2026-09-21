//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>

#include <opendmi/entity/memory-module.h>
#include <opendmi/entity/memory-controller-internal.h>

static void dmi_memory_controller_lint_size(
        dmi_lint_t                     *lint,
        const dmi_entity_t             *entity,
        dmi_handle_t                    handle,
        const char                     *name,
        const dmi_memory_module_size_t *size,
        dmi_size_t                      maximum);

static void dmi_memory_controller_format_size(char *buffer, size_t length, dmi_size_t size);

//
// Modules of a controller are no larger than the largest module the
// controller supports, whether installed or enabled.
//
void dmi_memory_controller_lint_module_size(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_memory_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CONTROLLER));
    if ((info == nullptr) or (info->module_handles == nullptr))
        return;

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    for (size_t i = 0; i < info->slot_count; i++) {
        dmi_handle_t handle = info->module_handles[i];

        const dmi_entity_t *module_entity =
                dmi_registry_lookup(registry, handle, DMI_TYPE(MEMORY_MODULE), true);
        if (module_entity == nullptr)
            continue;

        // Memory module may be left undecoded
        const dmi_memory_module_t *module = dmi_entity_info(module_entity, DMI_TYPE(MEMORY_MODULE));
        if (module == nullptr)
            continue;

        dmi_memory_controller_lint_size(lint, entity, handle, "installed",
                                        &module->installed_size, info->maximum_module_size);
        dmi_memory_controller_lint_size(lint, entity, handle, "enabled",
                                        &module->enabled_size, info->maximum_module_size);
    }
}

static void dmi_memory_controller_lint_size(
        dmi_lint_t                     *lint,
        const dmi_entity_t             *entity,
        dmi_handle_t                    handle,
        const char                     *name,
        const dmi_memory_module_size_t *size,
        dmi_size_t                      maximum)
{
    if ((size->status != DMI_MEMORY_MODULE_SIZE_STATUS_PRESENT) or (size->value <= maximum))
        return;

    char actual[32];
    char largest[32];

    dmi_memory_controller_format_size(actual, sizeof(actual), size->value);
    dmi_memory_controller_format_size(largest, sizeof(largest), maximum);

    dmi_lint_issue(lint, entity, "module-handles", dmi_lint_entity_offset(lint, entity),
                   "module 0x%04X: %s size of %s is larger than the %s the controller supports",
                   (unsigned)handle, name, actual, largest);
}

//
// Sizes are written in the largest binary unit they are a whole number of.
//
static void dmi_memory_controller_format_size(char *buffer, size_t length, dmi_size_t size)
{
    static const char *const units[] = { "bytes", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB" };

    size_t unit = 0;

    while ((unit + 1 < countof(units)) and (size >= 1024) and ((size % 1024) == 0)) {
        size /= 1024;
        unit++;
    }

    snprintf(buffer, length, "%llu %s", (unsigned long long)size, units[unit]);
}
