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

static const dmi_data_t dmi_base64_decode_table[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,
    0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63,
    0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

static const int dmi_base64_mod_table[] = {0, 2, 1};

char *dmi_base64_encode(
        const dmi_data_t *data,
        size_t            data_length,
        size_t           *poutput_length)
{
    size_t  output_length;
    char   *output_data;

    if (data == nullptr)
        return nullptr;
    if (data_length == 0)
        return nullptr;

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
        return nullptr;
    if ((data_length == 0) or (data_length % 4 != 0))
        return nullptr;

    output_length = data_length / 4 * 3;

    if (data[data_length - 1] == '=')
        output_length--;
    if (data[data_length - 2] == '=')
        output_length--;

    output_data = dmi_alloc(nullptr, output_length);
    if (output_data == nullptr)
        return nullptr;

    for (size_t i = 0, j = 0; i < data_length;) {
        dmi_data_t *raw_data = dmi_cast(raw_data, data);

        uint32_t sextet_a = (raw_data[i] == '=') ? 0 & i++ : dmi_base64_decode_table[raw_data[i++]];
        uint32_t sextet_b = (raw_data[i] == '=') ? 0 & i++ : dmi_base64_decode_table[raw_data[i++]];
        uint32_t sextet_c = (raw_data[i] == '=') ? 0 & i++ : dmi_base64_decode_table[raw_data[i++]];
        uint32_t sextet_d = (raw_data[i] == '=') ? 0 & i++ : dmi_base64_decode_table[raw_data[i++]];

        uint32_t triple =
            (sextet_a << 3 * 6) +
            (sextet_b << 2 * 6) +
            (sextet_c << 1 * 6) +
            (sextet_d << 0 * 6);

        if (j < output_length)
            output_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length)
            output_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length)
            output_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    if (poutput_length != nullptr)
        *poutput_length = output_length;

    return output_data;
}
