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
#include <opendmi/platform.h>
#include <opendmi/registry.h>
#include <opendmi/utils/vector.h>
#include <opendmi/utils/version.h>

typedef struct dmi_context_state dmi_context_state_t;

/**
 * @brief Number of the specifications a type number is mapped to at most.
 */
#define DMI_TYPE_CANDIDATES 4

/**
 * @brief Specifications a type number is mapped to: the one without a
 * signature, or @c nullptr, followed by the ones with signatures, see
 * `dmi_signature_t`, and @c nullptr past the last one.
 */
typedef const dmi_entity_spec_t *dmi_type_candidates_t[DMI_TYPE_CANDIDATES];

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
    DMI_CONTEXT_FLAG_OVERLAY = (1 << 2),

    /**
     * Enable the extension modules of the platform as the context is opened,
     * in addition to the ones enabled explicitly. A module is enabled for the
     * platforms its `platforms` member lists, unless its types conflict with
     * the types of the modules enabled before. Modules enabled this way stay
     * enabled until the context is closed.
     */
    DMI_CONTEXT_FLAG_AUTO_MODULES = (1 << 3)
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
     * @brief SMBIOS entry point data, as the backend has read it, which may
     * be longer than the entry point length.
     *
     * The data belongs to the context, which holds it for as long as it is
     * open.
     */
    dmi_buffer_t *entry;

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
     * @brief SMBIOS table area data, as the backend has read it.
     *
     * The data belongs to the context, which holds it for as long as it is
     * open: the decoded structures refer to it in place, and so do the
     * strings they carry.
     */
    dmi_buffer_t *table;

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
     * @brief Platform the data comes from, as told from the data, owned by
     * the context.
     */
    dmi_platform_t *platform;

    /**
     * @brief Extension modules enabled for the platform as the context was
     * opened (`const dmi_module_t *`).
     */
    dmi_vector_t modules;

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
    dmi_type_candidates_t *type_map;

    /**
     * @brief Extension modules enabled explicitly (`const dmi_module_t *`).
     */
    dmi_vector_t modules;

    /**
     * @brief Platform set with `dmi_set_platform()`, which is used instead of
     * the one told from the data, owned by the context. @c nullptr if none
     * has been set.
     */
    dmi_platform_t *platform;

    /**
     * @brief Error state.
     */
    dmi_error_queue_t error_queue;

    /**
     * @brief Flags.
     */
    unsigned int flags;

    /**
     * @brief State of opened context.
     */
    dmi_context_state_t state;
};

/**
 * @brief Flags of `dmi_save`(3).
 */
typedef enum dmi_save_flags
{
    /**
     * Overwrite the file if it exists.
     */
    DMI_SAVE_FLAG_OVERWRITE = (1 << 0),

    /**
     * Replace the values identifying the system, see `dmi_anonymize`(3).
     */
    DMI_SAVE_FLAG_ANONYMIZE = (1 << 1)
} dmi_save_flags_t;

/**
 * @brief Check whether a context is open.
 *
 * @param[in] context Context in question.
 *
 * @return `true` if the context holds a structure table and the registry of
 *         its structures, `false` otherwise.
 */
static inline bool dmi_context_is_open(const dmi_context_t *context)
{
    return (context->state.table != nullptr) && (context->state.registry != nullptr);
}

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
 * @brief Add DMI extension.
 *
 * Registers entity specifications provided by @p module in the context and
 * adds the module to the list of enabled modules. Specifications are mapped
 * to their types, or to the types the relocations of the enabled modules
 * give them, and the ones bound to a range of platform generations are mapped
 * only if the generation of the platform lies within the range. Fails if any
 * of the types is already mapped to another specification, including the
 * case when the module is already enabled.
 *
 * Modules enabled before the context is opened are mapped again as it is
 * opened, once the platform is known.
 *
 * @param[in] context DMI context handle.
 * @param[in] module  Extension module to enable.
 *
 * @return The function returns `true` on success and `false` otherwise.
 *
 * @error DMI_ERROR_NULL_ARGUMENT The module is `nullptr`.
 * @error DMI_ERROR_MODULE_CONFLICT The module is already enabled, or its
 *        types are mapped to specifications of other modules.
 */
__dmi_api bool dmi_add_extension(dmi_context_t *context, const dmi_module_t *module);

/**
 * @brief Check whether DMI extension is enabled.
 *
 * @param[in] context DMI context handle.
 * @param[in] module  Extension module.
 *
 * @return `true` if @p module has been enabled with `dmi_add_extension()`,
 *         or for the platform as the context was opened; `false` otherwise.
 */
__dmi_api bool dmi_has_extension(const dmi_context_t *context, const dmi_module_t *module);

/**
 * @brief Get the platform of the context.
 *
 * The platform is told from the firmware and system information as the
 * context is opened, and is gone when it is closed. A platform set with
 * `dmi_set_platform()` is used instead, whether the context is open or not.
 *
 * @param[in] context DMI context handle.
 *
 * @return The platform, which belongs to the context and is valid until the
 *         context is closed or the platform is set again, or @c nullptr if
 *         @p context is @c nullptr or there is no platform, since the context
 *         is closed and no platform has been set.
 */
__dmi_api const dmi_platform_t *dmi_get_platform(const dmi_context_t *context);

/**
 * @brief Set the platform of the context.
 *
 * Sets the platform the structures are decoded for, instead of the one told
 * from the data, and maps the specifications of the enabled modules for it
 * again. This allows decoding the data of a platform whose firmware
 * information is missing or wrong, and decoding structures without opening
 * the context. A platform set this way is kept as the context is opened and
 * closed. Passing @c nullptr makes the context use the platform told from the
 * data again.
 *
 * The context keeps a copy of @p platform, which the caller still owns.
 *
 * @param[in] context  DMI context handle.
 * @param[in] platform Platform to set, or @c nullptr.
 *
 * @return `true` on success, `false` otherwise. On failure, the platform
 *         and the mapping of the specifications are left unchanged.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 * @error DMI_ERROR_MODULE_CONFLICT The specifications of the enabled modules
 *        which apply to the platform are mapped to the same type.
 */
__dmi_api bool dmi_set_platform(dmi_context_t *context, const dmi_platform_t *platform);

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
 * @brief Load dump file into DMI context.
 *
 * @param[in] context DMI context handle.
 * @param[in] path Path to dump file.
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_load(dmi_context_t *context, const char *path);

/**
 * @brief Save DMI context to dump file.
 *
 * @details
 * The dump file format is compatible with `dmidecode --dump-bin`: the entry
 * point structure, padded with zeroes to #DMI_ENTRY_MAX_SIZE bytes, followed
 * by the structure table. The table address in the entry point is set to the
 * table offset in the file. If the context has no entry point data, a 64-bit
 * entry point is generated. With `DMI_SAVE_FLAG_ANONYMIZE`, the table is the
 * copy `dmi_anonymize`(3) makes of it, and the context itself is left as it
 * is.
 *
 * A dump into a regular file is written to a temporary file in the same
 * directory, which replaces @p path only once it is complete, so that a
 * failure neither leaves an incomplete dump behind nor destroys the file
 * being overwritten. Devices, pipes and symbolic links are written directly.
 *
 * @param[in] context DMI context handle.
 * @param[in] path    Path to dump file.
 * @param[in] flags   Flags of `dmi_save_flags_t`.
 *
 * @error DMI_ERROR_NULL_ARGUMENT Path is `nullptr`
 * @error DMI_ERROR_INVALID_STATE Context is not open, or its structures carry
 *        additional information entries applied to them and the table is to
 *        be anonymized
 * @error DMI_ERROR_INVALID_EPS_LENGTH Entry point is longer than a dump holds
 * @error DMI_ERROR_FILE_OPEN File or its temporary file cannot be created, or
 *        the file exists and is not to be overwritten
 * @error DMI_ERROR_FILE_WRITE File cannot be written, or the temporary file
 *        cannot be renamed over it
 * @error DMI_ERROR_OUT_OF_MEMORY Memory cannot be allocated
 * @error DMI_ERROR_INTERNAL Table cannot be anonymized
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_save(dmi_context_t *context, const char *path, unsigned flags);

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
 * @brief Get logging handler of DMI context.
 *
 * @param[in] context DMI context handle.
 *
 * @return Logging handler, or @c nullptr if there is none.
 */
__dmi_api dmi_log_t *dmi_get_logger(const dmi_context_t *context);

/**
 * @brief Set logging level of DMI context.
 *
 * Messages of levels above @p level are dropped before they reach the logging
 * handler, which has a level of its own.
 *
 * @param[in] context DMI context handle.
 * @param[in] level   Logging level.
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_set_log_level(dmi_context_t *context, dmi_log_level_t level);

/**
 * @brief Get logging level of DMI context.
 *
 * @param[in] context DMI context handle.
 *
 * @return Logging level, or `DMI_LOG_INVALID` if @p context is @c nullptr.
 */
__dmi_api dmi_log_level_t dmi_get_log_level(const dmi_context_t *context);

/**
 * @brief Write a message to the logging handler of DMI context.
 *
 * The message is dropped if @p level is above the logging level of the
 * context, see `dmi_set_log_level`(3), or above the level of the handler.
 *
 * @param[in] context DMI context handle.
 * @param[in] level   Logging level of the message.
 * @param[in] format  Message format string.
 *
 * @return `true` if the message has been written, `false` otherwise.
 */
__dmi_api bool dmi_log(dmi_context_t *context, dmi_log_level_t level, const char *format, ...);

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

/**
 * @name Logging shorthands
 *
 * Write a message of the given level to the logging handler of the context,
 * see `dmi_log`(3).
 * @{
 */
#define dmi_log_error(context, format, ...) \
        dmi_log(context, DMI_LOG_ERROR, format, ##__VA_ARGS__)
#define dmi_log_warning(context, format, ...) \
        dmi_log(context, DMI_LOG_WARNING, format, ##__VA_ARGS__)
#define dmi_log_notice(context, format, ...) \
        dmi_log(context, DMI_LOG_NOTICE, format, ##__VA_ARGS__)
#define dmi_log_info(context, format, ...) \
        dmi_log(context, DMI_LOG_INFO, format, ##__VA_ARGS__)
#define dmi_log_debug(context, format, ...) \
        dmi_log(context, DMI_LOG_DEBUG, format, ##__VA_ARGS__)
/** @} */

#endif // !OPENDMI_CONTEXT_H
