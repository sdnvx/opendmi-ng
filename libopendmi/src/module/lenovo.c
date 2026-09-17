//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/lenovo.h>

/**
 * @brief Lenovo extension module.
 */
const dmi_module_t dmi_lenovo_module =
{
    .code     = "lenovo",
    .name     = "IBM/Lenovo extensions",
    .entities = nullptr
};
