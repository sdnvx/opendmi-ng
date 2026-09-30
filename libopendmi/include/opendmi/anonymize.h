//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ANONYMIZE_H
#define OPENDMI_ANONYMIZE_H

#pragma once

#include <opendmi/types.h>

#ifndef DMI_BUFFER_T
#   define DMI_BUFFER_T
    typedef struct dmi_buffer dmi_buffer_t;
#endif // !DMI_BUFFER_T

__BEGIN_DECLS

/**
 * @brief Make an anonymized copy of the structure table of a context.
 *
 * The values which identify a system rather than describe it, e.g. serial
 * numbers, asset tags, UUIDs and MAC addresses, are replaced, and so is every
 * other occurrence of them in the table, e.g. a serial number repeated in the
 * OEM strings or in a structure of an unknown type. The values are those of
 * the attributes marked with `DMI_ATTRIBUTE_FLAG_PRIVATE`.
 *
 * The copy has the very layout of the table: every value is replaced with one
 * of the same length, strings keep their letters, digits and punctuation in
 * place, MAC addresses keep the manufacturer part, and UUIDs keep their
 * version and variant. The same value is replaced with the same one within a
 * table, and with a different one each time a table is anonymized, since the
 * replacements are keyed by a random key, which is never saved. Values which
 * identify nothing are kept, e.g. empty strings, placeholders of the firmware
 * vendor, and the values standing for "unspecified".
 *
 * Values of the structures the context has no specification for are replaced
 * only where they repeat a value found elsewhere, so a table carrying unknown
 * OEM structures may still identify its system.
 *
 * The context itself is left as it is.
 *
 * @param[in]  context Open DMI context, whose structures are decoded.
 * @param[out] table   Buffer to write the anonymized table into.
 *
 * @error DMI_ERROR_NULL_ARGUMENT Table is `nullptr`
 * @error DMI_ERROR_INVALID_STATE Context is not open, or its structures carry
 *        additional information entries applied to them
 * @error DMI_ERROR_OUT_OF_MEMORY Memory cannot be allocated
 * @error DMI_ERROR_INTERNAL Random key cannot be generated, or a structure
 *        is not encoded back into its own length
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_anonymize(dmi_context_t *context, dmi_buffer_t *table);

__END_DECLS

#endif // !OPENDMI_ANONYMIZE_H
