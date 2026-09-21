//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/revisions-internal.h>

//
// Implementation version is carried as the major and the minor number, one
// byte each, which the version number puts in the order it counts them.
//
uintmax_t dmi_dell_revisions_convert_version(uintmax_t raw)
{
    return dmi_version((unsigned int)(raw & 0xFFu), (unsigned int)((raw >> 8) & 0xFFu), 0);
}
