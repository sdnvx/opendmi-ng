//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTRY_H
#define OPENDMI_ENTRY_H

#pragma once

#include <opendmi/context.h>
#include <opendmi/attribute.h>

/**
 * @brief SMBIOS 2.0 or earlier (32-bit) entry point anchor string.
 */
#define DMI_ANCHOR_LEGACY "_DMI_"

/**
 * @brief SMBIOS 2.1 or later (32-bit) entry point anchor string.
 */
#define DMI_ANCHOR_V21 "_SM_"

/**
 * @brief SMBIOS 3.1 or later (64-bit) entry point anchor string.
 */
#define DMI_ANCHOR_V30 "_SM3_"

#ifndef DMI_ENTRY_SPEC_T
#   define DMI_ENTRY_SPEC_T
    typedef struct dmi_entry_spec dmi_entry_spec_t;
#endif // !DMI_ENTRY_SPEC_T

typedef struct dmi_entry_legacy dmi_entry_legacy_t;
typedef struct dmi_entry_v21    dmi_entry_v21_t;
typedef struct dmi_entry_v30    dmi_entry_v30_t;

/**
 * @brief Maximum size of SMBIOS entry point structure.
 */
#define DMI_ENTRY_MAX_SIZE 0x20

/**
 * @brief SMBIOS entry point format specification.
 */
struct dmi_entry_spec
{
    /**
     * @brief Entry point format name.
     */
    const char *name;

    /**
     * @brief Anchor string.
     */
    const char *anchor;

    /**
     * @brief Entry point version number.
     */
    dmi_version_t version;

    /**
     * @brief Minimum length of entry point structure.
     */
    const size_t min_length;

    /**
     * @brief Entry point decoding function.
     */
    bool (*handler)(dmi_context_t *context, const void *data, const size_t length);

    /**
     * @brief Entry point attributes.
     */
    const dmi_attribute_t *attributes;
};

/**
 * @brief The 32-bit SMBIOS legacy Entry Point Structure (EPS).
 *
 * @details
 * On modern 32-bit systems with SMBIOS v2.1 or newer, this is also used as a
 * part of EPS called IEPS (Intermediate EPS) for compatibility reasons.
 */
dmi_packed_struct(dmi_entry_legacy)
{
    /**
     * @brief Anchor string.
     *
     * @details
     * "_DMI_", specified as five ASCII characters (5F 44 4D 49 5F).
     */
    const char anchor[5];

    /**
     * @brief Checksum of Entry Point Structure (EPS).
     *
     * @details
     * This value, when added to all other bytes in the EPS (or IEPS on modern
     * systems), results in the value 00h (using 8-bit addition calculations).
     * Values in the EPS/IEPS are summed starting at offset 10h, for 0Fh bytes.
     */
    const dmi_byte_t checksum;

    /**
     * @brief Structure table length.
     *
     * @details
     * Total length of SMBIOS structure table, pointed to by the structure
     * table address, in bytes.
     */
    const dmi_word_t table_area_size;

    /**
     * @brief Structure table address.
     *
     * @details
     * 32-bit physical starting address of the read-only SMBIOS structure
     * table, which can start at any 32-bit address. This area contains all of
     * the SMBIOS structures fully packed together.
     */
    const dmi_dword_t table_area_addr;

    /**
     * @brief Number of SMBIOS tables.
     *
     * @details
     * Total number of structures present in the SMBIOS structure table.
     *
     * @note
     * Some vendors report zero in this field, e.g. Synology.
     */
    const dmi_word_t entity_count;

    /**
     * @brief SMBIOS revision (BCD).
     *
     * @details
     * Indicates compliance with a revision of SMBIOS specification.
     *
     * It is a BCD value where the upper nibble indicates the major version
     * and the lower nibble the minor version. For revision 2.1, the returned
     * value is `0x21`. If the value is `0x00`, only the Major and Minor
     * Versions in offsets 6 and 7 of the v2.1+ EPS provide the version
     * information.
     */
    const dmi_byte_t version;
};

/**
 * @brief The 32-bit SMBIOS 2.1+ Entry Point Structure (EPS).
 */
dmi_packed_struct(dmi_entry_v21)
{
    /**
     * @brief Anchor string.
     *
     * @details
     * "_SM_", specified as four ASCII characters (5F 53 4D 5F).
     */
    const char anchor[4];

    /**
     * @brief Checksum of Entry Point Structure (EPS).
     *
     * @details
     * This value, when added to all other bytes in the EPS, results in the
     * value 00h (using 8-bit addition calculations). Values in the EPS are
     * summed starting at offset `0x00`, for Entry Point Length bytes.
     */
    const dmi_byte_t checksum;

    /**
     * @brief Entry Point Structure length.
     *
     * @details
     * Length of the Entry Point Structure, starting with the Anchor String
     * field, in bytes, currently 1Fh.
     *
     * @note This value was incorrectly stated in version 2.1 of SMBIOS
     * specification as 1Eh. Because of this, there might be version 2.1
     * implementations that use either the `0x1E` or the `0x1F` value, but
     * version 2.2 or later implementations must use the `0x1F` value.
     */
    const dmi_byte_t length;

    /**
     * @brief SMBIOS major version.
     *
     * @details
     * Major version of this specification implemented in the table structures
     * (for example, the value is `0x0A` for revision 10.22 and `0x02` for
     * revision 2.1).
     */
    const dmi_byte_t version_major;

    /**
     * @brief SMBIOS minor version.
     *
     * @details
     * Minor version of this specification implemented in the table structures
     * (for example, the value is `0x16` for revision 10.22 and `0x01` for
     * revision 2.1).
     */
    const dmi_byte_t version_minor;

    /**
     * @brief Maximum SMBIOS structure size.
     *
     * @details
     * Size of the largest SMBIOS structure, in bytes, and encompasses the
     * structure’s formatted area and text strings.
     */
    const dmi_word_t entity_max_size;

    /**
     * @brief Entry Point Structure revision.
     *
     * @details
     * EPS revision implemented in this structure and identifies the formatting
     * of offsets `0x0B` to `0x0F` as follows:
     *
     * - `0x00`        -- Entry Point is based on SMBIOS 2.1 definition,
     *                    formatted area is reserved and set to all 00h.
     * - `0x01`-`0xFF` -- Reserved for assignment by this specification.
     */
    const dmi_byte_t revision;

    /**
     * @brief Formatted area.
     *
     * @details
     * Value present in the Entry Point Revision field defines the
     * interpretation to be placed upon these 5 bytes.
     */
    const char formatted[5];

    /**
     * @brief Intermediate Entry Point Structure (IEPS).
     */
    const dmi_entry_legacy_t ieps;
};

/**
 * @brief The 64-bit SMBIOS 3.0+ Entry Point Structure (EPS).
 */
dmi_packed_struct(dmi_entry_v30)
{
    /**
     * @brief Anchor string.
     *
     * @details
     * "_SM3_", specified as five ASCII characters (5F 53 4D 33 5F).
     */
    const char anchor[5];

    /**
     * @brief Checksum of the Entry Point Structure (EPS).
     *
     * This value, when added to all other bytes in the EPS, results in the
     * value `0x00` (using 8-bit addition calculations). Values in the EPS are
     * summed starting at offset `0x00`, for Entry Point Length bytes.
     */
    const dmi_byte_t checksum;

    /**
     * @brief Entry Point Structure length.
     *
     * @details
     * Length of the Entry Point Structure, starting with the Anchor String
     * field, in bytes, currently `0x18`.
     */
    const dmi_byte_t length;

    /**
     * @brief SMBIOS major version.
     *
     * @details
     * Major version of this specification implemented in the table structures
     * (for example, the value is `0x0A` for revision 10.22 and `0x02` for
     * revision 2.1).
     */
    const dmi_byte_t version_major;

    /**
     * @brief SMBIOS minor version.
     *
     * @details
     * Minor version of this specification implemented in the table structures
     * (for example, the value is `0x16` for revision 10.22 and `0x01` for
     * revision 2.1).
     */
    const dmi_byte_t version_minor;

    /**
     * @brief SMBIOS version revision.
     *
     * @details
     * Identifies the docrev of SMBIOS specification implemented in the table
     * structures (for example, the value is `0x00` for revision 10.22.0 and
     * `0x01` for revision 2.7.1).
     */
    const dmi_byte_t version_rev;

    /**
     * @brief Entry point revision.
     *
     * @details
     * EPS revision implemented in this structure and identifies the formatting
     * of offsets `0x0B` and beyond as follows:
     *
     * - `0x00`        -- Reserved for assignment by this specification.
     * - `0x01`        -- Entry Point is based on SMBIOS 3.0 definition.
     * - `0x02`-`0xFF` -- Reserved for assignment by this specification,
     *                    offsets 0Ch-17h are defined per revision `0x01`.
     */
    const dmi_byte_t revision;

    /**
     * @brief Reserved.
     *
     * @details
     * Reserved for assignment by SMBIOS specification, set to 0.
     */
    const dmi_byte_t __reserved;

    /**
     * @brief Structure table maximum size.
     *
     * Maximum size of SMBIOS structure table, pointed to by the structure
     * table address, in bytes. The actual size is guaranteed to be less or
     * equal to the maximum size.
     */
    const dmi_dword_t table_area_max_size;

    /**
     * @brief Structure table address.
     *
     * @details
     * The 64-bit physical starting address of the read-only SMBIOS structure
     * table, which can start at any 64-bit address. This area contains all of
     * the SMBIOS structures fully packed together.
     */
    const dmi_qword_t table_area_addr;
};

__BEGIN_DECLS

/**
 * @internal
 * @brief Decode DMI entry point and initialize related context properties.
 *
 * @param[in] context Context handle to initialize.
 * @param[in] data Pointer to entry point data.
 * @param[in] length Entry point data length.
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
__dmi_api bool dmi_entry_decode(dmi_context_t *context, const void *data, size_t length);

__END_DECLS

#endif // !OPENDMI_ENTRY_H
