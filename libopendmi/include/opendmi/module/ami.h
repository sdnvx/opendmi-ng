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
typedef enum dmi_ami_type_id
{
    DMI_TYPE_ID_AMI_FIREWIRE_GUID = 139 ///< FireWire GUID
} dmi_ami_type_id_t;

__BEGIN_DECLS

/** @brief FireWire GUID */
extern __dmi_api const dmi_type_t dmi_type_ami_firewire_guid;

extern __dmi_api const dmi_module_t dmi_ami_module;

__END_DECLS

#endif // !OPENDMI_MODULE_AMI_H
