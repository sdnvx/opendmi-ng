//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include <opendmi/entry.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/internal.h>

/**
 * @internal
 * @brief Decode legacy SMBIOS entry point (32-bit).
 *
 * @param context DMI context handle
 * @param data Pointer to DMI entry point data
 * @param length DMI entry point data length
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
static bool dmi_entry_decode_legacy(dmi_context_t *context,
                                    const void *data, size_t length);

/**
 * @internal
 * @brief Decode SMBIOS 2.1+ entry point (32-bit).
 *
 * @param context DMI context handle
 * @param data Pointer to DMI entry point data
 * @param length DMI entry point data length
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
static bool dmi_entry_decode_v21(dmi_context_t *context,
                                 const void *data, size_t length);

/**
 * @internal
 * @brief Decode SMBIOS 3.0+ entry point (64-bit).
 *
 * @param context DMI context handle
 * @param data Pointer to DMI entry point data
 * @param length DMI entry point data length
 *
 * @return The function returns `true` on success and `false` otherwise.
 */
static bool dmi_entry_decode_v30(dmi_context_t *context,
                                 const void *data, size_t length);

static const dmi_entry_spec_t dmi_entry_specs[] =
{
    {
        .name       = "SMBIOS 3.0+ (64-bit)",
        .anchor     = DMI_ANCHOR_V30,
        .version    = DMI_VERSION(3, 0, 0),
        .min_length = sizeof(dmi_entry_v30_t),
        .handler    = dmi_entry_decode_v30,
        .attributes = DMI_ATTRIBUTES({
            DMI_ATTRIBUTE(dmi_context_t, state.entry_length, SIZE, {
                .code  = "length",
                .name  = "Entry point length"
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.entry_revision, INTEGER, {
                .code  = "revision",
                .name  = "Entry point revision",
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.table_area_max_size, SIZE, {
                .code  = "table-area-max-size",
                .name  = "Maximum size of table area"
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.table_area_addr, ADDRESS, {
                .code  = "table-area-addr",
                .name  = "Table area address",
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            {}
        })
    },
    {
        .name       = "SMBIOS 2.1+ (32-bit)",
        .anchor     = DMI_ANCHOR_V21,
        .version    = DMI_VERSION(2, 1, 0),
        .min_length = sizeof(dmi_entry_v21_t) - 1,
        .handler    = dmi_entry_decode_v21,
        .attributes = DMI_ATTRIBUTES({
            DMI_ATTRIBUTE(dmi_context_t, state.entry_length, SIZE, {
                .code  = "length",
                .name  = "Entry point length"
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.entry_revision, INTEGER, {
                .code  = "revision",
                .name  = "Entry point revision",
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.table_area_size, SIZE, {
                .code  = "table-area-size",
                .name  = "Table area size"
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.table_area_addr, ADDRESS, {
                .code  = "table-area-addr",
                .name  = "Table area address",
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.entity_max_size, SIZE, {
                .code  = "entity-max-size",
                .name  = "Maximum entity size"
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.entity_count, INTEGER, {
                .code  = "entity-count",
                .name  = "Entity count"
            }),
            {}
        })
    },
    {
        .name       = "Legacy (32-bit)",
        .anchor     = DMI_ANCHOR_LEGACY,
        .version    = DMI_VERSION(2, 0, 0),
        .min_length = sizeof(dmi_entry_legacy_t),
        .handler    = dmi_entry_decode_legacy,
        .attributes = DMI_ATTRIBUTES({
            DMI_ATTRIBUTE(dmi_context_t, state.entry_length, SIZE, {
                .code  = "length",
                .name  = "Entry point length"
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.table_area_size, SIZE, {
                .code  = "table-area-size",
                .name  = "Table area size"
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.table_area_addr, ADDRESS, {
                .code  = "table-area-addr",
                .name  = "Table area address",
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            DMI_ATTRIBUTE(dmi_context_t, state.entity_count, INTEGER, {
                .code  = "entity-count",
                .name  = "Entity count"
            }),
            {}
        })
    },
    {}
};

bool dmi_entry_decode(dmi_context_t *context, const void *data, size_t length)
{
    const dmi_entry_spec_t *spec;

    if (context == nullptr)
        return dmi_trace_argument_null(nullptr, context);

    if (data == nullptr)
        return dmi_trace_argument_null(context, data);
    if (length == 0)
        return dmi_trace_argument_invalid(context, length);

    for (spec = dmi_entry_specs; spec->name != nullptr; spec++) {
        if (length < spec->min_length)
            continue;
        if  (memcmp(data, spec->anchor, strlen(spec->anchor)) == 0)
            break;
    }

    if (spec->name == nullptr) {
        dmi_error_raise(context, DMI_ERROR_ENTRY_ANCHOR_UNKNOWN);
        return false;
    }

    context->state.entry_spec    = spec;
    context->state.entry_version = spec->version;

    return spec->handler(context, data, length);
}

static bool dmi_entry_decode_legacy(dmi_context_t *context,
                                    const void *data, size_t length)
{
    assert(context != nullptr);
    assert(data != nullptr);
    assert(length >= sizeof(dmi_entry_legacy_t) - 1);

    const dmi_entry_legacy_t *entry = dmi_cast(entry, data);

    // An intermediate entry point of SMBIOS 2.1 entry point with length 0x1E
    // (see dmi_entry_decode_v21()) can lack the BCD revision byte. Its
    // checksum then cannot be verified, but the enclosing entry point
    // checksum covers all the remaining fields.
    bool complete = length >= sizeof(dmi_entry_legacy_t);

    // Verify EPS checksum value
    if (complete and not dmi_checksum_test(data, sizeof(dmi_entry_legacy_t))) {
        dmi_error_raise(context, DMI_ERROR_ENTRY_CHECKSUM_INVALID);
        return false;
    }

    // Decode SMBIOS version
    dmi_byte_t entry_version = complete ? dmi_decode(entry->version) : 0;
    if ((entry_version != 0) and (context->state.smbios_version == 0)) {
        uint8_t major = (entry_version & 0xF0) >> 4;
        uint8_t minor = (entry_version & 0x0F);

        context->state.smbios_version = dmi_version(major, minor, 0);
    }

    // Set address size
    if (context->state.address_size == 0)
        context->state.address_size = sizeof(uint32_t);

    // Decode table area parameters
    context->state.entry_length        = sizeof(dmi_entry_legacy_t);
    context->state.entity_count        = dmi_decode(entry->entity_count);
    context->state.table_area_addr     = dmi_decode(entry->table_area_addr);
    context->state.table_area_max_size = dmi_decode(entry->table_area_size);
    context->state.table_area_size     = dmi_decode(entry->table_area_size);

    return true;
}

static bool dmi_entry_decode_v21(dmi_context_t *context,
                                 const void *data, size_t length)
{
    assert(context != nullptr);
    assert(data != nullptr);
    assert(length >= sizeof(dmi_entry_v21_t) - 1);

    const dmi_entry_v21_t *entry = dmi_cast(entry, data);
    size_t entry_length = dmi_decode(entry->length);

    // Check maximum entry point length to prevent checksum run beyond
    // the buffer.
    if (entry_length > length) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTRY_LENGTH_INVALID, "%zu", entry_length);
        return false;
    }

    // Check minimum entry point length. The size of this structure is 0x1F
    // bytes, but we also accept value 0x1E due to a mistake in SMBIOS
    // specification version 2.1.
    if (entry_length < sizeof(dmi_entry_v21_t) - 1) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTRY_LENGTH_INVALID, "%zu", entry_length);
        return false;
    }

    // Verify EPS checksum value
    if (not dmi_checksum_test(data, entry_length)) {
        dmi_error_raise(context, DMI_ERROR_ENTRY_CHECKSUM_INVALID);
        return false;
    }

    // Decode entry point revision and SMBIOS version, which has no revision
    // number in this entry point
    context->state.entry_revision = dmi_decode(entry->revision);
    context->state.smbios_version = dmi_version(dmi_decode(entry->version_major),
                                                dmi_decode(entry->version_minor),
                                                0);

    // Set address size
    context->state.address_size = sizeof(uint32_t);

    // Decode structure parameters
    context->state.entity_max_size = dmi_decode(entry->entity_max_size);

    // Intermediate entry point is truncated if the entry point of length
    // 0x1E is supplied at its exact length
    size_t ieps_length = length - offsetof(dmi_entry_v21_t, ieps);
    if (ieps_length > sizeof(entry->ieps))
        ieps_length = sizeof(entry->ieps);

    if (not dmi_entry_decode_legacy(context, (const void *)&entry->ieps, ieps_length))
        return false;

    // Intermediate entry point length is overridden
    context->state.entry_length = entry_length;

    return true;
}

static bool dmi_entry_decode_v30(dmi_context_t *context,
                                 const void *data, size_t length)
{
    assert(context != nullptr);
    assert(data != nullptr);
    assert(length >= sizeof(dmi_entry_v30_t));

    const dmi_entry_v30_t *entry = dmi_cast(entry, data);
    size_t entry_length = dmi_decode(entry->length);

    // Check maximum entry point length to prevent checksum run beyond
    // the buffer.
    if (entry_length > length) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTRY_LENGTH_INVALID, "%zu", entry_length);
        return false;
    }

    // Check minimum entry point length.
    if (entry_length < sizeof(dmi_entry_v30_t)) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTRY_LENGTH_INVALID, "%zu", entry_length);
        return false;
    }

    // Verify EPS checksum value
    if (not dmi_checksum_test(data, entry_length)) {
        dmi_error_raise(context, DMI_ERROR_ENTRY_CHECKSUM_INVALID);
        return false;
    }

    // Decode SMBIOS version
    context->state.entry_revision = dmi_decode(entry->revision);
    context->state.smbios_version = dmi_version(dmi_decode(entry->version_major),
                                                dmi_decode(entry->version_minor),
                                                dmi_decode(entry->version_rev));

    // Set address size
    context->state.address_size = sizeof(uint64_t);

    // Decode table parameters
    context->state.entry_length        = entry_length;
    context->state.table_area_addr     = dmi_decode(entry->table_area_addr);
    context->state.table_area_max_size = dmi_decode(entry->table_area_max_size);

    return true;
}
