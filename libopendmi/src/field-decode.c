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

#include <opendmi/field-internal.h>

/**
 * @internal
 * @brief State of a structure being decoded, which the fields are read into.
 */
typedef struct dmi_field_state
{
    dmi_decoder_t *decoder;
    dmi_entity_t  *entity;
    dmi_reader_t  *reader;
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

    // Version the entry point declares, which decides the ranges of bits
    // defined by a version, and the latest of the versions those read have
    // been defined from
    dmi_version_t version;
    dmi_version_t defined;
} dmi_field_state_t;

/**
 * @internal
 * @brief Read a list of fields into a structure, which is the decoded
 * structure itself for the fields of its specification, and an element of an
 * array or a nested structure for the fields of one.
 */
static bool dmi_field_decode_list(
        dmi_field_state_t *state,
        const dmi_field_t *fields,
        dmi_data_t        *info);

/**
 * @internal
 * @brief Read the fields of an array element or of a nested structure, which
 * the groups of the structure holding them say nothing about: an element is
 * either there in full or not at all, and the field reading it decides what
 * the data ending inside it means.
 */
static bool dmi_field_decode_nested(
        dmi_field_state_t *state,
        const dmi_field_t *fields,
        dmi_data_t        *info);

/**
 * @internal
 * @brief Read a field of a list into its member, as the kind of the field
 * says.
 *
 * @details
 * The plain field an extended one is keyed on keeps the value it has read in
 * the choice of its member, which tells whether the extended field carries the
 * real value.
 */
static bool dmi_field_decode_one(
        dmi_field_state_t   *state,
        const dmi_field_t   *field,
        dmi_data_t          *info,
        dmi_field_choices_t *choices);

/**
 * @internal
 * @brief Read the elements of a vector, which are held in place, one after
 * another, and are read the way the fields of a nested structure are.
 */
static bool dmi_field_decode_vector(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        dmi_data_t        *value);

/**
 * @internal
 * @brief Read an extended field, which replaces the value only when the plain
 * field it is keyed on says that the real value is here, and is read past
 * either way.
 */
static bool dmi_field_decode_extended(
        dmi_field_state_t        *state,
        const dmi_field_t        *field,
        void                     *value,
        const dmi_field_choice_t *choice);

/**
 * @internal
 * @brief Read a value of a fixed width into a member of the decoded structure,
 * which may be wider than the field is, so that the values standing for
 * "unknown" do not collide with the ones a device may really have.
 */
static bool dmi_field_decode_value(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        void              *value,
        uintmax_t         *raw);

/**
 * @internal
 * @brief Read a binary-coded decimal, which its own decoder spells out into
 * the number the digits stand for.
 *
 * @details
 * The value standing for "unknown" is no digits, and is taken as the data
 * holds it.
 */
static bool dmi_field_read_bcd(dmi_field_state_t *state, const dmi_field_t *field, uintmax_t *number);

/**
 * @internal
 * @brief Read bytes taken as they are: as many as the field declares, the rest
 * of the structure, or as many as a field before has declared into the member.
 */
static bool dmi_field_read_binary(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        void              *value,
        dmi_binary_t      *binary);

/**
 * @internal
 * @brief Take a range of the bits the fields of a unit share.
 *
 * @details
 * Bits are taken from the least significant one up, and a range may span the
 * bytes of the unit, which is why the bytes are read one at a time as the
 * ranges need them: the bits of a little-endian value come in the same order
 * either way.
 */
static bool dmi_field_decode_bits(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        void              *value);

/**
 * @internal
 * @brief Close a unit the ranges of bits share, stepping over the ones the
 * specification reserves at its end.
 */
static bool dmi_field_decode_pad(dmi_field_state_t *state, const dmi_field_t *field);

/**
 * @internal
 * @brief Take the next range of the bits the fields of a unit share.
 *
 * @details
 * Bytes are read as the ranges need them, so that a unit no field reaches the
 * end of is not read past.
 */
static bool dmi_field_take_bits(dmi_field_state_t *state, unsigned bits, uintmax_t *value);

/**
 * @internal
 * @brief Read an array of the elements described by their own fields, counting
 * the ones the data holds completely.
 */
static bool dmi_field_decode_array(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        dmi_data_t        *info);

/**
 * @internal
 * @brief Read the numbers an array begins with: how many elements it holds,
 * which is as many as fit into the rest of the structure when the data
 * declares no number of them, and how long an element is, which is how a
 * structure makes room for the fields a later version adds to its elements.
 *
 * @details
 * The bytes left over by elements running to the end of the structure are a
 * structure which ends in the middle of an element, which is reported once the
 * whole array has been read.
 */
static bool dmi_field_decode_array_header(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        dmi_data_t        *info,
        size_t            *count,
        size_t            *stride,
        size_t            *leftover);

/**
 * @internal
 * @brief Read an element of an array, which is there in full or not at all,
 * and step over the bytes of it the fields do not describe.
 */
static bool dmi_field_decode_element(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        dmi_data_t        *element,
        size_t             stride);

/**
 * @internal
 * @brief Read an unsigned integer of the given width, little-endian.
 */
static bool dmi_field_read_number(
        dmi_field_state_t *state,
        size_t             length,
        uintmax_t         *number);

bool dmi_fields_decode(dmi_decoder_t *decoder)
{
    if (decoder == nullptr) {
        dmi_error_raise_ex(nullptr, DMI_ERROR_NULL_ARGUMENT, "decoder");
        return false;
    }

    dmi_entity_t            *entity = dmi_decoder_entity(decoder);
    const dmi_entity_spec_t *spec   = entity->spec;

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
        .decoder = decoder,
        .entity  = entity,
        .reader  = dmi_decoder_reader(decoder),
        .info    = info,
        .version = dmi_entity_context(entity)->state.smbios_version
    };

    bool status = dmi_field_decode_list(&state, spec->fields, info);

    // Groups set the level as they are reached, so the ranges of bits defined
    // by a later version than the fields the data holds raise it afterwards
    if (state.defined > entity->level)
        entity->level = state.defined;

    return status;
}

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
    dmi_field_choices_t choices = {};
    dmi_field_choices_collect(fields, &choices);

    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        if (field->type == DMI_FIELD_TYPE_OFFSET) {
            if (not dmi_field_check_offset(state->entity, dmi_reader_tell(state->reader), field))
                return false;

            continue;
        }

        // Presets carry no data, and have been applied by the pass above
        if (field->type == DMI_FIELD_TYPE_PRESET)
            continue;

        // Groups carry no data of their own, and tell where the structure is
        // allowed to end
        if (field->type == DMI_FIELD_TYPE_GROUP) {
            if (dmi_reader_is_done(state->reader))
                return dmi_decoder_stop(state->decoder);

            // Version of the group becomes the level of the structure, while
            // the groups the specification does not version leave it as it is
            if (field->params.since != DMI_VERSION_NONE)
                state->entity->level = field->params.since;

            state->optional = true;
            continue;
        }

        // Ranges of bits the table's version does not define are left to the
        // padding of their unit, as the bits it reserves
        if (not dmi_field_is_defined(field, state->version)) {
            assert(field->type == DMI_FIELD_TYPE_BITS);
            continue;
        }

        if (not dmi_field_decode_one(state, field, info, &choices)) {
            // The data ending where the structure is allowed to be short of
            // what it declares leaves the fields after it unread, while
            // ending anywhere else breaks the structure
            return (state->optional or state->incomplete)
                    ? dmi_decoder_incomplete(state->decoder)
                    : false;
        }

        if (field->params.from > state->defined)
            state->defined = field->params.from;
    }

    return true;
}

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
        dmi_field_state_t   *state,
        const dmi_field_t   *field,
        dmi_data_t          *info,
        dmi_field_choices_t *choices)
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

    case DMI_FIELD_TYPE_VECTOR:
        return dmi_field_decode_vector(state, field, value);

    case DMI_FIELD_TYPE_STRUCT:
        return dmi_field_decode_nested(state, field->params.fields, value);

    default:
        break;
    }

    dmi_field_choice_t *kept = dmi_field_choice_find(choices, dmi_field_when_offset(field));

    if (field->params.extended)
        return dmi_field_decode_extended(state, field, value, kept);

    return dmi_field_decode_value(state, field, value, (kept != nullptr) ? &kept->raw : nullptr);
}

static bool dmi_field_decode_vector(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        dmi_data_t        *value)
{
    for (size_t i = 0; i < field->params.count; i++) {
        if (not dmi_field_decode_nested(state, field->params.fields, value + (i * field->member.size)))
            return false;
    }

    return true;
}

static bool dmi_field_decode_extended(
        dmi_field_state_t        *state,
        const dmi_field_t        *field,
        void                     *value,
        const dmi_field_choice_t *choice)
{
    uintmax_t raw = 0;

    if (not dmi_field_decode_value(state, field, nullptr, &raw))
        return false;

    if ((choice == nullptr) or (choice->raw != field->params.when_raw))
        return true;

    dmi_field_data_t data = {
        .entity = state->entity,
        .number = raw
    };

    return dmi_field_apply(field, &data, value);
}

static bool dmi_field_decode_value(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        void              *value,
        uintmax_t         *raw)
{
    assert(state != nullptr);
    assert(field != nullptr);

    dmi_field_data_t data = {
        .entity = state->entity
    };

    switch (field->type) {
    case DMI_FIELD_TYPE_SKIP:
        return dmi_reader_skip(state->reader, field->params.length);

    case DMI_FIELD_TYPE_UUID:
        return dmi_decoder_get_uuid(state->decoder, (dmi_uuid_t *)value);

    case DMI_FIELD_TYPE_STRING: {
        dmi_string_t number = 0;
        if (not dmi_decoder_get(state->decoder, dmi_string_t, &number))
            return false;
        data.number = number;
        data.string = dmi_entity_string(state->entity, number);
        break;
    }

    case DMI_FIELD_TYPE_BCD:
        if (not dmi_field_read_bcd(state, field, &data.number))
            return false;
        break;

    case DMI_FIELD_TYPE_BINARY:
        if (not dmi_field_read_binary(state, field, value, &data.binary))
            return false;
        break;

    case DMI_FIELD_TYPE_INTEGER:
        if (not dmi_field_read_number(state, field->params.length, &data.number))
            return false;
        break;

    default:
        assert(false);
        return false;
    }

    if (raw != nullptr)
        *raw = data.number;

    return dmi_field_apply(field, &data, value);
}

static bool dmi_field_read_bcd(dmi_field_state_t *state, const dmi_field_t *field, uintmax_t *number)
{
    uintmax_t raw;

    if (not dmi_field_read_number(state, field->params.length, &raw))
        return false;

    if ((field->params.unknown_raw != 0) and (raw == field->params.unknown_raw)) {
        *number = raw;
        return true;
    }

    dmi_byte_t digits[sizeof(uintmax_t)] = {};
    dmi_field_le_store(digits, field->params.length, raw);

    *number = __dmi_decode_bcd(digits, field->params.length);

    return true;
}

static bool dmi_field_read_binary(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        void              *value,
        dmi_binary_t      *binary)
{
    size_t length = field->params.length;

    if (length == DMI_FIELD_LENGTH_REST)
        length = dmi_reader_remaining(state->reader);
    else if (length == DMI_FIELD_LENGTH_MEMBER)
        length = ((const dmi_binary_t *)value)->length;

    // Bytes a field before declares the number of may not all be there, and
    // the ones which are there are kept
    if ((field->params.length == DMI_FIELD_LENGTH_MEMBER) and
        (length > dmi_reader_remaining(state->reader))) {
        dmi_field_data_t data = {
            .entity = state->entity
        };

        dmi_decoder_get_binary(state->decoder, dmi_decoder_remaining(state->decoder), &data.binary);
        dmi_field_apply(field, &data, value);

        state->incomplete = true;
        return false;
    }

    return dmi_decoder_get_binary(state->decoder, length, binary);
}

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

        if (not dmi_decoder_get(state->decoder, dmi_byte_t, &data))
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

static bool dmi_field_decode_array(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        dmi_data_t        *info)
{
    assert(state != nullptr);
    assert(field != nullptr);
    assert(info != nullptr);

    size_t count    = 0;
    size_t stride   = 0;
    size_t leftover = 0;

    if (not dmi_field_decode_array_header(state, field, info, &count, &stride, &leftover))
        return false;

    // Number of the decoded elements is counted in place, so the member
    // holding it is a size_t, the way every counter of an array is declared
    assert(field->params.counter.size == sizeof(size_t));

    size_t *counter = (size_t *)(info + field->params.counter.offset);
    *counter = 0;

    if (count == 0)
        return (leftover == 0) or dmi_decoder_incomplete(state->decoder);

    // Fields after the elements begin where the elements end either way, so
    // elements of no use are stepped over. Elements of a length the structure
    // does not carry say nothing about where the data ends, so whatever is
    // there is stepped over
    if (dmi_field_array_is_skipped(field, stride)) {
        size_t length    = count * stride;
        size_t remaining = dmi_reader_remaining(state->reader);

        return dmi_reader_skip(state->reader, (length < remaining) ? length : remaining);
    }

    dmi_context_t *context  = dmi_entity_context(state->entity);
    dmi_data_t   **elements = (dmi_data_t **)(info + field->member.offset);

    *elements = dmi_alloc_array(context, field->member.size, count);
    if (*elements == nullptr)
        return false;

    // Only the elements the data holds completely are counted, so that the
    // ones a truncated structure cuts in half are left out. A structure which
    // ends in the middle of an element is short of the elements it declares
    // rather than broken, since the ones before it are there to be read
    for (size_t i = 0; i < count; i++) {
        if (not dmi_field_decode_element(state, field, *elements + (i * field->member.size), stride))
            return false;

        (*counter)++;
    }

    return (leftover == 0) or dmi_decoder_incomplete(state->decoder);
}

static bool dmi_field_decode_array_header(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        dmi_data_t        *info,
        size_t            *count,
        size_t            *stride,
        size_t            *leftover)
{
    uintmax_t raw = 0;

    *stride   = field->params.stride;
    *leftover = 0;

    if (field->params.count_length == 0) {
        assert(*stride != 0);

        size_t remaining = dmi_reader_remaining(state->reader);

        *count    = remaining / *stride;
        *leftover = remaining % *stride;
    } else if (dmi_field_read_number(state, field->params.count_length, &raw)) {
        *count = (size_t)raw;
    } else {
        return false;
    }

    if (field->params.stride_length != 0) {
        if (not dmi_field_read_number(state, field->params.stride_length, &raw))
            return false;

        *stride = (size_t)raw;
    }

    // Numbers the data declares are worth keeping whenever the structure is
    // allowed to hold fewer elements than it says, or to make them longer
    // than the fields they are known to hold
    if (dmi_member_is_present(field->params.count_member) and
        not dmi_field_store_member(field->params.count_member,
                                   info + field->params.count_member.offset, *count))
        return false;

    if (dmi_member_is_present(field->params.stride_member) and
        not dmi_field_store_member(field->params.stride_member,
                                   info + field->params.stride_member.offset, *stride))
        return false;

    return true;
}

static bool dmi_field_decode_element(
        dmi_field_state_t *state,
        const dmi_field_t *field,
        dmi_data_t        *element,
        size_t             stride)
{
    if ((stride != 0) and not dmi_reader_has(state->reader, stride)) {
        state->incomplete = true;
        return false;
    }

    dmi_reader_mark_t start = dmi_reader_mark(state->reader);

    if (not dmi_field_decode_nested(state, field->params.fields, element)) {
        // Element is not counted, so whatever it has allocated before
        // failing is freed here rather than by the release of the array
        dmi_field_release_list(field->params.fields, element);

        // The data running out stops exactly at the end of the structure,
        // while anything else leaves the element unread for its own reason
        state->incomplete = dmi_reader_is_done(state->reader);
        return false;
    }

    if ((stride != 0) and not dmi_reader_skip_ex(state->reader, start, stride)) {
        state->incomplete = true;
        return false;
    }

    return true;
}

static bool dmi_field_read_number(
        dmi_field_state_t *state,
        size_t             length,
        uintmax_t         *number)
{
    assert(state != nullptr);
    assert(number != nullptr);
    assert((length > 0) and (length <= sizeof(uintmax_t)));

    dmi_byte_t data[sizeof(uintmax_t)];

    if (not dmi_reader_get_bytes(state->reader, data, length))
        return false;

    *number = dmi_field_le_load(data, length);

    return true;
}
