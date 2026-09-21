//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <limits.h>
#include <stdlib.h>
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

static bool dmi_field_read_number(
        dmi_field_state_t *state,
        size_t             length,
        bool               bcd,
        uintmax_t         *number);

static bool dmi_field_decode_bits(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        void              *value);

static bool dmi_field_decode_pad(dmi_field_state_t *state, const dmi_field_t *field);

static bool dmi_field_take_bits(dmi_field_state_t *state, unsigned bits, uintmax_t *value);

static bool dmi_field_store(const dmi_field_t *field, void *value, uintmax_t raw);

static bool dmi_field_apply(const dmi_field_t *field, const dmi_field_data_t *data, void *value);

static bool dmi_field_store_member(dmi_member_ref_t member, void *value, uintmax_t raw);

static uintmax_t dmi_field_load_member(dmi_member_ref_t member, const void *value);

static size_t dmi_field_width(const dmi_field_t *field);

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

uintmax_t dmi_field_get(const dmi_field_t *field, const void *value)
{
    assert(field != nullptr);

    return dmi_field_load_member(field->member, value);
}

bool dmi_field_set(const dmi_field_t *field, void *value, uintmax_t number)
{
    assert(field != nullptr);

    return dmi_field_store(field, value, number);
}

bool dmi_field_decode_kilobytes(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, data->number << 10);
}

bool dmi_field_encode_kilobytes(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    data->number = dmi_field_get(field, value) >> 10;

    return true;
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

        dmi_field_data_t data = {
            .entity = state->entity,
            .number = raw
        };

        return dmi_field_apply(field, &data, value);
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

    dmi_field_data_t data = {
        .entity = state->entity
    };

    switch (field->type) {
    case DMI_FIELD_TYPE_SKIP:
        return dmi_stream_skip(state->stream, field->params.length);

    case DMI_FIELD_TYPE_UUID:
        return dmi_stream_decode_uuid(state->stream, (dmi_uuid_t *)value);

    case DMI_FIELD_TYPE_STRING: {
        dmi_string_t number = 0;
        if (not dmi_stream_read(state->stream, &number))
            return false;
        data.number = number;
        data.string = dmi_entity_string(state->entity, number);
        break;
    }

    // Binary-coded decimals are read by their own decoder, which spells the
    // digits out into the number they stand for
    case DMI_FIELD_TYPE_BCD:
        if (not dmi_field_read_number(state, field->params.length, true, &data.number))
            return false;
        break;

    case DMI_FIELD_TYPE_BINARY: {
        size_t length = field->params.length;

        if (length == DMI_FIELD_LENGTH_REST)
            length = dmi_stream_remaining(state->stream);
        else if (length == DMI_FIELD_LENGTH_MEMBER)
            length = ((const dmi_binary_t *)value)->length;

        // Bytes a field before declares the number of may not all be there,
        // and the ones which are there are kept
        if ((field->params.length == DMI_FIELD_LENGTH_MEMBER) and
            (length > dmi_stream_remaining(state->stream))) {
            dmi_stream_decode_bin(state->stream, dmi_stream_remaining(state->stream), &data.binary);
            dmi_field_apply(field, &data, value);
            state->incomplete = true;
            return false;
        }

        if (not dmi_stream_decode_bin(state->stream, length, &data.binary))
            return false;
        break;
    }

    case DMI_FIELD_TYPE_INTEGER:
        if (not dmi_field_read_number(state, field->params.length, false, &data.number))
            return false;
        break;

    default:
        assert(false);
        return false;
    }

    if (raw_out != nullptr)
        *raw_out = data.number;

    return dmi_field_apply(field, &data, value);
}

//
// Decode the data a field carries into its member, which is what the decoder
// does with the data it reads, and what the encoder does with the data it is
// about to write, to tell whether it decodes into the value the structure
// holds. Data standing for "unknown" becomes the largest value the member can
// hold, and the rest is up to the handler of the field.
//
static bool dmi_field_apply(const dmi_field_t *field, const dmi_field_data_t *data, void *value)
{
    assert(field != nullptr);
    assert(data != nullptr);

    if (value == nullptr)
        return true;

    // Unknown is the largest value of the width of the member, which is wider
    // than the field whenever the field has a value standing for it
    if ((field->params.unknown_raw != 0) and (data->number == field->params.unknown_raw) and
        (field->type != DMI_FIELD_TYPE_STRING) and (field->type != DMI_FIELD_TYPE_BINARY))
        return dmi_field_store(field, value, UINTMAX_MAX);

    if (field->params.decode != nullptr)
        return field->params.decode(field, data, value);

    switch (field->type) {
    case DMI_FIELD_TYPE_STRING:
        *(const char **)value = data->string;
        return true;

    case DMI_FIELD_TYPE_BINARY:
        *(dmi_binary_t *)value = data->binary;
        return true;

    default:
        return dmi_field_store(field, value, data->number);
    }
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

    dmi_field_data_t data = {
        .entity = state->entity
    };

    if (not dmi_field_take_bits(state, field->params.bits, &data.number))
        return false;

    return dmi_field_apply(field, &data, value);
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

    uintmax_t raw = 0;

    if (field->params.count_length == 0) {
        assert(stride != 0);

        size_t remaining = dmi_stream_remaining(state->stream);

        count    = remaining / stride;
        leftover = remaining % stride;
    } else if (dmi_field_read_number(state, field->params.count_length, false, &raw)) {
        count = (size_t)raw;
    } else {
        return false;
    }

    if (field->params.stride_length != 0) {
        if (not dmi_field_read_number(state, field->params.stride_length, false, &raw))
            return false;

        stride = (size_t)raw;
    }

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

//
// Read an unsigned integer of the given width, little-endian, or the
// binary-coded decimal the firmware writes the digits of a number as.
//
static bool dmi_field_read_number(
        dmi_field_state_t *state,
        size_t             length,
        bool               bcd,
        uintmax_t         *number)
{
    assert(state != nullptr);
    assert(number != nullptr);
    assert((length > 0) and (length <= sizeof(uintmax_t)));

    dmi_byte_t data[sizeof(uintmax_t)];

    if (not dmi_stream_read_data(state->stream, data, length))
        return false;

    if (bcd) {
        *number = __dmi_decode_bcd(data, length);
        return true;
    }

    *number = 0;

    for (size_t i = 0; i < length; i++)
        *number |= (uintmax_t)data[i] << (i * CHAR_BIT);

    return true;
}

//
// State of a structure being encoded, which mirrors the one of decoding: the
// ranges of bits sharing a unit are gathered before the bytes of the unit are
// written.
//
typedef struct dmi_field_writer
{
    dmi_encoder_t *encoder;

    // Source data ended before the field being written, at the beginning of
    // a group or in the middle of a field, so nothing more is written
    bool stopped;

    // Bits gathered and not written yet, and the bits the ranges have taken
    // since the run of them started
    uintmax_t bits_value;
    unsigned  bits_count;
    unsigned  bits_taken;
} dmi_field_writer_t;

//
// Representation chosen for the members an extended field may carry, which
// the plain field governing them decides once it is written.
//
typedef struct dmi_field_choice
{
    size_t    offset;
    uintmax_t when_raw;
    bool      extended;
} dmi_field_choice_t;

static bool dmi_field_encode_list(
        dmi_field_writer_t *writer,
        const dmi_field_t  *fields,
        const dmi_data_t   *info);

static bool dmi_field_encode_one(
        dmi_field_writer_t *writer,
        const dmi_field_t  *fields,
        const dmi_field_t  *field,
        const dmi_data_t   *info,
        dmi_field_choice_t *choices,
        unsigned            choice_count);

static bool dmi_field_encode_array(
        dmi_field_writer_t *writer,
        const dmi_field_t  *field,
        const dmi_data_t   *info);

static bool dmi_field_encode_bits(
        dmi_field_writer_t *writer,
        const dmi_field_t  *field,
        const void         *value);

static bool dmi_field_encode_pad(dmi_field_writer_t *writer, const dmi_field_t *field);

static bool dmi_field_put_bits(dmi_field_writer_t *writer, uintmax_t raw, unsigned bits);

static uintmax_t dmi_field_source_bits(const dmi_field_writer_t *writer, unsigned bits);

static bool dmi_field_put_raw(dmi_field_writer_t *writer, size_t width, uintmax_t raw);

static bool dmi_field_put_reserved(dmi_field_writer_t *writer, size_t length);

static bool dmi_field_peek_raw(const dmi_field_writer_t *writer, size_t width, uintmax_t *raw);

static bool dmi_field_governs(const dmi_field_t *fields, const dmi_field_t *field, size_t offset);

static bool dmi_field_value_data(
        const dmi_field_writer_t *writer,
        const dmi_field_t        *field,
        const void               *value,
        const dmi_field_data_t   *original,
        dmi_field_data_t         *data,
        bool                     *exact);

static bool dmi_field_model_data(
        const dmi_field_writer_t *writer,
        const dmi_field_t        *field,
        const void               *value,
        dmi_field_data_t         *data);

static bool dmi_field_decodes_into(const dmi_field_t *field, const dmi_field_data_t *data, const void *value);

static bool dmi_field_source_data(const dmi_field_writer_t *writer, const dmi_field_t *field, dmi_field_data_t *data);

static bool dmi_field_put_data(dmi_field_writer_t *writer, const dmi_field_t *field, const dmi_field_data_t *data);


static size_t dmi_fields_size(const dmi_field_t *fields);

static bool dmi_field_cannot_encode(const dmi_field_writer_t *writer, const char *reason);

static dmi_field_choice_t *dmi_field_choice_find(
        dmi_field_choice_t *choices,
        unsigned            count,
        size_t              offset);

bool dmi_fields_encode(dmi_encoder_t *encoder)
{
    if (encoder == nullptr) {
        dmi_error_raise_ex(nullptr, DMI_ERROR_NULL_ARGUMENT, "encoder");
        return false;
    }

    const dmi_entity_t      *entity = encoder->entity;
    const dmi_entity_spec_t *spec   = entity->spec;

    if ((spec == nullptr) or (spec->fields == nullptr) or (entity->info == nullptr)) {
        dmi_error_raise_ex(dmi_entity_context(entity), DMI_ERROR_INVALID_STATE,
                           "Handle 0x%04hx: specification declares no fields",
                           dmi_entity_handle(entity));
        return false;
    }

    dmi_field_writer_t writer = {
        .encoder = encoder
    };

    if (not dmi_field_encode_list(&writer, spec->fields, entity->info))
        return false;

    // Bytes after the ones the fields describe are the structure's own, and
    // are kept as they are
    size_t remaining = dmi_encoder_remaining(encoder);

    if ((remaining > 0) and not dmi_field_put_reserved(&writer, remaining))
        return false;

    return dmi_encoder_finish(encoder);
}

//
// Write a list of fields from a structure, which is the decoded structure
// itself for the fields of its specification, and an element of an array or a
// nested structure for the fields of one.
//
static bool dmi_field_encode_list(
        dmi_field_writer_t *writer,
        const dmi_field_t  *fields,
        const dmi_data_t   *info)
{
    assert(writer != nullptr);
    assert(fields != nullptr);
    assert(info != nullptr);

    dmi_encoder_t *encoder = writer->encoder;

    // Members an extended field may carry the value of, whose representation
    // the plain field governing them decides
    dmi_field_choice_t choices[DMI_FIELD_PLAIN_MAX] = {};
    unsigned choice_count = 0;

    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        if (not field->params.extended)
            continue;

        size_t offset = dmi_field_when_offset(field);

        if (dmi_field_choice_find(choices, choice_count, offset) != nullptr)
            continue;

        assert(choice_count < DMI_FIELD_PLAIN_MAX);

        if (choice_count < DMI_FIELD_PLAIN_MAX) {
            choices[choice_count++] = (dmi_field_choice_t){
                .offset   = offset,
                .when_raw = field->params.when_raw
            };
        }
    }

    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        if (writer->stopped)
            return true;

        if (field->type == DMI_FIELD_TYPE_OFFSET) {
            size_t position = dmi_encoder_tell(encoder);

            if (position != field->params.offset) {
                dmi_error_raise_ex(dmi_entity_context(encoder->entity), DMI_ERROR_INTERNAL,
                                   "0x%04hx (%s): fields reach offset 0x%02zX, 0x%02zX declared",
                                   dmi_entity_handle(encoder->entity),
                                   encoder->entity->spec->code, position, field->params.offset);
                return false;
            }

            continue;
        }

        if (field->type == DMI_FIELD_TYPE_PRESET)
            continue;

        // Groups are where the structure is allowed to end: where the source
        // data ends in the preserve mode, and at the first group of a later
        // version than the one written for in the canonical one
        if (field->type == DMI_FIELD_TYPE_GROUP) {
            bool ends = (encoder->mode == DMI_ENCODE_MODE_PRESERVE)
                      ? (dmi_encoder_remaining(encoder) == 0)
                      : ((field->params.since != DMI_VERSION_NONE) and
                         (field->params.since > encoder->version));

            if (ends)
                writer->stopped = true;

            continue;
        }

        if (not dmi_field_encode_one(writer, fields, field, info, choices, choice_count))
            return false;
    }

    return true;
}

static bool dmi_field_encode_one(
        dmi_field_writer_t *writer,
        const dmi_field_t  *fields,
        const dmi_field_t  *field,
        const dmi_data_t   *info,
        dmi_field_choice_t *choices,
        unsigned            choice_count)
{
    assert(writer != nullptr);
    assert(field != nullptr);

    dmi_encoder_t *encoder = writer->encoder;

    const void *value = dmi_member_is_present(field->member)
            ? (info + field->member.offset)
            : nullptr;

    assert((field->type == DMI_FIELD_TYPE_BITS) or
           (field->type == DMI_FIELD_TYPE_PAD) or
           (writer->bits_count == 0));

    switch (field->type) {
    case DMI_FIELD_TYPE_BITS:
        return dmi_field_encode_bits(writer, field, value);

    case DMI_FIELD_TYPE_PAD:
        return dmi_field_encode_pad(writer, field);

    case DMI_FIELD_TYPE_ARRAY:
        return dmi_field_encode_array(writer, field, info);

    case DMI_FIELD_TYPE_STRUCT:
        return dmi_field_encode_list(writer, field->params.fields, value);

    case DMI_FIELD_TYPE_SKIP:
        return dmi_field_put_reserved(writer, field->params.length);

    default:
        break;
    }

    // Source data which ends in the middle of the field is kept as it is,
    // since the model holds nothing of a field it has not read in full
    size_t width = dmi_field_width(field);

    if (encoder->mode == DMI_ENCODE_MODE_PRESERVE) {
        size_t remaining = dmi_encoder_remaining(encoder);

        if (remaining < width) {
            writer->stopped = true;
            return dmi_field_put_reserved(writer, remaining);
        }
    }

    switch (field->type) {
    case DMI_FIELD_TYPE_UUID: {
        dmi_byte_t data[16];
        dmi_uuid_encode(dmi_deref(dmi_uuid_t, value), data);
        return dmi_encoder_write(encoder, data, sizeof(data));
    }

    case DMI_FIELD_TYPE_BINARY: {
        // Bytes running to the end of the structure, or as many as a field
        // before declares, are the member's own, whichever way it is written
        if ((field->params.length != DMI_FIELD_LENGTH_REST) and
            (field->params.length != DMI_FIELD_LENGTH_MEMBER))
            break;

        const dmi_binary_t *binary = value;

        return dmi_encoder_write(encoder, binary->data, binary->length);
    }

    case DMI_FIELD_TYPE_BCD: {
        uintmax_t number = dmi_field_load_member(field->member, value);
        uintmax_t digits = 0;

        for (unsigned shift = 0; shift < width * CHAR_BIT; shift += 4, number /= 10)
            digits |= (number % 10) << shift;

        return dmi_field_put_raw(writer, width, digits);
    }

    default:
        break;
    }

    dmi_field_data_t original = {};
    dmi_field_data_t data     = {};
    bool             exact    = true;

    bool from_source = dmi_field_source_data(writer, field, &original);

    // Extended field carries the value only when the plain one governing it
    // has chosen so, and holds what the source data had otherwise
    dmi_field_choice_t *choice =
            dmi_field_choice_find(choices, choice_count, dmi_field_when_offset(field));

    if (field->params.extended) {
        if ((choice == nullptr) or not choice->extended)
            return dmi_field_put_reserved(writer, width);

        if (not dmi_field_value_data(writer, field, value, from_source ? &original : nullptr,
                                     &data, &exact))
            return false;

        return dmi_field_put_data(writer, field, &data);
    }

    if (not dmi_field_value_data(writer, field, value, from_source ? &original : nullptr,
                                 &data, &exact))
        return false;

    // Plain field governing extended ones chooses the representation: the
    // one the source data has in the preserve mode, unless the value no
    // longer fits, and the plain field whenever the value fits otherwise
    if (choice != nullptr) {
        // Every member the choice governs has to fit the plain field for it
        // to be chosen, since they are widened together
        bool fits = exact and (data.number != choice->when_raw);

        for (const dmi_field_t *other = fields; fits and (other->type != DMI_FIELD_TYPE_NONE); other++) {
            if ((other == field) or other->params.extended or
                not dmi_field_governs(fields, other, choice->offset))
                continue;

            dmi_field_data_t other_data  = {};
            bool             other_exact = true;

            if (not dmi_field_value_data(writer, other, info + other->member.offset, nullptr,
                                         &other_data, &other_exact))
                return false;

            fits = other_exact;
        }

        choice->extended = not fits or (from_source and (original.number == choice->when_raw));

        if (choice->extended)
            data.number = choice->when_raw;

        return dmi_field_put_data(writer, field, &data);
    }

    // Plain field of a member whose extended field another member governs
    // holds what the source data had, since the extended one replaced it
    for (const dmi_field_t *other = fields; other->type != DMI_FIELD_TYPE_NONE; other++) {
        if (not other->params.extended or (other->member.offset != field->member.offset))
            continue;

        dmi_field_choice_t *governing =
                dmi_field_choice_find(choices, choice_count, dmi_field_when_offset(other));

        if ((governing != nullptr) and governing->extended) {
            if (encoder->mode == DMI_ENCODE_MODE_PRESERVE)
                return dmi_field_put_reserved(writer, width);

            data.number = governing->when_raw;
        }

        break;
    }

    return dmi_field_put_data(writer, field, &data);
}

static bool dmi_field_encode_array(
        dmi_field_writer_t *writer,
        const dmi_field_t  *field,
        const dmi_data_t   *info)
{
    assert(writer != nullptr);
    assert(field != nullptr);

    dmi_encoder_t *encoder = writer->encoder;
    bool preserve = (encoder->mode == DMI_ENCODE_MODE_PRESERVE);

    size_t counter = dmi_deref(size_t, info + field->params.counter.offset);

    const dmi_data_t *elements = dmi_deref(dmi_data_t *, info + field->member.offset);

    // Number of the elements the data declares, which is greater than the
    // number of the decoded ones when the source data ends before the last
    // element does
    if (field->params.count_length != 0) {
        uintmax_t count = counter;

        if (dmi_member_is_present(field->params.count_member)) {
            count = dmi_field_load_member(field->params.count_member,
                                          info + field->params.count_member.offset);
        } else if (preserve) {
            uintmax_t original = 0;

            if (dmi_field_peek_raw(writer, field->params.count_length, &original) and
                (original > counter))
                count = original;
        }

        if (not dmi_field_put_raw(writer, field->params.count_length, count))
            return false;
    }

    // Length of an element is written from the member holding it, or taken
    // from the source data, or counted from the fields otherwise
    size_t stride = field->params.stride;

    if (field->params.stride_length != 0) {
        uintmax_t length = dmi_fields_size(field->params.fields);

        if (dmi_member_is_present(field->params.stride_member)) {
            length = dmi_field_load_member(field->params.stride_member,
                                           info + field->params.stride_member.offset);
        } else if (preserve) {
            uintmax_t original = 0;

            if (dmi_field_peek_raw(writer, field->params.stride_length, &original))
                length = original;
        }

        if (not dmi_field_put_raw(writer, field->params.stride_length, length))
            return false;

        stride = (size_t)length;
    }

    // Elements too short for the fields they are known to hold have not been
    // decoded, and are kept as they are
    if ((field->params.stride_minimum != 0) and (stride < field->params.stride_minimum)) {
        size_t remaining = dmi_encoder_remaining(encoder);
        size_t length    = counter * stride;

        return dmi_field_put_reserved(writer, (length < remaining) ? length : remaining);
    }

    for (size_t i = 0; i < counter; i++) {
        size_t start = dmi_encoder_tell(encoder);

        if (not dmi_field_encode_list(writer, field->params.fields,
                                      elements + (i * field->member.size)))
            return false;

        if (writer->stopped)
            return true;

        // Elements longer than the fields they are known to hold keep the
        // rest of their bytes
        size_t written = dmi_encoder_tell(encoder) - start;

        if ((stride > written) and not dmi_field_put_reserved(writer, stride - written))
            return false;
    }

    return true;
}

static bool dmi_field_encode_bits(
        dmi_field_writer_t *writer,
        const dmi_field_t  *field,
        const void         *value)
{
    assert(writer != nullptr);
    assert(field != nullptr);

    unsigned  bits = field->params.bits;
    uintmax_t raw  = 0;

    // Bits reserved between the ranges hold what the source data had
    if (value == nullptr) {
        if (writer->encoder->mode == DMI_ENCODE_MODE_PRESERVE)
            raw = dmi_field_source_bits(writer, bits);

        return dmi_field_put_bits(writer, raw, bits);
    }

    dmi_field_data_t original = {};
    dmi_field_data_t data     = {};
    bool             exact    = true;

    bool from_source = dmi_field_source_data(writer, field, &original);

    if (not dmi_field_value_data(writer, field, value, from_source ? &original : nullptr,
                                 &data, &exact))
        return false;

    return dmi_field_put_bits(writer, data.number, bits);
}

static bool dmi_field_encode_pad(dmi_field_writer_t *writer, const dmi_field_t *field)
{
    assert(writer != nullptr);
    assert(field != nullptr);

    unsigned unit  = field->params.bits;
    unsigned taken = writer->bits_taken % unit;

    if (taken != 0) {
        unsigned  bits = unit - taken;
        uintmax_t raw  = 0;

        if (writer->encoder->mode == DMI_ENCODE_MODE_PRESERVE)
            raw = dmi_field_source_bits(writer, bits);

        if (not dmi_field_put_bits(writer, raw, bits))
            return false;
    }

    assert(writer->bits_count == 0);

    writer->bits_taken = 0;

    return true;
}

//
// Gather a range of bits, and write the bytes of the unit as they fill up,
// which is the order the decoder takes them in.
//
static bool dmi_field_put_bits(dmi_field_writer_t *writer, uintmax_t raw, unsigned bits)
{
    assert((bits > 0) and (bits <= (sizeof(uintmax_t) * CHAR_BIT) - CHAR_BIT));

    writer->bits_value |= (raw & ((((uintmax_t)1) << bits) - 1)) << writer->bits_count;
    writer->bits_count += bits;
    writer->bits_taken += bits;

    while (writer->bits_count >= CHAR_BIT) {
        dmi_byte_t data = (dmi_byte_t)(writer->bits_value & 0xFFu);

        if (not dmi_encoder_write(writer->encoder, &data, sizeof(data)))
            return false;

        writer->bits_value >>= CHAR_BIT;
        writer->bits_count  -= CHAR_BIT;
    }

    return true;
}

//
// Take the bits the source data holds at the position of the next range,
// which starts in the byte not written yet.
//
static uintmax_t dmi_field_source_bits(const dmi_field_writer_t *writer, unsigned bits)
{
    size_t     length = (writer->bits_count + bits + CHAR_BIT - 1) / CHAR_BIT;
    dmi_byte_t data[sizeof(uintmax_t)] = {};

    if ((length > sizeof(data)) or not dmi_encoder_peek(writer->encoder, data, length))
        return 0;

    uintmax_t value = 0;

    for (size_t i = 0; i < length; i++)
        value |= (uintmax_t)data[i] << (i * CHAR_BIT);

    return (value >> writer->bits_count) & ((((uintmax_t)1) << bits) - 1);
}

//
// Write a value of a fixed width, little-endian.
//
static bool dmi_field_put_raw(dmi_field_writer_t *writer, size_t width, uintmax_t raw)
{
    dmi_byte_t data[sizeof(uintmax_t)];

    assert((width > 0) and (width <= sizeof(data)));

    for (size_t i = 0; i < width; i++)
        data[i] = (dmi_byte_t)((raw >> (i * CHAR_BIT)) & 0xFFu);

    return dmi_encoder_write(writer->encoder, data, width);
}

//
// Read a value of a fixed width the source data holds at the current
// position, little-endian.
//
static bool dmi_field_peek_raw(const dmi_field_writer_t *writer, size_t width, uintmax_t *raw)
{
    dmi_byte_t data[sizeof(uintmax_t)];

    assert(width <= sizeof(data));

    if (not dmi_encoder_peek(writer->encoder, data, width))
        return false;

    *raw = 0;

    for (size_t i = 0; i < width; i++)
        *raw |= (uintmax_t)data[i] << (i * CHAR_BIT);

    return true;
}

//
// Check whether the plain field of a member is one whose extended field the
// plain field at the given offset governs, rather than the member's own.
//
static bool dmi_field_governs(const dmi_field_t *fields, const dmi_field_t *field, size_t offset)
{
    if (field->member.offset == offset)
        return false;

    for (const dmi_field_t *other = fields; other->type != DMI_FIELD_TYPE_NONE; other++) {
        if (other->params.extended and (other->member.offset == field->member.offset) and
            (dmi_field_when_offset(other) == offset))
            return true;
    }

    return false;
}

//
// Write bytes the model does not hold: the ones of the source data in the
// preserve mode, and zeros in the canonical one.
//
static bool dmi_field_put_reserved(dmi_field_writer_t *writer, size_t length)
{
    return dmi_encoder_copy(writer->encoder, length);
}

//
// Data a field carries for the member it has been decoded into. The data the
// source has is kept whenever it decodes into the value the member holds,
// since a value may be spelled in more than one way, e.g. a size in either of
// the granules a field allows, and which one the data has used is not in the
// model. The data is exact if it decodes into the member and fits the field,
// which is how a plain field tells a value it cannot carry.
//
static bool dmi_field_value_data(
        const dmi_field_writer_t *writer,
        const dmi_field_t        *field,
        const void               *value,
        const dmi_field_data_t   *original,
        dmi_field_data_t         *data,
        bool                     *exact)
{
    *exact = true;

    // Handlers are required in pairs even when the source data is there, so
    // that a structure can be written from scratch
    if ((field->params.decode != nullptr) and (field->params.encode == nullptr))
        return dmi_field_cannot_encode(writer, "no encoding handler");

    if ((original != nullptr) and dmi_field_decodes_into(field, original, value)) {
        *data = *original;
        return true;
    }

    if (not dmi_field_model_data(writer, field, value, data))
        return false;

    *exact = dmi_field_decodes_into(field, data, value);

    // Data wider than the field is not something it can carry
    unsigned bits = (field->type == DMI_FIELD_TYPE_BITS)
                  ? field->params.bits
                  : (unsigned)(dmi_field_width(field) * CHAR_BIT);

    bool numeric = (field->type != DMI_FIELD_TYPE_STRING) and (field->type != DMI_FIELD_TYPE_BINARY);

    if (numeric and (bits > 0) and (bits < sizeof(uintmax_t) * CHAR_BIT) and ((data->number >> bits) != 0))
        *exact = false;

    return true;
}

//
// Data the specification spells the value of a member with, which undoes what
// the decoder has done: the largest value of the member stands for the value
// meaning "unknown", and the handler of the field says the rest.
//
static bool dmi_field_model_data(
        const dmi_field_writer_t *writer,
        const dmi_field_t        *field,
        const void               *value,
        dmi_field_data_t         *data)
{
    *data = (dmi_field_data_t){};

    bool scalar = (field->type != DMI_FIELD_TYPE_STRING) and (field->type != DMI_FIELD_TYPE_BINARY) and
                  ((field->member.size == sizeof(uint8_t))  or (field->member.size == sizeof(uint16_t)) or
                   (field->member.size == sizeof(uint32_t)) or (field->member.size == sizeof(uint64_t)));

    if (scalar and (field->params.unknown_raw != 0)) {
        uintmax_t largest = (field->member.size >= sizeof(uintmax_t))
                          ? UINTMAX_MAX
                          : ((((uintmax_t)1) << (field->member.size * CHAR_BIT)) - 1);

        if (dmi_field_load_member(field->member, value) == largest) {
            data->number = field->params.unknown_raw;
            return true;
        }
    }

    if (field->params.encode != nullptr) {
        if (not field->params.encode(field, value, data))
            return dmi_field_cannot_encode(writer, "value the field cannot carry");

        return true;
    }

    switch (field->type) {
    case DMI_FIELD_TYPE_STRING:
        data->string = *(const char * const *)value;
        break;

    case DMI_FIELD_TYPE_BINARY:
        data->binary = *(const dmi_binary_t *)value;
        break;

    default:
        data->number = dmi_field_load_member(field->member, value);
        break;
    }

    return true;
}

//
// Check whether data decodes into the value a member holds, by decoding it
// into a copy of the member, or of the whole structure for a split field.
//
static bool dmi_field_decodes_into(const dmi_field_t *field, const dmi_field_data_t *data, const void *value)
{
    dmi_byte_t  local[512];
    dmi_byte_t *copy = local;

    if (field->member.size > sizeof(local)) {
        copy = malloc(field->member.size);
        if (copy == nullptr)
            return false;
    }

    // Values decoded to be compared are not reported
    dmi_field_data_t probe = *data;
    probe.entity = nullptr;

    memcpy(copy, value, field->member.size);

    bool equal = dmi_field_apply(field, &probe, copy) and
                 (memcmp(copy, value, field->member.size) == 0);

    if (copy != local)
        free(copy);

    return equal;
}

//
// Data the source carries at the field being written, which only the
// preserve mode has.
//
static bool dmi_field_source_data(const dmi_field_writer_t *writer, const dmi_field_t *field, dmi_field_data_t *data)
{
    const dmi_encoder_t *encoder = writer->encoder;

    if (encoder->mode != DMI_ENCODE_MODE_PRESERVE)
        return false;

    *data = (dmi_field_data_t){};

    switch (field->type) {
    case DMI_FIELD_TYPE_BITS: {
        size_t length = (writer->bits_count + field->params.bits + CHAR_BIT - 1) / CHAR_BIT;

        if (dmi_encoder_remaining(encoder) < length)
            return false;

        data->number = dmi_field_source_bits(writer, field->params.bits);
        return true;
    }

    case DMI_FIELD_TYPE_STRING: {
        dmi_byte_t number = 0;

        if (not dmi_encoder_peek(encoder, &number, sizeof(number)))
            return false;

        // String is looked up the way the decoder does, without making it
        // referenced a second time
        const dmi_entity_t *entity = encoder->entity;

        data->number = number;

        if ((number != 0) and (number <= entity->string_count) and
            (entity->strings[number - 1].raw != nullptr))
            data->string = entity->strings[number - 1].pretty;

        return true;
    }

    case DMI_FIELD_TYPE_BINARY: {
        size_t length = field->params.length;

        if (dmi_encoder_remaining(encoder) < length)
            return false;

        data->binary = (dmi_binary_t){
            .data   = encoder->source.data + encoder->source.position,
            .length = length
        };

        return true;
    }

    default:
        return dmi_field_peek_raw(writer, dmi_field_width(field), &data->number);
    }
}

//
// Write the data a field carries: a string by the number it is referred to,
// and the rest as it is.
//
static bool dmi_field_put_data(dmi_field_writer_t *writer, const dmi_field_t *field, const dmi_field_data_t *data)
{
    switch (field->type) {
    case DMI_FIELD_TYPE_STRING: {
        // Data taken from the source refers to the string by its number
        if ((data->number != 0) or (data->string == nullptr)) {
            dmi_byte_t number = (dmi_byte_t)data->number;
            return dmi_encoder_write(writer->encoder, &number, sizeof(number));
        }

        return dmi_encoder_write_str(writer->encoder, data->string);
    }

    case DMI_FIELD_TYPE_BINARY:
        if (data->binary.length < field->params.length)
            return dmi_field_cannot_encode(writer, "binary shorter than the field");

        return dmi_encoder_write(writer->encoder, data->binary.data, field->params.length);

    default:
        return dmi_field_put_raw(writer, dmi_field_width(field), data->number);
    }
}


static uintmax_t dmi_field_load_member(dmi_member_ref_t member, const void *value)
{
    switch (member.size) {
    case sizeof(uint8_t):  return dmi_deref(uint8_t,  value);
    case sizeof(uint16_t): return dmi_deref(uint16_t, value);
    case sizeof(uint32_t): return dmi_deref(uint32_t, value);
    case sizeof(uint64_t): return dmi_deref(uint64_t, value);
    default:
        assert(false);
        return 0;
    }
}

//
// Number of the bytes a field of a fixed width occupies, and zero for the
// ones whose width the data decides.
//
static size_t dmi_field_width(const dmi_field_t *field)
{
    switch (field->type) {
    case DMI_FIELD_TYPE_INTEGER:
    case DMI_FIELD_TYPE_BCD:
    case DMI_FIELD_TYPE_STRING:
    case DMI_FIELD_TYPE_UUID:
        return field->params.length;

    case DMI_FIELD_TYPE_BINARY:
        if ((field->params.length == DMI_FIELD_LENGTH_REST) or
            (field->params.length == DMI_FIELD_LENGTH_MEMBER))
            return 0;

        return field->params.length;

    default:
        return 0;
    }
}

//
// Number of the bytes a list of fields occupies, and zero if the data decides
// it rather than the fields.
//
static size_t dmi_fields_size(const dmi_field_t *fields)
{
    size_t size = 0;
    size_t bits = 0;

    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        switch (field->type) {
        case DMI_FIELD_TYPE_GROUP:
        case DMI_FIELD_TYPE_OFFSET:
        case DMI_FIELD_TYPE_PRESET:
            break;

        case DMI_FIELD_TYPE_BITS:
            bits += field->params.bits;
            break;

        case DMI_FIELD_TYPE_PAD:
            size += (bits + field->params.bits - 1) / field->params.bits * (field->params.bits / CHAR_BIT);
            bits  = 0;
            break;

        case DMI_FIELD_TYPE_SKIP:
            size += field->params.length;
            break;

        case DMI_FIELD_TYPE_STRUCT: {
            size_t nested = dmi_fields_size(field->params.fields);
            if (nested == 0)
                return 0;
            size += nested;
            break;
        }

        default: {
            // Extended fields occupy their bytes as much as the plain ones
            size_t width = dmi_field_width(field);
            if (width == 0)
                return 0;
            size += width;
            break;
        }
        }
    }

    return size;
}

static bool dmi_field_cannot_encode(const dmi_field_writer_t *writer, const char *reason)
{
    const dmi_entity_t *entity = writer->encoder->entity;

    dmi_error_raise_ex(dmi_entity_context(entity), DMI_ERROR_INVALID_STATE,
                       "0x%04hx (%s): field at offset 0x%02zX cannot be encoded: %s",
                       dmi_entity_handle(entity), entity->spec->code,
                       dmi_encoder_tell(writer->encoder), reason);

    return false;
}

static dmi_field_choice_t *dmi_field_choice_find(
        dmi_field_choice_t *choices,
        unsigned            count,
        size_t              offset)
{
    for (unsigned i = 0; i < count; i++) {
        if (choices[i].offset == offset)
            return &choices[i];
    }

    return nullptr;
}
