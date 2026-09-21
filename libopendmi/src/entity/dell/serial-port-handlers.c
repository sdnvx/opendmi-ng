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
uintmax_t dmi_dell_serial_port_convert_speed(uintmax_t raw)
{
    return raw * 100;
}
