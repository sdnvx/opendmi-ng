//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/utils/utf8.h>

/**
 * @internal
 * @brief U+FFFD replacement character.
 */
static const char dmi_utf8_replacement[] = "\xEF\xBF\xBD";

/**
 * @internal
 * @brief Decode a valid UTF-8 sequence at the beginning of a string.
 *
 * @details Overlong encodings, surrogates and code points out of range are
 * not valid. String terminator is never a part of a valid multibyte
 * sequence, since it is not a continuation byte.
 *
 * @param[in]  str  String to decode.
 * @param[out] code Variable to store the code point in.
 *
 * @return Length of the sequence, or zero if the sequence is not valid.
 */
static size_t dmi_utf8_decode(const unsigned char *str, uint32_t *code);

/**
 * @internal
 * @brief Get the length of a valid and accepted character at the beginning
 * of a string.
 *
 * @details For rejected characters, the number of bytes to replace is
 * returned via @p skip: the whole sequence for valid characters rejected by
 * the filter, and a single byte for invalid sequences.
 *
 * @param[in]  str    String to check.
 * @param[in]  filter Filter of the accepted code points, or `nullptr` to
 *                    accept every valid one.
 * @param[out] skip   Variable to store the number of bytes to replace in, if
 *                    the character is rejected.
 *
 * @return Length of the character, or zero if it is rejected.
 */
static size_t dmi_utf8_accept(const unsigned char *str, dmi_utf8_filter_fn *filter, size_t *skip);

bool dmi_utf8_is_valid_ex(const char *str, dmi_utf8_filter_fn *filter)
{
    assert(str != nullptr);

    const unsigned char *pos = dmi_cast(pos, str);

    while (*pos != 0) {
        size_t skip;
        size_t length = dmi_utf8_accept(pos, filter, &skip);
        if (length == 0)
            return false;

        pos += length;
    }

    return true;
}

char *dmi_utf8_repair_ex(dmi_context_t *context, const char *str, dmi_utf8_filter_fn *filter)
{
    assert(str != nullptr);

    const unsigned char *pos;
    size_t size = 1;

    // Calculate resulting string size
    for (pos = dmi_cast(pos, str); *pos != 0; ) {
        size_t skip;
        size_t length = dmi_utf8_accept(pos, filter, &skip);

        if (length != 0) {
            size += length;
            pos  += length;
        } else {
            size += sizeof(dmi_utf8_replacement) - 1;
            pos  += skip;
        }
    }

    char *result = dmi_alloc(context, size);
    if (result == nullptr)
        return nullptr;

    char *out = result;

    for (pos = dmi_cast(pos, str); *pos != 0; ) {
        size_t skip;
        size_t length = dmi_utf8_accept(pos, filter, &skip);

        if (length != 0) {
            memcpy(out, pos, length);
            out += length;
            pos += length;
        } else {
            memcpy(out, dmi_utf8_replacement, sizeof(dmi_utf8_replacement) - 1);
            out += sizeof(dmi_utf8_replacement) - 1;
            pos += skip;
        }
    }

    *out = 0;

    return result;
}

bool dmi_utf8_is_control(uint32_t code)
{
    return (code < 0x20u) or ((code >= 0x7Fu) and (code <= 0x9Fu));
}

char *dmi_utf8_escape(dmi_context_t *context, const char *str)
{
    assert(str != nullptr);

    static const char digits[] = "0123456789ABCDEF";

    const unsigned char *pos;
    size_t size = 1;

    // Calculate resulting string size
    for (pos = dmi_cast(pos, str); *pos != 0; ) {
        uint32_t code;
        size_t length = dmi_utf8_decode(pos, &code);

        if (length == 0) {
            size += sizeof("\\xNN") - 1;
            pos++;
        } else if (dmi_utf8_is_control(code)) {
            size += (code < 0x80u) ? sizeof("\\xNN") - 1 : sizeof("\\u00NN") - 1;
            pos  += length;
        } else {
            size += length;
            pos  += length;
        }
    }

    char *result = dmi_alloc(context, size);
    if (result == nullptr)
        return nullptr;

    char *out = result;

    for (pos = dmi_cast(pos, str); *pos != 0; ) {
        uint32_t code;
        size_t length = dmi_utf8_decode(pos, &code);

        if ((length != 0) and not dmi_utf8_is_control(code)) {
            memcpy(out, pos, length);
            out += length;
            pos += length;
            continue;
        }

        // Invalid bytes are escaped by their values, and valid control
        // characters by their code points
        unsigned value;

        *out++ = '\\';
        if (length == 0) {
            value = *pos++;
            *out++ = 'x';
        } else if (code < 0x80u) {
            value = code;
            pos  += length;
            *out++ = 'x';
        } else {
            value = code;
            pos  += length;
            *out++ = 'u';
            *out++ = '0';
            *out++ = '0';
        }

        *out++ = digits[(value >> 4) & 0x0Fu];
        *out++ = digits[value & 0x0Fu];
    }

    *out = 0;

    return result;
}

static size_t dmi_utf8_accept(const unsigned char *str, dmi_utf8_filter_fn *filter, size_t *skip)
{
    uint32_t code;
    size_t length = dmi_utf8_decode(str, &code);

    if (length == 0) {
        *skip = 1;
        return 0;
    }

    if ((filter != nullptr) and not filter(code)) {
        *skip = length;
        return 0;
    }

    return length;
}

static size_t dmi_utf8_decode(const unsigned char *str, uint32_t *code)
{
    size_t length;
    uint32_t value;
    uint32_t minimum;

    if (str[0] < 0x80u) {
        *code = str[0];
        return 1;
    }

    if ((str[0] & 0xE0u) == 0xC0u) {
        length  = 2;
        value   = str[0] & 0x1Fu;
        minimum = 0x80u;
    } else if ((str[0] & 0xF0u) == 0xE0u) {
        length  = 3;
        value   = str[0] & 0x0Fu;
        minimum = 0x800u;
    } else if ((str[0] & 0xF8u) == 0xF0u) {
        length  = 4;
        value   = str[0] & 0x07u;
        minimum = 0x10000u;
    } else {
        return 0;
    }

    for (size_t i = 1; i < length; i++) {
        if ((str[i] & 0xC0u) != 0x80u)
            return 0;

        value = (value << 6) | (str[i] & 0x3Fu);
    }

    // Overlong encodings, surrogates and out of range code points
    if ((value < minimum) or (value > 0x10FFFFu))
        return 0;
    if ((value >= 0xD800u) and (value <= 0xDFFFu))
        return 0;

    *code = value;

    return length;
}
