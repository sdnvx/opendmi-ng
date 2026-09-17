//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/acer.h>

/**
 * @brief Acer extension module.
 */
const dmi_module_t dmi_acer_module =
{
    .code     = "acer",
    .name     = "Acer extensions",
    .entities = nullptr
};
