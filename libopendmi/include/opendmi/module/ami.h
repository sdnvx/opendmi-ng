//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_AMI_H
#define OPENDMI_MODULE_AMI_H

#pragma once

#include <opendmi/module.h>

/**
 * @brief AMI structure type identifiers.
 */
typedef enum dmi_ami_type
{
    DMI_TYPE_AMI_221 = 221 ///< Type 221
} dmi_ami_type_t;

__BEGIN_DECLS

extern const dmi_module_t dmi_ami_module;

__END_DECLS

#endif // !OPENDMI_MODULE_AMI_H
