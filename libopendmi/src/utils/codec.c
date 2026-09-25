//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/utils/codec.h>

uintmax_t __dmi_decode_bcd(const dmi_byte_t *value, size_t length)
{
    uintmax_t result = 0;
    uintmax_t factor = 1;

    assert(value != nullptr);
    assert(length > 0);

    while (length > 0) {
        result += (*value & 0x0F) * factor;
        result += ((*value & 0xF0) >> 4) * factor * 10;
        factor *= 100;

        length--, value++;
    }

    return result;
}

void __dmi_encode_bcd(uintmax_t value, dmi_byte_t *data, size_t length)
{
    assert(data != nullptr);
    assert(length > 0);

    while (length > 0) {
        *data = (dmi_byte_t)((value % 10) | (((value / 10) % 10) << 4));
        value /= 100;

        length--, data++;
    }
}
