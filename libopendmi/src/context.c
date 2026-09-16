//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include "config.h"

#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>

#ifdef ENABLE_ICU
#   include <unicode/uclean.h>
#   include <unicode/udata.h>
#   include <unicode/ures.h>
#endif // ENABLE_ICU

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/entity.h>
#include <opendmi/vendor.h>
#include <opendmi/utils.h>
#include <opendmi/utils/file.h>

#include <opendmi/backend/dump.h>

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
#include <opendmi/entity/system-config.h>
#include <opendmi/entity/temperature-probe.h>
#include <opendmi/entity/tpm-device.h>
#include <opendmi/entity/voltage-probe.h>

static bool dmi_open_ex(
        dmi_context_t       *context,
        const dmi_backend_t *backend,
        const char          *device);

/**
 * @internal
 * @brief Setup vendor-specific extensions.
 */
static bool dmi_setup_extensions(dmi_context_t *context);

/**
 * @internal
 * @brief Fixup DMI version number.
 */
static void dmi_version_fixup(dmi_context_t *context);

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
    .type        = DMI_TYPE(INACTIVE)
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
    .type        = DMI_TYPE(END_OF_TABLE)
};

/**
 * @brief Predefined entity specifications map.
 */
static const dmi_entity_spec_t *dmi_entity_specs[] =
{
    [DMI_TYPE_FIRMWARE]                = &dmi_firmware_spec,
    [DMI_TYPE_SYSTEM]                  = &dmi_system_spec,
    [DMI_TYPE_BASEBOARD]               = &dmi_baseboard_spec,
    [DMI_TYPE_CHASSIS]                 = &dmi_chassis_spec,
    [DMI_TYPE_PROCESSOR]               = &dmi_processor_spec,
    [DMI_TYPE_MEMORY_CONTROLLER]       = &dmi_memory_controller_spec,
    [DMI_TYPE_MEMORY_MODULE]           = &dmi_memory_module_spec,
    [DMI_TYPE_CACHE]                   = &dmi_cache_spec,
    [DMI_TYPE_PORT_CONNECTOR]          = &dmi_port_connector_spec,
    [DMI_TYPE_SYSTEM_SLOTS]            = &dmi_slot_spec,
    [DMI_TYPE_ONBOARD_DEVICE]          = &dmi_onboard_device_spec,
    [DMI_TYPE_OEM_STRINGS]             = &dmi_oem_strings_spec,
    [DMI_TYPE_SYSTEM_CONFIG_OPTIONS]   = &dmi_system_config_opts_spec,
    [DMI_TYPE_FIRMWARE_LANGUAGE]       = &dmi_firmware_language_spec,
    [DMI_TYPE_GROUP_ASSOC]             = &dmi_group_assoc_spec,
    [DMI_TYPE_SYSTEM_EVENT_LOG]        = &dmi_system_event_log_spec,
    [DMI_TYPE_MEMORY_ARRAY]            = &dmi_memory_array_spec,
    [DMI_TYPE_MEMORY_DEVICE]           = &dmi_memory_device_spec,
    [DMI_TYPE_MEMORY_ERROR_32]         = &dmi_memory_error_32_spec,
    [DMI_TYPE_MEMORY_ARRAY_ADDR]       = &dmi_memory_array_addr_spec,
    [DMI_TYPE_MEMORY_DEVICE_ADDR]      = &dmi_memory_device_addr_spec,
    [DMI_TYPE_POINTING_DEVICE]         = &dmi_pointing_device_spec,
    [DMI_TYPE_PORTABLE_BATTERY]        = &dmi_battery_spec,
    [DMI_TYPE_SYSTEM_RESET]            = &dmi_system_reset_spec,
    [DMI_TYPE_HARDWARE_SECURITY]       = &dmi_hardware_security_spec,
    [DMI_TYPE_POWER_CONTROLS]          = &dmi_power_controls_spec,
    [DMI_TYPE_VOLTAGE_PROBE]           = &dmi_voltage_probe_spec,
    [DMI_TYPE_COOLING_DEVICE]          = &dmi_cooling_device_spec,
    [DMI_TYPE_TEMPERATURE_PROBE]       = &dmi_temperature_probe_spec,
    [DMI_TYPE_CURRENT_PROBE]           = &dmi_current_probe_spec,
    [DMI_TYPE_OOB_REMOTE_ACCESS]       = &dmi_oob_remote_access_spec,
    [DMI_TYPE_BIS_ENTRY_POINT]         = &dmi_bis_entry_point_spec,
    [DMI_TYPE_SYSTEM_BOOT]             = &dmi_system_boot_spec,
    [DMI_TYPE_MEMORY_ERROR_64]         = &dmi_memory_error_64_spec,
    [DMI_TYPE_MGMT_DEVICE]             = &dmi_mgmt_device_spec,
    [DMI_TYPE_MGMT_DEVICE_COMPONENT]   = &dmi_mgmt_device_component_spec,
    [DMI_TYPE_MGMT_DEVICE_THRESHOLD]   = &dmi_mgmt_device_threshold_spec,
    [DMI_TYPE_MEMORY_CHANNEL]          = &dmi_memory_channel_spec,
    [DMI_TYPE_IPMI_DEVICE]             = &dmi_ipmi_device_spec,
    [DMI_TYPE_POWER_SUPPLY]            = &dmi_power_supply_spec,
    [DMI_TYPE_ADDITIONAL_INFO]         = &dmi_additional_info_spec,
    [DMI_TYPE_ONBOARD_DEVICE_EX]       = &dmi_onboard_device_ex_spec,
    [DMI_TYPE_MGMT_CONTROLLER_HOST_IF] = &dmi_mgmt_controller_host_if_spec,
    [DMI_TYPE_TPM_DEVICE]              = &dmi_tpm_device_spec,
    [DMI_TYPE_PROCESSOR_EX]            = &dmi_processor_ex_spec,
    [DMI_TYPE_FIRMWARE_INVENTORY]      = &dmi_firmware_inventory_spec,
    [DMI_TYPE_STRING_PROPERTY]         = &dmi_string_property_spec,
    [DMI_TYPE_INACTIVE]                = &dmi_inactive_spec,
    [DMI_TYPE_END_OF_TABLE]            = &dmi_end_of_table_spec
};

#ifdef ENABLE_ICU
    extern const char dmi_resources_dat[];
#endif

dmi_context_t *dmi_create(unsigned int flags)
{
    bool success = false;
    dmi_context_t *context = nullptr;

    // Allocate context descriptor
    context = dmi_alloc(nullptr, sizeof(dmi_context_t));
    if (context == nullptr)
        return nullptr;

    context->state.vendor = DMI_VENDOR_OTHER;
    context->flags        = flags;
    context->log_level    = DMI_LOG_INFO;

    do {
        // Allocate type map
        context->type_map = dmi_alloc_array(context, sizeof(dmi_entity_spec_t *), DMI_TYPE_MAX + 1);
        if (context->type_map == nullptr)
            break;

        // Initialize type map
        for (size_t i = 0; i < countof(dmi_entity_specs); i++) {
            const dmi_entity_spec_t *spec = dmi_entity_specs[i];

            if (spec == nullptr)
                continue;

            context->type_map[spec->type] = spec;
        }

#       ifdef ENABLE_ICU
            UErrorCode status = U_ZERO_ERROR;

            u_init(&status);
            if (U_FAILURE(status)) {
                fprintf(stderr, "Unable to initialize ICU: %s\n", u_errorName(status));
                break;
            }

            // Register resources package
            udata_setAppData("opendmi", dmi_resources_dat, &status);
            if (U_FAILURE(status)) {
                fprintf(stderr, "Unable register resources package: %s\n", u_errorName(status));
                break;
            }

            // Load i18n resources
            context->resources = ures_open("opendmi", nullptr, &status);
            if (U_FAILURE(status)) {
                fprintf(stderr, "Unable to open resource bundle: %s\n", u_errorName(status));
                break;
            }
#       endif

        success = true;
    } while (false);

    if (not success) {
        dmi_free(context->type_map);
        dmi_free(context);

        return nullptr;
    }

    return context;
}

void dmi_set_flags(dmi_context_t *context, unsigned flags)
{
    if (context == nullptr)
        return;

    context->flags = flags;
}

unsigned dmi_get_flags(const dmi_context_t *context) {
    if (context == nullptr)
        return 0;

    return context->flags;
}

bool dmi_open(dmi_context_t *context, const char *device)
{
    if (context == nullptr)
        return false;

    return dmi_open_ex(context, dmi_backend, device);
}

bool dmi_add_extension(dmi_context_t *context, const dmi_module_t *module)
{
    if (context == nullptr)
        return false;

    if (module == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "module");
        return false;
    }

    dmi_log_info(context->logger, "Enabling extension: %s", module->name);

    if (module->entities) {
        const dmi_entity_spec_t **pspec;

        // Check type map for conflicts
        for (pspec = module->entities; *pspec != nullptr; pspec++) {
            if (context->type_map[(*pspec)->type] != nullptr) {
                dmi_error_raise_ex(context, DMI_ERROR_MODULE_CONFLICT, "%s", module->name);
                return false;
            }
        }

        // Update type map
        for (pspec = module->entities; *pspec != nullptr; pspec++) {
            context->type_map[(*pspec)->type] = *pspec;
        }
    }

    return true;
}

bool dmi_dump_load(dmi_context_t *context, const char *path)
{
    if (context == nullptr)
        return false;

    if (path == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "path");
        return false;
    }

    dmi_log_info(context->logger, "Loading DMI dump: %s...", path);

    return dmi_open_ex(context, &dmi_dump_backend, path);
}

bool dmi_dump_save(dmi_context_t *context, const char *path, bool overwrite)
{
    int flags;
    int fd;
    bool success;
    ssize_t nwrite;

    if (context == nullptr)
        return false;

    if (path == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "path");
        return false;
    }
    if (context->state.table_data == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_STATE, "Context is not open");
        return false;
    }
    if (context->state.entry_size > DMI_ENTRY_MAX_SIZE) {
        dmi_error_raise(context, DMI_ERROR_INVALID_EPS_LENGTH);
        return false;
    }

    flags = O_CREAT | O_WRONLY | O_TRUNC;
#if defined(O_BINARY)
    flags |= O_BINARY;
#endif
    if (not overwrite)
        flags |= O_EXCL;

    fd = open(path, flags, 0666);
    if (fd < 0) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(errno));
        return false;
    }

    success = false;
    do {
        dmi_byte_t entry[DMI_ENTRY_MAX_SIZE] = {};

        // Entry point data is optional, Windows backend does not provide it
        if (context->state.entry_data != nullptr)
            memcpy(entry, context->state.entry_data, context->state.entry_size);

    write_entry:
        nwrite = write(fd, entry, sizeof(entry));
        if (nwrite < 0) {
            if (errno == EINTR)
                goto write_entry;

            dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s: %s", path, strerror(errno));
            break;
        }

    write_table:
        nwrite = write(fd, context->state.table_data, context->state.table_area_size);
        if (nwrite < 0) {
            if (errno == EINTR)
                goto write_table;

            dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s: %s", path, strerror(errno));
            break;
        }

        success = true;
    } while (false);

    dmi_file_close(fd);

    return success;
}

dmi_type_t dmi_type_find(dmi_context_t *context, const char *code)
{
    if ((context == nullptr) or (code == nullptr))
        return DMI_TYPE_INVALID;

    for (size_t i = 0; i <= DMI_TYPE_MAX; i++) {
        const dmi_entity_spec_t *spec = context->type_map[i];

        if (spec == nullptr)
            continue;

        if (strcmp(spec->code, code) == 0)
            return spec->type;
    }

    return DMI_TYPE_INVALID;
}

const dmi_entity_spec_t *dmi_type_spec(dmi_context_t *context, dmi_type_t type)
{
    if (context == nullptr)
        return nullptr;

    if ((type <= DMI_TYPE_INVALID) or (type > DMI_TYPE_MAX)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ARGUMENT, "type");
        return nullptr;
    }

    return context->type_map[type];
}

const char *dmi_type_name(dmi_context_t *context, dmi_type_t type)
{
    const char *name;
    const dmi_entity_spec_t *spec = dmi_type_spec(context, type);

    if (spec != nullptr)
        name = spec->name;
    else
        name = type > 0x7F ? "OEM-specific" : "Unknown";

    return name;
}

bool dmi_set_logger(dmi_context_t *context, dmi_log_t *logger)
{
    if (context == nullptr)
        return false;

    context->logger = logger;

    return true;
}

bool dmi_close(dmi_context_t *context)
{
    if (context == nullptr)
        return false;

    dmi_registry_destroy(context->state.registry);

    if ((context->state.backend != nullptr) and (context->state.session != nullptr))
        context->state.backend->close(context);

    memset(&context->state, 0, sizeof(context->state));
    context->state.vendor = DMI_VENDOR_OTHER;

    return true;
}

void dmi_destroy(dmi_context_t *context)
{
    if (context == nullptr)
        return;

    // Close and free context
    dmi_close(context);
    dmi_error_clear(context);

#   ifdef ENABLE_ICU
        // Close resources
        if (context->resources != nullptr)
            ures_close((UResourceBundle *)context->resources);
#   endif

    dmi_free(context->type_map);
    dmi_free(context);
}

static bool dmi_open_ex(
        dmi_context_t       *context,
        const dmi_backend_t *backend,
        const char          *device)
{
    assert(context != nullptr);

    if ((context->state.backend != nullptr) or (context->state.session != nullptr)) {
        dmi_error_raise_ex(context,  DMI_ERROR_INVALID_STATE, "Context already initialized");
        return false;
    }

    dmi_log_info(context->logger, "Opening DMI context...");
    dmi_log_info(context->logger, "Using backend: %s", backend->name);

    // Initialize context
    bool success = false;
    do {
        context->state.backend = backend;

        // Initialize backend
        if (not context->state.backend->open(context, device)) {
            dmi_error_raise_ex(context, DMI_ERROR_BACKEND_INIT, "%s", backend->name);
            break;
        }

        // Read and decode entry point, if backend provides it
        if (backend->read_entry != nullptr) {
            dmi_log_info(context->logger, "Reading DMI entry point...");
            context->state.entry_data = backend->read_entry(context, &context->state.entry_size);
            if (context->state.entry_data == nullptr)
                break;

            dmi_log_info(context->logger, "Decoding DMI entry point...");
            if (not dmi_entry_decode(context, context->state.entry_data, context->state.entry_size))
                break;
        }

        // Fixup SMBIOS version number
        dmi_version_fixup(context);
        dmi_log_info(context->logger, "SMBIOS %u.%u.%u present",
                     dmi_version_major(context->state.smbios_version),
                     dmi_version_minor(context->state.smbios_version),
                     dmi_version_revision(context->state.smbios_version));

        // Read and decode SMBIOS structures
        // TODO: Use separate variable for size
        dmi_log_info(context->logger, "Reading DMI structures...");
        context->state.table_data = context->state.backend->read_table(context, &context->state.table_area_size);
        if (context->state.table_data == nullptr)
            break;

        // Create registry
        context->state.registry = dmi_registry_create(context, 0);
        if (context->state.registry == nullptr)
            break;

        // Scan for SMBIOS structures
        if (not dmi_registry_scan(context->state.registry))
            break;

        if (not dmi_setup_extensions(context))
            break;

        // Decode and link SMBIOS structures
        if (not dmi_registry_decode(context->state.registry))
            break;

        if (context->flags & DMI_CONTEXT_FLAG_LINK) {
            if (not dmi_registry_link(context->state.registry))
                break;
        }

        success = true;
    } while (false);

    if (not success) {
        dmi_error_raise(context, DMI_ERROR_CONTEXT_OPEN);
        dmi_close(context);
    }

    return success;
}

static bool dmi_setup_extensions(dmi_context_t *context)
{
    dmi_entity_t *entity;
    const dmi_firmware_t *firmware;
    const dmi_vendor_spec_t *vendor;

    dmi_log_debug(context->logger, "Detecting SMBIOS vendor...");

    entity = dmi_registry_get(context->state.registry, DMI_HANDLE_INVALID, DMI_TYPE(FIRMWARE), true);
    if (entity == nullptr) {
        if ((context->flags & DMI_CONTEXT_FLAG_STRICT) == 0) {
            dmi_log_notice(context->logger, dmi_error_message(DMI_ERROR_MISSING_FIRMWARE_INFO));
            return true;
        }

        dmi_error_raise(context, DMI_ERROR_MISSING_FIRMWARE_INFO);
        return false;
    }

    if (not dmi_entity_decode(entity))
        return false;

    firmware = dmi_cast(firmware, entity->info);
    vendor   = dmi_vendor_detect(firmware->vendor);

    context->state.vendor_name = firmware->vendor;
    if (vendor != nullptr)
        context->state.vendor = vendor->id;

    dmi_log_info(context->logger, "SMBIOS vendor: %s (%s)",
                 dmi_vendor_name(context->state.vendor), firmware->vendor);

    //
    // TODO: Implement fully-feature module probing
    //
    //if ((vendor != nullptr) and (vendor->module != nullptr)) {
    //    if (!dmi_add_extension(context, vendor->module))
    //        return false;
    //}

    return true;
}

static void dmi_version_fixup(dmi_context_t *context)
{
    unsigned int major    = dmi_version_major(context->state.smbios_version);
    unsigned int minor    = dmi_version_minor(context->state.smbios_version);
    unsigned int revision = dmi_version_revision(context->state.smbios_version);

    if (major != 2)
        return;

    // Some BIOS report weird SMBIOS version, fix that up
    switch (minor) {
    case 0x1F:
    case 0x21:
        minor = 3;
        break;

    case 0x33:
        minor = 6;
        break;

    default:
        return;
    }

    context->state.smbios_version = dmi_version(major, minor, revision);
}
