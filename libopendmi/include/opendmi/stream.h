//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_STREAM_H
#define OPENDMI_STREAM_H

#pragma once

#include <opendmi/utils/codec.h>

typedef struct dmi_stream dmi_stream_t;

/**
 * @brief Sequential reader over an SMBIOS entity body.
 *
 * Provides a cursor-based view of the body data (the portion of the SMBIOS
 * structure that follows its fixed-size header). Initialise with
 * `dmi_stream_initialize`(3) before use.
 *
 * @note All fields are maintained internally. Do not modify them directly;
 *       use the stream API instead.
 */
struct dmi_stream
{
    /**
     * @brief Entity whose body data is being read.
     */
    const dmi_entity_t *entity;

    /**
     * @brief Current byte offset from the beginning of the entity body.
     *
     * Advanced by sequential read operations. May be repositioned with
     * `dmi_stream_seek`(3) or reset to zero with `dmi_stream_reset`(3).
     */
    size_t position;

    /**
     * @brief Number of bytes available from the current position to the end
     *        of the entity body.
     *
     * Always equal to `entity->body_length - position`.
     */
    size_t remaining;
};

__BEGIN_DECLS

/**
 * @brief Initialize a stream for reading an entity's body data.
 *
 * Sets up the stream to read the body of @p entity, positioning the cursor
 * at the beginning of the body (past the SMBIOS structure header).
 *
 * @param[out] stream Stream to initialize.
 * @param[in]  entity Entity whose body data will be read.
 *
 * @return `true` on success, `false` if @p stream or @p entity is @c nullptr.
 */
bool dmi_stream_initialize(dmi_stream_t *stream, const dmi_entity_t *entity);

/**
 * @brief Seek to a byte position within the stream.
 *
 * Repositions the stream cursor to @p position bytes from the beginning of
 * the entity body.
 *
 * @param[in,out] stream Stream to reposition.
 * @param[in]     position Byte offset from the beginning of the entity body.
 *
 * @return `true` on success, `false` if @p position is beyond the end of the
 *         stream.
 */
bool dmi_stream_seek(dmi_stream_t *stream, size_t position);

/**
 * @brief Read data from the stream at the current position.
 *
 * Copies @p length bytes from the stream into the buffer pointed to by
 * @p ptr and advances the stream cursor.
 *
 * @param[in,out] stream Stream to read from.
 * @param[out]    ptr    Buffer that receives the data; must be at least
 *                       @p length bytes.
 * @param[in]     length Number of bytes to read.
 *
 * @return `true` on success, `false` if there are fewer than @p length bytes
 *         remaining in the stream.
 */
bool dmi_stream_read_data(dmi_stream_t *stream, void *ptr, size_t length);

/**
 * @brief Read data from the stream at a given offset without advancing the
 *        cursor.
 *
 * Copies @p length bytes starting at @p offset from the beginning of the
 * entity body into the buffer pointed to by @p ptr. The stream cursor is not
 * modified.
 *
 * @param[in]  stream Stream to read from.
 * @param[out] ptr    Buffer that receives the data; must be at least
 *                    @p length bytes.
 * @param[in]  offset Byte offset from the beginning of the entity body.
 * @param[in]  length Number of bytes to read.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         stream bounds.
 */
bool dmi_stream_read_data_at(const dmi_stream_t *stream, void *ptr, size_t offset, size_t length);

/**
 * @brief Advance the stream cursor without reading data.
 *
 * Moves the stream cursor forward by @p length bytes, effectively discarding
 * that many bytes from the current position.
 *
 * @param[in,out] stream Stream to advance.
 * @param[in]     length Number of bytes to skip.
 *
 * @return `true` on success, `false` if there are fewer than @p length bytes
 *         remaining in the stream.
 */
bool dmi_stream_skip(dmi_stream_t *stream, size_t length);

/**
 * @brief Return the number of unread bytes in the stream.
 *
 * @param[in] stream Stream to query.
 *
 * @return Number of bytes from the current cursor position to the end of the
 *         entity body.
 */
size_t dmi_stream_remaining(const dmi_stream_t *stream);

/**
 * @brief Check whether all data in the stream has been consumed.
 *
 * @param[in] stream Stream to check.
 *
 * @return `true` if there are no remaining bytes to read, `false` otherwise.
 */
bool dmi_stream_is_done(const dmi_stream_t *stream);

/**
 * @brief Check if the stream contains at least the given number of bytes.
 *
 * @param[in] stream Stream to check.
 * @param[in] length Number of bytes.
 *
 * @return `true` if at least @p length bytes remain from the current
 *         position, `false` otherwise.
 */
bool dmi_stream_has(const dmi_stream_t *stream, size_t length);

/**
 * @brief Reset the stream cursor to the beginning.
 *
 * Repositions the stream cursor back to the start of the entity body so that
 * it can be read again from the beginning.
 *
 * @param[in,out] stream Stream to reset.
 */
void dmi_stream_reset(dmi_stream_t *stream);

__END_DECLS

/**
 * @def dmi_stream_read(__stream, __pvalue)
 * @brief Read a typed value from the stream at the current position.
 *
 * Reads `sizeof(*__pvalue)` bytes from the stream into the object pointed to
 * by @p __pvalue and advances the stream cursor. No byte-order conversion is
 * performed.
 *
 * @param[in,out] __stream Stream to read from.
 * @param[out]    __pvalue Pointer to the object that receives the raw data.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_stream_read(__stream, __pvalue) \
        dmi_stream_read_data(__stream, __pvalue, sizeof(*(__pvalue)))

/**
 * @internal
 * @def __dmi_stream_decode(__stream, __type, __pvalue, __decoder)
 * @brief Implementation helper for sequential stream decode macros.
 */
#define __dmi_stream_decode(__stream, __type, __pvalue, __decoder)            \
        ({                                                                    \
            __type __value;                                                   \
            bool rv = dmi_stream_read(__stream, &__value);                    \
            if (rv)                                                           \
                *(__pvalue) = (typeof(*(__pvalue)))__decoder(__value);        \
            rv;                                                               \
        })

/**
 * @def dmi_stream_decode(__stream, __type, __pvalue)
 * @brief Read and decode a value from the stream at the current position.
 *
 * Reads `sizeof(__type)` bytes from the stream, converts the value from SMBIOS
 * wire format (little-endian) into host byte order using `dmi_decode`, and
 * stores the result in the object pointed to by @p __pvalue. The stream cursor
 * is advanced.
 *
 * @param[in,out] __stream Stream to read from.
 * @param[in]     __type   Wire-format type to read (e.g., `dmi_word_t`).
 * @param[out]    __pvalue Pointer to the object that receives the decoded
 *                         value.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_stream_decode(__stream, __type, __pvalue) \
        __dmi_stream_decode(__stream, __type, __pvalue, dmi_decode)

/**
 * @def dmi_stream_decode_bcd(__stream, __type, __pvalue)
 * @brief Read and decode a BCD-encoded value from the stream at the current
 *        position.
 *
 * Reads `sizeof(__type)` bytes from the stream, decodes the value from BCD
 * encoding using `dmi_decode_bcd`, and stores the result in the object pointed
 * to by @p __pvalue. The stream cursor is advanced.
 *
 * @param[in,out] __stream Stream to read from.
 * @param[in]     __type   Wire-format type to read (e.g., `dmi_byte_t`).
 * @param[out]    __pvalue Pointer to the object that receives the decoded
 *                         value.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_stream_decode_bcd(__stream, __type, __pvalue) \
        __dmi_stream_decode(__stream, __type, __pvalue, dmi_decode_bcd)

/**
 * @def dmi_stream_decode_str(__stream, __pvalue)
 * @brief Read and decode an SMBIOS string reference from the stream at the
 *        current position.
 *
 * Reads a `dmi_string_t` index from the stream and resolves it to a string
 * pointer via `dmi_entity_string`. The stream cursor is advanced.
 *
 * @param[in,out] __stream Stream to read from.
 * @param[out]    __pvalue Pointer to a `const char *` that receives the
 *                         resolved string.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_stream_decode_str(__stream, __pvalue)                             \
        ({                                                                    \
            dmi_string_t __value;                                             \
            bool rv = dmi_stream_read(__stream, &__value);                    \
            if (rv)                                                           \
                *(__pvalue) = dmi_entity_string((__stream)->entity, __value); \
            rv;                                                               \
        })

/**
 * @def dmi_stream_decode_uuid(__stream, __pvalue)
 * @brief Read and decode a UUID from the stream at the current position.
 *
 * Reads 16 bytes from the stream, converts the UUID from SMBIOS byte order
 * into RFC 4122 representation using `dmi_uuid_decode`, and stores the result
 * in the object pointed to by @p __pvalue. The stream cursor is advanced.
 *
 * @param[in,out] __stream Stream to read from.
 * @param[out]    __pvalue Pointer to a `dmi_uuid_t` that receives the decoded
 *                         UUID.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_stream_decode_uuid(__stream, __pvalue)                              \
        ({                                                                      \
            dmi_byte_t __value[16];                                             \
            bool rv = dmi_stream_read_data(__stream, __value, sizeof(__value)); \
            if (rv)                                                             \
                *(__pvalue) = dmi_uuid_decode(__value);                         \
            rv;                                                                 \
        })

/**
 * @def dmi_stream_read_at(__stream, __offset, __pvalue)
 * @brief Read a typed value from the stream at a given offset.
 *
 * Reads `sizeof(*__pvalue)` bytes at @p __offset from the beginning of the
 * entity body into the object pointed to by @p __pvalue. The stream cursor is
 * not modified. No byte-order conversion is performed.
 *
 * @param[in]  __stream Stream to read from.
 * @param[in]  __offset Byte offset from the beginning of the entity body.
 * @param[out] __pvalue Pointer to the object that receives the raw data.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         stream bounds.
 */
#define dmi_stream_read_at(__stream, __offset, __pvalue) \
        dmi_stream_read_data_at(__stream, __pvalue, __offset, sizeof(*(__pvalue)))

/**
 * @internal
 * @def __dmi_stream_decode_at(__stream, __type, __offset, __pvalue, __decoder)
 * @brief Implementation helper for random-access stream decode macros.
 */
#define __dmi_stream_decode_at(__stream, __type, __offset, __pvalue, __decoder) \
        ({                                                                      \
            __type __value;                                                     \
            bool rv = dmi_stream_read_at(__stream, __offset, &__value);         \
            if (rv)                                                             \
                *(__pvalue) = (typeof(*(__pvalue)))__decoder(__value);          \
            rv;                                                                 \
        })

/**
 * @def dmi_stream_decode_at(__stream, __offset, __type, __pvalue)
 * @brief Read and decode a value from the stream at a given offset.
 *
 * Reads `sizeof(__type)` bytes at @p __offset from the beginning of the entity
 * body, converts the value from SMBIOS wire format into host byte order using
 * `dmi_decode`, and stores the result in the object pointed to by @p __pvalue.
 * The stream cursor is not modified.
 *
 * @param[in]  __stream Stream to read from.
 * @param[in]  __offset Byte offset from the beginning of the entity body.
 * @param[in]  __type   Wire-format type to read (e.g., `dmi_word_t`).
 * @param[out] __pvalue Pointer to the object that receives the decoded value.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         stream bounds.
 */
#define dmi_stream_decode_at(__stream, __offset, __type, __pvalue) \
        __dmi_stream_decode_at(__stream, __type, __offset, __pvalue, dmi_decode)

/**
 * @def dmi_stream_decode_bcd_at(__stream, __offset, __type, __pvalue)
 * @brief Read and decode a BCD-encoded value from the stream at a given
 *        offset.
 *
 * Reads `sizeof(__type)` bytes at @p __offset from the beginning of the entity
 * body, decodes the value from BCD encoding using `dmi_decode_bcd`, and stores
 * the result in the object pointed to by @p __pvalue. The stream cursor is not
 * modified.
 *
 * @param[in]  __stream Stream to read from.
 * @param[in]  __offset Byte offset from the beginning of the entity body.
 * @param[in]  __type   Wire-format type to read (e.g., `dmi_byte_t`).
 * @param[out] __pvalue Pointer to the object that receives the decoded value.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         stream bounds.
 */
#define dmi_stream_decode_bcd_at(__stream, __offset, __type, __pvalue) \
        __dmi_stream_decode_at(__stream, __type, __offset, __pvalue, dmi_decode_bcd)

/**
 * @def dmi_stream_decode_str_at(__stream, __offset, __pvalue)
 * @brief Read and decode an SMBIOS string reference from the stream at a
 *        given offset.
 *
 * Reads a `dmi_string_t` index at @p __offset from the beginning of the
 * entity body and resolves it to a string pointer via `dmi_entity_string`. The
 * stream cursor is not modified.
 *
 * @param[in]  __stream Stream to read from.
 * @param[in]  __offset Byte offset from the beginning of the entity body.
 * @param[out] __pvalue Pointer to a `const char *` that receives the resolved
 *                      string.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         stream bounds.
 */
#define dmi_stream_decode_str_at(__stream, __offset, __pvalue)                \
        ({                                                                    \
            dmi_string_t __value;                                             \
            bool rv = dmi_stream_read_at(__stream, __offset, &__value);       \
            if (rv)                                                           \
                *(__pvalue) = dmi_entity_string((__stream)->entity, __value); \
            rv;                                                               \
        })

/**
 * @def dmi_stream_decode_uuid_at(__stream, __offset, __pvalue)
 * @brief Read and decode a UUID from the stream at a given offset.
 *
 * Reads 16 bytes at @p __offset from the beginning of the entity body,
 * converts the UUID from SMBIOS byte order into RFC 4122 representation using
 * `dmi_uuid_decode`, and stores the result in the object pointed to by
 * @p __pvalue. The stream cursor is not modified.
 *
 * @param[in]  __stream Stream to read from.
 * @param[in]  __offset Byte offset from the beginning of the entity body.
 * @param[out] __pvalue Pointer to a `dmi_uuid_t` that receives the decoded
 *                      UUID.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         stream bounds.
 */
#define dmi_stream_decode_uuid_at(__stream, __offset, __pvalue)                              \
        ({                                                                                   \
            dmi_byte_t __value[16];                                                          \
            bool rv = dmi_stream_read_data_at(__stream, __value, __offset, sizeof(__value)); \
            if (rv)                                                                          \
                *(__pvalue) = dmi_uuid_decode(__value);                                      \
            rv;                                                                              \
        })

#endif // !OPENDMI_STREAM_H
