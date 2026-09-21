//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/serial-port-internal.h>

//
// Speeds are carried in hundreds of bits per second.
//
bool dmi_dell_serial_port_decode_speed(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, data->number * 100);
}

bool dmi_dell_serial_port_encode_speed(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    data->number = dmi_field_get(field, value) / 100;

    return true;
}
