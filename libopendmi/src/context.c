//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>

// Files are not accessible from the kernel
#if !defined(__KERNEL__)
#   include <fcntl.h>
#endif

// Dumps are moved into place by the system, see dmi_save_commit()
#if defined(_WIN32)
#   define WIN32_LEAN_AND_MEAN
#   define NOMINMAX
#   include <windows.h>
#   include <process.h>
#endif

#include <opendmi/anonymize.h>
#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/entity.h>
#include <opendmi/vendor.h>
#include <opendmi/internal.h>

#include <opendmi/utils.h>

#if !defined(__KERNEL__)
#   include <opendmi/utils/file.h>
#   include <opendmi/utils/string.h>
#endif

#if defined(_WIN32)
#   include <opendmi/utils/win32.h>
#endif

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
 *
 * @details
 * Tells the vendor and the platform from the firmware and system information,
 * enables the modules of the platform, if requested, and maps the
 * specifications of the enabled modules for the platform.
 */
static bool dmi_setup_extensions(dmi_context_t *context);

/**
 * @internal
 * @brief Tell the vendor from the firmware information.
 *
 * @return `false` if the vendor is not told in strict mode, `true` otherwise.
 */
static bool dmi_setup_vendor(dmi_context_t *context);

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

/**
 * @internal
 * @brief Get the platform the structures are decoded for: the one set
 * explicitly, or the one told from the data.
 */
static const dmi_platform_t *dmi_context_platform(const dmi_context_t *context);

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
 * @brief Allocate a map of `DMI_TYPE_ID_MAX + 1` candidate lists.
 *
 * @details
 * Maps are some kilobytes large, too large for the stack of the Linux kernel,
 * so the ones built to be checked before they replace the map of the context
 * are allocated too.
 *
 * @return Map to be freed with `dmi_free()`, or @c nullptr if memory is
 *         exhausted.
 */
static dmi_type_candidates_t *dmi_types_create(dmi_context_t *context);

/**
 * @internal
 * @brief Map specifications to types for the platform of the context.
 *
 * @details
 * Standard specifications are mapped first, followed by the specifications of
 * the enabled modules and of @p extra, which apply to the platform, and by
 * the ones of the modules which yield their types to the rest, into the types
 * left free. The map of the context is not changed, so that it stays as it was
 * on conflicts.
 *
 * @param[in]  context Context descriptor.
 * @param[in]  extra   Module which is being enabled, or @c nullptr.
 * @param[in]  report  Whether a conflict is raised as an error.
 * @param[out] map     Map of `DMI_TYPE_ID_MAX + 1` candidate lists to fill.
 *
 * @return `true` on success, `false` if two specifications are mapped to the
 *         same type.
 */
static bool dmi_types_map(
        dmi_context_t         *context,
        const dmi_module_t    *extra,
        bool                   report,
        dmi_type_candidates_t *map);

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

/**
 * @internal
 * @brief Fixup DMI version number.
 */
static void dmi_version_fixup(dmi_context_t *context);

#if !defined(__KERNEL__)
/**
 * @internal
 * @brief Number of names tried for the temporary file of a dump.
 */
#define DMI_SAVE_TEMP_ATTEMPTS 100

/**
 * @internal
 * @brief Tell the status of the target of a dump, without following it if it
 * is a symbolic link.
 */
static int dmi_save_stat(const char *path, dmi_file_stat_t *st);

/**
 * @internal
 * @brief Create the temporary file a dump is written to.
 *
 * @details
 * The file is created next to the target, so that it can be renamed over it,
 * and is never one which has existed before.
 *
 * @return Path to the file, which is to be freed by the caller, or
 *         @c nullptr on failure.
 */
static char *dmi_save_temp_open(dmi_context_t *context, const char *path, int *pfd);

/**
 * @internal
 * @brief Move a completely written dump into place.
 *
 * @details
 * Existing file is replaced atomically if @p overwrite is set, and is never
 * replaced otherwise, even if it has been created while the dump was being
 * written.
 */
static bool dmi_save_commit(dmi_context_t *context, const char *temp, const char *path, bool overwrite);

/**
 * @internal
 * @brief Write dump data completely, raising an error on failures.
 */
static bool dmi_dump_write(
        dmi_context_t    *context,
        int               fd,
        const char       *path,
        const dmi_data_t *data,
        size_t            size);

/**
 * @internal
 * @brief Build entry point structure for a dump file.
 *
 * @details
 * Dump file layout is compatible with dmidecode: the entry point structure
 * is padded with zeroes to #DMI_ENTRY_MAX_SIZE bytes and followed by the
 * structure table. The table address in the entry point is replaced with the
 * table offset in the file, and the checksum is adjusted accordingly. If the
 * backend provides no entry point data, a 64-bit entry point is generated.
 */
static bool dmi_dump_entry_build(dmi_context_t *context, dmi_byte_t *entry);

/**
 * @internal
 * @brief Generate 64-bit entry point structure for a dump file.
 */
static bool dmi_dump_entry_generate(dmi_context_t *context, dmi_byte_t *entry);
#endif // !defined(__KERNEL__)

/**
 * @internal
 * @brief Set field of entry point structure placed in a writable buffer.
 *
 * @details
 * Entry point structure fields are read-only, so the value is copied to the
 * field address. Copying also avoids unaligned access to packed fields.
 */
#define dmi_entry_set(field, value) \
    memcpy((void *)&(field), &(const __typeof__(field)){ value }, sizeof(field))

/**
 * @internal
 * @brief Update checksum of `length` bytes of entry point structure placed
 * in a writable buffer.
 */
#define dmi_entry_set_checksum(eps, length)                                  \
    do {                                                                     \
        dmi_entry_set((eps)->checksum, 0);                                   \
        dmi_entry_set((eps)->checksum, dmi_checksum_calc((eps), (length))); \
    } while (false)

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
    context->log_level    = DMI_LOG_DEBUG;

    do {
        // Allocate type map
        context->type_map = dmi_types_create(context);
        if (context->type_map == nullptr)
            break;

        // Initialize type map, which has no modules to conflict yet
        dmi_types_map(context, nullptr, false, context->type_map);

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

    dmi_log_info(context, "Enabling extension: %s", module->name);

    if (dmi_has_extension(context, module)) {
        dmi_error_raise_ex(context, DMI_ERROR_MODULE_CONFLICT, "%s: already enabled", module->name);
        return false;
    }

    dmi_type_candidates_t *map = dmi_types_create(context);
    if (map == nullptr)
        return false;

    bool success = false;
    do {
        // Check the module against the enabled ones, before it is enabled
        if (not dmi_types_map(context, module, true, map))
            break;

        // Register enabled module
        if (not dmi_vector_push(&context->modules, (uintptr_t)module)) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            break;
        }

        // Update type map
        memcpy(context->type_map, map, sizeof(*map) * (DMI_TYPE_ID_MAX + 1));

        success = true;
    } while (false);

    dmi_free(map);

    return success;
}

bool dmi_has_extension(const dmi_context_t *context, const dmi_module_t *module)
{
    if ((context == nullptr) or (module == nullptr))
        return false;

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
        return nullptr;

    return dmi_context_platform(context);
}

bool dmi_set_platform(dmi_context_t *context, const dmi_platform_t *platform)
{
    if (context == nullptr)
        return false;

    dmi_platform_t *copy = nullptr;
    if (platform != nullptr) {
        copy = dmi_platform_clone(platform);
        if (copy == nullptr) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }
    }

    dmi_type_candidates_t *map = dmi_types_create(context);
    if (map == nullptr) {
        dmi_platform_destroy(copy);
        return false;
    }

    dmi_platform_t *previous = context->platform;
    context->platform = copy;

    if (not dmi_types_map(context, nullptr, true, map)) {
        context->platform = previous;
        dmi_platform_destroy(copy);
        dmi_free(map);
        return false;
    }

    memcpy(context->type_map, map, sizeof(*map) * (DMI_TYPE_ID_MAX + 1));
    dmi_platform_destroy(previous);
    dmi_free(map);

    return true;
}

bool dmi_load(dmi_context_t *context, const char *path)
{
    if (context == nullptr)
        return false;

    if (path == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "path");
        return false;
    }

    dmi_log_info(context, "Loading DMI dump: %s...", path);

    return dmi_open_ex(context, &dmi_dump_backend, path);
}

#if defined(__KERNEL__)
bool dmi_save(dmi_context_t *context, const char *path, unsigned flags)
{
    dmi_unused(flags);

    if (context == nullptr)
        return false;

    dmi_error_raise_ex(context, DMI_ERROR_SERVICE_UNAVAILABLE,
                       "%s: files are not accessible from the kernel", path);

    return false;
}
#else
bool dmi_save(dmi_context_t *context, const char *path, unsigned flags)
{
    int fd = -1;
    bool success;

    if (context == nullptr)
        return false;

    if (path == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "path");
        return false;
    }
    if (context->state.table == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_STATE, "Context is not open");
        return false;
    }

    // Backends which have no access to the entry point leave the context
    // without one, and its data is generated on saving
    if ((context->state.entry != nullptr) and
        (context->state.entry->length > DMI_ENTRY_MAX_SIZE))
    {
        dmi_error_raise(context, DMI_ERROR_INVALID_EPS_LENGTH);
        return false;
    }

    dmi_byte_t entry[DMI_ENTRY_MAX_SIZE];
    if (not dmi_dump_entry_build(context, entry))
        return false;

    // Anonymized copy has the length of the table, so the entry point built
    // for the table holds for it too
    const dmi_buffer_t *table = context->state.table;
    dmi_buffer_t *anonymized = nullptr;

    if (flags & DMI_SAVE_FLAG_ANONYMIZE) {
        anonymized = dmi_buffer_create(context);
        if (anonymized == nullptr)
            return false;

        if (not dmi_anonymize(context, anonymized)) {
            dmi_buffer_destroy(anonymized);
            return false;
        }

        table = anonymized;
    }

    bool overwrite = (flags & DMI_SAVE_FLAG_OVERWRITE) != 0;

    // Regular file is never written in place: the dump goes to a temporary
    // file, which replaces the target only once it is complete, so that a
    // failure neither destroys the file being overwritten nor leaves an
    // incomplete dump behind. Devices, pipes and symbolic links, e.g.
    // /dev/stdout, cannot be replaced that way, and are written directly.
    dmi_file_stat_t st;
    bool exists    = (dmi_save_stat(path, &st) == 0);
    bool is_direct = exists and not S_ISREG(st.st_mode);
    char *temp     = nullptr;

    if (exists and not is_direct and not overwrite) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(EEXIST));
        dmi_buffer_destroy(anonymized);
        return false;
    }

    if (is_direct) {
        int mode = O_CREAT | O_WRONLY | O_TRUNC;
#if defined(O_BINARY)
        mode |= O_BINARY;
#endif
        if (not overwrite)
            mode |= O_EXCL;

        fd = open(path, mode, 0666);
        if (fd < 0)
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(errno));
    } else {
        temp = dmi_save_temp_open(context, path, &fd);
    }

    if (fd < 0) {
        dmi_buffer_destroy(anonymized);
        return false;
    }

#if !defined(_WIN32)
    // Replaced file keeps its permissions
    if (exists and (temp != nullptr))
        (void)fchmod(fd, st.st_mode & 0777);
#endif

    success = false;
    do {
        if (not dmi_dump_write(context, fd, path, entry, sizeof(entry)))
            break;
        if (not dmi_dump_write(context, fd, path, table->data, table->length))
            break;

        success = true;
    } while (false);

    if (dmi_file_close(fd) < 0) {
        if (success)
            dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s: %s", path, strerror(errno));
        success = false;
    }

    if (temp != nullptr) {
        if (success)
            success = dmi_save_commit(context, temp, path, overwrite);

        // Do not leave incomplete dump behind
        if (not success)
            remove(temp);

        dmi_free(temp);
    }

    dmi_buffer_destroy(anonymized);

    return success;
}
#endif // !defined(__KERNEL__)

dmi_type_id_t dmi_type_find(dmi_context_t *context, const char *code)
{
    if ((context == nullptr) or (code == nullptr))
        return DMI_TYPE_ID_INVALID;

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
        return nullptr;

    if ((type <= DMI_TYPE_ID_INVALID) or (type > DMI_TYPE_ID_MAX)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ARGUMENT, "type");
        return nullptr;
    }

    // Types told by signatures only are represented by the first of them
    const dmi_type_candidates_t *candidates = &context->type_map[type];

    return ((*candidates)[0] != nullptr) ? (*candidates)[0] : (*candidates)[1];
}

const char *dmi_spec_name(const dmi_entity_spec_t *spec)
{
    if (spec == nullptr)
        return nullptr;

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

dmi_log_t *dmi_get_logger(const dmi_context_t *context)
{
    if (context == nullptr)
        return nullptr;

    return context->logger;
}

bool dmi_set_log_level(dmi_context_t *context, dmi_log_level_t level)
{
    if ((context == nullptr) or (level == DMI_LOG_INVALID))
        return false;

    context->log_level = level;

    return true;
}

dmi_log_level_t dmi_get_log_level(const dmi_context_t *context)
{
    if (context == nullptr)
        return DMI_LOG_INVALID;

    return context->log_level;
}

bool dmi_log(dmi_context_t *context, dmi_log_level_t level, const char *format, ...)
{
    if (context == nullptr)
        return false;

    // Context level is checked before the level of the handler
    if (level > context->log_level)
        return false;

    va_list args;
    va_start(args, format);
    bool status = dmi_log_message_va(context->logger, level, format, args);
    va_end(args);

    return status;
}

bool dmi_set_logger(dmi_context_t *context, dmi_log_t *logger)
{
    if (context == nullptr)
        return false;

    context->logger = logger;

    return true;
}

dmi_registry_t *dmi_get_registry(dmi_context_t *context)
{
    if (context == nullptr)
        return nullptr;

    return context->state.registry;
}

bool dmi_close(dmi_context_t *context)
{
    if (context == nullptr)
        return false;

    dmi_registry_destroy(context->state.registry);

    if ((context->state.backend != nullptr) and (context->state.session != nullptr))
        context->state.backend->close(context);

    // Data of the platform is held by the context rather than by the backend,
    // and nothing which refers to it outlives the context being closed
    dmi_buffer_destroy(context->state.entry);
    dmi_buffer_destroy(context->state.table);

    dmi_vector_clear(&context->state.modules);
    dmi_platform_destroy(context->state.platform);

    memset(&context->state, 0, sizeof(context->state));
    context->state.vendor = DMI_VENDOR_OTHER;

    // Modules enabled for the platform are gone, and so are specifications
    // of the modules enabled explicitly, which apply to its generations only.
    // What is left has been mapped without conflicts before, so the map is
    // updated in place.
    dmi_types_map(context, nullptr, false, context->type_map);

    return true;
}

void dmi_destroy(dmi_context_t *context)
{
    if (context == nullptr)
        return;

    // Modules are dropped before closing, since closing maps the modules
    // enabled explicitly again, and they need not outlive the context
    dmi_vector_clear(&context->modules);

    // Close and free context
    dmi_close(context);
    dmi_error_clear(context);

    dmi_platform_destroy(context->platform);
    dmi_free(context->type_map);
    dmi_free(context);
}

bool dmi_anonymize_context(dmi_context_t *context)
{
    if (context == nullptr)
        return false;

    dmi_buffer_t *table = dmi_buffer_create(context);
    if (table == nullptr)
        return false;

    if (not dmi_anonymize(context, table)) {
        dmi_buffer_destroy(table);
        return false;
    }

    // Structures are read anew from the copy, which has the very layout of
    // the table, so the platform and the modules told from it stay the same.
    // The old structures are kept until the new ones are read, so that the
    // context is left as it is on failure
    dmi_buffer_t   *old_table    = context->state.table;
    dmi_registry_t *old_registry = context->state.registry;
    const char     *old_vendor   = context->state.vendor_name;

    context->state.table    = table;
    context->state.registry = dmi_registry_create(context, 0);

    bool success = (context->state.registry != nullptr) and
                   dmi_registry_scan(context->state.registry) and
                   dmi_setup_vendor(context) and
                   dmi_registry_decode(context->state.registry) and
                   (((context->flags & DMI_CONTEXT_FLAG_LINK) == 0) or
                    dmi_registry_link(context->state.registry));

    if (not success) {
        if (context->state.registry != nullptr)
            dmi_registry_destroy(context->state.registry);
        dmi_buffer_destroy(table);

        context->state.table       = old_table;
        context->state.registry    = old_registry;
        context->state.vendor_name = old_vendor;
        return false;
    }

    dmi_registry_destroy(old_registry);
    dmi_buffer_destroy(old_table);

    return true;
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

    dmi_log_info(context, "Opening DMI context...");
    dmi_log_info(context, "Using backend: %s", backend->name);

    // Initialize context
    bool success = false;
    do {
        context->state.backend = backend;

        // Data the backend reads belongs to the context, which holds it for
        // as long as it is open
        context->state.table = dmi_buffer_create(context);
        if (context->state.table == nullptr)
            break;

        // Initialize backend
        if (not context->state.backend->open(context, device)) {
            dmi_error_raise_ex(context, DMI_ERROR_BACKEND_INIT, "%s", backend->name);
            break;
        }

        // Read and decode entry point, if backend provides it
        if (backend->read_entry != nullptr) {
            dmi_log_info(context, "Reading DMI entry point...");

            // Backends which have no access to the entry point leave the
            // context without one at all
            context->state.entry = dmi_buffer_create(context);
            if (context->state.entry == nullptr)
                break;

            if (not backend->read_entry(context, context->state.entry))
                break;

            dmi_log_info(context, "Decoding DMI entry point...");
            if (not dmi_entry_decode(context, context->state.entry->data, context->state.entry->length))
                break;
        }

        // Fixup SMBIOS version number
        dmi_version_fixup(context);
        dmi_log_info(context, "SMBIOS %u.%u.%u present",
                     dmi_version_major(context->state.smbios_version),
                     dmi_version_minor(context->state.smbios_version),
                     dmi_version_revision(context->state.smbios_version));

        // Read and decode SMBIOS structures
        dmi_log_info(context, "Reading DMI structures...");
        if (not context->state.backend->read_table(context, context->state.table))
            break;

        // Table data size is used for bounds checking while scanning
        if (context->state.table->length == 0) {
            dmi_error_raise_ex(context, DMI_ERROR_ENTITY_TRUNCATED, "SMBIOS table area is empty");
            break;
        }

        // Create registry
        context->state.registry = dmi_registry_create(context, 0);
        if (context->state.registry == nullptr)
            break;

        // Scan for SMBIOS structures
        if (not dmi_registry_scan(context->state.registry))
            break;

        // Additional information is applied before any structure is decoded,
        // including firmware information used to detect the vendor
        if (context->flags & DMI_CONTEXT_FLAG_OVERLAY) {
            if (not dmi_registry_overlay(context->state.registry))
                break;
        }

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
    dmi_type_candidates_t *map = dmi_types_create(context);
    if (map == nullptr)
        return false;

    bool success = dmi_types_map(context, nullptr, true, map);
    if (success)
        memcpy(context->type_map, map, sizeof(*map) * (DMI_TYPE_ID_MAX + 1));

    dmi_free(map);

    return success;
}

static bool dmi_setup_vendor(dmi_context_t *context)
{
    dmi_entity_t *entity;
    const dmi_firmware_t *firmware;
    const dmi_vendor_spec_t *vendor;

    dmi_log_debug(context, "Detecting SMBIOS vendor...");

    entity = dmi_registry_lookup_first(context->state.registry, DMI_TYPE(firmware), true);
    if (entity == nullptr) {
        if ((context->flags & DMI_CONTEXT_FLAG_STRICT) == 0) {
            dmi_log_notice(context, dmi_error_message(DMI_ERROR_MISSING_FIRMWARE_INFO));
            return true;
        }

        dmi_error_raise(context, DMI_ERROR_MISSING_FIRMWARE_INFO);
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
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
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
    dmi_registry_iter_init(&iter, registry, nullptr);

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

static const dmi_platform_t *dmi_context_platform(const dmi_context_t *context)
{
    return (context->platform != nullptr) ? context->platform : context->state.platform;
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

static dmi_type_candidates_t *dmi_types_create(dmi_context_t *context)
{
    return dmi_alloc_array(context, sizeof(dmi_type_candidates_t), DMI_TYPE_ID_MAX + 1);
}

static bool dmi_types_map(
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

#if !defined(__KERNEL__)
static int dmi_save_stat(const char *path, dmi_file_stat_t *st)
{
#if defined(_WIN32)
    return _stat(path, st);
#else
    return lstat(path, st);
#endif
}

static char *dmi_save_temp_open(dmi_context_t *context, const char *path, int *pfd)
{
    int mode = O_CREAT | O_EXCL | O_WRONLY;
#if defined(O_BINARY)
    mode |= O_BINARY;
#endif

#if defined(_WIN32)
    long pid = (long)_getpid();
#else
    long pid = (long)getpid();
#endif

    *pfd = -1;

    // Name is tried again if it is taken, e.g. by another dump of the same
    // process being saved at the same time
    for (unsigned attempt = 0; attempt < DMI_SAVE_TEMP_ATTEMPTS; attempt++) {
        char *temp = nullptr;

        if (dmi_asprintf(&temp, "%s.%ld-%u.tmp", path, pid, attempt) < 0) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            return nullptr;
        }

        int fd = open(temp, mode, 0666);
        if (fd >= 0) {
            *pfd = fd;
            return temp;
        }

        int error = errno;
        dmi_free(temp);

        if (error != EEXIST) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(error));
            return nullptr;
        }
    }

    dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: Unable to create temporary file", path);

    return nullptr;
}

static bool dmi_save_commit(dmi_context_t *context, const char *temp, const char *path, bool overwrite)
{
#if defined(_WIN32)
    // Unlike rename(), the system call replaces an existing file on request,
    // and fails if there is one otherwise
    if (MoveFileExA(temp, path, overwrite ? MOVEFILE_REPLACE_EXISTING : 0))
        return true;

    DWORD error = GetLastError();
    bool  is_existing = (error == ERROR_ALREADY_EXISTS) or (error == ERROR_FILE_EXISTS);

    dmi_error_raise_ex(context, is_existing ? DMI_ERROR_FILE_OPEN : DMI_ERROR_FILE_WRITE,
                       "%s: %s", path, dmi_win32err_to_string(error));

    return false;
#else
    if (not overwrite) {
        // Hard link is made only if there is no such file yet, unlike a
        // rename, which would replace a file created in the meantime
        if (link(temp, path) == 0) {
            remove(temp);
            return true;
        }

        if (errno == EEXIST) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(errno));
            return false;
        }

        // File systems with no hard links, e.g. FAT, leave nothing but a
        // rename, so the target is checked to be missing once again
        dmi_file_stat_t st;
        if (dmi_save_stat(path, &st) == 0) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(EEXIST));
            return false;
        }
    }

    if (rename(temp, path) < 0) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s: %s", path, strerror(errno));
        return false;
    }

    return true;
#endif
}

static bool dmi_dump_write(
        dmi_context_t    *context,
        int               fd,
        const char       *path,
        const dmi_data_t *data,
        size_t            size)
{
    ssize_t nwritten = dmi_file_write(fd, data, size);

    if (nwritten < 0) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s: %s", path, strerror(errno));
        return false;
    }
    if ((size_t)nwritten < size) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s: Incomplete write", path);
        return false;
    }

    return true;
}

static bool dmi_dump_entry_build(dmi_context_t *context, dmi_byte_t *entry)
{
    const dmi_entry_spec_t *spec = context->state.entry_spec;
    size_t length;

    memset(entry, 0, DMI_ENTRY_MAX_SIZE);

    // Entry point data is optional, Windows backend does not provide it
    if ((context->state.entry == nullptr) or (spec == nullptr))
        return dmi_dump_entry_generate(context, entry);

    memcpy(entry, context->state.entry->data, context->state.entry->length);

    if (spec->version >= DMI_VERSION(3, 0, 0)) {
        dmi_entry_v30_t *eps = dmi_cast(eps, entry);

        length = dmi_decode(eps->length);

        dmi_entry_set(eps->table_area_addr, dmi_encode_qword(DMI_ENTRY_MAX_SIZE));
        dmi_entry_set_checksum(eps, length);
    } else if (spec->version >= DMI_VERSION(2, 1, 0)) {
        dmi_entry_v21_t *eps = dmi_cast(eps, entry);
        dmi_entry_legacy_t *ieps = dmi_cast(ieps, &eps->ieps);

        // Unlike dmidecode, the intermediate entry point is always written
        // completely, even if the entry point length is 0x1E. Otherwise, its
        // checksum would become invalid.
        length = dmi_decode(eps->length);
        if (length < sizeof(dmi_entry_v21_t))
            length = sizeof(dmi_entry_v21_t);

        // Intermediate entry point bytes sum to zero before and after the
        // relocation, so the entry point checksum needs no adjustment.
        dmi_entry_set(ieps->table_area_addr, dmi_encode_dword(DMI_ENTRY_MAX_SIZE));
        dmi_entry_set_checksum(ieps, sizeof(dmi_entry_legacy_t));
    } else {
        dmi_entry_legacy_t *eps = dmi_cast(eps, entry);

        length = sizeof(dmi_entry_legacy_t);

        dmi_entry_set(eps->table_area_addr, dmi_encode_dword(DMI_ENTRY_MAX_SIZE));
        dmi_entry_set_checksum(eps, length);
    }

    // Only the entry point itself is written, the rest is zero-filled
    assert(length <= DMI_ENTRY_MAX_SIZE);
    memset(entry + length, 0, DMI_ENTRY_MAX_SIZE - length);

    return true;
}

static bool dmi_dump_entry_generate(dmi_context_t *context, dmi_byte_t *entry)
{
    dmi_version_t version = context->state.smbios_version;

    if (context->state.table->length > UINT32_MAX) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_STATE,
                           "SMBIOS table is too large: %zu bytes", context->state.table->length);
        return false;
    }

    dmi_entry_v30_t *eps = dmi_cast(eps, entry);

    const dmi_entry_v30_t data = {
        .length              = dmi_encode_byte(sizeof(dmi_entry_v30_t)),
        .version_major       = dmi_encode_byte((uint8_t)dmi_version_major(version)),
        .version_minor       = dmi_encode_byte((uint8_t)dmi_version_minor(version)),
        .version_rev         = dmi_encode_byte((uint8_t)dmi_version_revision(version)),
        .revision            = dmi_encode_byte(0x01), // SMBIOS 3.0 entry point
        .table_area_max_size = dmi_encode_dword((uint32_t)context->state.table->length),
        .table_area_addr     = dmi_encode_qword(DMI_ENTRY_MAX_SIZE)
    };

    memcpy(eps, &data, sizeof(data));

    // Anchor is not null-terminated, so it is not initialized from string
    memcpy(eps, DMI_ANCHOR_V30, strlen(DMI_ANCHOR_V30));

    dmi_entry_set_checksum(eps, sizeof(dmi_entry_v30_t));

    return true;
}
#endif // !defined(__KERNEL__)
