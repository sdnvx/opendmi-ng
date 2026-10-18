//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/base64.h>

static const char dmi_base64_encode_table[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

static const int dmi_base64_mod_table[] = {0, 2, 1};

/**
 * @internal
 * @brief Get the six bits a character of the alphabet stands for.
 *
 * @param[in] c Character of the text.
 *
 * @return Value of the character, or a negative value if it is not of the
 *         alphabet.
 */
static int dmi_base64_sextet(char c);

char *dmi_base64_encode(
        const dmi_data_t *data,
        size_t            data_length,
        size_t           *poutput_length)
{
    size_t  output_length;
    char   *output_data;

    if (data == nullptr)
        return dmi_trace_argument_null(nullptr, data, nullptr);
    if (data_length == 0)
        return dmi_trace_argument_invalid(nullptr, data_length, nullptr);

    output_length = 4 * ((data_length + 2) / 3);
    output_data   = dmi_alloc(nullptr, output_length + 1);

    if (output_data == nullptr)
        return nullptr;

    for (size_t i = 0, j = 0; i < data_length;) {
        uint32_t octet_a = data[i++];
        uint32_t octet_b = i < data_length ? data[i++] : 0;
        uint32_t octet_c = i < data_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        output_data[j++] = dmi_base64_encode_table[(triple >> 3 * 6) & 0x3F];
        output_data[j++] = dmi_base64_encode_table[(triple >> 2 * 6) & 0x3F];
        output_data[j++] = dmi_base64_encode_table[(triple >> 1 * 6) & 0x3F];
        output_data[j++] = dmi_base64_encode_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < dmi_base64_mod_table[data_length % 3]; i++)
        output_data[output_length - 1 - i] = '=';

    output_data[output_length] = 0;

    if (poutput_length != nullptr)
        *poutput_length = output_length;

    return output_data;
}

dmi_data_t *dmi_base64_decode(
        const char *data,
        size_t      data_length,
        size_t     *poutput_length)
{
    size_t      output_length;
    dmi_data_t *output_data;

    if (data == nullptr)
        return dmi_trace_argument_null(nullptr, data, nullptr);
    if ((data_length == 0) or (data_length % 4 != 0))
        return dmi_trace_argument_invalid(nullptr, data_length, nullptr);

    // Padding stands for the bytes the last group is short of, so it is found
    // at the end of the text alone
    size_t padding = 0;

    if (data[data_length - 1] == '=')
        padding++;
    if ((padding > 0) and (data[data_length - 2] == '='))
        padding++;

    output_length = data_length / 4 * 3 - padding;
    output_data   = dmi_alloc(nullptr, output_length);

    if (output_data == nullptr)
        return nullptr;

    for (size_t i = 0, j = 0; i < data_length; i += 4) {
        uint32_t triple = 0;

        for (size_t k = 0; k < 4; k++) {
            // Characters the padding takes the place of carry no bits
            int sextet = (i + k < data_length - padding) ? dmi_base64_sextet(data[i + k]) : 0;

            if (sextet < 0) {
                dmi_free(output_data);
                return dmi_trace_argument_invalid(nullptr, data, nullptr);
            }

            triple = (triple << 6) | (uint32_t)sextet;
        }

        for (int shift = 2 * 8; (shift >= 0) and (j < output_length); shift -= 8)
            output_data[j++] = (dmi_data_t)((triple >> shift) & 0xFF);
    }

    if (poutput_length != nullptr)
        *poutput_length = output_length;

    return output_data;
}

static int dmi_base64_sextet(char c)
{
    if ((c >= 'A') and (c <= 'Z'))
        return c - 'A';
    if ((c >= 'a') and (c <= 'z'))
        return c - 'a' + 26;
    if ((c >= '0') and (c <= '9'))
        return c - '0' + 52;
    if (c == '+')
        return 62;
    if (c == '/')
        return 63;

    return -1;
}
