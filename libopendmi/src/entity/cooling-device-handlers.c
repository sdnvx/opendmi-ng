//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cooling-device-internal.h>

//
// Speeds are carried in revolutions per minute, with the most significant bit
// set aside, and the value of exactly 0x8000 stands for "unknown".
//
bool dmi_cooling_device_decode_speed(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, (data->number != 0x8000u) ? (data->number & 0x7FFFu) : data->number);
}

bool dmi_cooling_device_encode_speed(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    data->number = dmi_field_get(field, value) & 0xFFFFu;

    return true;
}
