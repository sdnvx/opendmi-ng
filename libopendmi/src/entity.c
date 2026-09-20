//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/internal.h>

#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/string-property.h>
#include <opendmi/entity/additional-info.h>

const dmi_name_set_t dmi_entity_state_names =
{
    .code  = "entity-states",
    .names = (dmi_name_t[]){
        { 0, "decoded",    "Decoded"    },
        { 1, "linked",     "Linked"     },
        { 2, "valid",      "Valid"      },
        { 3, "incomplete", "Incomplete" },
        { 4, "partial",    "Partial"    },
        DMI_NAME_NULL
    }
};

const dmi_name_set_t dmi_property_names =
{
    .code  = "property-names",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_PROPERTY_ID_RESERVED,
            .code = "reserved",
            .name = "Reserved"
        },
        {
            .id   = DMI_PROPERTY_ID_UEFI_DEVICE_PATH,
            .code = "uefi-device-path",
            .name = "UEFI device path"
        },
        DMI_NAME_NULL
    },
    .ranges = (dmi_name_range_t[]){
        {
            .start_id = __DMI_PROPERTY_ID_RESERVED_START,
            .end_id   = __DMI_PROPERTY_ID_RESERVED_END,
            .code     = "reserved",
            .name     = "Reserved"
        },
        {
            .start_id = __DMI_PROPERTY_ID_VENDOR_START,
            .end_id   = __DMI_PROPERTY_ID_VENDOR_END,
            .code     = "vendor-specific",
            .name     = "Firmware vendor specific"
        },
        {
            .start_id = __DMI_PROPERTY_ID_OEM_START,
            .end_id   = __DMI_PROPERTY_ID_OEM_END,
            .code     = "oem-specific",
            .name     = "OEM specific"
        },
        DMI_NAME_RANGE_NULL
    }
};

/**
 * @internal
 * @brief Create copy of structure body with additional information applied.
 */
static bool dmi_entity_apply_overlays(dmi_entity_t *entity);

/**
 * @internal
 */
static bool dmi_entity_decode_length(
        dmi_entity_t *entity,
        size_t        max_length);

/**
 * @internal
 */
static bool dmi_entity_decode_strings(dmi_entity_t *entity);

/**
 * @internal
 */
static char *dmi_entity_string_trim(dmi_context_t *context, const char *ptr);

dmi_entity_t *dmi_entity_create(
        dmi_context_t *context,
        const void    *data,
        size_t         max_length)
{
    dmi_entity_t *entity = nullptr;

    if (context == nullptr)
        return nullptr;

    if (data == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "data");
        return nullptr;
    }

    // Structure header must be followed by at least two bytes of string set
    if (max_length < sizeof(dmi_header_t) + 2) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_TRUNCATED,
                           "%zu bytes available", max_length);
        return nullptr;
    }

    dmi_header_t *header = dmi_cast(header, data);
    dmi_type_t    type   = dmi_cast(type, dmi_decode(header->type));
    size_t        length = dmi_decode(header->length);
    dmi_handle_t  handle = dmi_decode(header->handle);

    dmi_log_debug(context,
                  "%p: Handle 0x%04x, length %zu, type %d (%s)",
                  data, handle, length, (int)header->type,
                  dmi_type_name(context, type));

    // Check structure length
    if (length == 0) {
        type   = DMI_TYPE(END_OF_TABLE);
        length = sizeof(dmi_header_t);
    } else if (length < sizeof(dmi_header_t)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ENTITY_LENGTH,
                           "0x%04hx (%s): %zu bytes",
                           handle, dmi_type_name(context, type), length);
        return nullptr;
    } else if (length > max_length) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_TRUNCATED,
                           "0x%04hx (%s): %zu bytes long, only %zu available",
                           handle, dmi_type_name(context, type), length, max_length);
        return nullptr;
    }

    // Allocate new entity descriptor
    entity = dmi_alloc(context, sizeof(dmi_entity_t));
    if (entity == nullptr)
        return nullptr;

    dmi_vector_init(&entity->properties, nullptr);

    // Decode structure header
    entity->context     = context;
    entity->type        = type;
    entity->body_length = length;
    entity->handle      = handle;
    entity->data        = data;

    // Decode structure data
    bool success = false;
    do {
        // Decode strings
        if (not dmi_entity_decode_length(entity, max_length)) {
            dmi_error_raise_ex(context, DMI_ERROR_ENTITY_TRUNCATED,
                               "0x%04hx (%s): Unformatted area exceeds limits",
                               handle, dmi_type_name(context, type));
            break;
        }
        if (not dmi_entity_decode_strings(entity))
            break;

        success = true;
    } while (false);

    if (not success) {
        dmi_entity_destroy(entity);
        return nullptr;
    }

    return entity;
}

bool dmi_entity_decode(dmi_entity_t *entity)
{
    if (entity == nullptr)
        return false;
    if (entity->state & DMI_ENTITY_STATE_DECODED)
        return true;

    dmi_context_t *context = entity->context;

    const dmi_entity_spec_t *spec = dmi_type_spec(entity->context, entity->type);
    if (spec == nullptr)
        return true;

    entity->spec = spec;

    // Check minimum length constraint
    if ((spec->minimum_length != 0) and (entity->body_length < spec->minimum_length)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ENTITY_LENGTH,
                           "0x%04x (%s): %zu bytes, at least %zu expected",
                           entity->handle, spec->name, entity->body_length, spec->minimum_length);
        return false;
    }

    entity->level = spec->minimum_version;

    if (spec->handlers.decode == nullptr)
        return true;

    // Decoder reads the copy of structure body with additional information
    // applied, if there is any
    if ((entity->overlays != nullptr) and (entity->overlay_data == nullptr)) {
        if (not dmi_entity_apply_overlays(entity))
            return false;
    }

    // Allocate structure descriptor
    entity->info = dmi_alloc(context, spec->decoded_length);
    if (entity->info == nullptr)
        return false;

    // Initialize stream
    dmi_stream_initialize(&entity->stream, entity);
    dmi_stream_seek(&entity->stream, sizeof(dmi_header_t));

    // Execute decoder
    bool status = spec->handlers.decode(entity);

    if (status) {
        entity->state |= DMI_ENTITY_STATE_DECODED;
    } else {
        const dmi_error_t *error = dmi_error_peek_last(context);

        // Out of memory is left as the last error, since it is not caused by
        // structure data
        if ((error == nullptr) or (error->reason != DMI_ERROR_OUT_OF_MEMORY)) {
            dmi_error_raise_ex(context, DMI_ERROR_ENTITY_DECODE,
                               "0x%04x (%s)", entity->handle, entity->spec->name);
        }

        // Call cleanup handler on errors
        if (spec->handlers.cleanup != nullptr)
            spec->handlers.cleanup(entity);

        dmi_free(entity->info);

        entity->info  = nullptr;
        entity->level = DMI_VERSION_NONE;
    }

    // Reset stream after decoding
    dmi_stream_reset(&entity->stream);

    return status;
}

bool dmi_entity_link(dmi_entity_t *entity)
{
    if (entity == nullptr)
        return false;

    if (entity->spec == nullptr)
        return false;
    if (entity->spec->handlers.link == nullptr)
        return false;

    if (entity->state & DMI_ENTITY_STATE_LINKED)
        return true;

    if (not entity->spec->handlers.link(entity)) {
        dmi_context_t *context = entity->context;
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_LINK,
                           "0x%04x (%s)", entity->handle, entity->spec->name);
        return false;
    }

    entity->state |= DMI_ENTITY_STATE_LINKED;

    return true;
}

dmi_context_t *dmi_entity_context(const dmi_entity_t *entity)
{
    if (entity == nullptr)
        return nullptr;

    return entity->context;
}

dmi_stream_t *dmi_entity_stream(dmi_entity_t *entity)
{
    if (entity == nullptr)
        return nullptr;

    return &entity->stream;
}

dmi_handle_t dmi_entity_handle(const dmi_entity_t *entity)
{
    if (entity == nullptr)
        return DMI_HANDLE_INVALID;

    return entity->handle;
}

dmi_type_t dmi_entity_type(const dmi_entity_t *entity)
{
    if (entity == nullptr)
        return DMI_TYPE_INVALID;

    return entity->type;
}

const char *dmi_entity_name(const dmi_entity_t *entity)
{
    if (entity == nullptr)
        return nullptr;

    return dmi_type_name(entity->context, entity->type);
}

const void *dmi_entity_data(const dmi_entity_t *entity, dmi_type_t type)
{
    if (entity == nullptr)
        return nullptr;

    if ((type != DMI_TYPE_ANY) and (entity->type != type)) {
        dmi_error_raise(entity->context, DMI_ERROR_INVALID_ENTITY_TYPE);
        return nullptr;
    }

    return entity->data;
}

void *dmi_entity_info(const dmi_entity_t *entity, dmi_type_t type)
{
    if (entity == nullptr)
        return nullptr;

    if ((type != DMI_TYPE_ANY) and (entity->type != type)) {
        dmi_error_raise(entity->context, DMI_ERROR_INVALID_ENTITY_TYPE);
        return nullptr;
    }

    return entity->info;
}

const char *dmi_entity_string_ex(const dmi_entity_t *entity, size_t num, bool raw)
{
    dmi_string_entry_t *entry;

    if ((entity == nullptr) or (num == 0))
        return nullptr;

    if (num > entity->string_count) {
        dmi_error_raise_ex(entity->context, DMI_ERROR_STRING_NOT_FOUND,
                           "Handle 0x%04x, index %zu", entity->handle, num);
        return nullptr;
    }

    entry = &entity->strings[num - 1];

    if (entry->raw == nullptr)
        return nullptr;

    return raw ? entry->raw : entry->pretty;
}

const char *dmi_entity_property(const dmi_entity_t *entity, dmi_property_t property)
{
    if (entity == nullptr)
        return nullptr;

    for (size_t i = 0; i < entity->properties.length; i++) {
        uintptr_t value;

        if (not dmi_vector_get(&entity->properties, i, &value))
            break;

        const dmi_string_property_t *info = dmi_cast(info, value);

        if (info->ident == property)
            return info->value;
    }

    return nullptr;
}

bool dmi_entity_add_property(dmi_entity_t *entity, const dmi_string_property_t *property)
{
    if (entity == nullptr)
        return false;

    if (property == nullptr) {
        dmi_error_raise_ex(entity->context, DMI_ERROR_NULL_ARGUMENT, "property");
        return false;
    }

    if (not dmi_vector_push(&entity->properties, (uintptr_t)property)) {
        dmi_error_raise(entity->context, DMI_ERROR_OUT_OF_MEMORY);
        return false;
    }

    return true;
}

bool dmi_entity_add_overlay(dmi_entity_t *entity, const dmi_entity_t *source, size_t index)
{
    if (entity == nullptr)
        return false;

    dmi_context_t *context = entity->context;

    const dmi_additional_info_t *info = nullptr;
    if (source != nullptr)
        info = dmi_entity_info(source, DMI_TYPE(ADDITIONAL_INFO));

    if ((info == nullptr) or (index >= info->entry_count)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ARGUMENT, "source");
        return false;
    }

    const dmi_additional_info_entry_t *entry = &info->entries[index];

    // Values are applied on decoding
    if (entity->state & DMI_ENTITY_STATE_DECODED) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_STATE,
                           "Additional information 0x%04x[%zu]: structure 0x%04x is already decoded",
                           source->handle, index, entity->handle);
        return false;
    }

    // Additional information itself is decoded before other structures
    if (entity->type == DMI_TYPE(ADDITIONAL_INFO)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_OVERLAY,
                           "Additional information 0x%04x[%zu]: refers to additional information 0x%04x",
                           source->handle, index, entity->handle);
        return false;
    }

    // Structure header and strings cannot be changed
    if ((entry->ref_offset < sizeof(dmi_header_t)) or
        (entry->ref_offset + entry->value.length > entity->body_length))
    {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_OVERLAY,
                           "Additional information 0x%04x[%zu]: %zu bytes at offset 0x%02x "
                           "of structure 0x%04x, which is %zu bytes long",
                           source->handle, index, entry->value.length, entry->ref_offset,
                           entity->handle, entity->body_length);
        return false;
    }

    dmi_entity_overlay_t *node = dmi_alloc(context, sizeof(*node));
    if (node == nullptr)
        return false;

    *node = (dmi_entity_overlay_t){
        .source = source,
        .index   = index,
        .entry   = entry
    };

    // Entries are applied in the order they are attached
    dmi_entity_overlay_t **tail = &entity->overlays;
    while (*tail != nullptr)
        tail = &(*tail)->next;

    *tail = node;

    return true;
}

bool dmi_entity_stop(dmi_entity_t *entity)
{
    assert(entity != nullptr);
    assert(dmi_stream_is_done(&entity->stream));

    if (not dmi_stream_is_done(&entity->stream))
        return dmi_entity_incomplete(entity);

    entity->state |= DMI_ENTITY_STATE_PARTIAL;

    return true;
}

bool dmi_entity_incomplete(dmi_entity_t *entity)
{
    assert(entity != nullptr);

    entity->state |= DMI_ENTITY_STATE_INCOMPLETE;

    size_t remaining = dmi_stream_remaining(&entity->stream);

    if (remaining > 0) {
        dmi_log_notice(entity->context,
                       "Handle 0x%04hx (%s): Incomplete fields at offset 0x%02zx, %zu byte%s ignored",
                       entity->handle, dmi_type_name(entity->context, entity->type),
                       entity->stream.position, remaining, (remaining == 1) ? "" : "s");
    } else {
        dmi_log_notice(entity->context,
                       "Handle 0x%04hx (%s): Incomplete fields at offset 0x%02zx",
                       entity->handle, dmi_type_name(entity->context, entity->type),
                       entity->stream.position);
    }

    return true;
}

void dmi_entity_destroy(dmi_entity_t *entity)
{
    if (entity == nullptr)
        return;

    if ((entity->spec != nullptr) and (entity->info != nullptr)) {
        if (entity->spec->handlers.cleanup != nullptr)
            entity->spec->handlers.cleanup(entity);

        dmi_free(entity->info);
    }

    if (entity->strings) {
        for (size_t i = 0; i < entity->string_count; i++)
            dmi_free(entity->strings[i].pretty);

        dmi_free(entity->strings);
    }

    // String properties and additional information entries are owned by the
    // registry, only the lists are freed
    dmi_vector_clear(&entity->properties);

    dmi_entity_overlay_t *node = entity->overlays;
    while (node != nullptr) {
        dmi_entity_overlay_t *next = node->next;

        dmi_free(node);

        node = next;
    }

    dmi_free(entity->overlay_data);
    dmi_free(entity);
}

static bool dmi_entity_decode_length(
        dmi_entity_t *entity,
        size_t        max_length)
{
    assert(entity != nullptr);
    assert(entity->data != nullptr);

    const char *data  = dmi_cast(data, entity->data);
    size_t      start = entity->body_length;
    size_t      pos   = start;
    size_t      count = 0;

    // String set is terminated by double NUL, so it takes at least two bytes.
    // Every byte is accessed only after checking that it is within limits.
    if ((start > max_length) or (max_length - start < 2))
        return false;

    // String set starting with NUL contains no strings, even if it is not
    // immediately followed by another NUL (the same as dmidecode does)
    bool has_strings = (data[start] != 0);

    // Find double NUL terminating the string set, and count strings
    while ((data[pos] != 0) or (data[pos + 1] != 0)) {
        if ((data[pos] == 0) and has_strings)
            count++;

        if (++pos + 1 == max_length)
            return false;
    }

    // The first NUL of the terminator ends the last string
    if (has_strings)
        count++;

    pos += 2;

    entity->extra_length = pos - start;
    entity->total_length = entity->body_length + entity->extra_length;
    entity->string_count = count;

    return true;
}

static bool dmi_entity_decode_strings(dmi_entity_t *entity)
{
    assert(entity != nullptr);
    assert(entity->data != nullptr);

    bool success = true;
    dmi_string_entry_t *strings = nullptr;

    // Allocate strings index
    strings = dmi_alloc_array(entity->context, sizeof(dmi_string_entry_t), entity->string_count + 1);
    if (strings == nullptr)
        return false;

    // Fetch string pointers. String set bounds and number of strings are
    // already validated by dmi_entity_decode_length().
    const char *ptr = dmi_cast(ptr, entity->data) + entity->body_length;

    for (size_t i = 0; i < entity->string_count; i++) {
        strings[i].raw    = ptr;
        strings[i].pretty = dmi_entity_string_trim(entity->context, ptr);

        if (strings[i].pretty == nullptr) {
            success = false;
            dmi_error_raise(entity->context, DMI_ERROR_OUT_OF_MEMORY);
            break;
        }

        ptr += strlen(ptr) + 1;
    }

    if (not success) {
        // Index is zero-initialized, so unfilled entries are safe to free
        for (size_t i = 0; i < entity->string_count; i++)
            dmi_free(strings[i].pretty);
        dmi_free(strings);

        return false;
    }

    entity->strings = strings;

    return true;
}

static char *dmi_entity_string_trim(dmi_context_t *context, const char *ptr)
{
    assert(ptr != nullptr);

    size_t length;
    char  *str = nullptr;

    // Trim leading whitespaces
    ptr += strspn(ptr, "\t\n\v\f\r ");

    // Trim trailing whitespaces
    length = strlen(ptr);
    while (length > 0) {
        if (strchr("\t\n\v\f\r ", ptr[length - 1]) == nullptr)
            break;
        length--;
    }

    // Allocate string
    str = dmi_alloc(context, length + 1);
    if (str == nullptr)
        return nullptr;

    // Copy string value
    memcpy(str, ptr, length);

    return str;
}

static bool dmi_entity_apply_overlays(dmi_entity_t *entity)
{
    dmi_context_t *context = entity->context;

    entity->overlay_data = dmi_alloc(context, entity->body_length);
    if (entity->overlay_data == nullptr)
        return false;

    memcpy(entity->overlay_data, entity->data, entity->body_length);

    // Entries have been checked when attached, later ones take precedence
    for (const dmi_entity_overlay_t *node = entity->overlays; node != nullptr; node = node->next) {
        const dmi_additional_info_entry_t *entry = node->entry;

        dmi_log_debug(context, "0x%04x: Applying %zu bytes at offset 0x%02x",
                      entity->handle, entry->value.length, entry->ref_offset);

        memcpy(entity->overlay_data + entry->ref_offset, entry->value.data, entry->value.length);
    }

    return true;
}

