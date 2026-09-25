//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_DECODER_H
#define OPENDMI_DECODER_H

#pragma once

#include <opendmi/types.h>
#include <opendmi/reader.h>
#include <opendmi/utils/codec.h>

#ifndef DMI_DECODER_T
#   define DMI_DECODER_T
    typedef struct dmi_decoder dmi_decoder_t;
#endif // !DMI_DECODER_T

/**
 * @brief Reading of a structure, as its decoder sees it.
 *
 * A decoder reads the data of one structure into the values the model holds:
 * `dmi_reader_t` gives it the bytes, and the structure being decoded says
 * what they refer to, e.g. which string a number stands for. Neither knows of
 * the other, which is what this brings together.
 *
 * @note All fields are maintained internally. Do not modify them directly;
 *       use the decoder API instead.
 */
struct dmi_decoder
{
    /**
     * @brief Structure being decoded.
     */
    dmi_entity_t *entity;

    /**
     * @brief Reader of the data of the structure.
     */
    dmi_reader_t reader;
};

__BEGIN_DECLS

/**
 * @brief Initialize a decoder of a structure.
 *
 * The reader is set up over the body of the structure, positioned right after
 * the header, and reads the copy with additional information applied whenever
 * there is one.
 *
 * @param[out] decoder Decoder to initialize.
 * @param[in]  entity  Structure to decode.
 *
 * @error DMI_ERROR_NULL_ARGUMENT Decoder or entity is `nullptr`
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_decoder_initialize(dmi_decoder_t *decoder, dmi_entity_t *entity);

/**
 * @brief Get the structure being decoded.
 *
 * @param[in] decoder Decoder to query.
 *
 * @return Structure being decoded, or @c nullptr if @p decoder is @c nullptr.
 */
__dmi_api dmi_entity_t *dmi_decoder_entity(const dmi_decoder_t *decoder);

/**
 * @brief Get the context the structure being decoded belongs to.
 *
 * @param[in] decoder Decoder to query.
 *
 * @return Context of the structure, or @c nullptr if @p decoder is
 *         @c nullptr.
 */
__dmi_api dmi_context_t *dmi_decoder_context(const dmi_decoder_t *decoder);

/**
 * @brief Get the reader of the data of the structure.
 *
 * Bytes are read through the reader, which the decoder positions and the
 * decoding handlers move as they read.
 *
 * @param[in] decoder Decoder to query.
 *
 * @return Reader of the data, or @c nullptr if @p decoder is @c nullptr.
 */
__dmi_api dmi_reader_t *dmi_decoder_reader(dmi_decoder_t *decoder);

/**
 * @brief Read a reference to a string and resolve it.
 *
 * Reads the number a string is referred to by and looks the string up among
 * the ones of the structure. A reference to no string gives @c nullptr, and
 * so does a reference to a string the structure does not carry, which raises
 * an error of its own.
 *
 * @param[in,out] decoder Decoder to read from.
 * @param[out]    value   String the reference stands for.
 *
 * @error DMI_ERROR_STRING_NOT_FOUND Structure carries no such string
 *
 * @return `true` if the reference has been read, `false` if there are not
 *         enough bytes remaining.
 */
__dmi_api bool dmi_decoder_get_string(dmi_decoder_t *decoder, const char **value);

/**
 * @brief Read bytes at the current position.
 *
 * @param[in,out] decoder Decoder to read from.
 * @param[out]    ptr     Buffer that receives the bytes.
 * @param[in]     length  Number of the bytes to read.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
__dmi_api bool dmi_decoder_get_bytes(dmi_decoder_t *decoder, void *ptr, size_t length);

/**
 * @brief Read bytes at a given offset without moving the cursor.
 *
 * @param[in]  decoder Decoder to read from.
 * @param[out] ptr     Buffer that receives the bytes.
 * @param[in]  offset  Byte offset from the beginning of the structure.
 * @param[in]  length  Number of the bytes to read.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         structure bounds.
 */
__dmi_api bool dmi_decoder_get_bytes_at(
        const dmi_decoder_t *decoder,
        void                *ptr,
        size_t               offset,
        size_t               length);

/**
 * @brief Read binary data at the current position, referring to it in place.
 *
 * @param[in,out] decoder Decoder to read from.
 * @param[in]     length  Number of the bytes to refer to, may be zero.
 * @param[out]    value   Binary data descriptor.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
__dmi_api bool dmi_decoder_get_binary(dmi_decoder_t *decoder, size_t length, dmi_binary_t *value);

/**
 * @brief Step over bytes of the structure without reading them.
 *
 * @param[in,out] decoder Decoder to advance.
 * @param[in]     length  Number of the bytes to skip.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
__dmi_api bool dmi_decoder_skip(dmi_decoder_t *decoder, size_t length);

/**
 * @brief Get the number of the bytes of the structure left to read.
 *
 * @param[in] decoder Decoder to query.
 *
 * @return Number of the bytes.
 */
__dmi_api size_t dmi_decoder_remaining(const dmi_decoder_t *decoder);

/**
 * @brief Stop decoding at the end of the data of the structure.
 *
 * Intended for the decoders of the structures which newer versions of the
 * specification have extended, and is called once the data has been read to
 * its end while the decoder knows of more fields. Marks the structure with
 * `DMI_ENTITY_STATE_PARTIAL`.
 *
 * The data is required to be exhausted. A structure which ends anywhere else
 * is incomplete instead, see `dmi_decoder_incomplete`(3).
 *
 * @param[in,out] decoder Decoder of the structure.
 *
 * @return Always `true`, so that it can be returned by the decoder.
 */
__dmi_api bool dmi_decoder_stop(dmi_decoder_t *decoder);

/**
 * @brief Stop decoding at an incomplete set of the fields of the structure.
 *
 * Intended for the decoders of the structures which newer versions of the
 * specification have extended, and is called once the data ends in the middle
 * of a set of fields, so that the length of the structure matches no version
 * of the specification. The fields which are there in full are decoded before
 * the call. Marks the structure with `DMI_ENTITY_STATE_INCOMPLETE`, and the
 * data left over, a part of the next field if any, is ignored.
 *
 * @param[in,out] decoder Decoder of the structure.
 *
 * @return Always `true`, so that it can be returned by the decoder.
 */
__dmi_api bool dmi_decoder_incomplete(dmi_decoder_t *decoder);

__END_DECLS

/**
 * @internal
 * @def __dmi_decoder_get(__decoder, __type, __pvalue, __convert)
 * @brief Implementation helper for sequential get macros.
 */
#define __dmi_decoder_get(__decoder, __type, __pvalue, __convert)                  \
        ({                                                                       \
            __type __value;                                                      \
            bool rv = dmi_decoder_get_bytes(__decoder, &__value, sizeof(__value)); \
            if (rv)                                                              \
                *(__pvalue) = (__dmi_typeof(*(__pvalue)))__convert(__value);     \
            rv;                                                                  \
        })

/**
 * @def dmi_decoder_get(__decoder, __type, __pvalue)
 * @brief Read and decode a value from the structure at the current position.
 *
 * Reads `sizeof(__type)` bytes from the structure, converts the value from SMBIOS
 * wire format (little-endian) into host byte order using `dmi_decode`, and
 * stores the result in the variable pointed to by @p __pvalue. The decoder cursor
 * is advanced.
 *
 * @param[in,out] __decoder Decoder to read from.
 * @param[in]     __type   Wire-format type to read (e.g., `dmi_word_t`).
 * @param[out]    __pvalue Pointer to the variable that receives the decoded
 *                         value.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_decoder_get(__decoder, __type, __pvalue) \
        __dmi_decoder_get(__decoder, __type, __pvalue, dmi_decode)

/**
 * @def dmi_decoder_get_bcd(__decoder, __type, __pvalue)
 * @brief Read and decode a BCD-encoded value from the structure at the current
 *        position.
 *
 * Reads `sizeof(__type)` bytes from the structure, decodes the value from BCD
 * encoding using `dmi_decode_bcd`, and stores the result in the object pointed
 * to by @p __pvalue. The decoder cursor is advanced.
 *
 * @param[in,out] __decoder Decoder to read from.
 * @param[in]     __type   Wire-format type to read (e.g., `dmi_byte_t`).
 * @param[out]    __pvalue Pointer to the variable that receives the decoded
 *                         value.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_decoder_get_bcd(__decoder, __type, __pvalue) \
        __dmi_decoder_get(__decoder, __type, __pvalue, dmi_decode_bcd)

/**
 * @def dmi_decoder_get_uuid(__decoder, __pvalue)
 * @brief Read and decode a UUID from the structure at the current position.
 *
 * Reads 16 bytes from the structure, converts the UUID from SMBIOS byte order
 * into RFC 4122 representation using `dmi_uuid_decode`, and stores the result
 * in the variable pointed to by @p __pvalue. The decoder cursor is advanced.
 *
 * @param[in,out] __decoder Decoder to read from.
 * @param[out]    __pvalue Pointer to a `dmi_uuid_t` that receives the decoded
 *                         UUID.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_decoder_get_uuid(__decoder, __pvalue)                                 \
        ({                                                                      \
            dmi_byte_t __value[16];                                             \
            bool rv = dmi_decoder_get_bytes(__decoder, __value, sizeof(__value)); \
            if (rv)                                                             \
                *(__pvalue) = dmi_uuid_decode(__value);                         \
            rv;                                                                 \
        })

/**
 * @internal
 * @def __dmi_decoder_get_at(__decoder, __type, __offset, __pvalue, __convert)
 * @brief Implementation helper for random-access get macros.
 */
#define __dmi_decoder_get_at(__decoder, __type, __offset, __pvalue, __convert)                  \
        ({                                                                                    \
            __type __value;                                                                   \
            bool rv = dmi_decoder_get_bytes_at(__decoder, &__value, __offset, sizeof(__value)); \
            if (rv)                                                                           \
                *(__pvalue) = (__dmi_typeof(*(__pvalue)))__convert(__value);                  \
            rv;                                                                               \
        })

/**
 * @def dmi_decoder_get_at(__decoder, __offset, __type, __pvalue)
 * @brief Read and decode a value from the structure at a given offset.
 *
 * Reads `sizeof(__type)` bytes at @p __offset from the beginning of the
 * range, converts the value from SMBIOS wire format into host byte order using
 * `dmi_decode`, and stores the result in the variable pointed to by @p __pvalue.
 * The decoder cursor is not modified.
 *
 * @param[in]  __decoder Decoder to read from.
 * @param[in]  __offset Byte offset from the beginning of the range.
 * @param[in]  __type   Wire-format type to read (e.g., `dmi_word_t`).
 * @param[out] __pvalue Pointer to the variable that receives the decoded value.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         structure bounds.
 */
#define dmi_decoder_get_at(__decoder, __offset, __type, __pvalue) \
        __dmi_decoder_get_at(__decoder, __type, __offset, __pvalue, dmi_decode)

/**
 * @def dmi_decoder_get_bcd_at(__decoder, __offset, __type, __pvalue)
 * @brief Read and decode a BCD-encoded value from the structure at a given
 *        offset.
 *
 * Reads `sizeof(__type)` bytes at @p __offset from the beginning of the
 * range, decodes the value from BCD encoding using `dmi_decode_bcd`, and stores
 * the result in the variable pointed to by @p __pvalue. The decoder cursor is not
 * modified.
 *
 * @param[in]  __decoder Decoder to read from.
 * @param[in]  __offset Byte offset from the beginning of the range.
 * @param[in]  __type   Wire-format type to read (e.g., `dmi_byte_t`).
 * @param[out] __pvalue Pointer to the variable that receives the decoded value.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         structure bounds.
 */
#define dmi_decoder_get_bcd_at(__decoder, __offset, __type, __pvalue) \
        __dmi_decoder_get_at(__decoder, __type, __offset, __pvalue, dmi_decode_bcd)

/**
 * @def dmi_decoder_get_uuid_at(__decoder, __offset, __pvalue)
 * @brief Read and decode a UUID from the structure at a given offset.
 *
 * Reads 16 bytes at @p __offset from the beginning of the range,
 * converts the UUID from SMBIOS byte order into RFC 4122 representation using
 * `dmi_uuid_decode`, and stores the result in the variable pointed to by
 * @p __pvalue. The decoder cursor is not modified.
 *
 * @param[in]  __decoder Decoder to read from.
 * @param[in]  __offset Byte offset from the beginning of the range.
 * @param[out] __pvalue Pointer to a `dmi_uuid_t` that receives the decoded
 *                      UUID.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         structure bounds.
 */
#define dmi_decoder_get_uuid_at(__decoder, __offset, __pvalue)                                 \
        ({                                                                                   \
            dmi_byte_t __value[16];                                                          \
            bool rv = dmi_decoder_get_bytes_at(__decoder, __value, __offset, sizeof(__value)); \
            if (rv)                                                                          \
                *(__pvalue) = dmi_uuid_decode(__value);                                      \
            rv;                                                                              \
        })

#endif // !OPENDMI_DECODER_H
