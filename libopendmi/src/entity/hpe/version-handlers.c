//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <ctype.h>
#include <stdio.h>

#include <opendmi/context.h>
#include <opendmi/platform.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "version-internal.h"

/**
 * @internal
 * @brief Version data of a version indicator, and the place to format it
 * into.
 */
typedef struct dmi_hpe_version_text
{
    /**
     * @brief Buffer to format the version into.
     */
    char *buf;

    /**
     * @brief Size of the buffer.
     */
    size_t size;

    /**
     * @brief Generation of the server, since the layout of a format may
     * depend on it.
     */
    unsigned generation;

    /**
     * @brief Bytes of the version data.
     */
    unsigned d[12];
} dmi_hpe_version_text_t;

/**
 * @internal
 * @brief Function formatting the version data of a single data format.
 *
 * @param[in] text Version data and the place to format it into.
 *
 * @return Number of the characters written, or a negative value if the data
 *         cannot be formatted.
 */
typedef int dmi_hpe_version_format_fn(const dmi_hpe_version_text_t *text);

/**
 * @internal
 * @brief Format the version data of a version indicator.
 *
 * @param[in,out] info       Version indicator, whose buffer is written.
 * @param[in]     generation Generation of the server.
 *
 * @return Number of the characters written, or a negative value if the
 *         format is not known.
 */
static int dmi_hpe_version_format(dmi_hpe_version_t *info, unsigned generation);

/**
 * @internal
 * @brief Read a word of the version data, which is held low byte first.
 *
 * @param[in] text  Version data.
 * @param[in] index Index of the low byte.
 *
 * @return Value of the word.
 */
static inline unsigned dmi_hpe_version_word(const dmi_hpe_version_text_t *text, size_t index);

/**
 * @internal
 * @brief Read a double word of the version data, which is held low word
 * first.
 *
 * @param[in] text  Version data.
 * @param[in] index Index of the low byte.
 *
 * @return Value of the double word.
 */
static inline unsigned long dmi_hpe_version_dword(const dmi_hpe_version_text_t *text, size_t index);

//
// Formats of the version data, by their numbers. Format 0 carries no version
// data, and format 3 is reserved.
//
static dmi_hpe_version_format_fn dmi_hpe_version_format_1;
static dmi_hpe_version_format_fn dmi_hpe_version_format_2;
static dmi_hpe_version_format_fn dmi_hpe_version_format_4;
static dmi_hpe_version_format_fn dmi_hpe_version_format_5;
static dmi_hpe_version_format_fn dmi_hpe_version_format_6;
static dmi_hpe_version_format_fn dmi_hpe_version_format_7;
static dmi_hpe_version_format_fn dmi_hpe_version_format_8;
static dmi_hpe_version_format_fn dmi_hpe_version_format_9;
static dmi_hpe_version_format_fn dmi_hpe_version_format_10;
static dmi_hpe_version_format_fn dmi_hpe_version_format_11;
static dmi_hpe_version_format_fn dmi_hpe_version_format_12;
static dmi_hpe_version_format_fn dmi_hpe_version_format_13;
static dmi_hpe_version_format_fn dmi_hpe_version_format_14;
static dmi_hpe_version_format_fn dmi_hpe_version_format_15;
static dmi_hpe_version_format_fn dmi_hpe_version_format_16;
static dmi_hpe_version_format_fn dmi_hpe_version_format_17;
static dmi_hpe_version_format_fn dmi_hpe_version_format_18;
static dmi_hpe_version_format_fn dmi_hpe_version_format_19;
static dmi_hpe_version_format_fn dmi_hpe_version_format_20;

static dmi_hpe_version_format_fn *const dmi_hpe_version_formats[] =
{
    [1]  = dmi_hpe_version_format_1,
    [2]  = dmi_hpe_version_format_2,
    [4]  = dmi_hpe_version_format_4,
    [5]  = dmi_hpe_version_format_5,
    [6]  = dmi_hpe_version_format_6,
    [7]  = dmi_hpe_version_format_7,
    [8]  = dmi_hpe_version_format_8,
    [9]  = dmi_hpe_version_format_9,
    [10] = dmi_hpe_version_format_10,
    [11] = dmi_hpe_version_format_11,
    [12] = dmi_hpe_version_format_12,
    [13] = dmi_hpe_version_format_13,
    [14] = dmi_hpe_version_format_14,
    [15] = dmi_hpe_version_format_15,
    [16] = dmi_hpe_version_format_16,
    [17] = dmi_hpe_version_format_17,
    [18] = dmi_hpe_version_format_18,
    [19] = dmi_hpe_version_format_19,
    [20] = dmi_hpe_version_format_20
};

bool dmi_hpe_version_derive(dmi_entity_t *entity)
{
    dmi_hpe_version_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_version));
    if (info == nullptr)
        return false;

    const dmi_platform_t *platform = dmi_get_platform(dmi_entity_context(entity));
    unsigned generation = (platform != nullptr) ? platform->generation : 0;

    int length = dmi_hpe_version_format(info, generation);
    if ((length > 0) and ((size_t)length < sizeof(info->version_buffer)))
        info->version = info->version_buffer;

    return true;
}

static int dmi_hpe_version_format(dmi_hpe_version_t *info, unsigned generation)
{
    if (info->data_format >= countof(dmi_hpe_version_formats))
        return -1;

    dmi_hpe_version_format_fn *format = dmi_hpe_version_formats[info->data_format];
    if (format == nullptr)
        return -1;

    dmi_hpe_version_text_t text = {
        .buf        = info->version_buffer,
        .size       = sizeof(info->version_buffer),
        .generation = generation
    };

    static_assert(countof(text.d) == countof(info->version_data));
    for (size_t i = 0; i < countof(text.d); i++)
        text.d[i] = info->version_data[i];

    return format(&text);
}

static inline unsigned dmi_hpe_version_word(const dmi_hpe_version_text_t *text, size_t index)
{
    return text->d[index] | (text->d[index + 1] << 8);
}

static inline unsigned long dmi_hpe_version_dword(const dmi_hpe_version_text_t *text, size_t index)
{
    return (unsigned long)dmi_hpe_version_word(text, index) |
           ((unsigned long)dmi_hpe_version_word(text, index + 2) << 16);
}

static int dmi_hpe_version_format_1(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    // High bit of the first byte tells that the version has a bank
    if (d[0] & 0x80)
        return snprintf(text->buf, text->size, "0x%02X B.0x%02X", d[1] & 0x7F, d[0] & 0x7F);

    return snprintf(text->buf, text->size, "0x%02X", d[1] & 0x7F);
}

static int dmi_hpe_version_format_2(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "%u.%u", d[0] >> 4, d[0] & 0x0F);
}

static int dmi_hpe_version_format_4(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "%u.%u.%u", d[0] >> 4, d[0] & 0x0F, d[1] & 0x7F);
}

static int dmi_hpe_version_format_5(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    // The layout of the format has changed with the generations
    switch (text->generation) {
    case DMI_HPE_GEN9:
        return dmi_hpe_version_format_4(text);

    case DMI_HPE_GEN10:
    case DMI_HPE_GEN10_PLUS:
        return snprintf(text->buf, text->size, "%u.%u.%u.%u", d[1] & 0x0F, d[3] & 0x0F,
                        d[5] & 0x0F, d[6] & 0x0F);

    default:
        return -1;
    }
}

static int dmi_hpe_version_format_6(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "%u.%u", d[1], d[0]);
}

static int dmi_hpe_version_format_7(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "v%u.%02u (%02u/%02u/%u)",
                    d[0], d[1], d[2], d[3], dmi_hpe_version_word(text, 4));
}

static int dmi_hpe_version_format_8(const dmi_hpe_version_text_t *text)
{
    return snprintf(text->buf, text->size, "%u.%u",
                    dmi_hpe_version_word(text, 4), dmi_hpe_version_word(text, 0));
}

static int dmi_hpe_version_format_9(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "%u.%u.%u", d[0], d[1], dmi_hpe_version_word(text, 2));
}

static int dmi_hpe_version_format_10(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "%u.%u.%u Build %u", d[0], d[1], d[2], d[3]);
}

static int dmi_hpe_version_format_11(const dmi_hpe_version_text_t *text)
{
    return snprintf(text->buf, text->size, "%u.%u %lu",
                    dmi_hpe_version_word(text, 2), dmi_hpe_version_word(text, 0),
                    dmi_hpe_version_dword(text, 4));
}

static int dmi_hpe_version_format_12(const dmi_hpe_version_text_t *text)
{
    return snprintf(text->buf, text->size, "%u.%u.%u.%u",
                    dmi_hpe_version_word(text, 0), dmi_hpe_version_word(text, 2),
                    dmi_hpe_version_word(text, 4), dmi_hpe_version_word(text, 6));
}

static int dmi_hpe_version_format_13(const dmi_hpe_version_text_t *text)
{
    return snprintf(text->buf, text->size, "%u", text->d[0]);
}

static int dmi_hpe_version_format_14(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "%u.%u.%u.%u", d[0], d[1], d[2], dmi_hpe_version_word(text, 3));
}

static int dmi_hpe_version_format_15(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "%u.%u.%u.%u (%02u/%02u/%u)",
                    dmi_hpe_version_word(text, 0), dmi_hpe_version_word(text, 2),
                    dmi_hpe_version_word(text, 4), dmi_hpe_version_word(text, 6),
                    d[8], d[9], dmi_hpe_version_word(text, 10));
}

static int dmi_hpe_version_format_16(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    // Version starts with four characters, which are not trusted to be
    // printable
    for (size_t i = 0; i < 4; i++) {
        if (not isprint((int)d[i]))
            return -1;
    }

    return snprintf(text->buf, text->size, "%c%c%c%c.%u%u",
                    (int)d[0], (int)d[1], (int)d[2], (int)d[3], d[4], d[5]);
}

static int dmi_hpe_version_format_17(const dmi_hpe_version_text_t *text)
{
    return snprintf(text->buf, text->size, "%08lX", dmi_hpe_version_dword(text, 0));
}

static int dmi_hpe_version_format_18(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "%u.%02u", d[0], d[1]);
}

static int dmi_hpe_version_format_19(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "0x%02x.0x%02x.0x%02x", d[0], d[1], d[2]);
}

static int dmi_hpe_version_format_20(const dmi_hpe_version_text_t *text)
{
    const unsigned *d = text->d;

    return snprintf(text->buf, text->size, "%u.%u.%u.%u", d[0], d[1], d[2], d[3]);
}
