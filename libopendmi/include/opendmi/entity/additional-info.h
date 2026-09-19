//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ADDITIONAL_INFO_H
#define OPENDMI_ENTITY_ADDITIONAL_INFO_H

#pragma once

#include <opendmi/entity.h>

/**
 * @brief Length of additional information entry header: entry length,
 * referenced handle, referenced offset and string number.
 */
#define DMI_ADDITIONAL_INFO_ENTRY_HEADER 5

typedef struct dmi_additional_info       dmi_additional_info_t;

#ifndef DMI_ADDITIONAL_INFO_ENTRY_T
#   define DMI_ADDITIONAL_INFO_ENTRY_T
    typedef struct dmi_additional_info_entry dmi_additional_info_entry_t;
#endif // !DMI_ADDITIONAL_INFO_ENTRY_T

/**
 * @brief Additional information entry.
 */
struct dmi_additional_info_entry
{
    /**
     * @brief Handle, or instance number, associated with the structure for
     * which additional information is provided.
     */
    dmi_handle_t ref_handle;

    /**
     * @brief Offset of the field within the structure referenced by the
     * referenced handle for which additional information is provided.
     */
    unsigned ref_offset;

    /**
     * @brief Number of the optional string to be associated with the field
     * referenced by the referenced offset.
     */
    const char *string;

    /**
     * @brief Enumerated value or updated field content that has not yet been
     * approved for publication in this specification and therefore could not
     * be used in the field referenced by referenced offset.
     *
     * Value is referenced in the structure data. Its length is determined by
     * the entry length, and must match the length of the referenced field.
     *
     * @note
     * This field is the same type and size as the field being referenced by
     * this additional information entry.
     */
    dmi_binary_t value;
};

/**
 * @brief Additional information structure (type 40).
 */
struct dmi_additional_info
{
    /**
     * @brief Number of additional information entries that follow.
     */
    size_t entry_count;

    /**
     * @brief Additional information entries.
     */
    dmi_additional_info_entry_t *entries;
};

/**
 * @brief Additional information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_additional_info_spec;

#endif // !OPENDMI_ENTITY_ADDITIONAL_INFO_H
