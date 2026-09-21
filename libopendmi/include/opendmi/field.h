//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FIELD_H
#define OPENDMI_FIELD_H

#pragma once

#include <opendmi/types.h>
#include <opendmi/encoder.h>
#include <opendmi/stream.h>
#include <opendmi/utils/version.h>

#ifndef DMI_FIELD_T
#   define DMI_FIELD_T
    typedef struct dmi_field dmi_field_t;
#endif // !DMI_FIELD_T

typedef struct dmi_field_params dmi_field_params_t;

/**
 * @brief Layout of a field on the wire.
 *
 * Field types name the bytes a field occupies in the SMBIOS data, which is
 * not the same as the type of the member it is decoded into: a width of the
 * `DMI_FIELD_TYPE_WORD` may be decoded into a wider member, so that the value
 * standing for "unknown" does not collide with a value a device may really
 * have.
 */
typedef enum dmi_field_type
{
    /**
     * @brief No field, which terminates a list of them.
     */
    DMI_FIELD_TYPE_NONE,

    DMI_FIELD_TYPE_BYTE,  ///< One byte
    DMI_FIELD_TYPE_WORD,  ///< Two bytes, little-endian
    DMI_FIELD_TYPE_DWORD, ///< Four bytes, little-endian
    DMI_FIELD_TYPE_QWORD, ///< Eight bytes, little-endian

    /**
     * @brief String reference, decoded into a pointer to the string.
     */
    DMI_FIELD_TYPE_STRING,

    /**
     * @brief Binary-coded decimal of the width the field declares, decoded
     * into the number it spells.
     */
    DMI_FIELD_TYPE_BCD,

    /**
     * @brief Bytes taken as they are, of the length the field declares,
     * decoded into a `dmi_binary_t` referring to them in place.
     */
    DMI_FIELD_TYPE_BINARY,

    /**
     * @brief Sixteen bytes holding a UUID, decoded into a `dmi_uuid_t` with
     * the fields put into the order RFC 4122 gives them.
     */
    DMI_FIELD_TYPE_UUID,

    /**
     * @brief Structure, described by the fields of `dmi_field_params_t`.
     */
    DMI_FIELD_TYPE_STRUCT,

    /**
     * @brief Array of the elements described by the fields of
     * `dmi_field_params_t`, preceded by the number of them.
     */
    DMI_FIELD_TYPE_ARRAY,


    /**
     * @brief Range of the bits of a byte, a word or a wider unit, which
     * several fields of a structure share.
     *
     * Ranges are taken from the least significant bit up, and a range is
     * allowed to span the bytes of the unit holding it, the way the bit
     * fields of a little-endian value do.
     */
    DMI_FIELD_TYPE_BITS,

    /**
     * @brief Bits left over at the end of a unit the ranges of bits share,
     * which the specification reserves and the data leaves set to zero.
     */
    DMI_FIELD_TYPE_PAD,

    /**
     * @brief Beginning of a group of the fields the data may end before,
     * which occupies no bytes of its own.
     */
    DMI_FIELD_TYPE_GROUP,

    /**
     * @brief Offset the fields after it begin at, which occupies no bytes of
     * its own and says nothing about where the data may end.
     */
    DMI_FIELD_TYPE_OFFSET,

    /**
     * @brief Member the data does not carry, which is set before anything is
     * read and occupies no bytes of its own.
     */
    DMI_FIELD_TYPE_PRESET,

    /**
     * @brief Bytes which are stepped over, because the structure holds them
     * and nothing reads them.
     */
    DMI_FIELD_TYPE_SKIP
} dmi_field_type_t;

/**
 * @brief Widths of the units the ranges of bits share, in bits.
 */
#define DMI_FIELD_UNIT_BYTE   8
#define DMI_FIELD_UNIT_WORD  16
#define DMI_FIELD_UNIT_DWORD 32
#define DMI_FIELD_UNIT_QWORD 64

/**
 * @brief Data a field carries, as the engine reads and writes it.
 *
 * Handlers of a field deal with values rather than with the bytes of the
 * structure: the engine reads the bytes the field occupies, resolves the
 * string a string field refers to, and writes them back, so that a handler
 * says only what the value means for the decoded structure.
 */
typedef struct dmi_field_data
{
    /**
     * @brief Structure the field belongs to, which a decoding handler reports
     * the values it cannot make sense of against, or @c nullptr when the
     * value is decoded to be compared rather than kept, and nothing is to be
     * reported.
     */
    const dmi_entity_t *entity;

    /**
     * @brief Value of an integer field or of a range of bits.
     */
    uintmax_t number;

    /**
     * @brief String a string field refers to, or @c nullptr if it refers to
     * none.
     */
    const char *string;

    /**
     * @brief Bytes of a binary field.
     */
    dmi_binary_t binary;

    /**
     * @brief Storage for the text or the bytes an encoding handler makes up,
     * e.g. a date, which `string` or `binary` points at.
     */
    dmi_byte_t buffer[32];
} dmi_field_data_t;

/**
 * @brief Decode the data a field carries into the member of the decoded
 * structure, or into the members of the structure a split field names.
 *
 * The members a handler writes are a function of the data alone: whatever
 * combines several fields is derived once all of them have been read, see
 * `dmi_entity_ops_t::derive`, and goes into the members no field decodes into.
 * This is what lets the encoder tell whether the data it has decodes into the
 * value the structure holds.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Member the field decodes into, or the structure a split
 *                   field names.
 *
 * @return `true` if the data has been decoded, `false` otherwise.
 */
typedef bool dmi_field_decode_fn(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @brief Encode the member of the decoded structure into the data a field
 * carries, which undoes the decoding handler of the field.
 *
 * The data written is the one the specification spells the value with. The
 * encoder keeps the data the source has whenever it decodes into the same
 * value, so that a handler has no need to tell the ways a value may be
 * spelled apart.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Member the field has been decoded into, or the structure
 *                   a split field names.
 * @param[out] data  Data the field is to carry.
 *
 * @return `true` if the value has been encoded, `false` if the field cannot
 *         carry it.
 */
typedef bool dmi_field_encode_fn(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @brief Parameters of a field.
 */
struct dmi_field_params
{
    /**
     * @brief Version of the specification the field appears in, which starts
     * a group of the fields the data may end before.
     *
     * The structures written by the firmware of a platform are allowed to end
     * at the beginning of any such group, and the version the data has been
     * read up to becomes the level of the structure.
     */
    dmi_version_t since;

    /**
     * @brief Pointer to the value the member holds when the data ends before
     * the field, which is left unset for the members the data leaves zeroed.
     *
     * This is not the value standing for "unspecified", which the data
     * carries and `dmi_attribute_params_t::unspec` describes; it is what the
     * member means when the field is not there at all.
     */
    const void *absent;

    /**
     * @brief Offset the fields after a boundary begin at, counted from the
     * beginning of the structure the way the specification counts it.
     */
    size_t offset;

    /**
     * @brief Value standing for "unknown" in the data, which the member holds
     * as the largest value of its own width, so that it does not collide with
     * a value a device may really have.
     *
     * Zero means the field has no such value, since a field whose unknown
     * value is zero is decoded by a conversion instead.
     */
    uintmax_t unknown_raw;

    /**
     * @brief Value of the plain field which says that the real one is carried
     * by the extended field, see `DMI_FIELD_EXTENDED`.
     */
    uintmax_t when_raw;

    /**
     * @brief Member whose plain field says that the real value is carried by
     * the extended one, which the specification keys a whole run of the
     * extended fields on when they are widened together.
     *
     * Left unset for the extended fields answering to their own member.
     */
    dmi_member_ref_t when;


    /**
     * @brief Width of a range of bits, or of the unit the bits before a
     * padding share, in bits.
     */
    unsigned int bits;

    /**
     * @brief Number of the bytes of a binary field, or
     * `DMI_FIELD_LENGTH_REST` for the ones which run to the end of the
     * structure.
     */
    size_t length;

    /**
     * @brief Width of the field holding the number of the elements of an
     * array, which precedes the elements themselves.
     *
     * `DMI_FIELD_TYPE_NONE` means the data carries no number of its own and
     * the array runs to the end of the structure, which is how the
     * specification writes the arrays whose elements are all of one width;
     * such an array declares that width as `stride`.
     */
    dmi_field_type_t count_type;

    /**
     * @brief Member holding the number of the elements the data declares,
     * which is greater than the number of the decoded ones when the data ends
     * before the last element does.
     *
     * Left unset for the arrays whose declared number is of no interest once
     * the elements have been read.
     */
    dmi_member_ref_t count_member;

    /**
     * @brief Number of the bytes an element of an array takes, which the
     * arrays running to the end of the structure are measured in.
     */
    size_t stride;

    /**
     * @brief Width of the field holding the number of the bytes an element of
     * an array takes, which follows the number of the elements.
     *
     * Elements may be longer than the fields they are known to hold, so the
     * next one is found by that length rather than by what has been read.
     * Left unset for the arrays whose elements are exactly their fields.
     */
    dmi_field_type_t stride_type;

    /**
     * @brief Smallest length an element of an array may have, below which the
     * array is stepped over instead of being decoded.
     */
    size_t stride_minimum;

    /**
     * @brief Member holding the number of the bytes an element of an array
     * takes, which is left unset for the arrays whose elements are of no
     * length of their own.
     */
    dmi_member_ref_t stride_member;

    /**
     * @brief Member holding the number of the elements which have been
     * decoded, which is no greater than the number the data declares.
     */
    dmi_member_ref_t counter;

    /**
     * @brief Fields of a structure or of an array element.
     */
    const dmi_field_t *fields;

    /**
     * @brief Handler decoding the data the field carries into its member,
     * which stores the data as it is when left unset.
     */
    dmi_field_decode_fn *decode;

    /**
     * @brief Handler encoding the member back into the data, which undoes
     * `decode` and is required along with it for the field to be encoded.
     */
    dmi_field_encode_fn *encode;

    /**
     * @brief Field carries the value of a member the plain field before it is
     * too narrow for, see `DMI_FIELD_EXTENDED`.
     */
    bool extended;
};

/**
 * @brief Field of a structure, as it is laid out in the SMBIOS data.
 *
 * Fields describe the bytes, while the attributes of `dmi_attribute_t`
 * describe the values they are decoded into. The two are joined by the member
 * of the decoded structure they name, and neither is a complete description
 * of the other: a field may have no member of its own, and a member may be
 * computed rather than read.
 */
struct dmi_field
{
    /**
     * @brief Member of the decoded structure the field is read into, which is
     * `DMI_MEMBER_NULL` for the fields carrying no value of their own.
     */
    dmi_member_ref_t member;

    /**
     * @brief Layout of the field on the wire.
     */
    dmi_field_type_t type;

    /**
     * @brief Parameters of the field.
     */
    dmi_field_params_t params;
};

/**
 * @brief Field read into a member of the decoded structure.
 *
 * Parameters are given as the designated initializers of
 * `dmi_field_params_t`, e.g. `DMI_FIELD(dmi_slot_t, height, BYTE)` for a
 * field which needs none of them.
 */
#define DMI_FIELD(__entity, __member, __type, ...) \
    {                                              \
        .member = dmi_member(__entity, __member),  \
        .type   = DMI_FIELD_TYPE_ ## __type,       \
        .params = { __VA_ARGS__ }                  \
    }

/**
 * @brief Range of @p __bits bits, which the field shares with the ones
 * declared next to it.
 *
 * Ranges are taken from the least significant bit of the unit up, in the
 * order they are declared, and the unit is closed by `DMI_FIELD_PAD`, which
 * every run of the ranges ends with.
 */
#define DMI_FIELD_BITS(__entity, __member, __bits, ...) \
    {                                                   \
        .member = dmi_member(__entity, __member),       \
        .type   = DMI_FIELD_TYPE_BITS,                  \
        .params = { .bits = (__bits), __VA_ARGS__ }     \
    }

/**
 * @brief Bits the specification reserves between the ranges of a unit, which
 * carry no value of their own.
 */
#define DMI_FIELD_BITS_SKIP(__bits)    \
    {                                  \
        .member = DMI_MEMBER_NULL,     \
        .type   = DMI_FIELD_TYPE_BITS, \
        .params = { .bits = (__bits) } \
    }

/**
 * @brief Bits left over at the end of a unit the ranges before it share,
 * which the specification reserves.
 *
 * @p __unit is the width of the unit: `BYTE`, `WORD`, `DWORD` or `QWORD`.
 */
#define DMI_FIELD_PAD(__unit)                           \
    {                                                   \
        .member = DMI_MEMBER_NULL,                      \
        .type   = DMI_FIELD_TYPE_PAD,                   \
        .params = { .bits = DMI_FIELD_UNIT_ ## __unit } \
    }

/**
 * @brief Array of the elements described by their own fields, preceded by the
 * number of them.
 *
 * @p __counter is the member holding the number of the elements which have
 * been decoded, which is smaller than the one the data declares when the data
 * ends before the last element does.
 */
#define DMI_FIELD_ARRAY(__entity, __member, __counter, ...) \
    {                                                       \
        .member = dmi_member_array(__entity, __member),     \
        .type   = DMI_FIELD_TYPE_ARRAY,                     \
        .params = {                                         \
            .counter = dmi_member(__entity, __counter),     \
            __VA_ARGS__                                     \
        }                                                   \
    }

/**
 * @brief Element of an array of plain values, which is the whole element
 * rather than a member of one.
 *
 * Arrays of structures describe their elements by the fields of the
 * structure, while the ones holding plain values have a single field naming
 * the array itself, since the element has no member to name.
 */
#define DMI_FIELD_ELEMENT(__entity, __member, __type, ...)              \
    {                                                                   \
        .member = { .size = dmi_element_size(__entity, __member) },     \
        .type   = DMI_FIELD_TYPE_ ## __type,                            \
        .params = { __VA_ARGS__ }                                       \
    }

/**
 * @brief Field carrying the value of a member the plain field before it is
 * too narrow for.
 *
 * The specification widens a field by leaving the plain one in place and
 * adding a wider one to a later version, which the plain one points at by
 * holding the value of `when_raw`. Both fields name the same member: the
 * plain one decodes into it, and this one replaces the value when the plain
 * one says the real value is here.
 *
 * The extended field is read wherever it is declared even when the plain one
 * does not point at it, since it occupies its bytes either way.
 */
#define DMI_FIELD_EXTENDED(__entity, __member, __type, ...) \
    {                                                       \
        .member = dmi_member(__entity, __member),           \
        .type   = DMI_FIELD_TYPE_ ## __type,                \
        .params = { .extended = true, __VA_ARGS__ }         \
    }

/**
 * @brief Binary-coded decimal, which the firmware writes as the digits of the
 * number rather than as the number itself.
 *
 * @p __type is the width of the field: `BYTE`, `WORD`, `DWORD` or `QWORD`.
 */
#define DMI_FIELD_BCD(__entity, __member, __type, ...) \
    {                                                  \
        .member = dmi_member(__entity, __member),      \
        .type   = DMI_FIELD_TYPE_BCD,                  \
        .params = { .count_type = DMI_FIELD_TYPE_ ## __type, __VA_ARGS__ } \
    }

/**
 * @brief Length of a binary field which runs to the end of the structure,
 * because the structure carries no length of its own for it.
 */
#define DMI_FIELD_LENGTH_REST SIZE_MAX

/**
 * @brief Length of a binary field which the member holds already, because a
 * field before it carries the length.
 */
#define DMI_FIELD_LENGTH_MEMBER (SIZE_MAX - 1)

/**
 * @brief Bytes the structure holds and nothing reads, which are stepped over.
 */
#define DMI_FIELD_SKIP(__length)             \
    {                                        \
        .member = DMI_MEMBER_NULL,           \
        .type   = DMI_FIELD_TYPE_SKIP,       \
        .params = { .length = (__length) }   \
    }

/**
 * @brief Bytes taken as they are, which the member refers to in place.
 */
#define DMI_FIELD_BINARY(__entity, __member, __length, ...)   \
    {                                                        \
        .member = dmi_member(__entity, __member),            \
        .type   = DMI_FIELD_TYPE_BINARY,                     \
        .params = { .length = (__length), __VA_ARGS__ }      \
    }

/**
 * @brief Member the data does not carry, set to @p __value before the fields
 * are read.
 *
 * Some members mean something other than zero until the rest of the table
 * gives them a value, e.g. the type of the component a threshold belongs to,
 * which linking fills in. They are not fields, since the data holds nothing
 * for them, but they belong to the layout all the same: it is the only place
 * which says what a structure looks like before it is read.
 */
#define DMI_FIELD_PRESET(__entity, __member, __value)          \
    {                                                          \
        .member = dmi_member(__entity, __member),              \
        .type   = DMI_FIELD_TYPE_PRESET,                       \
        .params = {                                            \
            .absent = &(const __dmi_typeof(__value)){ __value } \
        }                                                      \
    }

/**
 * @brief Field whose handlers are given the whole structure being decoded
 * rather than one member of it, which is how the data feeding several
 * members at once is decoded.
 *
 * @p __structure is the type the fields belong to: the decoded structure for
 * the fields of a specification, and the element type for the fields of an
 * array. @p __type is the type of the data the field carries.
 */
#define DMI_FIELD_SPLIT(__structure, __type, ...)  \
    {                                              \
        .member = { .size = sizeof(__structure) }, \
        .type   = DMI_FIELD_TYPE_ ## __type,       \
        .params = { __VA_ARGS__ }                  \
    }

/**
 * @brief Beginning of a group of the fields the data may end before, which
 * the structures of the platforms older than the fields are shorter than.
 *
 * The group carries the version of the specification the fields appear in,
 * or `DMI_VERSION_NONE` for the fields which the firmware is known to leave
 * out without the specification saying so.
 */
#define DMI_FIELD_GROUP(...)            \
    {                                   \
        .member = DMI_MEMBER_NULL,      \
        .type   = DMI_FIELD_TYPE_GROUP, \
        .params = { __VA_ARGS__ }       \
    }

/**
 * @brief Boundary between the parts of a structure, which the fields after it
 * begin at the offset of.
 *
 * Unlike `DMI_FIELD_GROUP`, this says nothing about where the data may end:
 * it separates the parts of a structure which are read as one, such as the
 * ranges of bits sharing a unit, and states where they begin. The offset is
 * counted from the beginning of the structure, the way the tables of the
 * specification count it, and is checked while the structure is decoded, so
 * that a field left out of the declaration or added to it twice is caught
 * where it happens rather than in the values of everything after it.
 */
#define DMI_FIELD_OFFSET(__offset)        \
    {                                     \
        .member = DMI_MEMBER_NULL,        \
        .type   = DMI_FIELD_TYPE_OFFSET,  \
        .params = { .offset = (__offset) } \
    }

/**
 * @brief List of the fields a structure is laid out as, terminated for the code
 * which walks it.
 *
 * The terminator is added by the macro, so that a list which has lost it
 * cannot be written in the first place.
 */
#define DMI_FIELDS(...) (const dmi_field_t[])__VA_ARGS__

__BEGIN_DECLS

/**
 * @brief Decode a structure according to the fields of its specification.
 *
 * The fields are read in the order they are declared, starting at the
 * beginning of the structure body. The data is allowed to end at the
 * beginning of a group declared with `DMI_FIELD_GROUP`, which leaves the
 * fields of that group and of the ones after it unset and marks the structure
 * as stopped rather than as broken. Ending anywhere else marks the structure
 * as incomplete, except within the fields before the first group, which every
 * structure of the type is required to carry.
 *
 * This is the decoding handler of the specifications which describe their
 * layout rather than decode it themselves.
 *
 * @param[in,out] entity Structure to decode.
 *
 * @error DMI_ERROR_NULL_ARGUMENT Entity is `nullptr`
 * @error DMI_ERROR_INVALID_STATE Specification declares no fields
 * @error DMI_ERROR_OUT_OF_MEMORY Elements of an array cannot be allocated
 *
 * @return `true` if the structure has been decoded, `false` if it cannot be.
 */
__dmi_api bool dmi_fields_decode(dmi_entity_t *entity);

/**
 * @brief Encode a structure according to the fields of its specification.
 *
 * Fields are written in the order they are declared, after the header the
 * encoder has written, and every value the decoded structure holds is written
 * from the structure, undoing the conversions it has been decoded with. How
 * the bytes the model does not hold are written is up to the mode of the
 * encoder, see `dmi_encode_mode_t`: in the preserve mode, encoding a structure
 * which has not been changed gives back the bytes it has been decoded from.
 *
 * @param[in,out] encoder Encoder of the structure, see
 *                        `dmi_encoder_initialize()`.
 *
 * @error DMI_ERROR_NULL_ARGUMENT Encoder is `nullptr`
 * @error DMI_ERROR_INVALID_STATE Specification declares no fields, or a field
 *        it cannot write back, e.g. a conversion without the one undoing it
 * @error DMI_ERROR_INTERNAL Fields reach another offset than the one declared
 * @error DMI_ERROR_OUT_OF_MEMORY Buffers of the encoder cannot grow
 *
 * @return `true` if the structure has been encoded, `false` otherwise.
 */
__dmi_api bool dmi_fields_encode(dmi_encoder_t *encoder);

/**
 * @brief Get the number of the bytes a field occupies on the wire.
 *
 * Fields of the types which have no fixed width, such as arrays, occupy no
 * bytes of their own.
 *
 * @param[in] type Type of the field.
 *
 * @return Number of the bytes, or zero if the type has no fixed width.
 */
__dmi_api size_t dmi_field_type_size(dmi_field_type_t type);

/**
 * @brief Decode a value carried in kilobytes into the number of the bytes it
 * stands for, which is how the specification writes the sizes and the
 * addresses the fields of a structure are too narrow for.
 */
__dmi_api bool dmi_field_decode_kilobytes(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @brief Encode a number of the bytes into the kilobytes a field carries,
 * which undoes `dmi_field_decode_kilobytes()`.
 */
__dmi_api bool dmi_field_encode_kilobytes(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

/**
 * @brief Get the value of the integer member of a field.
 *
 * @param[in] field Field whose member is read.
 * @param[in] value Member of the field.
 *
 * @return Value of the member.
 */
__dmi_api uintmax_t dmi_field_get(const dmi_field_t *field, const void *value);

/**
 * @brief Set the value of the integer member of a field, which may be wider
 * than the data the field carries.
 *
 * @param[in]  field  Field whose member is written.
 * @param[out] value  Member of the field.
 * @param[in]  number Value to set.
 *
 * @return `true` on success, `false` if the member is not an integer.
 */
__dmi_api bool dmi_field_set(const dmi_field_t *field, void *value, uintmax_t number);

__END_DECLS

#endif // !OPENDMI_FIELD_H
