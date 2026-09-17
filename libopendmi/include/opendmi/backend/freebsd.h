//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_BACKEND_FREEBSD_H
#define OPENDMI_BACKEND_FREEBSD_H

#pragma once

#include <opendmi/backend.h>

#define DMI_FREEBSD_DEV_MEMORY  "/dev/mem"
#define DMI_FREEBSD_KENV_SMBIOS "hint.smbios.0.mem"

extern __dmi_api dmi_backend_t dmi_freebsd_backend;

#endif // !OPENDMI_BACKEND_FREEBSD_H
