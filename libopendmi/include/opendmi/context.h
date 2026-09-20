//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_CONTEXT_H
#define OPENDMI_CONTEXT_H

#pragma once

#include <opendmi/types.h>
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/backend.h>
#include <opendmi/vendor.h>
#include <opendmi/module.h>
#include <opendmi/registry.h>
#include <opendmi/utils/vector.h>
#include <opendmi/utils/version.h>

typedef struct dmi_context_state dmi_context_state_t;

#ifndef DMI_ENTRY_SPEC_T
#   define DMI_ENTRY_SPEC_T
    typedef struct dmi_entry_spec dmi_entry_spec_t;
#endif // !DMI_ENTRY_SPEC_T

/**
 * @brief Context flags.
 */
typedef enum dmi_context_flags
{
    /**
     * Default mode, in which as much of the table as possible is made
     * available. Malformed structures are left undecoded, broken references
     * and invalid additional information entries are skipped, and the errors
     * are left in the error queue. A structure with invalid length ends the
     * table, and missing firmware information leaves the vendor unknown.
     */
    DMI_CONTEXT_FLAG_RELAXED = 0,

    /**
     * Fail to open the context if the table has any error: invalid structure
     * length, missing firmware information, malformed structure, broken
     * reference or invalid additional information entry. Decoding and linking
     * still go on after the first error, so that all errors are reported to
     * the error queue.
     */
    DMI_CONTEXT_FLAG_STRICT  = (1 << 0),

    /**
     * Resolve references between structures after decoding, and attach
     * string properties (type 46) to their parent structures.
     */
    DMI_CONTEXT_FLAG_LINK    = (1 << 1),

    /**
     * Decode structures with values of additional information entries
     * (type 40) applied. Entries usually do not carry field values, but
     * annotations with placeholder values, so this is disabled by default.
     */
    DMI_CONTEXT_FLAG_OVERLAY = (1 << 2)
} dmi_context_flags_t;

/**
 * @brief State of opened DMI context. Populated when the context is opened
 * and reset to all zeroes when it is closed.
 */
struct dmi_context_state
{
    /**
     * @brief SMBIOS version number.
     */
    dmi_version_t smbios_version;

    /**
     * @brief Platform address size in bytes, 4 for 32-bit platforms, 8 for
     * 64-bit platforms. Zero means unspecified.
     */
    size_t address_size;

    /**
     * @brief Address of SMBIOS entry point.
     */
    uint64_t entry_address;

    /**
     * @brief Size of entry point data, provided by backend. It may differ
     * from the entry point length.
     */
    size_t entry_data_size;

    /**
     * @brief Entry point length, as specified in the entry point.
     */
    size_t entry_length;

    /**
     * @brief SMBIOS entry point format version.
     */
    dmi_version_t entry_version;

    /**
     * @brief SMBIOS entry point revision.
     */
    unsigned entry_revision;

    /**
     * @brief Entry point specification.
     */
    const dmi_entry_spec_t *entry_spec;

    /**
     * @brief Pointer to SMBIOS entry point data.
     */
    void *entry_data;

    /**
     * @brief Total number of SMBIOS structures.
     */
    size_t entity_count;

    /**
     * @brief Address of SMBIOS table area.
     */
    uint64_t table_area_addr;

    /**
     * @brief Size of SMBIOS table area, specified in the entry point.
     */
    size_t table_area_size;

    /**
     * @brief Maximum size of SMBIOS table area.
     */
    size_t table_area_max_size;

    /**
     * @brief Pointer to SMBIOS table area data.
     */
    dmi_data_t *table_data;

    /**
     * @brief Actual size of SMBIOS table area data, provided by backend. It
     * may differ from the size specified in the entry point.
     */
    size_t table_size;

    /**
     * @brief Maximum size of SMBIOS structure.
     */
    size_t entity_max_size;

    /**
     * @brief Backend handle.
     */
    const dmi_backend_t *backend;

    /**
     * @brief Backend-specific data.
     */
    void *session;

    /**
     * @brief Vendor identifier.
     */
    dmi_vendor_t vendor;

    /**
     * @brief Vendor name.
     */
    const char *vendor_name;

    /**
     * @brief Entity registry.
     */
    dmi_registry_t *registry;
};

/**
 * @brief DMI context descriptor.
 */
struct dmi_context
{
    /**
     * @brief Context logger.
     */
    dmi_log_t *logger;

    /**
     * @brief Logging level.
     */
    dmi_log_level_t log_level;

    /**
     * @brief Entity specifications map.
     */
    const dmi_entity_spec_t **type_map;

    /**
     * @brief Enabled extension modules (`const dmi_module_t *`).
     */
    dmi_vector_t modules;

    /**
     * @brief Error state.
     */
    dmi_error_queue_t error_queue;

    /**
     * @brief ICU4C resources.
     */
    void *resources;

    /**
     * @brief Flags.
     */
    unsigned int flags;

    /**
     * @brief State of opened context.
     */
    dmi_context_state_t state;
};

__BEGIN_DECLS

/**
 * @brief Create DMI context.
 */
__dmi_api dmi_context_t *dmi_create(unsigned int flags);

/**
 * @brief Set DMI context flags.
 *
 * @param[in] context DMI context handle.
 * @param[in] flags Flags
 */
__dmi_api void dmi_set_flags(dmi_context_t *context, unsigned flags);

/**
 * @brief Get DMI context flags.
 *
 * @param[in] context DMI context handle.
 * @return Flags
 */
__dmi_api unsigned dmi_get_flags(const dmi_context_t *context);

/**
 * @brief Open DMI context.
 *
 * @param[in] context DMI context handle.
 * @param[in] device  Path to memory device.
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_open(dmi_context_t *context, const char *device);

/**
 * @brief Add DMI extension.
 *
 * Registers entity specifications provided by @p module in the context and
 * adds the module to the list of enabled modules. Fails if any of the entity
 * types is already registered, including the case when the module is already
 * enabled.
 *
 * @param[in] context DMI context handle.
 * @param[in] module  Extension module to enable.
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_add_extension(dmi_context_t *context, const dmi_module_t *module);

/**
 * @brief Check whether DMI extension is enabled.
 *
 * @param[in] context DMI context handle.
 * @param[in] module  Extension module.
 *
 * @return `true` if @p module has been enabled with `dmi_add_extension()`,
 *         `false` otherwise.
 */
__dmi_api bool dmi_has_extension(const dmi_context_t *context, const dmi_module_t *module);

/**
 * @brief Load dump file into DMI context.
 *
 * @param[in] context DMI context handle.
 * @param[in] path Path to dump file.
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_dump_load(dmi_context_t *context, const char *path);

/**
 * @brief Save DMI context to dump file.
 *
 * @details
 * The dump file format is compatible with `dmidecode --dump-bin`: the entry
 * point structure, padded with zeroes to #DMI_ENTRY_MAX_SIZE bytes, followed
 * by the structure table. The table address in the entry point is set to the
 * table offset in the file. If the context has no entry point data, a 64-bit
 * entry point is generated.
 *
 * @param[in] context   DMI context handle.
 * @param[in] path      Path to dump file.
 * @param[in] overwrite Overwrite existing files flag
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_dump_save(dmi_context_t *context, const char *path, bool overwrite);

/**
 * @brief Find entity type identifier by its code.
 */
__dmi_api dmi_type_t dmi_type_find(dmi_context_t *context, const char *code);

/**
 * @brief Get entity type specification.
 */
__dmi_api const dmi_entity_spec_t *dmi_type_spec(dmi_context_t *context, dmi_type_t type);

/**
 * @brief Get entity type name.
 */
__dmi_api const char *dmi_type_name(dmi_context_t *context, dmi_type_t type);

/**
 * @brief Set logging handler.
 *
 * @param[in] context DMI context handle.
 * @param[in] logger Logging handler.
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_set_logger(dmi_context_t *context, dmi_log_t *logger);

/**
 * @brief Get entity registry of an opened DMI context.
 *
 * @param[in] context DMI context handle.
 *
 * @return Non-owning pointer to the registry, or @c nullptr if the context is
 *         not opened.
 */
__dmi_api dmi_registry_t *dmi_get_registry(dmi_context_t *context);

/**
 * @brief Close DMI context.
 *
 * @param[in] context DMI context handle.
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_close(dmi_context_t *context);

/**
 * @brief Destroy DMI context.
 *
 * @param[in] context DMI context handle.
 */
__dmi_api void dmi_destroy(dmi_context_t *context);

__END_DECLS

#endif // !OPENDMI_CONTEXT_H
