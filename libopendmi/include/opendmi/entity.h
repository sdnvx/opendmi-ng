//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_H
#define OPENDMI_ENTITY_H

#pragma once

#include <opendmi/stream.h>
#include <opendmi/attribute.h>
#include <opendmi/utils/version.h>

typedef struct dmi_entity       dmi_entity_t;
typedef struct dmi_entity_spec  dmi_entity_spec_t;
typedef struct dmi_entity_ops   dmi_entity_ops_t;
typedef struct dmi_header       dmi_header_t;
typedef struct dmi_string_entry dmi_string_entry_t;

typedef bool dmi_entity_validate_fn(dmi_entity_t *entity);
typedef bool dmi_entity_decode_fn(dmi_entity_t *entity);
typedef bool dmi_entity_link_fn(dmi_entity_t *entity);
typedef void dmi_entity_cleanup_fn(dmi_entity_t *entity);

typedef enum dmi_entity_state
{
    /** Entity has been decoded. */
    DMI_ENTITY_STATE_DECODED    = (1 << 0),

    /** Cross-references of entity have been resolved. */
    DMI_ENTITY_STATE_LINKED     = (1 << 1),

    /** Entity has been validated. */
    DMI_ENTITY_STATE_VALID      = (1 << 2),

    /**
     * Entity has been decoded, but its data ends in the middle of a set of
     * fields, and only completely present fields are decoded. Such entity
     * length does not match any specification version.
     */
    DMI_ENTITY_STATE_INCOMPLETE = (1 << 3),

    /**
     * Entity conforms to an older specification version, and does not
     * contain all fields known to the decoder.
     */
    DMI_ENTITY_STATE_PARTIAL    = (1 << 4)
} dmi_entity_state_t;

/**
 * @brief Entity state flag names, identified by bit numbers.
 */
extern const dmi_name_set_t dmi_entity_state_names;

/**
 * @brief Entity operations.
 */
struct dmi_entity_ops
{
    /**
     * @brief Validation handler (optional).
     */
    dmi_entity_validate_fn *validate;

    /**
     * @brief Decoding handler.
     */
    dmi_entity_decode_fn *decode;

    /**
     * @brief Link handler.
     */
    dmi_entity_link_fn *link;

    /**
     * @brief Cleanup handler.
     */
    dmi_entity_cleanup_fn *cleanup;
};

/**
 * @brief Entity specification.
 */
struct dmi_entity_spec
{
    /**
     * @brief Code name, used to identify the entity during serialization.
     */
    const char *code;

    /**
     * @brief Printable name, used to identify the entity when printing.
     */
    const char *name;

    /**
     * @brief Short description.
     */
    const char **description;

    /**
     * @brief SMBIOS type.
     */
    dmi_type_t type;

    /**
     * @brief Minimum SMBIOS version. Should be set to `DMI_VERSION_NONE` when
     * not specified.
     */
    dmi_version_t minimum_version;

    /**
     * @brief The SMBIOS version starting from which the structure is mandatory.
     * Should be set to `DMI_VERSION_NONE` for optional structures.
     */
    dmi_version_t required_from;

    /**
     * @brief The SMBIOS version up to which the structure is mandatory.
     * Should be set to `DMI_VERSION_NONE` for optional structures.
     */
    dmi_version_t required_till;

    /**
     * @brief Should be set to true if the structure should be the only one
     * in the table.
     */
    bool unique;

    /**
     * @brief Minimum length. Zero means that the minimum length is not
     * specified.
     *
     * Starting with SMBIOS 2.3, each SMBIOS structure type has a minimum
     * length - enabling the addition of new, but optional, fields to SMBIOS
     * structures. In no case shall a structure’s length result in a field
     * being less than fully populated. For example, a voltage probe structure
     * with length of 0x15 is invalid because the nominal value field would
     * not be fully specified.
     *
     * @since SMBIOS 2.3
     */
    size_t minimum_length;

    /**
     * @brief The size of the structure descriptor. Used for automatic memory
     * allocation during decoding.
     */
    size_t decoded_length;

    /**
     * @brief Attribute specifications.
     */
    const dmi_attribute_t *attributes;

    /**
     * @brief Operation handlers.
     */
    dmi_entity_ops_t handlers;
};

/**
 * @brief SMBIOS structure header.
 */
dmi_packed_struct(dmi_header)
{
    /**
     * @brief Specifies the type of structure. Types 0 through 127 (7Fh) are
     * reserved for and defined by this specification. Types 128 through 256
     * (0x80 to 0xFF) are available for system- and OEM-specific information.
     */
    dmi_byte_t type;

    /**
     * @brief Specifies the length of the formatted area of the structure,
     * starting at the Type field. The length of the structure’s string-set is
     * not included.
     */
    dmi_byte_t length;

    /**
     * @brief Specifies the structure’s handle.
     *
     * If the system configuration changes, a previously assigned handle might
     * no longer exist. However, after a handle has been assigned by the
     * platform firmware, the firmware cannot re-assign that handle number to
     * another structure.
     *
     * Unless otherwise specified, when referring to another structure’s
     * handle, the value 0x0FFFF is used to indicate that the referenced handle
     * is not applicable or does not exist.
     *
     * @see dmi_handle_t
     */
    dmi_handle_t handle;
};

/**
 * @brief String entry.
 */
struct dmi_string_entry
{
    /**
     * @brief Raw value.
     */
    const char *raw;

    /**
     * @brief Pretty value.
     */
    char *pretty;
};

/**
 * @brief Entity descriptor.
 */
struct dmi_entity
{
    /**
     * @brief Context handle.
     */
    dmi_context_t *context;

    /**
     * @brief Entity type.
     */
    dmi_type_t type;

    /**
     * @brief Pointer to specification.
     */
    const dmi_entity_spec_t *spec;

    /**
     * @brief DMI handle.
     */
    dmi_handle_t handle;

    /**
     * @brief Pointer to raw SMBIOS structure data, including header.
     */
    const dmi_data_t *data;

    /**
     * @brief Total length of the SMBIOS structure.
     */
    size_t total_length;

    /**
     * @brief Formatted section length.
     */
    size_t body_length;

    /**
     * @brief Unformed section length. The unformed section of the SMBIOS
     * structure is used for passing variable data such as text strings.
     */
    size_t extra_length;

    /**
     * @brief Data stream.
     */
    dmi_stream_t stream;

    /**
     * @brief String data.
     */
    dmi_string_entry_t *strings;

    /**
     * @brief Number of strings in the SMBIOS structure.
     */
    size_t string_count;

    /**
     * @brief Decoded information.
     */
    void *info;

    /**
     * @brief Structure version.
     */
    dmi_version_t level;

    /**
     * @brief Entity state mask.
     */
    unsigned int state;
};

__BEGIN_DECLS

/**
 * @internal
 * @brief Create SMBIOS entity.
 *
 * Allocates a new entity descriptor and initializes it from the raw SMBIOS
 * structure data. Parses the structure header, computes the total length
 * (formatted area plus string set), and decodes the string table.
 *
 * @param[in] context    DMI context handle.
 * @param[in] data       Pointer to raw SMBIOS structure data, starting with the
 *                       structure header.
 * @param[in] max_length Total amount of remaining data in the table area.
 *
 * @return A newly allocated entity descriptor on success, or @c nullptr on
 *         failure (e.g., invalid arguments, invalid structure length, or
 *         allocation error).
 */
dmi_entity_t *dmi_entity_create(
        dmi_context_t *context,
        const void    *data,
        size_t         max_length);

/**
 * @internal
 * @brief Decode SMBIOS entity.
 *
 * Looks up the entity specification by type, validates the minimum length
 * constraint, allocates the decoded structure descriptor, and invokes the
 * type-specific decode handler. If decoding has already been performed, this
 * function returns `true` immediately.
 *
 * @param[in] entity Entity descriptor.
 *
 * @return `true` on success (including when already decoded or no decoder is
 *         registered), `false` on failure.
 */
bool dmi_entity_decode(dmi_entity_t *entity);

/**
 * @internal
 * @brief Link SMBIOS entity.
 *
 * Invokes the type-specific link handler to resolve cross-references between
 * entities (e.g., handle-based references to other structures). If linking has
 * already been performed, this function returns `true` immediately.
 *
 * @param[in] entity Entity descriptor.
 *
 * @return `true` on success, `false` if linking failed or the entity has no
 *         specification or link handler.
 */
bool dmi_entity_link(dmi_entity_t *entity);

/**
 * @brief Get entity handle.
 *
 * @param[in] entity Entity descriptor.
 *
 * @return The handle associated with the entity, or `DMI_HANDLE_INVALID` if
 *         @p entity is @c nullptr.
 */
dmi_handle_t dmi_entity_handle(const dmi_entity_t *entity);

/**
 * @brief Get entity type.
 *
 * @param[in] entity Entity descriptor.
 *
 * @return The SMBIOS type of the entity, or `DMI_TYPE_INVALID` if @p entity
 *         is @c nullptr.
 */
dmi_type_t dmi_entity_type(const dmi_entity_t *entity);

/**
 * @brief Get entity type name.
 *
 * Returns the human-readable name of the entity's SMBIOS structure type.
 *
 * @param[in] entity Entity descriptor.
 *
 * @return The type name string, or @c nullptr if @p entity is @c nullptr.
 */
const char *dmi_entity_name(const dmi_entity_t *entity);

/**
 * @brief Get pointer to raw SMBIOS data area of entity of the specified type.
 *
 * Returns a pointer to the raw SMBIOS structure data (including the header)
 * if the entity matches the specified type.
 *
 * @param[in] entity Entity descriptor.
 * @param[in] type   Expected SMBIOS type. Pass `DMI_TYPE_INVALID` to skip
 *                   type checking.
 *
 * @return Pointer to the raw SMBIOS data, or @c nullptr if @p entity is @c nullptr
 *         or the entity type does not match @p type.
 */
const void *dmi_entity_data(const dmi_entity_t *entity, dmi_type_t type);

/**
 * @brief Get pointer to decoded data of entity of the specified type.
 *
 * Returns a pointer to the decoded structure descriptor if the entity matches
 * the specified type. The returned pointer should be cast to the appropriate
 * structure type (e.g., `dmi_cooling_device_t *`).
 *
 * @param[in] entity Entity descriptor.
 * @param[in] type   Expected SMBIOS type. Pass `DMI_TYPE_INVALID` to skip
 *                   type checking.
 *
 * @return Pointer to the decoded data, or @c nullptr if @p entity is @c nullptr,
 *         the entity has not been decoded, or the entity type does not match
 *         @p type.
 */
void *dmi_entity_info(const dmi_entity_t *entity, dmi_type_t type);

/**
 * @brief Get entity string.
 *
 * Retrieves a string from the entity's string table by its 1-based index.
 * When @p raw is `false`, the returned string is trimmed of leading and
 * trailing whitespace.
 *
 * @param[in] entity Entity descriptor.
 * @param[in] num    1-based string index. String set may contain more strings
 *                   than can be referenced by `dmi_string_t` fields.
 * @param[in] raw    If `true`, return the raw (untrimmed) string; if `false`,
 *                   return the trimmed version.
 *
 * @return The requested string, or @c nullptr if @p entity is @c nullptr, @p num is
 *         zero, or @p num exceeds the number of strings in the entity.
 */
const char *dmi_entity_string_ex(const dmi_entity_t *entity, size_t num, bool raw);

/**
 * @internal
 * @brief Stop decoding at the end of entity data.
 *
 * Intended for decoders of structures, which were extended in newer
 * specification versions. Called when all entity data has been decoded, but
 * the decoder knows more fields. Marks the entity with
 * `DMI_ENTITY_STATE_PARTIAL`.
 *
 * Entity data must be exhausted. Otherwise, the entity is handled as
 * incomplete (see `dmi_entity_incomplete`).
 *
 * @param[in,out] entity Entity descriptor.
 *
 * @return Always `true`, so that it can be returned by the decoder.
 */
bool dmi_entity_stop(dmi_entity_t *entity);

/**
 * @internal
 * @brief Stop decoding at incomplete set of entity fields.
 *
 * Intended for decoders of structures, which were extended in newer
 * specification versions. Called when entity data ends in the middle of a set
 * of fields, so the length of entity does not match any specification
 * version. Fields, which are completely present, should be decoded before
 * the call. Marks the entity with `DMI_ENTITY_STATE_INCOMPLETE`, and the
 * remaining data (a part of the next field, if any) is ignored.
 *
 * @param[in,out] entity Entity descriptor.
 *
 * @return Always `true`, so that it can be returned by the decoder.
 */
bool dmi_entity_incomplete(dmi_entity_t *entity);

/**
 * @brief Destroy entity descriptor.
 *
 * Releases all resources associated with the entity, including decoded data,
 * strings, and the entity descriptor itself. If @p entity is @c nullptr, this
 * function does nothing.
 *
 * @param[in] entity Entity descriptor to destroy.
 */
void dmi_entity_destroy(dmi_entity_t *entity);

__END_DECLS

/**
 * @brief Get entity string (trimmed).
 *
 * Convenience wrapper around `dmi_entity_string_ex`(3) that returns the
 * trimmed version of the string at @p num.
 *
 * @param[in] entity Entity descriptor.
 * @param[in] num    1-based string index.
 *
 * @return The trimmed string, or @c nullptr if @p entity is @c nullptr, @p num is
 *         zero, or @p num exceeds the number of strings in the entity.
 */
static inline const char *dmi_entity_string(const dmi_entity_t *entity, dmi_string_t num)
{
    return dmi_entity_string_ex(entity, num, false);
}

#endif // !OPENDMI_ENTITY_H
