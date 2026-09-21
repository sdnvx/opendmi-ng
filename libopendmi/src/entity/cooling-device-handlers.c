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
uintmax_t dmi_cooling_device_convert_speed(uintmax_t raw)
{
    return (raw != 0x8000u) ? (raw & 0x7FFFu) : raw;
}

