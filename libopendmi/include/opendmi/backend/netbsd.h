//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_BACKEND_NETBSD_H
#define OPENDMI_BACKEND_NETBSD_H

#pragma once

#include <opendmi/backend.h>

#define DMI_NETBSD_DEV_MEMORY    "/dev/mem"
#define DMI_NETBSD_DEV_SMBIOS    "/dev/smbios"
#define DMI_NETBSD_SYSCTL_SMBIOS "machdep.smbios"

extern __dmi_api dmi_backend_t dmi_netbsd_backend;

#endif // !OPENDMI_BACKEND_NETBSD_H
