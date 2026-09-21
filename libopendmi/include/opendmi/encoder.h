//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENCODER_H
#define OPENDMI_ENCODER_H

#pragma once

#include <opendmi/types.h>
#include <opendmi/stream.h>
#include <opendmi/utils/codec.h>
#include <opendmi/utils/version.h>

#ifndef DMI_ENCODER_T
#   define DMI_ENCODER_T
    typedef struct dmi_encoder dmi_encoder_t;
#endif // !DMI_ENCODER_T

/**
 * @brief How the bytes the model does not hold are written.
 *
 * Every value the decoded structure holds is written from the structure in
 * either mode, so that the bytes are derived from the model rather than
 * copied. The modes differ in the rest: the bits the specification reserves,
 * the choice between a plain field and its extended one, the numbers of the
 * strings, the tails of the records longer than the fields they are known to
 * hold, and where the structure ends.
 */
typedef enum dmi_encode_mode
{
    /**
     * @brief The bytes the model does not hold are taken from the data the
     * structure has been decoded from, so that encoding a structure which has
     * not been changed gives back the very bytes it has been decoded from.
     */
    DMI_ENCODE_MODE_PRESERVE,

    /**
     * @brief The bytes the model does not hold are written the way the
     * specification says they are, for the version the encoder is given,
     * which is how a structure is repaired or built from scratch.
     */
    DMI_ENCODE_MODE_CANONICAL
} dmi_encode_mode_t;

/**
 * @brief Sequential writer of an SMBIOS structure.
 *
 * Writes the formatted area of a structure, header included, and collects
 * the strings it refers to. In the preserve mode, the encoder reads the data
 * the structure has been decoded from alongside, so that the bytes the model
 * does not hold are found at the position they are written at.
 *
 * @note All fields are maintained internally. Do not modify them directly;
 *       use the encoder API instead.
 */
struct dmi_encoder
{
    /**
     * @brief Structure being encoded.
     */
    const dmi_entity_t *entity;

    /**
     * @brief How the bytes the model does not hold are written.
     */
    dmi_encode_mode_t mode;

    /**
     * @brief Version of the specification the canonical mode writes the
     * structure for, which decides the groups of the fields it carries.
     */
    dmi_version_t version;

    /**
     * @brief Data the structure has been decoded from, which the preserve
     * mode reads at the position it writes at.
     */
    dmi_stream_t source;

    /**
     * @brief Formatted area written so far, including the header.
     */
    dmi_byte_t *data;
    size_t      length;
    size_t      capacity;

    /**
     * @brief Strings the formatted area refers to, numbered from one, which
     * are copies the encoder owns.
     */
    char       **strings;
    size_t       string_count;
    size_t       string_capacity;
};

__BEGIN_DECLS

/**
 * @brief Initialize an encoder of a structure, and write its header.
 *
 * The length of the header is left for `dmi_encoder_finish()` to fill in,
 * once the length of the formatted area is known. In the preserve mode, the
 * strings of the structure are taken over as they are numbered, so that the
 * fields referring to them keep their numbers, and the strings nothing refers
 * to are kept too.
 *
 * @param[out] encoder Encoder to initialize.
 * @param[in]  entity  Structure to encode.
 * @param[in]  mode    How the bytes the model does not hold are written.
 * @param[in]  version Version of the specification the canonical mode writes
 *                     the structure for, ignored in the preserve mode.
 *
 * @error DMI_ERROR_NULL_ARGUMENT Encoder or entity is `nullptr`
 * @error DMI_ERROR_OUT_OF_MEMORY Buffers of the encoder cannot be allocated
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_encoder_initialize(
        dmi_encoder_t      *encoder,
        const dmi_entity_t *entity,
        dmi_encode_mode_t   mode,
        dmi_version_t       version);

/**
 * @brief Free the buffers of an encoder.
 *
 * @param[in,out] encoder Encoder to destroy, or @c nullptr.
 */
__dmi_api void dmi_encoder_destroy(dmi_encoder_t *encoder);

/**
 * @brief Write bytes at the current position.
 *
 * The position in the source data advances by the same number of bytes, so
 * that the two stay in step, as long as there are bytes left in it.
 *
 * @param[in,out] encoder Encoder to write to.
 * @param[in]     data    Bytes to write.
 * @param[in]     length  Number of the bytes.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer of the encoder cannot grow
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_encoder_write(dmi_encoder_t *encoder, const void *data, size_t length);

/**
 * @brief Write bytes the model does not hold at the current position: the
 * ones the source data has there in the preserve mode, and zeros in the
 * canonical one, or wherever the source data ends before them.
 *
 * @param[in,out] encoder Encoder to write to.
 * @param[in]     length  Number of the bytes.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer of the encoder cannot grow
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_encoder_copy(dmi_encoder_t *encoder, size_t length);

/**
 * @brief Write a reference to a string at the current position.
 *
 * The string is looked up among the ones the structure has been decoded
 * with, so that a field keeps the number of the string it refers to, and is
 * added to the strings of the encoder otherwise. A missing string is written
 * as zero.
 *
 * @param[in,out] encoder Encoder to write to.
 * @param[in]     value   String to refer to, or @c nullptr.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Buffers of the encoder cannot grow
 * @error DMI_ERROR_INVALID_ARGUMENT Structure refers to more strings than a
 *        byte can number
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_encoder_write_str(dmi_encoder_t *encoder, const char *value);

/**
 * @brief Read the source data at the current position without advancing.
 *
 * @param[in]  encoder Encoder to read from.
 * @param[out] data    Bytes the source holds at the position.
 * @param[in]  length  Number of the bytes to read.
 *
 * @return `true` if the source holds the bytes, `false` if it has none, e.g.
 *         in the canonical mode, or ends before them.
 */
__dmi_api bool dmi_encoder_peek(const dmi_encoder_t *encoder, void *data, size_t length);

/**
 * @brief Get the number of the bytes of the source left at the current
 * position, which is zero in the canonical mode.
 *
 * @param[in] encoder Encoder to query.
 *
 * @return Number of the bytes.
 */
__dmi_api size_t dmi_encoder_remaining(const dmi_encoder_t *encoder);

/**
 * @brief Get the current position, counted from the beginning of the
 * structure the way the specification counts it.
 *
 * @param[in] encoder Encoder to query.
 *
 * @return Position of the next byte to write.
 */
__dmi_api size_t dmi_encoder_tell(const dmi_encoder_t *encoder);

/**
 * @brief Complete the formatted area by filling in the length of the header.
 *
 * @param[in,out] encoder Encoder to complete.
 *
 * @error DMI_ERROR_INVALID_ARGUMENT Formatted area is longer than a byte can
 *        count
 *
 * @return `true` on success, `false` otherwise.
 */
__dmi_api bool dmi_encoder_finish(dmi_encoder_t *encoder);

__END_DECLS

/**
 * @def dmi_encoder_put(__encoder, __type, __value)
 * @brief Encode a value of the wire-format type and write it at the current
 *        position.
 *
 * @param[in,out] __encoder Encoder to write to.
 * @param[in]     __type    Wire-format type to write (e.g., `dmi_word_t`).
 * @param[in]     __value   Value to write.
 *
 * @return `true` on success, `false` otherwise.
 */
#define dmi_encoder_put(__encoder, __type, __value)                 \
        ({                                                          \
            __type __encoded = dmi_encode((__type)(__value));       \
            dmi_encoder_write(__encoder, &__encoded, sizeof(__type)); \
        })

#endif // !OPENDMI_ENCODER_H
