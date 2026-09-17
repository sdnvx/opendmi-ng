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

typedef struct dmi_additional_info       dmi_additional_info_t;
typedef struct dmi_additional_info_entry dmi_additional_info_entry_t;

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
     * @note
     * This field is the same type and size as the field being referenced by
     * this additional information entry.
     */
    dmi_data_t value[32];

    /**
     * @brief Length of the value data (determined from additional info
     * structure, must match length of the referenced field).
     */
    size_t value_length;
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
