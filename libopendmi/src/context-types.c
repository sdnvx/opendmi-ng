//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

#include "context-internal.h"

#include <opendmi/entity/additional-info.h>
#include <opendmi/entity/baseboard.h>
#include <opendmi/entity/battery.h>
#include <opendmi/entity/bis-entry-point.h>
#include <opendmi/entity/cache.h>
#include <opendmi/entity/chassis.h>
#include <opendmi/entity/cooling-device.h>
#include <opendmi/entity/current-probe.h>
#include <opendmi/entity/firmware.h>
#include <opendmi/entity/firmware-inventory.h>
#include <opendmi/entity/firmware-language.h>
#include <opendmi/entity/group-assoc.h>
#include <opendmi/entity/hardware-security.h>
#include <opendmi/entity/ipmi-device.h>
#include <opendmi/entity/memory-array.h>
#include <opendmi/entity/memory-array-addr.h>
#include <opendmi/entity/memory-channel.h>
#include <opendmi/entity/memory-controller.h>
#include <opendmi/entity/memory-device.h>
#include <opendmi/entity/memory-device-addr.h>
#include <opendmi/entity/memory-error-32.h>
#include <opendmi/entity/memory-error-64.h>
#include <opendmi/entity/memory-module.h>
#include <opendmi/entity/mgmt-controller.h>
#include <opendmi/entity/mgmt-device.h>
#include <opendmi/entity/mgmt-device-component.h>
#include <opendmi/entity/mgmt-device-threshold.h>
#include <opendmi/entity/oem-strings.h>
#include <opendmi/entity/onboard-device.h>
#include <opendmi/entity/onboard-device-ex.h>
#include <opendmi/entity/oob-remote-access.h>
#include <opendmi/entity/pointing-device.h>
#include <opendmi/entity/port-connector.h>
#include <opendmi/entity/power-controls.h>
#include <opendmi/entity/power-supply.h>
#include <opendmi/entity/probe.h>
#include <opendmi/entity/processor.h>
#include <opendmi/entity/processor-ex.h>
#include <opendmi/entity/slot.h>
#include <opendmi/entity/string-property.h>
#include <opendmi/entity/system.h>
#include <opendmi/entity/system-boot.h>
#include <opendmi/entity/system-config.h>
#include <opendmi/entity/system-event-log.h>
#include <opendmi/entity/system-reset.h>
#include <opendmi/entity/temperature-probe.h>
#include <opendmi/entity/tpm-device.h>
#include <opendmi/entity/voltage-probe.h>

/**
 * @internal
 * @brief Get enabled module by index.
 *
 * @details
 * Modules enabled explicitly go first, followed by the ones enabled for the
 * platform, and by @p extra, the module which is being enabled.
 *
 * @return Module, or @c nullptr past the last one.
 */
static const dmi_module_t *dmi_module_at(
        const dmi_context_t *context,
        const dmi_module_t  *extra,
        size_t               index);

/**
 * @internal
 * @brief Get a type number a specification is mapped to.
 *
 * @details
 * The relocations of the enabled modules and of @p extra give some
 * specifications type numbers of their own, one or several, while the rest
 * are mapped to their types.
 *
 * @param[in] context Context descriptor.
 * @param[in] extra   Module which is being enabled, or @c nullptr.
 * @param[in] spec    Specification to map.
 * @param[in] index   Index of the type number, from zero.
 *
 * @return Type number, or `DMI_TYPE_ID_INVALID` if the specification is
 *         mapped to fewer type numbers than @p index.
 */
static dmi_type_id_t dmi_spec_relocate(
        const dmi_context_t     *context,
        const dmi_module_t      *extra,
        const dmi_entity_spec_t *spec,
        size_t                   index);

/**
 * @internal
 * @brief Map a specification to a type.
 *
 * @details
 * Specification without a signature takes the first candidate of the type,
 * and the ones with signatures take the rest.
 *
 * @return `true` on success, `false` if the type has another specification
 *         without a signature, or no room for one more with a signature.
 */
static bool dmi_types_map_one(
        dmi_type_candidates_t   *candidates,
        const dmi_entity_spec_t *spec,
        bool                     yield);

static const dmi_entity_spec_t dmi_inactive_spec =
{
    .code        = "inactive",
    .name        = "Inactive",
    .description = (const char *[]){
        "This structure definition supports a system implementation where "
        "the SMBIOS structure-table is a superset of all supported system "
        "attributes and provides a standard mechanism for the platform "
        "firmware to signal that a structure is currently inactive and "
        "should not be interpreted by the upper-level software.",
        //
        "For example, a portable system might include System Slot "
        "structures that are reported only when the portable is docked. An "
        "undocked system would report those structures as Inactive. When "
        "the system is docked, the system-specific software would change "
        "the Type structure from Inactive to the System Slot equivalent.",
        //
        "Upper-level software that interprets the SMBIOS structure-table "
        "should bypass an Inactive structure just as it would for a "
        "structure type that the software does not recognize.",
        //
        nullptr
    },
    .type        = DMI_TYPE(inactive)
};

static const dmi_entity_spec_t dmi_end_of_table_spec =
{
    .code        = "end-of-table",
    .name        = "End of table",
    .description = (const char *[]){
        "This structure type identifies the end of the structure table that "
        "might be earlier than the last byte within the buffer specified by "
        "the structure.",
        //
        "To ensure backward compatibility with management software written "
        "to previous versions of SMBIOS specification, a system implementation "
        "should use the end-of-table indicator in a manner similar to the "
        "Inactive (Type 126) structure type; the structure table is still "
        "reported as a fixed-length, and the entire length of the table is "
        "still indexable. If the end-of-table indicator is used in the last "
        "physical structure in a table, the field’s length is encoded as 4.",
        //
        nullptr
    },
    .type        = DMI_TYPE(end_of_table)
};

/**
 * @brief Predefined entity specifications map.
 */
static const dmi_entity_spec_t *dmi_entity_specs[] =
{
    [DMI_TYPE_ID_FIRMWARE]                = &dmi_firmware_spec,
    [DMI_TYPE_ID_SYSTEM]                  = &dmi_system_spec,
    [DMI_TYPE_ID_BASEBOARD]               = &dmi_baseboard_spec,
    [DMI_TYPE_ID_CHASSIS]                 = &dmi_chassis_spec,
    [DMI_TYPE_ID_PROCESSOR]               = &dmi_processor_spec,
    [DMI_TYPE_ID_MEMORY_CONTROLLER]       = &dmi_memory_controller_spec,
    [DMI_TYPE_ID_MEMORY_MODULE]           = &dmi_memory_module_spec,
    [DMI_TYPE_ID_CACHE]                   = &dmi_cache_spec,
    [DMI_TYPE_ID_PORT_CONNECTOR]          = &dmi_port_connector_spec,
    [DMI_TYPE_ID_SYSTEM_SLOTS]            = &dmi_slot_spec,
    [DMI_TYPE_ID_ONBOARD_DEVICE]          = &dmi_onboard_device_spec,
    [DMI_TYPE_ID_OEM_STRINGS]             = &dmi_oem_strings_spec,
    [DMI_TYPE_ID_SYSTEM_CONFIG_OPTIONS]   = &dmi_system_config_opts_spec,
    [DMI_TYPE_ID_FIRMWARE_LANGUAGE]       = &dmi_firmware_language_spec,
    [DMI_TYPE_ID_GROUP_ASSOC]             = &dmi_group_assoc_spec,
    [DMI_TYPE_ID_SYSTEM_EVENT_LOG]        = &dmi_system_event_log_spec,
    [DMI_TYPE_ID_MEMORY_ARRAY]            = &dmi_memory_array_spec,
    [DMI_TYPE_ID_MEMORY_DEVICE]           = &dmi_memory_device_spec,
    [DMI_TYPE_ID_MEMORY_ERROR_32]         = &dmi_memory_error_32_spec,
    [DMI_TYPE_ID_MEMORY_ARRAY_ADDR]       = &dmi_memory_array_addr_spec,
    [DMI_TYPE_ID_MEMORY_DEVICE_ADDR]      = &dmi_memory_device_addr_spec,
    [DMI_TYPE_ID_POINTING_DEVICE]         = &dmi_pointing_device_spec,
    [DMI_TYPE_ID_PORTABLE_BATTERY]        = &dmi_battery_spec,
    [DMI_TYPE_ID_SYSTEM_RESET]            = &dmi_system_reset_spec,
    [DMI_TYPE_ID_HARDWARE_SECURITY]       = &dmi_hardware_security_spec,
    [DMI_TYPE_ID_POWER_CONTROLS]          = &dmi_power_controls_spec,
    [DMI_TYPE_ID_VOLTAGE_PROBE]           = &dmi_voltage_probe_spec,
    [DMI_TYPE_ID_COOLING_DEVICE]          = &dmi_cooling_device_spec,
    [DMI_TYPE_ID_TEMPERATURE_PROBE]       = &dmi_temperature_probe_spec,
    [DMI_TYPE_ID_CURRENT_PROBE]           = &dmi_current_probe_spec,
    [DMI_TYPE_ID_OOB_REMOTE_ACCESS]       = &dmi_oob_remote_access_spec,
    [DMI_TYPE_ID_BIS_ENTRY_POINT]         = &dmi_bis_entry_point_spec,
    [DMI_TYPE_ID_SYSTEM_BOOT]             = &dmi_system_boot_spec,
    [DMI_TYPE_ID_MEMORY_ERROR_64]         = &dmi_memory_error_64_spec,
    [DMI_TYPE_ID_MGMT_DEVICE]             = &dmi_mgmt_device_spec,
    [DMI_TYPE_ID_MGMT_DEVICE_COMPONENT]   = &dmi_mgmt_device_component_spec,
    [DMI_TYPE_ID_MGMT_DEVICE_THRESHOLD]   = &dmi_mgmt_device_threshold_spec,
    [DMI_TYPE_ID_MEMORY_CHANNEL]          = &dmi_memory_channel_spec,
    [DMI_TYPE_ID_IPMI_DEVICE]             = &dmi_ipmi_device_spec,
    [DMI_TYPE_ID_POWER_SUPPLY]            = &dmi_power_supply_spec,
    [DMI_TYPE_ID_ADDITIONAL_INFO]         = &dmi_additional_info_spec,
    [DMI_TYPE_ID_ONBOARD_DEVICE_EX]       = &dmi_onboard_device_ex_spec,
    [DMI_TYPE_ID_MGMT_CONTROLLER_HOST_IF] = &dmi_mgmt_controller_host_if_spec,
    [DMI_TYPE_ID_TPM_DEVICE]              = &dmi_tpm_device_spec,
    [DMI_TYPE_ID_PROCESSOR_EX]            = &dmi_processor_ex_spec,
    [DMI_TYPE_ID_FIRMWARE_INVENTORY]      = &dmi_firmware_inventory_spec,
    [DMI_TYPE_ID_STRING_PROPERTY]         = &dmi_string_property_spec,
    [DMI_TYPE_ID_INACTIVE]                = &dmi_inactive_spec,
    [DMI_TYPE_ID_END_OF_TABLE]            = &dmi_end_of_table_spec
};

dmi_type_id_t dmi_type_find(dmi_context_t *context, const char *code)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context, DMI_TYPE_ID_INVALID);
    if (code == nullptr)
        return dmi_trace_argument_null(context, code, DMI_TYPE_ID_INVALID);

    // Type number is the one the structures are found at, which relocations
    // may make different from the type of the specification
    for (size_t i = 0; i <= DMI_TYPE_ID_MAX; i++) {
        for (size_t j = 0; j < DMI_TYPE_CANDIDATES; j++) {
            const dmi_entity_spec_t *spec = context->type_map[i][j];

            if ((spec != nullptr) and (strcmp(spec->code, code) == 0))
                return (dmi_type_id_t)i;
        }
    }

    return DMI_TYPE_ID_INVALID;
}

const dmi_entity_spec_t *dmi_type_spec(dmi_context_t *context, dmi_type_id_t type)
{
    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context, nullptr);

    if ((type <= DMI_TYPE_ID_INVALID) or (type > DMI_TYPE_ID_MAX))
        return dmi_trace_argument_invalid(context, type, nullptr);

    // Types told by signatures only are represented by the first of them
    const dmi_type_candidates_t *candidates = &context->type_map[type];

    return ((*candidates)[0] != nullptr) ? (*candidates)[0] : (*candidates)[1];
}

const char *dmi_spec_name(const dmi_entity_spec_t *spec)
{
    if (spec == nullptr)
        return dmi_trace_argument_null(nullptr, spec, nullptr);

    // Names of structure types are translated, if the locale has a
    // translation for the type
    const char *translated = dmi_locale_string(spec->code, "name");

    return (translated != nullptr) ? translated : spec->name;
}

const char *dmi_type_name(dmi_context_t *context, dmi_type_id_t type)
{
    const dmi_entity_spec_t *spec = dmi_type_spec(context, type);

    if (spec != nullptr)
        return dmi_spec_name(spec);

    // Types of the modules which are not enabled have no specification of
    // their own in the context, and numbers out of range are no type at all
    bool is_oem = (type >= __DMI_TYPE_ID_OEM_START) and (type <= DMI_TYPE_ID_MAX);

    return is_oem
            ? dmi_value_text("oem-type", "OEM-specific")
            : dmi_value_text("unknown-type", "Unknown");
}

dmi_type_candidates_t *dmi_types_create(dmi_context_t *context)
{
    return dmi_alloc_array(context, sizeof(dmi_type_candidates_t), DMI_TYPE_ID_MAX + 1);
}

bool dmi_types_map(
        dmi_context_t         *context,
        const dmi_module_t    *extra,
        bool                   report,
        dmi_type_candidates_t *map)
{
    const dmi_platform_t *platform = dmi_context_platform(context);
    const dmi_module_t   *module;

    memset(map, 0, sizeof(*map) * (DMI_TYPE_ID_MAX + 1));

    for (size_t i = 0; i < countof(dmi_entity_specs); i++) {
        const dmi_entity_spec_t *spec = dmi_entity_specs[i];

        if (spec != nullptr)
            map[spec->type->id][0] = spec;
    }

    // Modules which yield their types to the rest are mapped last, into the
    // types left free
    for (int pass = 0; pass < 2; pass++) {
        bool yield = (pass == 1);

        for (size_t i = 0; (module = dmi_module_at(context, extra, i)) != nullptr; i++) {
            if ((module->entities == nullptr) or (((module->flags & DMI_MODULE_FLAG_YIELD) != 0) != yield))
                continue;

            for (const dmi_entity_spec_t **pspec = module->entities; *pspec != nullptr; pspec++) {
                const dmi_entity_spec_t *spec = *pspec;

                // Specifications of other generations of the platform are left out
                if (not dmi_platform_in_generations(platform, &spec->params.generations))
                    continue;

                // Platforms which never carry the structure are told by
                // a relocation to no type, and the ones carrying it at
                // several type numbers by several relocations
                dmi_type_id_t type;

                for (size_t k = 0; (type = dmi_spec_relocate(context, extra, spec, k)) != DMI_TYPE_ID_INVALID; k++) {
                    if (dmi_types_map_one(&map[type], spec, yield))
                        continue;

                    if (report) {
                        dmi_error_raise_ex(context, DMI_ERROR_MODULE_CONFLICT, "%s: type %d",
                                           module->name, (int)type);
                    }
                    return false;
                }
            }
        }
    }

    return true;
}

bool dmi_types_rebuild(dmi_context_t *context, const dmi_module_t *extra)
{
    dmi_type_candidates_t *map = dmi_types_create(context);
    if (map == nullptr)
        return false;

    bool success = dmi_types_map(context, extra, true, map);
    if (success)
        memcpy(context->type_map, map, sizeof(*map) * (DMI_TYPE_ID_MAX + 1));

    dmi_free(map);

    return success;
}

static const dmi_module_t *dmi_module_at(
        const dmi_context_t *context,
        const dmi_module_t  *extra,
        size_t               index)
{
    if (index < context->modules.length)
        return (const dmi_module_t *)context->modules.data[index];
    index -= context->modules.length;

    if (index < context->state.modules.length)
        return (const dmi_module_t *)context->state.modules.data[index];
    index -= context->state.modules.length;

    return (index == 0) ? extra : nullptr;
}

static dmi_type_id_t dmi_spec_relocate(
        const dmi_context_t     *context,
        const dmi_module_t      *extra,
        const dmi_entity_spec_t *spec,
        size_t                   index)
{
    const dmi_module_t *module;
    bool relocated = false;

    for (size_t i = 0; (module = dmi_module_at(context, extra, i)) != nullptr; i++) {
        if (module->relocations == nullptr)
            continue;

        for (const dmi_relocation_t *relocation = module->relocations; relocation->spec != nullptr; relocation++) {
            if (relocation->spec != spec)
                continue;

            // Relocation to no type maps the specification nowhere
            relocated = true;
            if (relocation->type == DMI_TYPE_ID_INVALID)
                continue;

            if (index == 0)
                return relocation->type;
            index--;
        }
    }

    return (relocated or (index != 0)) ? DMI_TYPE_ID_INVALID : spec->type->id;
}

static bool dmi_types_map_one(
        dmi_type_candidates_t   *candidates,
        const dmi_entity_spec_t *spec,
        bool                     yield)
{
    // The same specification may be brought by several modules
    for (size_t i = 0; i < DMI_TYPE_CANDIDATES; i++) {
        if ((*candidates)[i] == spec)
            return true;
    }

    // Structures of the type which no signature matches are decoded by the
    // specification without one, which the type has one of at most
    if (spec->params.signature == nullptr) {
        if ((*candidates)[0] != nullptr)
            return yield;

        (*candidates)[0] = spec;
        return true;
    }

    for (size_t i = 1; i < DMI_TYPE_CANDIDATES; i++) {
        if ((*candidates)[i] == nullptr) {
            (*candidates)[i] = spec;
            return true;
        }
    }

    return yield;
}
