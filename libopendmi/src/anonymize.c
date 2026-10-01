//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#if defined(_WIN32)
#   define _CRT_RAND_S
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#if defined(__KERNEL__)
#   include <linux/random.h>
#endif

#include <opendmi/anonymize.h>
#include <opendmi/attribute.h>
#include <opendmi/buffer.h>
#include <opendmi/context.h>
#include <opendmi/encoder.h>
#include <opendmi/entity.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/string.h>
#include <opendmi/utils/uuid.h>
#include <opendmi/utils/vector.h>

/**
 * @internal
 * @brief Shortest value replaced wherever a string holds it, besides the
 * string of the attribute itself.
 */
#define DMI_ANONYMIZE_SUBSTRING_MIN 4

/**
 * @internal
 * @brief Shortest value replaced wherever the formatted area of a structure
 * holds it, e.g. a serial number an OEM structure keeps in place.
 */
#define DMI_ANONYMIZE_PATTERN_MIN 6

/**
 * @internal
 * @brief Largest member saved while a structure is encoded with the values
 * replaced, which is the size of a UUID.
 */
#define DMI_ANONYMIZE_MEMBER_MAX 16

/**
 * @internal
 * @brief Value to replace, as the table holds it.
 */
typedef struct dmi_anonymize_value
{
    const dmi_byte_t *data;
    const dmi_byte_t *replacement;
    size_t            length;

    /**
     * @brief Value is replaced wherever it is found, rather than only where
     * the attribute holding it is.
     */
    bool propagate;
} dmi_anonymize_value_t;

/**
 * @internal
 * @brief Member of a decoded structure replaced while the structure is
 * encoded, whose value is put back afterwards.
 */
typedef struct dmi_anonymize_saved
{
    dmi_byte_t *ptr;
    size_t      size;
    dmi_byte_t  bytes[DMI_ANONYMIZE_MEMBER_MAX];
} dmi_anonymize_saved_t;

/**
 * @internal
 * @brief State of an anonymization.
 */
typedef struct dmi_anonymizer
{
    dmi_context_t      *context;
    const dmi_buffer_t *source;
    dmi_buffer_t       *table;

    /**
     * @brief Structure whose attributes are being replaced.
     */
    const dmi_entity_t *entity;

    /**
     * @brief Key of the replacements, which is never saved.
     */
    uint64_t key[2];

    /**
     * @brief Values replaced wherever they are found, as pointers to
     * `dmi_anonymize_value_t`.
     */
    dmi_vector_t values;

    /**
     * @brief Members of the structure being encoded, as pointers to
     * `dmi_anonymize_saved_t`.
     */
    dmi_vector_t saved;
} dmi_anonymizer_t;

static bool dmi_anonymize_key(dmi_anonymizer_t *anon);
static uint64_t dmi_anonymize_hash(const dmi_anonymizer_t *anon, const void *data, size_t length);
static uint64_t dmi_anonymize_next(uint64_t *state);

static bool dmi_anonymize_entity(dmi_anonymizer_t *anon, dmi_entity_t *entity);
static bool dmi_anonymize_attrs(
        dmi_anonymizer_t      *anon,
        const dmi_attribute_t *attrs,
        dmi_byte_t            *info,
        bool                  *changed);
static bool dmi_anonymize_attr(
        dmi_anonymizer_t      *anon,
        const dmi_attribute_t *attr,
        dmi_byte_t            *value,
        bool                  *changed);
static bool dmi_anonymize_member(
        dmi_anonymizer_t      *anon,
        const dmi_attribute_t *attr,
        dmi_byte_t            *value,
        bool                  *changed);
static bool dmi_anonymize_encode(dmi_anonymizer_t *anon, dmi_entity_t *entity);
static void dmi_anonymize_restore(dmi_anonymizer_t *anon);

static bool dmi_anonymize_add(
        dmi_anonymizer_t *anon,
        const void       *data,
        const void       *replacement,
        size_t            length,
        bool              propagate);
static bool dmi_anonymize_is_identifier(const char *text);
static bool dmi_anonymize_is_blank(const void *data, size_t length);
static void dmi_anonymize_text(const dmi_anonymizer_t *anon, const dmi_byte_t *data, size_t length, dmi_byte_t *out);
static void dmi_anonymize_bytes(
        const dmi_anonymizer_t *anon,
        const dmi_byte_t       *data,
        size_t                  length,
        size_t                  keep,
        dmi_byte_t             *out);

static void dmi_anonymize_strings(dmi_anonymizer_t *anon, const dmi_entity_t *entity);
static void dmi_anonymize_body(dmi_anonymizer_t *anon, const dmi_entity_t *entity);
static void dmi_anonymize_replace(
        dmi_anonymizer_t *anon,
        size_t            start,
        size_t            end,
        size_t            minimum);

static void dmi_anonymize_cleanup(dmi_anonymizer_t *anon);

bool dmi_anonymize(dmi_context_t *context, dmi_buffer_t *table)
{
    if (context == nullptr)
        return false;

    if (table == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "table");
        return false;
    }
    if ((context->state.table == nullptr) or (context->state.registry == nullptr)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_STATE, "Context is not open");
        return false;
    }
    // Structures carrying additional information are decoded from copies of
    // their own, which the table does not hold
    if (context->flags & DMI_CONTEXT_FLAG_OVERLAY) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_STATE,
                           "Structures carry additional information entries");
        return false;
    }

    dmi_anonymizer_t anon = {
        .context = context,
        .source  = context->state.table,
        .table   = table
    };

    dmi_vector_init(&anon.values, nullptr);
    dmi_vector_init(&anon.saved, nullptr);

    bool success = false;
    do {
        if (not dmi_anonymize_key(&anon))
            break;

        if (not dmi_buffer_assign(table, anon.source->data, anon.source->length))
            break;

        // Values of the structures are replaced first, which collects them
        // for the passes looking for them elsewhere
        dmi_registry_iter_t iter;
        if (not dmi_registry_iter_init(&iter, context->state.registry, nullptr))
            break;

        bool failed = false;
        while (dmi_registry_iter_has_next(&iter)) {
            if (not dmi_anonymize_entity(&anon, dmi_registry_iter_next(&iter))) {
                failed = true;
                break;
            }
        }
        if (failed)
            break;

        // Strings, including the ones of the structures of unknown types
        if (not dmi_registry_iter_init(&iter, context->state.registry, nullptr))
            break;
        while (dmi_registry_iter_has_next(&iter)) {
            const dmi_entity_t *entity = dmi_registry_iter_next(&iter);

            dmi_anonymize_strings(&anon, entity);
            dmi_anonymize_body(&anon, entity);
        }

        success = true;
    } while (false);

    dmi_anonymize_cleanup(&anon);

    if (not success)
        dmi_buffer_clear(table);

    return success;
}

static bool dmi_anonymize_key(dmi_anonymizer_t *anon)
{
    bool success = false;

#if defined(_WIN32)
    success = true;
    for (size_t i = 0; i < countof(anon->key); i++) {
        unsigned int low;
        unsigned int high;

        if ((rand_s(&low) != 0) or (rand_s(&high) != 0)) {
            success = false;
            break;
        }

        anon->key[i] = ((uint64_t)high << 32) | low;
    }
#elif defined(__KERNEL__)
    get_random_bytes(anon->key, sizeof(anon->key));
    success = true;
#else
    FILE *file = fopen("/dev/urandom", "rb");
    if (file != nullptr) {
        success = (fread(anon->key, sizeof(anon->key), 1, file) == 1);
        fclose(file);
    }
#endif

    // Replacements of a guessable key could be reversed by trying the values
    // a serial number may take, so there is no weaker fallback
    if (not success)
        dmi_error_raise_ex(anon->context, DMI_ERROR_INTERNAL, "Random key cannot be generated");

    return success;
}

static uint64_t dmi_anonymize_rotl(uint64_t value, unsigned bits)
{
    return (value << bits) | (value >> (64 - bits));
}

#define DMI_SIPROUND(v0, v1, v2, v3)                           \
    do {                                                       \
        v0 += v1; v1 = dmi_anonymize_rotl(v1, 13); v1 ^= v0;   \
        v0 = dmi_anonymize_rotl(v0, 32);                       \
        v2 += v3; v3 = dmi_anonymize_rotl(v3, 16); v3 ^= v2;   \
        v0 += v3; v3 = dmi_anonymize_rotl(v3, 21); v3 ^= v0;   \
        v2 += v1; v1 = dmi_anonymize_rotl(v1, 17); v1 ^= v2;   \
        v2 = dmi_anonymize_rotl(v2, 32);                       \
    } while (false)

//
// SipHash-2-4, a keyed hash whose output tells nothing of its input without
// the key, which is what makes the replacements irreversible
//
static uint64_t dmi_anonymize_hash(const dmi_anonymizer_t *anon, const void *data, size_t length)
{
    const dmi_byte_t *bytes = data;

    uint64_t v0 = anon->key[0] ^ UINT64_C(0x736F6D6570736575);
    uint64_t v1 = anon->key[1] ^ UINT64_C(0x646F72616E646F6D);
    uint64_t v2 = anon->key[0] ^ UINT64_C(0x6C7967656E657261);
    uint64_t v3 = anon->key[1] ^ UINT64_C(0x7465646279746573);

    size_t tail = length % 8;
    size_t end  = length - tail;

    for (size_t i = 0; i < end; i += 8) {
        uint64_t m = 0;
        for (size_t j = 0; j < 8; j++)
            m |= (uint64_t)bytes[i + j] << (8 * j);

        v3 ^= m;
        DMI_SIPROUND(v0, v1, v2, v3);
        DMI_SIPROUND(v0, v1, v2, v3);
        v0 ^= m;
    }

    uint64_t last = (uint64_t)length << 56;
    for (size_t j = 0; j < tail; j++)
        last |= (uint64_t)bytes[end + j] << (8 * j);

    v3 ^= last;
    DMI_SIPROUND(v0, v1, v2, v3);
    DMI_SIPROUND(v0, v1, v2, v3);
    v0 ^= last;

    v2 ^= 0xFF;
    for (size_t i = 0; i < 4; i++)
        DMI_SIPROUND(v0, v1, v2, v3);

    return v0 ^ v1 ^ v2 ^ v3;
}

#undef DMI_SIPROUND

//
// SplitMix64, which stretches the hash of a value into as many random bytes
// as its replacement takes
//
static uint64_t dmi_anonymize_next(uint64_t *state)
{
    uint64_t z = (*state += UINT64_C(0x9E3779B97F4A7C15));

    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);

    return z ^ (z >> 31);
}

static bool dmi_anonymize_entity(dmi_anonymizer_t *anon, dmi_entity_t *entity)
{
    if ((entity == nullptr) or (entity->spec == nullptr) or (entity->info == nullptr))
        return true;
    if (not (entity->state & DMI_ENTITY_STATE_DECODED) or (entity->spec->attributes == nullptr))
        return true;

    anon->entity = entity;

    bool changed = false;
    bool success = dmi_anonymize_attrs(anon, entity->spec->attributes, entity->info, &changed);

    // Members replaced in the decoded structure are written by encoding it,
    // since only the specification knows where they are
    if (success and changed)
        success = dmi_anonymize_encode(anon, entity);

    dmi_anonymize_restore(anon);

    return success;
}

static bool dmi_anonymize_attrs(
        dmi_anonymizer_t      *anon,
        const dmi_attribute_t *attrs,
        dmi_byte_t            *info,
        bool                  *changed)
{
    for (const dmi_attribute_t *attr = attrs; attr->type != DMI_ATTRIBUTE_TYPE_NONE; attr++) {
        const dmi_attribute_t *resolved = dmi_attribute_resolve(attr, info);
        if (resolved == nullptr)
            continue;

        dmi_byte_t *value = info + resolved->value.offset;

        if (dmi_attribute_is_array(resolved)) {
            dmi_byte_t *element = (dmi_byte_t *)dmi_attribute_get_elements(resolved, value);
            size_t count = dmi_attribute_get_count(resolved, info);

            for (size_t i = 0; (element != nullptr) and (i < count); i++) {
                if (not dmi_anonymize_attr(anon, resolved, element, changed))
                    return false;
                element += resolved->value.size;
            }
        } else {
            if (not dmi_anonymize_attr(anon, resolved, value, changed))
                return false;
        }
    }

    return true;
}

static bool dmi_anonymize_attr(
        dmi_anonymizer_t      *anon,
        const dmi_attribute_t *attr,
        dmi_byte_t            *value,
        bool                  *changed)
{
    if (attr->type == DMI_ATTRIBUTE_TYPE_STRUCT) {
        if (attr->params.attrs == nullptr)
            return true;

        return dmi_anonymize_attrs(anon, attr->params.attrs, value, changed);
    }

    if (not (attr->params.flags & DMI_ATTRIBUTE_FLAG_PRIVATE))
        return true;
    if (dmi_attribute_is_unspecified(attr, value) or dmi_attribute_is_unknown(attr, value))
        return true;

    return dmi_anonymize_member(anon, attr, value, changed);
}

static bool dmi_anonymize_member(
        dmi_anonymizer_t      *anon,
        const dmi_attribute_t *attr,
        dmi_byte_t            *value,
        bool                  *changed)
{
    switch (attr->type) {
    case DMI_ATTRIBUTE_TYPE_STRING: {
        const char *text = dmi_deref(char *, value);
        if ((text == nullptr) or dmi_string_is_placeholder(text))
            return true;

        size_t length = strlen(text);
        if ((length == 0) or dmi_anonymize_is_blank(text, length))
            return true;

        dmi_byte_t *replacement = dmi_alloc(anon->context, length);
        if (replacement == nullptr)
            return false;

        dmi_anonymize_text(anon, (const dmi_byte_t *)text, length, replacement);

        // String of the attribute is the one the value has been read from,
        // which holds it with the spaces the firmware pads it with
        const dmi_entity_t *entity = anon->entity;
        for (size_t i = 0; i < entity->string_count; i++) {
            const char *raw = entity->strings[i].raw;
            if ((entity->strings[i].pretty != text) or (raw == nullptr))
                continue;

            const char *found = strstr(raw, text);
            if (found != nullptr) {
                size_t offset = (size_t)((const dmi_byte_t *)found - anon->source->data);
                memcpy(anon->table->data + offset, replacement, length);
            }
            break;
        }

        // Values which do not look like identifiers, e.g. a battery named
        // "Battery 0" in place of its serial number, are left elsewhere
        bool success = dmi_anonymize_add(anon, text, replacement, length,
                                         dmi_anonymize_is_identifier(text));
        dmi_free(replacement);

        return success;
    }

    case DMI_ATTRIBUTE_TYPE_BINARY: {
        const dmi_binary_t *binary = (const dmi_binary_t *)value;
        const dmi_byte_t   *begin  = anon->source->data;

        if ((binary->data == nullptr) or (binary->length == 0))
            return true;
        if (dmi_anonymize_is_blank(binary->data, binary->length))
            return true;
        if ((binary->data < begin) or (binary->data + binary->length > begin + anon->source->length))
            return true;

        // Manufacturer part of a MAC address tells the vendor of the device
        // rather than the device
        size_t keep = (attr->params.flags & DMI_ATTRIBUTE_FLAG_MAC) ? 3 : 0;
        if (keep >= binary->length)
            keep = 0;

        size_t offset = (size_t)(binary->data - begin);
        dmi_byte_t *replacement = anon->table->data + offset;

        dmi_anonymize_bytes(anon, binary->data, binary->length, keep, replacement);

        return dmi_anonymize_add(anon, binary->data, replacement, binary->length, true);
    }

    case DMI_ATTRIBUTE_TYPE_UUID:
    case DMI_ATTRIBUTE_TYPE_INTEGER: {
        size_t size = attr->value.size;
        if ((size == 0) or (size > DMI_ANONYMIZE_MEMBER_MAX))
            return true;
        if (dmi_anonymize_is_blank(value, size))
            return true;

        dmi_anonymize_saved_t *saved = dmi_alloc(anon->context, sizeof(*saved));
        if (saved == nullptr)
            return false;

        saved->ptr  = value;
        saved->size = size;
        memcpy(saved->bytes, value, size);

        if (not dmi_vector_push(&anon->saved, (uintptr_t)saved)) {
            dmi_free(saved);
            dmi_error_raise(anon->context, DMI_ERROR_OUT_OF_MEMORY);
            return false;
        }

        dmi_byte_t replaced[DMI_ANONYMIZE_MEMBER_MAX];
        dmi_anonymize_bytes(anon, saved->bytes, size, 0, replaced);

        if (attr->type == DMI_ATTRIBUTE_TYPE_UUID) {
            // Version and variant tell how the UUID has been made
            const dmi_uuid_t *uuid = (const dmi_uuid_t *)saved->bytes;
            dmi_uuid_t *result = (dmi_uuid_t *)replaced;

            result->time_hi_and_version = (uint16_t)((result->time_hi_and_version & 0x0FFF) |
                                                     (uuid->time_hi_and_version & 0xF000));
            result->clock_seq_hi_and_reserved = (uint8_t)((result->clock_seq_hi_and_reserved & 0x3F) |
                                                          (uuid->clock_seq_hi_and_reserved & 0xC0));
        }

        // Replacement must not stand for "unspecified" itself
        if (dmi_anonymize_is_blank(replaced, size))
            replaced[0] ^= 0x01;

        memcpy(value, replaced, size);
        *changed = true;

        return true;
    }

    default:
        return true;
    }
}

static bool dmi_anonymize_encode(dmi_anonymizer_t *anon, dmi_entity_t *entity)
{
    if (entity->buffer != anon->source)
        return true;

    dmi_buffer_t *buffer = dmi_buffer_create(anon->context);
    if (buffer == nullptr)
        return false;

    dmi_encoder_t encoder;
    bool success = false;

    do {
        if (not dmi_encoder_initialize(&encoder, buffer, entity, DMI_ENCODE_MODE_PRESERVE, DMI_VERSION_NONE))
            break;

        bool encoded = dmi_entity_encode(&encoder);
        dmi_encoder_finalize(&encoder);

        if (not encoded)
            break;

        // Formatted area only, since the strings are replaced where they are
        if ((buffer->length != entity->body_length) or
            (entity->offset + entity->body_length > anon->table->length))
        {
            dmi_error_raise_ex(anon->context, DMI_ERROR_INTERNAL,
                               "Structure 0x%04X is not encoded back into its own length",
                               entity->handle);
            break;
        }

        memcpy(anon->table->data + entity->offset, buffer->data, buffer->length);

        // Replaced values are looked for elsewhere too, as the structure
        // holds them, which is known for the ones it holds as they are
        const dmi_byte_t *source = anon->source->data + entity->offset;
        const dmi_byte_t *result = anon->table->data + entity->offset;

        success = true;
        for (size_t i = 0; i < dmi_vector_length(&anon->saved); i++) {
            uintptr_t item;
            dmi_vector_get(&anon->saved, i, &item);

            const dmi_anonymize_saved_t *saved = (const dmi_anonymize_saved_t *)item;
            if (saved->size < DMI_ANONYMIZE_PATTERN_MIN)
                continue;

            for (size_t pos = 0; pos + saved->size <= entity->body_length; pos++) {
                if ((memcmp(source + pos, saved->bytes, saved->size) != 0) or
                    (memcmp(result + pos, saved->bytes, saved->size) == 0))
                {
                    continue;
                }

                if (not dmi_anonymize_add(anon, saved->bytes, result + pos, saved->size, true))
                    success = false;
                break;
            }

            if (not success)
                break;
        }
    } while (false);

    dmi_buffer_destroy(buffer);

    return success;
}

static void dmi_anonymize_restore(dmi_anonymizer_t *anon)
{
    uintptr_t item;

    // Members are put back in the reverse order, in case one is saved twice
    while (dmi_vector_pop(&anon->saved, &item)) {
        dmi_anonymize_saved_t *saved = (dmi_anonymize_saved_t *)item;

        memcpy(saved->ptr, saved->bytes, saved->size);
        dmi_free(saved);
    }
}

static bool dmi_anonymize_add(
        dmi_anonymizer_t *anon,
        const void       *data,
        const void       *replacement,
        size_t            length,
        bool              propagate)
{
    if ((length == 0) or dmi_anonymize_is_blank(data, length))
        return true;

    // Values are kept in the order they are found, and looked for longest
    // first by the passes, so a value holding another one is replaced whole
    for (size_t i = 0; i < dmi_vector_length(&anon->values); i++) {
        uintptr_t item;
        dmi_vector_get(&anon->values, i, &item);

        dmi_anonymize_value_t *value = (dmi_anonymize_value_t *)item;
        if ((value->length == length) and (memcmp(value->data, data, length) == 0)) {
            value->propagate = value->propagate or propagate;
            return true;
        }
    }

    dmi_anonymize_value_t *value = dmi_alloc(anon->context, sizeof(*value) + 2 * length);
    if (value == nullptr)
        return false;

    dmi_byte_t *copy = (dmi_byte_t *)(value + 1);
    memcpy(copy, data, length);
    memcpy(copy + length, replacement, length);

    value->data        = copy;
    value->replacement = copy + length;
    value->length      = length;
    value->propagate   = propagate;

    if (not dmi_vector_push(&anon->values, (uintptr_t)value)) {
        dmi_free(value);
        dmi_error_raise(anon->context, DMI_ERROR_OUT_OF_MEMORY);
        return false;
    }

    return true;
}

static bool dmi_anonymize_is_identifier(const char *text)
{
    bool has_digit = false;
    char seen[3] = { 0 };
    size_t distinct = 0;

    // Serial numbers and asset tags are single words holding digits, unlike
    // the names firmware puts in their place, and made of more than a couple
    // of characters, unlike counters such as 00000001
    for (const char *ptr = text; *ptr != 0; ptr++) {
        if (isspace((unsigned char)*ptr))
            return false;
        if (isdigit((unsigned char)*ptr))
            has_digit = true;

        if ((distinct < countof(seen)) and (memchr(seen, *ptr, distinct) == nullptr))
            seen[distinct++] = *ptr;
    }

    return has_digit and (distinct >= countof(seen));
}

static bool dmi_anonymize_is_blank(const void *data, size_t length)
{
    const dmi_byte_t *bytes = data;

    // Values of one byte repeated, e.g. all zeroes, all bits set or all
    // spaces, stand for no value at all
    for (size_t i = 1; i < length; i++) {
        if (bytes[i] != bytes[0])
            return false;
    }

    return true;
}

static void dmi_anonymize_text(const dmi_anonymizer_t *anon, const dmi_byte_t *data, size_t length, dmi_byte_t *out)
{
    uint64_t state = dmi_anonymize_hash(anon, data, length);

    // Letters and digits are replaced with ones of their own kind, so the
    // value keeps its format, and everything else is kept
    for (size_t i = 0; i < length; i++) {
        unsigned c = data[i];
        unsigned r = (unsigned)(dmi_anonymize_next(&state) >> 32);

        if ((c >= '0') and (c <= '9'))
            out[i] = (dmi_byte_t)('0' + r % 10);
        else if ((c >= 'A') and (c <= 'Z'))
            out[i] = (dmi_byte_t)('A' + r % 26);
        else if ((c >= 'a') and (c <= 'z'))
            out[i] = (dmi_byte_t)('a' + r % 26);
        else
            out[i] = (dmi_byte_t)c;
    }
}

static void dmi_anonymize_bytes(
        const dmi_anonymizer_t *anon,
        const dmi_byte_t       *data,
        size_t                  length,
        size_t                  keep,
        dmi_byte_t             *out)
{
    uint64_t state = dmi_anonymize_hash(anon, data, length);

    for (size_t i = 0; i < length; i++)
        out[i] = (i < keep) ? data[i] : (dmi_byte_t)(dmi_anonymize_next(&state) >> 56);
}

static void dmi_anonymize_strings(dmi_anonymizer_t *anon, const dmi_entity_t *entity)
{
    if ((entity == nullptr) or (entity->buffer != anon->source))
        return;

    const dmi_byte_t *begin = anon->source->data;

    for (size_t i = 0; i < entity->string_count; i++) {
        const char *raw = entity->strings[i].raw;
        if (raw == nullptr)
            continue;

        size_t start = (size_t)((const dmi_byte_t *)raw - begin);
        dmi_anonymize_replace(anon, start, start + strlen(raw), DMI_ANONYMIZE_SUBSTRING_MIN);
    }
}

static void dmi_anonymize_body(dmi_anonymizer_t *anon, const dmi_entity_t *entity)
{
    if ((entity == nullptr) or (entity->buffer != anon->source))
        return;

    // Header is left out, since its handle may happen to match
    size_t start = entity->offset + sizeof(dmi_header_t);
    size_t end   = entity->offset + entity->body_length;

    if ((end <= start) or (end > anon->table->length))
        return;

    dmi_anonymize_replace(anon, start, end, DMI_ANONYMIZE_PATTERN_MIN);
}

static void dmi_anonymize_replace(
        dmi_anonymizer_t *anon,
        size_t            start,
        size_t            end,
        size_t            minimum)
{
    size_t count = dmi_vector_length(&anon->values);
    dmi_byte_t *table = anon->table->data;

    // Longer values are replaced first, so that a value holding another one
    // is not replaced in part
    size_t previous = SIZE_MAX;
    for (;;) {
        size_t longest = 0;
        for (size_t i = 0; i < count; i++) {
            uintptr_t item;
            dmi_vector_get(&anon->values, i, &item);

            size_t length = ((const dmi_anonymize_value_t *)item)->length;
            if ((length < previous) and (length > longest))
                longest = length;
        }

        if ((longest == 0) or (longest < minimum))
            break;

        for (size_t i = 0; i < count; i++) {
            uintptr_t item;
            dmi_vector_get(&anon->values, i, &item);

            const dmi_anonymize_value_t *value = (const dmi_anonymize_value_t *)item;
            if (not value->propagate or (value->length != longest) or (end - start < value->length))
                continue;

            // Source is searched rather than the copy, whose bytes are
            // replaced already where a value has been found
            for (size_t pos = start; pos + value->length <= end; pos++) {
                if (memcmp(anon->source->data + pos, value->data, value->length) != 0)
                    continue;

                memcpy(table + pos, value->replacement, value->length);
                pos += value->length - 1;
            }
        }

        previous = longest;
    }
}

static void dmi_anonymize_cleanup(dmi_anonymizer_t *anon)
{
    uintptr_t item;

    dmi_anonymize_restore(anon);

    while (dmi_vector_pop(&anon->values, &item))
        dmi_free((void *)item);

    dmi_vector_clear(&anon->values);
    dmi_vector_clear(&anon->saved);
}
