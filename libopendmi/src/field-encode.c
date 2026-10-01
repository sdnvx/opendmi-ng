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
 * @brief Size of a member a value is decoded into on the stack to be compared
 * with the one the model holds; larger members, e.g. whole structures a split
 * field decodes into, are copied to the heap.
 */
#define DMI_FIELD_PROBE_SIZE 512

/**
 * @internal
 * @brief State of a structure being encoded, which mirrors the one of
 * decoding: the ranges of bits sharing a unit are gathered before the bytes of
 * the unit are written.
 */
typedef struct dmi_field_output
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

    // Version deciding the ranges of bits defined by a version: the one the
    // source data has been decoded for in the preserve mode, and the one
    // written for in the canonical one
    dmi_version_t version;
} dmi_field_output_t;

/**
 * @internal
 * @brief Whether the model holds the whole of a field, which is then written
 * from the member alone, with nothing of the source data to keep.
 */
static inline bool dmi_field_is_fixed(const dmi_field_t *field)
{
    switch (field->type) {
    case DMI_FIELD_TYPE_UUID:
    case DMI_FIELD_TYPE_BCD:
        return true;

    case DMI_FIELD_TYPE_BINARY:
        return (field->params.length == DMI_FIELD_LENGTH_REST) or
               (field->params.length == DMI_FIELD_LENGTH_MEMBER);

    default:
        return false;
    }
}

/**
 * @internal
 * @brief Write a list of fields from a structure, which is the decoded
 * structure itself for the fields of its specification, and an element of an
 * array or a nested structure for the fields of one.
 */
static bool dmi_field_encode_list(
        dmi_field_output_t *output,
        const dmi_field_t  *fields,
        const dmi_data_t   *info);

/**
 * @internal
 * @brief Write a field of a list from its member, as the kind of the field
 * says.
 *
 * @details
 * Extended fields and the plain fields governing them are written according to
 * the choices of the list, which the plain fields make as they are written.
 */
static bool dmi_field_encode_one(
        dmi_field_output_t  *output,
        const dmi_field_t   *fields,
        const dmi_field_t   *field,
        const dmi_data_t    *info,
        dmi_field_choices_t *choices);

/**
 * @internal
 * @brief Write the elements of a vector, which are held in place, one after
 * another.
 */
static bool dmi_field_encode_vector(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const dmi_data_t   *value);

/**
 * @internal
 * @brief Write a field the model holds the whole of: a UUID, bytes running to
 * the end of the structure or as many as a field before declares, which are
 * the member's own whichever way it is written, and a binary-coded decimal.
 */
static bool dmi_field_encode_fixed(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const void         *value,
        size_t              width);

/**
 * @internal
 * @brief Write a number as binary-coded decimal digits.
 *
 * @details
 * Unknown is the largest value of the member, and is written back as the value
 * the data stands for it with rather than as digits.
 */
static bool dmi_field_encode_bcd(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const void         *value,
        size_t              width);

/**
 * @internal
 * @brief Write a plain field governing extended ones, which chooses the
 * representation: the one the source data has in the preserve mode, unless the
 * value no longer fits, and the plain field whenever the value fits otherwise.
 */
static bool dmi_field_encode_governing(
        dmi_field_output_t     *output,
        const dmi_field_t      *fields,
        const dmi_field_t      *field,
        const dmi_data_t       *info,
        dmi_field_choice_t     *choice,
        const dmi_field_data_t *original,
        dmi_field_data_t       *data,
        bool                    exact);

/**
 * @internal
 * @brief Check whether every other member the plain field at the offset
 * governs fits its own plain field, since the members of a choice are widened
 * together.
 */
static bool dmi_field_governed_fit(
        const dmi_field_output_t *output,
        const dmi_field_t        *fields,
        const dmi_field_t        *field,
        const dmi_data_t         *info,
        size_t                    offset,
        bool                     *fits);

/**
 * @internal
 * @brief Write a plain field which governs nothing.
 *
 * @details
 * The plain field of a member whose extended field another member governs
 * holds what the source data had once the extended one has replaced it.
 */
static bool dmi_field_encode_plain(
        dmi_field_output_t  *output,
        const dmi_field_t   *fields,
        const dmi_field_t   *field,
        dmi_field_choices_t *choices,
        size_t               width,
        dmi_field_data_t    *data);

/**
 * @internal
 * @brief Write an array of the elements described by their own fields, as many
 * as have been decoded, preceded by the numbers the array begins with.
 */
static bool dmi_field_encode_array(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const dmi_data_t   *info);

/**
 * @internal
 * @brief Write the numbers an array begins with.
 *
 * @details
 * The number of the elements the data declares is greater than the number of
 * the decoded ones when the source data ends before the last element does, or
 * than none when the elements have been stepped over. The length of an element
 * is written from the member holding it, or taken from the source data, or
 * counted from the fields otherwise.
 */
static bool dmi_field_encode_array_header(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const dmi_data_t   *info,
        size_t              counter,
        uintmax_t          *count,
        size_t             *stride);

/**
 * @internal
 * @brief Write an element of an array.
 *
 * @details
 * Elements longer than the fields they are known to hold keep the rest of
 * their bytes.
 */
static bool dmi_field_encode_element(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const dmi_data_t   *element,
        size_t              stride);

/**
 * @internal
 * @brief Write a range of the bits the fields of a unit share.
 *
 * @details
 * Bits reserved between the ranges hold what the source data had in the
 * preserve mode, and zeros in the canonical one.
 */
static bool dmi_field_encode_bits(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const void         *value);

/**
 * @internal
 * @brief Close a unit the ranges of bits share, writing the bits the
 * specification reserves at its end: the ones of the source data in the
 * preserve mode, and zeros in the canonical one.
 */
static bool dmi_field_encode_pad(dmi_field_output_t *output, const dmi_field_t *field);

/**
 * @internal
 * @brief Gather a range of bits, and write the bytes of the unit as they fill
 * up, which is the order the decoder takes them in.
 */
static bool dmi_field_put_bits(dmi_field_output_t *output, uintmax_t raw, unsigned bits);

/**
 * @internal
 * @brief Take the bits the source data holds at the position of the next
 * range, which starts in the byte not written yet.
 */
static uintmax_t dmi_field_source_bits(const dmi_field_output_t *output, unsigned bits);

/**
 * @internal
 * @brief Write a value of a fixed width, little-endian.
 */
static bool dmi_field_put_raw(dmi_field_output_t *output, size_t width, uintmax_t raw);

/**
 * @internal
 * @brief Read a value of a fixed width the source data holds at the current
 * position, little-endian.
 */
static bool dmi_field_peek_raw(const dmi_field_output_t *output, size_t width, uintmax_t *raw);

/**
 * @internal
 * @brief Check whether the plain field of a member is one whose extended field
 * the plain field at the given offset governs, rather than the member's own.
 */
static bool dmi_field_governs(const dmi_field_t *fields, const dmi_field_t *field, size_t offset);

/**
 * @internal
 * @brief Write bytes the model does not hold: the ones of the source data in
 * the preserve mode, and zeros in the canonical one.
 */
static bool dmi_field_put_reserved(dmi_field_output_t *output, size_t length);

/**
 * @internal
 * @brief Data a field carries for the member it has been decoded into.
 *
 * @details
 * The data the source has is kept whenever it decodes into the value the
 * member holds, since a value may be spelled in more than one way, e.g. a size
 * in either of the granules a field allows, and which one the data has used is
 * not in the model. The data is exact if it decodes into the member and fits
 * the field, which is how a plain field tells a value it cannot carry.
 */
static bool dmi_field_value_data(
        const dmi_field_output_t *output,
        const dmi_field_t        *field,
        const void               *value,
        const dmi_field_data_t   *original,
        dmi_field_data_t         *data,
        bool                     *exact);

/**
 * @internal
 * @brief Data the specification spells the value of a member with, which
 * undoes what the decoder has done: the largest value of the member stands for
 * the value meaning "unknown", and the handler of the field says the rest.
 */
static bool dmi_field_model_data(
        const dmi_field_output_t *output,
        const dmi_field_t        *field,
        const void               *value,
        dmi_field_data_t         *data);

/**
 * @internal
 * @brief Check whether data decodes into the value a member holds, by decoding
 * it into a copy of the member, or of the whole structure for a split field.
 *
 * @details
 * The answer is stored into equal, and false is returned only when the copy
 * cannot be made, which is an error rather than a value of another spelling.
 */
static bool dmi_field_decodes_into(
        const dmi_field_output_t *output,
        const dmi_field_t        *field,
        const dmi_field_data_t   *data,
        const void               *value,
        bool                     *equal);

/**
 * @internal
 * @brief Data the source carries at the field being written, which only the
 * preserve mode has.
 */
static bool dmi_field_source_data(const dmi_field_output_t *output, const dmi_field_t *field, dmi_field_data_t *data);

/**
 * @internal
 * @brief Write the data a field carries: a string by the number it is referred
 * to, and the rest as it is.
 */
static bool dmi_field_put_data(dmi_field_output_t *output, const dmi_field_t *field, const dmi_field_data_t *data);

/**
 * @internal
 * @brief Number of the bytes a field of a fixed width occupies, and zero for
 * the ones whose width the data decides.
 */
static size_t dmi_field_width(const dmi_field_t *field);

/**
 * @internal
 * @brief Number of the bytes a list of fields occupies, and zero if the data
 * decides it rather than the fields.
 */
static size_t dmi_fields_size(const dmi_field_t *fields);

/**
 * @internal
 * @brief Report a field which cannot be encoded, naming the structure and the
 * offset it is written at.
 *
 * @return Always `false`, so that the caller can return what it returns.
 */
static bool dmi_field_cannot_encode(const dmi_field_output_t *output, const char *reason);

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

    dmi_field_output_t output = {
        .encoder = encoder,
        .version = (encoder->mode == DMI_ENCODE_MODE_PRESERVE)
                 ? dmi_entity_context(entity)->state.smbios_version
                 : encoder->version
    };

    if (not dmi_field_encode_list(&output, spec->fields, entity->info))
        return false;

    // Bytes after the ones the fields describe are the structure's own, and
    // are kept as they are
    size_t remaining = dmi_encoder_remaining(encoder);

    if ((remaining > 0) and not dmi_field_put_reserved(&output, remaining))
        return false;

    return dmi_encoder_finish(encoder);
}

static bool dmi_field_encode_list(
        dmi_field_output_t *output,
        const dmi_field_t  *fields,
        const dmi_data_t   *info)
{
    assert(output != nullptr);
    assert(fields != nullptr);
    assert(info != nullptr);

    dmi_encoder_t *encoder = output->encoder;

    // Members an extended field may carry the value of, whose representation
    // the plain field governing them decides
    dmi_field_choices_t choices = {};
    dmi_field_choices_collect(fields, &choices);

    for (const dmi_field_t *field = fields; field->type != DMI_FIELD_TYPE_NONE; field++) {
        if (output->stopped)
            return true;

        if (field->type == DMI_FIELD_TYPE_OFFSET) {
            if (not dmi_field_check_offset(encoder->entity, dmi_encoder_tell(encoder), field))
                return false;

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
                output->stopped = true;

            continue;
        }

        // Ranges of bits the version does not define are written by the
        // padding of their unit, as the bits it reserves
        if (not dmi_field_is_defined(field, output->version)) {
            assert(field->type == DMI_FIELD_TYPE_BITS);
            continue;
        }

        if (not dmi_field_encode_one(output, fields, field, info, &choices))
            return false;
    }

    return true;
}

static bool dmi_field_encode_one(
        dmi_field_output_t  *output,
        const dmi_field_t   *fields,
        const dmi_field_t   *field,
        const dmi_data_t    *info,
        dmi_field_choices_t *choices)
{
    assert(output != nullptr);
    assert(field != nullptr);

    dmi_encoder_t *encoder = output->encoder;

    const void *value = dmi_member_is_present(field->member)
            ? (info + field->member.offset)
            : nullptr;

    assert((field->type == DMI_FIELD_TYPE_BITS) or
           (field->type == DMI_FIELD_TYPE_PAD) or
           (output->bits_count == 0));

    switch (field->type) {
    case DMI_FIELD_TYPE_BITS:
        return dmi_field_encode_bits(output, field, value);

    case DMI_FIELD_TYPE_PAD:
        return dmi_field_encode_pad(output, field);

    case DMI_FIELD_TYPE_ARRAY:
        return dmi_field_encode_array(output, field, info);

    case DMI_FIELD_TYPE_VECTOR:
        return dmi_field_encode_vector(output, field, value);

    case DMI_FIELD_TYPE_STRUCT:
        return dmi_field_encode_list(output, field->params.fields, value);

    case DMI_FIELD_TYPE_SKIP:
        return dmi_field_put_reserved(output, field->params.length);

    default:
        break;
    }

    // Source data which ends in the middle of the field is kept as it is,
    // since the model holds nothing of a field it has not read in full
    size_t width = dmi_field_width(field);

    if (encoder->mode == DMI_ENCODE_MODE_PRESERVE) {
        size_t remaining = dmi_encoder_remaining(encoder);

        if (remaining < width) {
            output->stopped = true;
            return dmi_field_put_reserved(output, remaining);
        }
    }

    if (dmi_field_is_fixed(field))
        return dmi_field_encode_fixed(output, field, value, width);

    dmi_field_data_t original = {};
    dmi_field_data_t data     = {};
    bool             exact    = true;

    const dmi_field_data_t *source =
            dmi_field_source_data(output, field, &original) ? &original : nullptr;

    // Extended field carries the value only when the plain one governing it
    // has chosen so, and holds what the source data had otherwise
    dmi_field_choice_t *choice = dmi_field_choice_find(choices, dmi_field_when_offset(field));

    if (field->params.extended and ((choice == nullptr) or not choice->extended))
        return dmi_field_put_reserved(output, width);

    if (not dmi_field_value_data(output, field, value, source, &data, &exact))
        return false;

    if (field->params.extended)
        return dmi_field_put_data(output, field, &data);

    if (choice != nullptr)
        return dmi_field_encode_governing(output, fields, field, info, choice, source, &data, exact);

    return dmi_field_encode_plain(output, fields, field, choices, width, &data);
}

static bool dmi_field_encode_vector(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const dmi_data_t   *value)
{
    for (size_t i = 0; i < field->params.count; i++) {
        if (not dmi_field_encode_list(output, field->params.fields, value + (i * field->member.size)))
            return false;
    }

    return true;
}

static bool dmi_field_encode_fixed(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const void         *value,
        size_t              width)
{
    switch (field->type) {
    case DMI_FIELD_TYPE_UUID: {
        dmi_byte_t data[16];
        dmi_uuid_encode(dmi_deref(dmi_uuid_t, value), data);
        return dmi_encoder_put_bytes(output->encoder, data, sizeof(data));
    }

    case DMI_FIELD_TYPE_BINARY: {
        const dmi_binary_t *binary = value;
        return dmi_encoder_put_bytes(output->encoder, binary->data, binary->length);
    }

    case DMI_FIELD_TYPE_BCD:
        return dmi_field_encode_bcd(output, field, value, width);

    default:
        assert(false);
        return false;
    }
}

static bool dmi_field_encode_bcd(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const void         *value,
        size_t              width)
{
    uintmax_t number = dmi_field_load_member(field->member, value);
    uintmax_t digits = 0;

    if ((field->params.unknown_raw != 0) and (number == dmi_field_member_max(field->member)))
        return dmi_field_put_raw(output, width, field->params.unknown_raw);

    for (unsigned shift = 0; shift < width * CHAR_BIT; shift += 4, number /= 10)
        digits |= (number % 10) << shift;

    // Digits the field has no room for are not dropped silently
    if (number != 0)
        return dmi_field_cannot_encode(output, "value the field cannot carry");

    return dmi_field_put_raw(output, width, digits);
}

static bool dmi_field_encode_governing(
        dmi_field_output_t     *output,
        const dmi_field_t      *fields,
        const dmi_field_t      *field,
        const dmi_data_t       *info,
        dmi_field_choice_t     *choice,
        const dmi_field_data_t *original,
        dmi_field_data_t       *data,
        bool                    exact)
{
    bool fits = exact and (data->number != choice->when_raw);

    if (fits and not dmi_field_governed_fit(output, fields, field, info, choice->offset, &fits))
        return false;

    choice->extended = not fits or ((original != nullptr) and (original->number == choice->when_raw));

    if (choice->extended)
        data->number = choice->when_raw;

    return dmi_field_put_data(output, field, data);
}

static bool dmi_field_governed_fit(
        const dmi_field_output_t *output,
        const dmi_field_t        *fields,
        const dmi_field_t        *field,
        const dmi_data_t         *info,
        size_t                    offset,
        bool                     *fits)
{
    for (const dmi_field_t *other = fields; *fits and (other->type != DMI_FIELD_TYPE_NONE); other++) {
        if ((other == field) or other->params.extended or not dmi_field_governs(fields, other, offset))
            continue;

        dmi_field_data_t data = {};

        if (not dmi_field_value_data(output, other, info + other->member.offset, nullptr, &data, fits))
            return false;
    }

    return true;
}

static bool dmi_field_encode_plain(
        dmi_field_output_t  *output,
        const dmi_field_t   *fields,
        const dmi_field_t   *field,
        dmi_field_choices_t *choices,
        size_t               width,
        dmi_field_data_t    *data)
{
    for (const dmi_field_t *other = fields; other->type != DMI_FIELD_TYPE_NONE; other++) {
        if (not other->params.extended or (other->member.offset != field->member.offset))
            continue;

        const dmi_field_choice_t *governing = dmi_field_choice_find(choices, dmi_field_when_offset(other));

        if ((governing != nullptr) and governing->extended) {
            if (output->encoder->mode == DMI_ENCODE_MODE_PRESERVE)
                return dmi_field_put_reserved(output, width);

            data->number = governing->when_raw;
        }

        break;
    }

    return dmi_field_put_data(output, field, data);
}

static bool dmi_field_encode_array(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const dmi_data_t   *info)
{
    assert(output != nullptr);
    assert(field != nullptr);

    const dmi_encoder_t *encoder = output->encoder;

    size_t counter = dmi_deref(size_t, info + field->params.counter.offset);

    const dmi_data_t *elements = dmi_deref(dmi_data_t *, info + field->member.offset);

    // Source data which ends in the middle of the numbers the array begins
    // with is kept as it is, e.g. a number of no elements with no length of
    // an element after it
    if ((encoder->mode == DMI_ENCODE_MODE_PRESERVE) and
        (dmi_encoder_remaining(encoder) < field->params.count_length + field->params.stride_length)) {
        output->stopped = true;
        return dmi_field_put_reserved(output, dmi_encoder_remaining(encoder));
    }

    uintmax_t count  = counter;
    size_t    stride = field->params.stride;

    if (not dmi_field_encode_array_header(output, field, info, counter, &count, &stride))
        return false;

    // Elements too short for the fields they are known to hold have not been
    // decoded, and are kept as they are. None of them is counted then, so
    // the bytes they take are the ones of as many elements as the data
    // declares, lest the fields after them be misplaced
    if (dmi_field_array_is_skipped(field, stride)) {
        size_t remaining = dmi_encoder_remaining(encoder);
        size_t length    = ((stride != 0) and (count > remaining / stride))
                         ? remaining
                         : (size_t)count * stride;

        return dmi_field_put_reserved(output, length);
    }

    for (size_t i = 0; (i < counter) and not output->stopped; i++) {
        if (not dmi_field_encode_element(output, field, elements + (i * field->member.size), stride))
            return false;
    }

    return true;
}

static bool dmi_field_encode_array_header(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const dmi_data_t   *info,
        size_t              counter,
        uintmax_t          *count,
        size_t             *stride)
{
    bool preserve = (output->encoder->mode == DMI_ENCODE_MODE_PRESERVE);

    if (field->params.count_length != 0) {
        uintmax_t original = 0;

        if (dmi_member_is_present(field->params.count_member)) {
            *count = dmi_field_load_member(field->params.count_member,
                                           info + field->params.count_member.offset);
        } else if (preserve and dmi_field_peek_raw(output, field->params.count_length, &original) and
                   (original > counter)) {
            *count = original;
        }

        if (not dmi_field_put_raw(output, field->params.count_length, *count))
            return false;
    }

    if (field->params.stride_length != 0) {
        uintmax_t length   = dmi_fields_size(field->params.fields);
        uintmax_t original = 0;

        if (dmi_member_is_present(field->params.stride_member)) {
            length = dmi_field_load_member(field->params.stride_member,
                                           info + field->params.stride_member.offset);
        } else if (preserve and dmi_field_peek_raw(output, field->params.stride_length, &original)) {
            length = original;
        }

        if (not dmi_field_put_raw(output, field->params.stride_length, length))
            return false;

        *stride = (size_t)length;
    }

    return true;
}

static bool dmi_field_encode_element(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const dmi_data_t   *element,
        size_t              stride)
{
    size_t start = dmi_encoder_tell(output->encoder);

    if (not dmi_field_encode_list(output, field->params.fields, element))
        return false;

    if (output->stopped)
        return true;

    size_t written = dmi_encoder_tell(output->encoder) - start;

    if ((stride > written) and not dmi_field_put_reserved(output, stride - written))
        return false;

    return true;
}

static bool dmi_field_encode_bits(
        dmi_field_output_t *output,
        const dmi_field_t  *field,
        const void         *value)
{
    assert(output != nullptr);
    assert(field != nullptr);

    unsigned bits = field->params.bits;

    // Bits reserved between the ranges hold what the source data had
    if (value == nullptr) {
        uintmax_t raw = 0;

        if (output->encoder->mode == DMI_ENCODE_MODE_PRESERVE)
            raw = dmi_field_source_bits(output, bits);

        return dmi_field_put_bits(output, raw, bits);
    }

    dmi_field_data_t original = {};
    dmi_field_data_t data     = {};
    bool             exact    = true;

    bool from_source = dmi_field_source_data(output, field, &original);

    if (not dmi_field_value_data(output, field, value, from_source ? &original : nullptr,
                                 &data, &exact))
        return false;

    return dmi_field_put_bits(output, data.number, bits);
}

static bool dmi_field_encode_pad(dmi_field_output_t *output, const dmi_field_t *field)
{
    assert(output != nullptr);
    assert(field != nullptr);

    unsigned unit  = field->params.bits;
    unsigned taken = output->bits_taken % unit;

    if (taken != 0) {
        unsigned  bits = unit - taken;
        uintmax_t raw  = 0;

        if (output->encoder->mode == DMI_ENCODE_MODE_PRESERVE)
            raw = dmi_field_source_bits(output, bits);

        if (not dmi_field_put_bits(output, raw, bits))
            return false;
    }

    assert(output->bits_count == 0);

    output->bits_taken = 0;

    return true;
}

static bool dmi_field_put_bits(dmi_field_output_t *output, uintmax_t raw, unsigned bits)
{
    assert((bits > 0) and (bits <= (sizeof(uintmax_t) * CHAR_BIT) - CHAR_BIT));

    output->bits_value |= (raw & ((((uintmax_t)1) << bits) - 1)) << output->bits_count;
    output->bits_count += bits;
    output->bits_taken += bits;

    while (output->bits_count >= CHAR_BIT) {
        dmi_byte_t data = (dmi_byte_t)(output->bits_value & 0xFFu);

        if (not dmi_encoder_put_bytes(output->encoder, &data, sizeof(data)))
            return false;

        output->bits_value >>= CHAR_BIT;
        output->bits_count  -= CHAR_BIT;
    }

    return true;
}

static uintmax_t dmi_field_source_bits(const dmi_field_output_t *output, unsigned bits)
{
    size_t     length = (output->bits_count + bits + CHAR_BIT - 1) / CHAR_BIT;
    dmi_byte_t data[sizeof(uintmax_t)] = {};

    if ((length > sizeof(data)) or not dmi_encoder_peek(output->encoder, data, length))
        return 0;

    uintmax_t value = dmi_field_le_load(data, length);

    return (value >> output->bits_count) & ((((uintmax_t)1) << bits) - 1);
}

static bool dmi_field_put_raw(dmi_field_output_t *output, size_t width, uintmax_t raw)
{
    dmi_byte_t data[sizeof(uintmax_t)];

    assert((width > 0) and (width <= sizeof(data)));

    dmi_field_le_store(data, width, raw);

    return dmi_encoder_put_bytes(output->encoder, data, width);
}

static bool dmi_field_peek_raw(const dmi_field_output_t *output, size_t width, uintmax_t *raw)
{
    dmi_byte_t data[sizeof(uintmax_t)];

    assert(width <= sizeof(data));

    if (not dmi_encoder_peek(output->encoder, data, width))
        return false;

    *raw = dmi_field_le_load(data, width);

    return true;
}

static bool dmi_field_governs(
        const dmi_field_t *fields,
        const dmi_field_t *field,
        size_t             offset)
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

static bool dmi_field_put_reserved(dmi_field_output_t *output, size_t length)
{
    return dmi_encoder_copy(output->encoder, length);
}

static bool dmi_field_value_data(
        const dmi_field_output_t *output,
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
        return dmi_field_cannot_encode(output, "no encoding handler");

    if (original != nullptr) {
        bool equal = false;

        if (not dmi_field_decodes_into(output, field, original, value, &equal))
            return false;

        if (equal) {
            *data = *original;
            return true;
        }
    }

    if (not dmi_field_model_data(output, field, value, data))
        return false;

    if (not dmi_field_decodes_into(output, field, data, value, exact))
        return false;

    // Data wider than the field is not something it can carry
    unsigned bits = (field->type == DMI_FIELD_TYPE_BITS)
                  ? field->params.bits
                  : (unsigned)(dmi_field_width(field) * CHAR_BIT);

    if (dmi_field_is_numeric(field) and (bits > 0) and (bits < sizeof(uintmax_t) * CHAR_BIT) and ((data->number >> bits) != 0))
        *exact = false;

    return true;
}

static bool dmi_field_model_data(
        const dmi_field_output_t *output,
        const dmi_field_t        *field,
        const void               *value,
        dmi_field_data_t         *data)
{
    *data = (dmi_field_data_t){};

    bool scalar = dmi_field_is_numeric(field) and
                  ((field->member.size == sizeof(uint8_t))  or (field->member.size == sizeof(uint16_t)) or
                   (field->member.size == sizeof(uint32_t)) or (field->member.size == sizeof(uint64_t)));

    if (scalar and (field->params.unknown_raw != 0)) {
        if (dmi_field_load_member(field->member, value) == dmi_field_member_max(field->member)) {
            data->number = field->params.unknown_raw;
            return true;
        }
    }

    if (field->params.encode != nullptr) {
        if (not field->params.encode(field, value, data))
            return dmi_field_cannot_encode(output, "value the field cannot carry");

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

static bool dmi_field_decodes_into(
        const dmi_field_output_t *output,
        const dmi_field_t        *field,
        const dmi_field_data_t   *data,
        const void               *value,
        bool                     *equal)
{
    dmi_byte_t  local[DMI_FIELD_PROBE_SIZE];
    dmi_byte_t *copy = local;

    *equal = false;

    if (field->member.size > sizeof(local)) {
        copy = dmi_alloc(dmi_entity_context(output->encoder->entity), field->member.size);
        if (copy == nullptr)
            return false;
    }

    // Values decoded to be compared are not reported
    dmi_field_data_t probe = *data;
    probe.entity = nullptr;

    memcpy(copy, value, field->member.size);

    *equal = dmi_field_apply(field, &probe, copy) and
             (memcmp(copy, value, field->member.size) == 0);

    if (copy != local)
        dmi_free(copy);

    return true;
}

static bool dmi_field_source_data(
        const dmi_field_output_t *output,
        const dmi_field_t        *field,
        dmi_field_data_t         *data)
{
    const dmi_encoder_t *encoder = output->encoder;

    if (encoder->mode != DMI_ENCODE_MODE_PRESERVE)
        return false;

    *data = (dmi_field_data_t){};

    switch (field->type) {
    case DMI_FIELD_TYPE_BITS: {
        size_t length = (output->bits_count + field->params.bits + CHAR_BIT - 1) / CHAR_BIT;

        if (dmi_encoder_remaining(encoder) < length)
            return false;

        data->number = dmi_field_source_bits(output, field->params.bits);
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

        // Source data is referred to in place, the way the decoder refers to
        // the bytes of a binary field
        data->binary = (dmi_binary_t){
            .data   = dmi_buffer_at(encoder->source.buffer,
                                    encoder->source.base + encoder->source.position, length),
            .length = length
        };

        if (data->binary.data == nullptr)
            return false;

        return true;
    }

    default:
        return dmi_field_peek_raw(output, dmi_field_width(field), &data->number);
    }
}

static bool dmi_field_put_data(
        dmi_field_output_t     *output,
        const dmi_field_t      *field,
        const dmi_field_data_t *data)
{
    switch (field->type) {
    case DMI_FIELD_TYPE_STRING: {
        // Data taken from the source refers to the string by its number
        if ((data->number != 0) or (data->string == nullptr)) {
            dmi_byte_t number = (dmi_byte_t)data->number;
            return dmi_encoder_put_bytes(output->encoder, &number, sizeof(number));
        }

        return dmi_encoder_put_string(output->encoder, data->string);
    }

    case DMI_FIELD_TYPE_BINARY:
        // Field the source data ends before holds no bytes, and is written as
        // zeroes, the way a structure of a later version is filled in
        if (data->binary.length == 0)
            return dmi_field_put_reserved(output, field->params.length);

        if (data->binary.length < field->params.length)
            return dmi_field_cannot_encode(output, "binary shorter than the field");

        return dmi_encoder_put_bytes(output->encoder, data->binary.data, field->params.length);

    default:
        return dmi_field_put_raw(output, dmi_field_width(field), data->number);
    }
}

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

        // Ranges of bits take as many whole units as they fill
        case DMI_FIELD_TYPE_PAD: {
            size_t unit = field->params.bits;

            size += (bits + unit - 1) / unit * (unit / CHAR_BIT);
            bits  = 0;
            break;
        }

        case DMI_FIELD_TYPE_SKIP:
            size += field->params.length;
            break;

        // Structure is a vector of a single element
        case DMI_FIELD_TYPE_STRUCT:
        case DMI_FIELD_TYPE_VECTOR: {
            size_t nested = dmi_fields_size(field->params.fields);
            if (nested == 0)
                return 0;
            size += nested * ((field->type == DMI_FIELD_TYPE_VECTOR) ? field->params.count : 1);
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

static bool dmi_field_cannot_encode(const dmi_field_output_t *output, const char *reason)
{
    const dmi_entity_t *entity = output->encoder->entity;

    dmi_error_raise_ex(dmi_entity_context(entity), DMI_ERROR_INVALID_STATE,
                       "0x%04hx (%s): field at offset 0x%02zX cannot be encoded: %s",
                       dmi_entity_handle(entity), entity->spec->code,
                       dmi_encoder_tell(output->encoder), reason);

    return false;
}
