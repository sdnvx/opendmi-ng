//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/module.h>
#include <opendmi/vendor.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

#include "context-internal.h"

#include <opendmi/entity/baseboard.h>
#include <opendmi/entity/firmware.h>
#include <opendmi/entity/processor.h>
#include <opendmi/entity/system.h>

/**
 * @internal
 * @brief Enable the modules of the platform.
 *
 * @details
 * Modules whose types conflict with the modules enabled before are skipped.
 *
 * @return `false` if memory is exhausted, `true` otherwise.
 */
static bool dmi_setup_platform_modules(dmi_context_t *context);

/**
 * @internal
 * @brief Tell the platform from the firmware and system information, which
 * may be missing.
 *
 * @return `false` if memory is exhausted, `true` otherwise.
 */
static bool dmi_platform_detect(dmi_context_t *context);

/**
 * @internal
 * @brief Get decoded information of the first structure of a type, which may
 * be missing or malformed.
 */
static const void *dmi_platform_info(dmi_registry_t *registry, const dmi_type_t *type);

/**
 * @internal
 * @brief Tell the vendor from a vendor name, `DMI_VENDOR_OTHER` if unknown.
 */
static dmi_vendor_t dmi_platform_vendor(const char *name);

bool dmi_add_extension(dmi_context_t *context, const dmi_module_t *module)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);

    if (module == nullptr)
        return dmi_trace_argument_null(context, module);

    dmi_log_info(context, "Enabling extension: %s", module->name);

    if (dmi_has_extension(context, module)) {
        dmi_error_raise_ex(context, DMI_ERROR_MODULE_CONFLICT, "%s: already enabled", module->name);
        return false;
    }

    // Module is checked against the enabled ones before it is registered, and
    // the type map is updated only once it is
    dmi_type_candidates_t *map = dmi_types_create(context);
    if (map == nullptr)
        return false;

    bool success = false;
    do {
        if (not dmi_types_map(context, module, true, map))
            break;

        if (not dmi_vector_push(&context->modules, (uintptr_t)module))
            break;

        memcpy(context->type_map, map, sizeof(*map) * (DMI_TYPE_ID_MAX + 1));

        success = true;
    } while (false);

    dmi_free(map);

    return success;
}

bool dmi_has_extension(const dmi_context_t *context, const dmi_module_t *module)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);
    if (module == nullptr)
        return dmi_trace_argument_null(nullptr, module);

    for (size_t i = 0; i < context->modules.length; i++) {
        if (context->modules.data[i] == (uintptr_t)module)
            return true;
    }

    for (size_t i = 0; i < context->state.modules.length; i++) {
        if (context->state.modules.data[i] == (uintptr_t)module)
            return true;
    }

    return false;
}

const dmi_platform_t *dmi_get_platform(const dmi_context_t *context)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context, nullptr);

    return dmi_context_platform(context);
}

bool dmi_set_platform(dmi_context_t *context, const dmi_platform_t *platform)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);

    dmi_platform_t *copy = nullptr;
    if (platform != nullptr) {
        copy = dmi_platform_clone(platform);
        if (copy == nullptr)
            return dmi_trace_out_of_memory(context);
    }

    // Types are mapped for the new platform, and the previous one is restored
    // if they cannot be
    dmi_platform_t *previous = context->platform;
    context->platform = copy;

    if (not dmi_types_rebuild(context, nullptr)) {
        context->platform = previous;
        dmi_platform_destroy(copy);
        return false;
    }

    dmi_platform_destroy(previous);

    return true;
}

bool dmi_setup_extensions(dmi_context_t *context)
{
    if (not dmi_setup_vendor(context))
        return false;

    // Platform told from the data is kept even if another one has been set,
    // so that it is used again once that one is unset
    if (not dmi_platform_detect(context))
        return false;

    const dmi_platform_t *platform = dmi_context_platform(context);
    dmi_log_info(context, "Platform: %s, family %s, generation %u%s",
                 (platform->product != nullptr) ? platform->product : "unknown",
                 (platform->family != nullptr) ? platform->family : "unknown",
                 platform->generation,
                 (context->platform != nullptr) ? " (set explicitly)" : "");
    dmi_log_info(context, "Platform vendors: firmware %s, system %s, baseboard %s, processor %s",
                 dmi_vendor_name(platform->firmware_vendor),
                 dmi_vendor_name(platform->system_vendor),
                 dmi_vendor_name(platform->baseboard_vendor),
                 dmi_vendor_name(platform->processor_vendor));

    if (context->flags & DMI_CONTEXT_FLAG_AUTO_MODULES) {
        if (not dmi_setup_platform_modules(context))
            return false;
    }

    // Modules enabled before the context has been opened are mapped for the
    // platform only now
    return dmi_types_rebuild(context, nullptr);
}

bool dmi_setup_vendor(dmi_context_t *context)
{
    dmi_entity_t *entity;
    const dmi_firmware_t *firmware;
    const dmi_vendor_spec_t *vendor;

    dmi_log_debug(context, "Detecting SMBIOS vendor...");

    entity = dmi_registry_lookup_first(context->state.registry, DMI_TYPE(firmware), true);
    if (entity == nullptr) {
        if ((context->flags & DMI_CONTEXT_FLAG_STRICT) == 0) {
            dmi_log_notice(context, dmi_error_message(DMI_ERROR_FIRMWARE_INFO_NOT_FOUND));
            return true;
        }

        dmi_error_raise(context, DMI_ERROR_FIRMWARE_INFO_NOT_FOUND);
        return false;
    }

    if (not dmi_entity_decode(entity)) {
        if ((context->flags & DMI_CONTEXT_FLAG_STRICT) == 0) {
            dmi_log_notice(context, "Unable to decode firmware information, vendor is unknown");
            return true;
        }

        return false;
    }

    firmware = dmi_cast(firmware, entity->info);
    vendor   = dmi_vendor_detect(firmware->vendor);

    context->state.vendor_name = firmware->vendor;
    if (vendor != nullptr)
        context->state.vendor = vendor->id;

    dmi_log_info(context, "SMBIOS vendor: %s (%s)",
                 dmi_vendor_name(context->state.vendor), firmware->vendor);

    return true;
}

const dmi_platform_t *dmi_context_platform(const dmi_context_t *context)
{
    return (context->platform != nullptr) ? context->platform : context->state.platform;
}

static bool dmi_setup_platform_modules(dmi_context_t *context)
{
    const dmi_platform_t *platform = dmi_context_platform(context);

    // Map is only built to check modules for conflicts
    dmi_type_candidates_t *map = dmi_types_create(context);
    if (map == nullptr)
        return false;

    bool success = true;
    for (const dmi_module_t *module = dmi_module_next(nullptr); module != nullptr; module = dmi_module_next(module)) {
        if ((module->platforms == nullptr) or dmi_has_extension(context, module))
            continue;

        bool matched = false;
        for (const dmi_platform_match_t *match = module->platforms; match->firmware_vendor != DMI_VENDOR_INVALID; match++) {
            if (dmi_platform_match(platform, match)) {
                matched = true;
                break;
            }
        }

        if (not matched)
            continue;

        // Modules enabled explicitly take precedence
        if (not dmi_types_map(context, module, false, map)) {
            dmi_log_notice(context, "Extension %s conflicts with enabled extensions, skipping",
                           module->name);
            continue;
        }

        if (not dmi_vector_push(&context->state.modules, (uintptr_t)module)) {
            success = false;
            break;
        }

        dmi_log_info(context, "Enabling extension for the platform: %s", module->name);
    }

    dmi_free(map);

    return success;
}

static bool dmi_platform_detect(dmi_context_t *context)
{
    dmi_registry_t *registry = context->state.registry;

    dmi_platform_t *platform = dmi_platform_create(context);
    if (platform == nullptr)
        return false;

    platform->firmware_vendor = context->state.vendor;

    // System, baseboard and processor information is optional, and the
    // platform is told without it
    const char *product = nullptr;
    const dmi_system_t *system = dmi_platform_info(registry, DMI_TYPE(system));
    if (system != nullptr) {
        platform->system_vendor = dmi_platform_vendor(system->vendor);
        product = system->product;
    }

    const dmi_baseboard_t *baseboard = dmi_platform_info(registry, DMI_TYPE(baseboard));
    if (baseboard != nullptr)
        platform->baseboard_vendor = dmi_platform_vendor(baseboard->vendor);

    // Sockets may be empty, so the first processor of a known vendor is taken
    dmi_registry_iter_t iter;
    dmi_registry_iter_initialize(&iter, registry, nullptr);

    dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        if ((entity->type_id != DMI_TYPE_ID(PROCESSOR)) or not dmi_entity_decode(entity))
            continue;

        const dmi_processor_t *processor = dmi_entity_info(entity, DMI_TYPE(processor));
        platform->processor_vendor = dmi_platform_vendor(processor->vendor);
        if (platform->processor_vendor != DMI_VENDOR_OTHER)
            break;
    }

    bool success = dmi_platform_set_product(platform, product);

    const dmi_vendor_spec_t *vendor = dmi_vendor_detect(context->state.vendor_name);
    if (success and (vendor != nullptr) and (vendor->detect != nullptr) and (product != nullptr))
        success = vendor->detect(platform);

    if (not success) {
        dmi_platform_destroy(platform);
        return false;
    }

    dmi_platform_destroy(context->state.platform);
    context->state.platform = platform;

    return true;
}

static const void *dmi_platform_info(dmi_registry_t *registry, const dmi_type_t *type)
{
    dmi_entity_t *entity = dmi_registry_lookup_first(registry, type, true);

    if ((entity == nullptr) or not dmi_entity_decode(entity))
        return nullptr;

    return dmi_entity_info(entity, type);
}

static dmi_vendor_t dmi_platform_vendor(const char *name)
{
    const dmi_vendor_spec_t *vendor = dmi_vendor_detect(name);

    return (vendor != nullptr) ? vendor->id : DMI_VENDOR_OTHER;
}
