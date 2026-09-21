//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <limits.h>
#include <string.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/field.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/uuid.h>

//
// Largest number of the members an extended field may replace the value of,
// which is a handful in the structures the specification defines.
//
#define DMI_FIELD_PLAIN_MAX 8

//
// Value of a plain field kept for the extended one which may replace it,
// because the member no longer holds it once it has been converted.
//
typedef struct dmi_field_plain
{
    size_t    offset;
    uintmax_t raw;
} dmi_field_plain_t;

//
// State of a structure being decoded, which the fields are read into.
//
typedef struct dmi_field_state
{
    dmi_entity_t  *entity;
    dmi_stream_t  *stream;
    dmi_data_t    *info;

    // Fields before the first group are required, so the data ending within
    // them is a broken structure rather than a short one
    bool optional;

    // Data ended where the structure is allowed to carry less than it
    // declares, such as in the middle of the elements of an array, which
    // stops the fields after it from being read without breaking it
    bool incomplete;

    // Bits read from the data and not given to a field yet, which the ranges
    // of bits sharing a unit are taken from
    uintmax_t bits_value;
    unsigned  bits_count;

    // Bits the ranges have taken since the run of them started, which tells
    // how much of the unit the padding at its end has to close
    unsigned  bits_taken;
} dmi_field_state_t;

static bool dmi_field_decode_list(
        dmi_field_state_t *state,
        const dmi_field_t *fields,
        dmi_data_t        *info);

static bool dmi_field_decode_nested(
        dmi_field_state_t *state,
        const dmi_field_t *fields,
        dmi_data_t        *info);

static bool dmi_field_decode_one(
        dmi_field_state_t  *state,
        const dmi_field_t  *field,
        dmi_data_t         *info,
        dmi_field_plain_t  *plain,
        unsigned            plain_count);

static dmi_field_plain_t *dmi_field_plain_find(
        dmi_field_plain_t *plain,
        unsigned           count,
        size_t             offset);

static bool dmi_field_decode_array(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        dmi_data_t        *info);

static bool dmi_field_decode_value(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        void              *value,
        uintmax_t         *raw);

static bool dmi_field_read_count(
        dmi_field_state_t *state,
        dmi_field_type_t   type,
        size_t            *count);

static bool dmi_field_decode_bits(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        void              *value);

static bool dmi_field_decode_pad(dmi_field_state_t *state, const dmi_field_t *field);

static bool dmi_field_take_bits(dmi_field_state_t *state, unsigned bits, uintmax_t *value);

static bool dmi_field_store(const dmi_field_t *field, void *value, uintmax_t raw);

static bool dmi_field_store_value(const dmi_field_t *field, void *value, uintmax_t raw);

static bool dmi_field_store_member(dmi_member_ref_t member, void *value, uintmax_t raw);

//
// Offset of the member whose plain field says that an extended one carries
// the real value, which is the member of the field itself unless the
// specification keys one field on another.
//
static inline size_t dmi_field_when_offset(const dmi_field_t *field)
{
    return dmi_member_is_present(field->params.when)
            ? field->params.when.offset
            : field->member.offset;
}

uintmax_t dmi_field_kilobytes(uintmax_t raw)
{
    return raw << 10;
}

size_t dmi_field_type_size(dmi_field_type_t type)
{
    switch (type) {
    case DMI_FIELD_TYPE_BYTE:
    case DMI_FIELD_TYPE_STRING:
        return sizeof(dmi_byte_t);

    case DMI_FIELD_TYPE_WORD:
        return sizeof(dmi_word_t);

    case DMI_FIELD_TYPE_DWORD:
        return sizeof(dmi_dword_t);

    case DMI_FIELD_TYPE_QWORD:
        return sizeof(dmi_qword_t);

    case DMI_FIELD_TYPE_UUID:
        return 16 * sizeof(dmi_byte_t);

    default:
        return 0;
    }
}

bool dmi_fields_decode(dmi_entity_t *entity)
{
    if (entity == nullptr) {
        dmi_error_raise_ex(nullptr, DMI_ERROR_NULL_ARGUMENT, "entity");
        return false;
    }

    const dmi_entity_spec_t *spec = entity->spec;

    if ((spec == nullptr) or (spec->fields == nullptr)) {
        dmi_error_raise_ex(dmi_entity_context(entity), DMI_ERROR_INVALID_STATE,
                           "Handle 0x%04hx: specification declares no fields",
                           dmi_entity_handle(entity));
        return false;
    }

    dmi_data_t *info = dmi_entity_info(entity, spec->type);
    if (info == nullptr)
        return false;

    dmi_field_state_t state = {
        .entity = entity,
        .stream = dmi_entity_stream(entity),
        .info   = info
    };

    return dmi_field_decode_list(&state, spec->fields, info);
}

//
// Read a list of fields into a structure, which is the decoded structure
// itself for the fields of its specification, and an element of an array or a
// nested structure for the fields of one.
//
static bool dmi_field_decode_list(
        dmi_field_state_t *state,
        const dmi_field_t *fields,
        dmi_data_t        *info)
{
    assert(state != nullptr);
    assert(fields != nullptr);
    assert(info != nullptr);

    // Members which mean something other than zero when the data ends before
    // their fields are set before anything is read
    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        if ((field->params.absent != nullptr) and dmi_member_is_present(field->member))
            memcpy(info + field->member.offset, field->params.absent, field->member.size);
    }

    // Members an extended field replaces the value of, whose plain fields are
    // the ones worth keeping the value read from
    dmi_field_plain_t plain[DMI_FIELD_PLAIN_MAX] = {};
    unsigned plain_count = 0;

    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        if (not field->params.extended)
            continue;

        size_t offset = dmi_field_when_offset(field);

        if (dmi_field_plain_find(plain, plain_count, offset) != nullptr)
            continue;

        assert(plain_count < DMI_FIELD_PLAIN_MAX);

        if (plain_count < DMI_FIELD_PLAIN_MAX)
            plain[plain_count++].offset = offset;
    }

    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        // Groups carry no data of their own, and tell where the structure is
        // allowed to end
        // Boundaries state where the parts of a structure begin, and are
        // checked so that a declaration which has lost or gained a field is
        // caught where it happens. The check costs one comparison and is
        // made in every build, since a declaration which does not match the
        // data it describes decodes the rest of the structure into nonsense
        if (field->type == DMI_FIELD_TYPE_OFFSET) {
            size_t position = dmi_stream_tell(state->stream);

            if (position != field->params.offset) {
                dmi_error_raise_ex(dmi_entity_context(state->entity), DMI_ERROR_INTERNAL,
                                   "0x%04hx (%s): fields reach offset 0x%02zX, 0x%02zX declared",
                                   dmi_entity_handle(state->entity),
                                   state->entity->spec->code, position, field->params.offset);
                return false;
            }

            continue;
        }

        // Presets carry no data, and have been applied by the pass above
        if (field->type == DMI_FIELD_TYPE_PRESET)
            continue;

        if (field->type == DMI_FIELD_TYPE_GROUP) {
            if (dmi_stream_is_done(state->stream))
                return dmi_entity_stop(state->entity);

            // Version of the group becomes the level of the structure, while
            // the groups the specification does not version leave it as it is
            if (field->params.since != DMI_VERSION_NONE)
                state->entity->level = field->params.since;

            state->optional = true;
            continue;
        }

        if (not dmi_field_decode_one(state, field, info, plain, plain_count)) {
            // The data ending where the structure is allowed to be short of
            // what it declares leaves the fields after it unread, while
            // ending anywhere else breaks the structure
            return (state->optional or state->incomplete)
                    ? dmi_entity_incomplete(state->entity)
                    : false;
        }
    }

    return true;
}

//
// Read the fields of an array element or of a nested structure, which the
// groups of the structure holding them say nothing about: an element is
// either there in full or not at all, and the field reading it decides what
// the data ending inside it means.
//
static bool dmi_field_decode_nested(
        dmi_field_state_t *state,
        const dmi_field_t *fields,
        dmi_data_t        *info)
{
    assert(state != nullptr);

    bool optional = state->optional;

    state->optional = false;
    bool status = dmi_field_decode_list(state, fields, info);
    state->optional = optional;

    return status;
}

static bool dmi_field_decode_one(
        dmi_field_state_t  *state,
        const dmi_field_t  *field,
        dmi_data_t         *info,
        dmi_field_plain_t  *plain,
        unsigned            plain_count)
{
    assert(state != nullptr);
    assert(field != nullptr);
    assert(info != nullptr);

    // Fields which carry no value of their own are read for their side
    // effects, e.g. to step over the bytes they occupy
    void *value = dmi_member_is_present(field->member)
            ? (info + field->member.offset)
            : nullptr;

    // Ranges of bits are closed by a padding, so a field of another kind
    // never begins in the middle of a unit
    assert((field->type == DMI_FIELD_TYPE_BITS) or
           (field->type == DMI_FIELD_TYPE_PAD) or
           (state->bits_count == 0));

    switch (field->type) {
    case DMI_FIELD_TYPE_BITS:
        return dmi_field_decode_bits(state, field, value);

    case DMI_FIELD_TYPE_PAD:
        return dmi_field_decode_pad(state, field);

    case DMI_FIELD_TYPE_ARRAY:
        return dmi_field_decode_array(state, field, info);

    case DMI_FIELD_TYPE_STRUCT:
        return dmi_field_decode_nested(state, field->params.fields, value);

    case DMI_FIELD_TYPE_CUSTOM:
        return field->params.decode(state->entity, field, value);

    default:
        break;
    }

    dmi_field_plain_t *kept =
            dmi_field_plain_find(plain, plain_count, dmi_field_when_offset(field));

    // An extended field replaces the value only when the plain one says that
    // the real value is here, and is read past either way
    if (field->params.extended) {
        uintmax_t raw = 0;

        if (not dmi_field_decode_value(state, field, nullptr, &raw))
            return false;

        if ((kept == nullptr) or (kept->raw != field->params.when_raw))
            return true;

        return dmi_field_store_value(field, value, raw);
    }

    return dmi_field_decode_value(state, field, value, (kept != nullptr) ? &kept->raw : nullptr);
}

static dmi_field_plain_t *dmi_field_plain_find(
        dmi_field_plain_t *plain,
        unsigned           count,
        size_t             offset)
{
    for (unsigned i = 0; i < count; i++) {
        if (plain[i].offset == offset)
            return &plain[i];
    }

    return nullptr;
}

//
// Read a value of a fixed width into a member of the decoded structure, which
// may be wider than the field is, so that the values standing for "unknown"
// do not collide with the ones a device may really have.
//
static bool dmi_field_decode_value(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        void              *value,
        uintmax_t         *raw_out)
{
    assert(state != nullptr);
    assert(field != nullptr);

    if (field->type == DMI_FIELD_TYPE_STRING)
        return dmi_stream_decode_str(state->stream, (const char **)value);

    // Binary-coded decimals are read by their own decoder, which spells the
    // digits out into the number they stand for
    if (field->type == DMI_FIELD_TYPE_BCD) {
        switch (field->params.count_type) {
        case DMI_FIELD_TYPE_BYTE: {
            dmi_byte_t data = 0;
            if (not dmi_stream_decode_bcd(state->stream, dmi_byte_t, &data))
                return false;
            if (raw_out != nullptr)
                *raw_out = data;
            return dmi_field_store_value(field, value, data);
        }

        case DMI_FIELD_TYPE_WORD: {
            dmi_word_t data = 0;
            if (not dmi_stream_decode_bcd(state->stream, dmi_word_t, &data))
                return false;
            if (raw_out != nullptr)
                *raw_out = data;
            return dmi_field_store_value(field, value, data);
        }

        default:
            assert(false);
            return false;
        }
    }

    if (field->type == DMI_FIELD_TYPE_SKIP)
        return dmi_stream_skip(state->stream, field->params.length);

    if (field->type == DMI_FIELD_TYPE_UUID)
        return dmi_stream_decode_uuid(state->stream, (dmi_uuid_t *)value);

    if (field->type == DMI_FIELD_TYPE_BINARY) {
        size_t length = field->params.length;

        if (length == DMI_FIELD_LENGTH_REST)
            length = dmi_stream_remaining(state->stream);

        return dmi_stream_decode_bin(state->stream, length, (dmi_binary_t *)value);
    }

    uintmax_t raw = 0;

    switch (field->type) {
    case DMI_FIELD_TYPE_BYTE: {
        dmi_byte_t data = 0;
        if (not dmi_stream_decode(state->stream, dmi_byte_t, &data))
            return false;
        raw = data;
        break;
    }

    case DMI_FIELD_TYPE_WORD: {
        dmi_word_t data = 0;
        if (not dmi_stream_decode(state->stream, dmi_word_t, &data))
            return false;
        raw = data;
        break;
    }

    case DMI_FIELD_TYPE_DWORD: {
        dmi_dword_t data = 0;
        if (not dmi_stream_decode(state->stream, dmi_dword_t, &data))
            return false;
        raw = data;
        break;
    }

    case DMI_FIELD_TYPE_QWORD: {
        dmi_qword_t data = 0;
        if (not dmi_stream_decode(state->stream, dmi_qword_t, &data))
            return false;
        raw = data;
        break;
    }

    default:
        assert(false);
        return false;
    }

    if (raw_out != nullptr)
        *raw_out = raw;

    return dmi_field_store_value(field, value, raw);
}

//
// Write a value into the member of the decoded structure, applying whatever
// the field says the data means: a value standing for "unknown" becomes the
// largest value the member can hold, and the rest is converted if the field
// asks for it.
//
static bool dmi_field_store_value(const dmi_field_t *field, void *value, uintmax_t raw)
{
    assert(field != nullptr);

    if (value == nullptr)
        return true;

    // Unknown is the largest value of the width of the member, which is wider
    // than the field whenever the field has a value standing for it
    if ((field->params.unknown_raw != 0) and (raw == field->params.unknown_raw))
        return dmi_field_store(field, value, UINTMAX_MAX);

    if (field->params.convert != nullptr)
        raw = field->params.convert(raw);

    return dmi_field_store(field, value, raw);
}

//
// Write a value into the member of the decoded structure, which may be wider
// than the field it has been read from.
//
static bool dmi_field_store(const dmi_field_t *field, void *value, uintmax_t raw)
{
    assert(field != nullptr);

    return dmi_field_store_member(field->member, value, raw);
}

//
// Write a value into a member of the decoded structure of the width the
// member has, which is how the values the fields carry for one another, such
// as the number of the elements an array declares, are stored.
//
static bool dmi_field_store_member(dmi_member_ref_t member, void *value, uintmax_t raw)
{
    if (value == nullptr)
        return true;

    switch (member.size) {
    case sizeof(uint8_t):
        *(uint8_t *)value = (uint8_t)raw;
        break;

    case sizeof(uint16_t):
        *(uint16_t *)value = (uint16_t)raw;
        break;

    case sizeof(uint32_t):
        *(uint32_t *)value = (uint32_t)raw;
        break;

    case sizeof(uint64_t):
        *(uint64_t *)value = (uint64_t)raw;
        break;

    default:
        assert(false);
        return false;
    }

    return true;
}

//
// Take a range of the bits the fields of a unit share. Bits are taken from
// the least significant one up, and a range may span the bytes of the unit,
// which is why the bytes are read one at a time as the ranges need them: the
// bits of a little-endian value come in the same order either way.
//
static bool dmi_field_decode_bits(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        void              *value)
{
    assert(state != nullptr);
    assert(field != nullptr);

    uintmax_t raw = 0;

    if (not dmi_field_take_bits(state, field->params.bits, &raw))
        return false;

    return dmi_field_store_value(field, value, raw);
}

//
// Close a unit the ranges of bits share, stepping over the ones the
// specification reserves at its end.
//
static bool dmi_field_decode_pad(dmi_field_state_t *state, const dmi_field_t *field)
{
    assert(state != nullptr);
    assert(field != nullptr);

    unsigned unit = field->params.bits;

    assert(unit != 0);

    unsigned taken = state->bits_taken % unit;
    uintmax_t reserved;

    if ((taken != 0) and not dmi_field_take_bits(state, unit - taken, &reserved))
        return false;

    // Unit is a whole number of bytes, so closing it leaves no bits behind
    assert(state->bits_count == 0);

    state->bits_taken = 0;

    return true;
}

static bool dmi_field_take_bits(dmi_field_state_t *state, unsigned bits, uintmax_t *value)
{
    assert(state != nullptr);
    assert(value != nullptr);
    assert((bits > 0) and (bits <= (sizeof(uintmax_t) * CHAR_BIT) - CHAR_BIT));

    // Bytes are read as the ranges need them, so that a unit no field reaches
    // the end of is not read past
    while (state->bits_count < bits) {
        dmi_byte_t data = 0;

        if (not dmi_stream_decode(state->stream, dmi_byte_t, &data))
            return false;

        state->bits_value |= (uintmax_t)data << state->bits_count;
        state->bits_count += CHAR_BIT;
    }

    *value = state->bits_value & ((((uintmax_t)1 << bits) - 1));

    state->bits_value >>= bits;
    state->bits_count -= bits;
    state->bits_taken += bits;

    return true;
}

//
// Read an array of the elements described by their own fields, counting the
// ones the data holds completely.
//
static bool dmi_field_decode_array(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        dmi_data_t        *info)
{
    assert(state != nullptr);
    assert(field != nullptr);
    assert(info != nullptr);

    // Elements may declare how long they are, which is how a structure makes
    // room for the fields a later version adds to them
    size_t stride = field->params.stride;
    size_t count  = 0;

    // An array the data carries no number of its own for runs to the end of
    // the structure, and holds as many elements as fit into what is left. The
    // bytes left over are a structure which ends in the middle of an element,
    // which is reported once the whole array has been read
    size_t leftover = 0;

    if (field->params.count_type == DMI_FIELD_TYPE_NONE) {
        assert(stride != 0);

        size_t remaining = dmi_stream_remaining(state->stream);

        count    = remaining / stride;
        leftover = remaining % stride;
    } else if (not dmi_field_read_count(state, field->params.count_type, &count)) {
        return false;
    }

    if ((field->params.stride_type != DMI_FIELD_TYPE_NONE) and
        not dmi_field_read_count(state, field->params.stride_type, &stride))
        return false;

    // Numbers the data declares are worth keeping whenever the structure is
    // allowed to hold fewer elements than it says, or to make them longer
    // than the fields they are known to hold
    if (dmi_member_is_present(field->params.count_member) and
        not dmi_field_store_member(field->params.count_member,
                                   info + field->params.count_member.offset, count))
        return false;

    if (dmi_member_is_present(field->params.stride_member) and
        not dmi_field_store_member(field->params.stride_member,
                                   info + field->params.stride_member.offset, stride))
        return false;

    size_t *counter = (size_t *)(info + field->params.counter.offset);
    *counter = 0;

    if (count == 0)
        return (leftover == 0) or dmi_entity_incomplete(state->entity);

    // An element too short for the fields it is known to hold is of no use,
    // so the array is stepped over rather than decoded, since the fields
    // after it begin where the elements end either way
    if ((field->params.stride_minimum != 0) and (stride < field->params.stride_minimum)) {
        size_t length    = count * stride;
        size_t remaining = dmi_stream_remaining(state->stream);

        // Elements of a length the structure does not carry say nothing about
        // where the data ends, so whatever is there is stepped over
        return dmi_stream_skip(state->stream, (length < remaining) ? length : remaining);
    }

    dmi_context_t *context = dmi_entity_context(state->entity);
    dmi_data_t **elements  = (dmi_data_t **)(info + field->member.offset);

    *elements = dmi_alloc_array(context, field->member.size, count);
    if (*elements == nullptr)
        return false;

    // Only the elements the data holds completely are counted, so that the
    // ones a truncated structure cuts in half are left out. A structure which
    // ends in the middle of an element is short of the elements it declares
    // rather than broken, since the ones before it are there to be read
    for (size_t i = 0; i < count; i++) {
        dmi_data_t *element = *elements + (i * field->member.size);

        if ((stride != 0) and not dmi_stream_has(state->stream, stride)) {
            state->incomplete = true;
            return false;
        }

        dmi_stream_mark_t start = dmi_stream_mark(state->stream);

        if (not dmi_field_decode_nested(state, field->params.fields, element)) {
            // The data running out stops exactly at the end of the structure,
            // while anything else leaves the element unread for its own reason
            state->incomplete = dmi_stream_is_done(state->stream);
            return false;
        }

        if ((stride != 0) and not dmi_stream_skip_ex(state->stream, start, stride)) {
            state->incomplete = true;
            return false;
        }

        (*counter)++;
    }

    return (leftover == 0) or dmi_entity_incomplete(state->entity);
}

static bool dmi_field_read_count(
        dmi_field_state_t *state,
        dmi_field_type_t   type,
        size_t            *count)
{
    assert(state != nullptr);
    assert(count != nullptr);

    uintmax_t raw = 0;

    switch (type) {
    case DMI_FIELD_TYPE_BYTE: {
        dmi_byte_t data = 0;
        if (not dmi_stream_decode(state->stream, dmi_byte_t, &data))
            return false;
        raw = data;
        break;
    }

    case DMI_FIELD_TYPE_WORD: {
        dmi_word_t data = 0;
        if (not dmi_stream_decode(state->stream, dmi_word_t, &data))
            return false;
        raw = data;
        break;
    }

    default:
        assert(false);
        return false;
    }

    *count = (size_t)raw;

    return true;
}
