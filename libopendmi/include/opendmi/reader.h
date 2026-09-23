//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_READER_H
#define OPENDMI_READER_H

#pragma once

#include <opendmi/utils/codec.h>

#ifndef DMI_READER_T
#   define DMI_READER_T
    typedef struct dmi_reader dmi_reader_t;
#endif // !DMI_READER_T

#ifndef DMI_READER_MARK_T
#   define DMI_READER_MARK_T
    typedef struct dmi_reader_mark dmi_reader_mark_t;
#endif // !DMI_READER_MARK_T

/**
 * @brief Sequential reader over an SMBIOS entity body.
 *
 * Provides a cursor-based view of the body data (the portion of the SMBIOS
 * structure that follows its fixed-size header). Initialise with
 * `dmi_reader_initialize`(3) before use.
 *
 * @note All fields are maintained internally. Do not modify them directly;
 *       use the reader API instead.
 */
struct dmi_reader
{
    /**
     * @brief Entity whose body data is being read.
     */
    const dmi_entity_t *entity;

    /**
     * @brief Current byte offset from the beginning of the entity body.
     *
     * Advanced by sequential read operations. May be repositioned with
     * `dmi_reader_seek`(3) or reset to zero with `dmi_reader_reset`(3).
     */
    size_t position;

    /**
     * @brief Number of bytes available from the current position to the end
     *        of the entity body.
     *
     * Always equal to `entity->body_length - position`.
     */
    size_t remaining;

    /**
     * @brief Body data being read: the structure data, or its copy with
     * additional information applied.
     */
    const dmi_data_t *data;
};

/**
 * @brief Position of a reader cursor, taken with `dmi_reader_mark`(3).
 *
 * Marks are values: they are copied rather than allocated, nest without any
 * bookkeeping, and need no release. A mark stays valid for as long as the
 * reader reads the same entity.
 *
 * @note All fields are maintained internally. Do not read or modify them
 *       directly; pass the mark to `dmi_reader_rewind`(3) instead.
 */
struct dmi_reader_mark
{
    /**
     * @brief Entity the reader was reading when the mark was taken, which
     * tells the marks of different readers apart.
     */
    const dmi_entity_t *entity;

    /**
     * @brief Cursor position the mark was taken at.
     */
    size_t position;
};

__BEGIN_DECLS

/**
 * @brief Initialize a reader for reading an entity's body data.
 *
 * Sets up the reader to read the body of @p entity, positioning the cursor
 * at the beginning of the body (past the SMBIOS structure header).
 *
 * @param[out] reader Reader to initialize.
 * @param[in]  entity Entity whose body data will be read.
 *
 * @return `true` on success, `false` if @p reader or @p entity is @c nullptr.
 */
__dmi_api bool dmi_reader_initialize(dmi_reader_t *reader, const dmi_entity_t *entity);

/**
 * @brief Seek to a byte position within the reader.
 *
 * Repositions the reader cursor to @p position bytes from the beginning of
 * the entity body.
 *
 * @param[in,out] reader   Reader to reposition.
 * @param[in]     position Byte offset from the beginning of the entity body.
 *
 * @return `true` on success, `false` if @p position is beyond the end of the
 *         reader.
 */
__dmi_api bool dmi_reader_seek(dmi_reader_t *reader, size_t position);

/**
 * @brief Take a mark of the current position of the reader cursor.
 *
 * Marks name a place to come back to without counting the bytes read in
 * between, which is what records of a declared length need: mark the
 * beginning of the record, read whatever fields it holds, then rewind and
 * skip the length of the record.
 *
 * @param[in] reader Reader to mark.
 *
 * @return Mark of the current position, which is rejected by
 *         `dmi_reader_rewind`(3) if @p reader is @c nullptr.
 */
__dmi_api dmi_reader_mark_t dmi_reader_mark(const dmi_reader_t *reader);

/**
 * @brief Reposition the reader cursor to a mark taken earlier.
 *
 * Unlike `dmi_reader_seek`(3), a mark taken at the end of the body is
 * accepted, since the cursor has already been there.
 *
 * @param[in,out] reader Reader to reposition.
 * @param[in]     mark   Mark taken from the same reader with
 *                       `dmi_reader_mark`(3).
 *
 * @return `true` on success, `false` if the mark was taken from a reader
 *         reading another entity.
 */
__dmi_api bool dmi_reader_rewind(dmi_reader_t *reader, dmi_reader_mark_t mark);

/**
 * @brief Read data from the reader at the current position.
 *
 * Copies @p length bytes from the reader into the buffer pointed to by
 * @p ptr and advances the reader cursor.
 *
 * @param[in,out] reader Reader to read from.
 * @param[out]    ptr    Buffer that receives the data; must be at least
 *                       @p length bytes.
 * @param[in]     length Number of bytes to read.
 *
 * @return `true` on success, `false` if there are fewer than @p length bytes
 *         remaining in the reader.
 */
__dmi_api bool dmi_reader_get_bytes(dmi_reader_t *reader, void *ptr, size_t length);

/**
 * @brief Read data from the reader at a given offset without advancing the
 *        cursor.
 *
 * Copies @p length bytes starting at @p offset from the beginning of the
 * entity body into the buffer pointed to by @p ptr. The reader cursor is not
 * modified.
 *
 * @param[in]  reader Reader to read from.
 * @param[out] ptr    Buffer that receives the data; must be at least
 *                    @p length bytes.
 * @param[in]  offset Byte offset from the beginning of the entity body.
 * @param[in]  length Number of bytes to read.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         reader bounds.
 */
__dmi_api bool dmi_reader_get_bytes_at(const dmi_reader_t *reader, void *ptr, size_t offset, size_t length);

/**
 * @brief Advance the reader cursor without reading data.
 *
 * Moves the reader cursor forward by @p length bytes, effectively discarding
 * that many bytes from the current position.
 *
 * @param[in,out] reader Reader to advance.
 * @param[in]     length Number of bytes to skip.
 *
 * @return `true` on success, `false` if there are fewer than @p length bytes
 *         remaining in the reader.
 */
__dmi_api bool dmi_reader_skip(dmi_reader_t *reader, size_t length);

/**
 * @brief Advance the reader cursor from a mark taken earlier.
 *
 * Records of a declared length are stepped over by that length rather than
 * by the number of bytes read from them: mark the beginning of the record,
 * read whatever fields it is known to hold, then advance by the length of
 * the record from the mark.
 *
 * Counting from the current position is what `dmi_reader_skip`(3) does.
 *
 * @param[in,out] reader Reader to advance.
 * @param[in]     from   Mark to count @p length from, taken from the same
 *                       reader with `dmi_reader_mark`(3).
 * @param[in]     length Number of bytes to advance by.
 *
 * @return `true` on success, `false` if the resulting position is beyond the
 *         end of the reader, or if @p from was taken from a reader reading
 *         another entity.
 */
__dmi_api bool dmi_reader_skip_ex(
        dmi_reader_t      *reader,
        dmi_reader_mark_t  from,
        size_t             length);

/**
 * @brief Return the current position of the reader cursor.
 *
 * The position is the offset of the next byte to be read from the beginning
 * of the entity body, so reading a field right after the call gives the
 * offset of that field within the structure.
 *
 * @param[in] reader Reader to query.
 *
 * @return Offset of the cursor from the beginning of the entity body, or zero
 *         if @p reader is @c nullptr.
 */
__dmi_api size_t dmi_reader_tell(const dmi_reader_t *reader);

/**
 * @brief Return the number of unread bytes in the reader.
 *
 * @param[in] reader Reader to query.
 *
 * @return Number of bytes from the current cursor position to the end of the
 *         entity body.
 */
__dmi_api size_t dmi_reader_remaining(const dmi_reader_t *reader);

/**
 * @brief Check whether all data in the reader has been consumed.
 *
 * @param[in] reader Reader to check.
 *
 * @return `true` if there are no remaining bytes to read, `false` otherwise.
 */
__dmi_api bool dmi_reader_is_done(const dmi_reader_t *reader);

/**
 * @brief Check if the reader contains at least the given number of bytes.
 *
 * @param[in] reader Reader to check.
 * @param[in] length Number of bytes.
 *
 * @return `true` if at least @p length bytes remain from the current
 *         position, `false` otherwise.
 */
__dmi_api bool dmi_reader_has(const dmi_reader_t *reader, size_t length);

/**
 * @brief Reset the reader cursor to the beginning.
 *
 * Repositions the reader cursor back to the start of the entity body so that
 * it can be read again from the beginning.
 *
 * @param[in,out] reader Reader to reset.
 */
__dmi_api void dmi_reader_reset(dmi_reader_t *reader);

/**
 * @brief Decode binary data from the reader at the current position.
 *
 * Data is not copied: @p value refers to the data read by the reader, which is
 * the copy of the structure body with additional information applied, if
 * there is any. The data remains valid as long as the entity exists. The
 * reader cursor is advanced.
 *
 * @param[in,out] reader Reader to read from.
 * @param[in]     length Number of bytes to reference, may be zero.
 * @param[out]    value  Binary data descriptor. Data pointer is set to
 *                       @c nullptr if @p length is zero.
 *
 * @return `true` on success, `false` if there are fewer than @p length bytes
 *         remaining in the reader.
 */
__dmi_api bool dmi_reader_get_binary(dmi_reader_t *reader, size_t length, dmi_binary_t *value);

__END_DECLS

/**
 * @internal
 * @def __dmi_reader_get(__reader, __type, __pvalue, __decoder)
 * @brief Implementation helper for sequential get macros.
 */
#define __dmi_reader_get(__reader, __type, __pvalue, __decoder)                  \
        ({                                                                       \
            __type __value;                                                      \
            bool rv = dmi_reader_get_bytes(__reader, &__value, sizeof(__value)); \
            if (rv)                                                              \
                *(__pvalue) = (__dmi_typeof(*(__pvalue)))__decoder(__value);     \
            rv;                                                                  \
        })

/**
 * @def dmi_reader_get(__reader, __type, __pvalue)
 * @brief Read and decode a value from the reader at the current position.
 *
 * Reads `sizeof(__type)` bytes from the reader, converts the value from SMBIOS
 * wire format (little-endian) into host byte order using `dmi_decode`, and
 * stores the result in the object pointed to by @p __pvalue. The reader cursor
 * is advanced.
 *
 * @param[in,out] __reader Reader to read from.
 * @param[in]     __type   Wire-format type to read (e.g., `dmi_word_t`).
 * @param[out]    __pvalue Pointer to the object that receives the decoded
 *                         value.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_reader_get(__reader, __type, __pvalue) \
        __dmi_reader_get(__reader, __type, __pvalue, dmi_decode)

/**
 * @def dmi_reader_get_bcd(__reader, __type, __pvalue)
 * @brief Read and decode a BCD-encoded value from the reader at the current
 *        position.
 *
 * Reads `sizeof(__type)` bytes from the reader, decodes the value from BCD
 * encoding using `dmi_decode_bcd`, and stores the result in the object pointed
 * to by @p __pvalue. The reader cursor is advanced.
 *
 * @param[in,out] __reader Reader to read from.
 * @param[in]     __type   Wire-format type to read (e.g., `dmi_byte_t`).
 * @param[out]    __pvalue Pointer to the object that receives the decoded
 *                         value.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_reader_get_bcd(__reader, __type, __pvalue) \
        __dmi_reader_get(__reader, __type, __pvalue, dmi_decode_bcd)

/**
 * @def dmi_reader_get_string(__reader, __pvalue)
 * @brief Read and decode an SMBIOS string reference from the reader at the
 *        current position.
 *
 * Reads a `dmi_string_t` index from the reader and resolves it to a string
 * pointer via `dmi_entity_string`. The reader cursor is advanced.
 *
 * @param[in,out] __reader Reader to read from.
 * @param[out]    __pvalue Pointer to a `const char *` that receives the
 *                         resolved string.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_reader_get_string(__reader, __pvalue)                                \
        ({                                                                       \
            dmi_string_t __value;                                                \
            bool rv = dmi_reader_get_bytes(__reader, &__value, sizeof(__value)); \
            if (rv)                                                              \
                *(__pvalue) = dmi_entity_string((__reader)->entity, __value);    \
            rv;                                                                  \
        })

/**
 * @def dmi_reader_get_uuid(__reader, __pvalue)
 * @brief Read and decode a UUID from the reader at the current position.
 *
 * Reads 16 bytes from the reader, converts the UUID from SMBIOS byte order
 * into RFC 4122 representation using `dmi_uuid_decode`, and stores the result
 * in the object pointed to by @p __pvalue. The reader cursor is advanced.
 *
 * @param[in,out] __reader Reader to read from.
 * @param[out]    __pvalue Pointer to a `dmi_uuid_t` that receives the decoded
 *                         UUID.
 *
 * @return `true` on success, `false` if there are not enough bytes remaining.
 */
#define dmi_reader_get_uuid(__reader, __pvalue)                                 \
        ({                                                                      \
            dmi_byte_t __value[16];                                             \
            bool rv = dmi_reader_get_bytes(__reader, __value, sizeof(__value)); \
            if (rv)                                                             \
                *(__pvalue) = dmi_uuid_decode(__value);                         \
            rv;                                                                 \
        })

/**
 * @internal
 * @def __dmi_reader_get_at(__reader, __type, __offset, __pvalue, __decoder)
 * @brief Implementation helper for random-access get macros.
 */
#define __dmi_reader_get_at(__reader, __type, __offset, __pvalue, __decoder)                  \
        ({                                                                                    \
            __type __value;                                                                   \
            bool rv = dmi_reader_get_bytes_at(__reader, &__value, __offset, sizeof(__value)); \
            if (rv)                                                                           \
                *(__pvalue) = (__dmi_typeof(*(__pvalue)))__decoder(__value);                  \
            rv;                                                                               \
        })

/**
 * @def dmi_reader_get_at(__reader, __offset, __type, __pvalue)
 * @brief Read and decode a value from the reader at a given offset.
 *
 * Reads `sizeof(__type)` bytes at @p __offset from the beginning of the entity
 * body, converts the value from SMBIOS wire format into host byte order using
 * `dmi_decode`, and stores the result in the object pointed to by @p __pvalue.
 * The reader cursor is not modified.
 *
 * @param[in]  __reader Reader to read from.
 * @param[in]  __offset Byte offset from the beginning of the entity body.
 * @param[in]  __type   Wire-format type to read (e.g., `dmi_word_t`).
 * @param[out] __pvalue Pointer to the object that receives the decoded value.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         reader bounds.
 */
#define dmi_reader_get_at(__reader, __offset, __type, __pvalue) \
        __dmi_reader_get_at(__reader, __type, __offset, __pvalue, dmi_decode)

/**
 * @def dmi_reader_get_bcd_at(__reader, __offset, __type, __pvalue)
 * @brief Read and decode a BCD-encoded value from the reader at a given
 *        offset.
 *
 * Reads `sizeof(__type)` bytes at @p __offset from the beginning of the entity
 * body, decodes the value from BCD encoding using `dmi_decode_bcd`, and stores
 * the result in the object pointed to by @p __pvalue. The reader cursor is not
 * modified.
 *
 * @param[in]  __reader Reader to read from.
 * @param[in]  __offset Byte offset from the beginning of the entity body.
 * @param[in]  __type   Wire-format type to read (e.g., `dmi_byte_t`).
 * @param[out] __pvalue Pointer to the object that receives the decoded value.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         reader bounds.
 */
#define dmi_reader_get_bcd_at(__reader, __offset, __type, __pvalue) \
        __dmi_reader_get_at(__reader, __type, __offset, __pvalue, dmi_decode_bcd)

/**
 * @def dmi_reader_get_string_at(__reader, __offset, __pvalue)
 * @brief Read and decode an SMBIOS string reference from the reader at a
 *        given offset.
 *
 * Reads a `dmi_string_t` index at @p __offset from the beginning of the
 * entity body and resolves it to a string pointer via `dmi_entity_string`. The
 * reader cursor is not modified.
 *
 * @param[in]  __reader Reader to read from.
 * @param[in]  __offset Byte offset from the beginning of the entity body.
 * @param[out] __pvalue Pointer to a `const char *` that receives the resolved
 *                      string.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         reader bounds.
 */
#define dmi_reader_get_string_at(__reader, __offset, __pvalue)                                \
        ({                                                                                    \
            dmi_string_t __value;                                                             \
            bool rv = dmi_reader_get_bytes_at(__reader, &__value, __offset, sizeof(__value)); \
            if (rv)                                                                           \
                *(__pvalue) = dmi_entity_string((__reader)->entity, __value);                 \
            rv;                                                                               \
        })

/**
 * @def dmi_reader_get_uuid_at(__reader, __offset, __pvalue)
 * @brief Read and decode a UUID from the reader at a given offset.
 *
 * Reads 16 bytes at @p __offset from the beginning of the entity body,
 * converts the UUID from SMBIOS byte order into RFC 4122 representation using
 * `dmi_uuid_decode`, and stores the result in the object pointed to by
 * @p __pvalue. The reader cursor is not modified.
 *
 * @param[in]  __reader Reader to read from.
 * @param[in]  __offset Byte offset from the beginning of the entity body.
 * @param[out] __pvalue Pointer to a `dmi_uuid_t` that receives the decoded
 *                      UUID.
 *
 * @return `true` on success, `false` if the requested range exceeds the
 *         reader bounds.
 */
#define dmi_reader_get_uuid_at(__reader, __offset, __pvalue)                                 \
        ({                                                                                   \
            dmi_byte_t __value[16];                                                          \
            bool rv = dmi_reader_get_bytes_at(__reader, __value, __offset, sizeof(__value)); \
            if (rv)                                                                          \
                *(__pvalue) = dmi_uuid_decode(__value);                                      \
            rv;                                                                              \
        })

#endif // !OPENDMI_READER_H
