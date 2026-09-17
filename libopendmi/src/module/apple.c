//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/apple.h>

/**
 * @brief Apple extension module.
 */
const dmi_module_t dmi_apple_module =
{
    .code     = "apple",
    .name     = "Apple extensions",
    .entities = nullptr
};
