//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_COMMON_INTERNAL_H
#define OPENDMI_ENTITY_HPE_COMMON_INTERNAL_H

#pragma once

#include <opendmi/types.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/common.h>

/**
 * @internal
 * @brief Names of the values of a flag the structure may leave undefined.
 */
extern const dmi_name_set_t dmi_hpe_flag_names;

/**
 * @internal
 * @brief Names of the encryption statuses of a memory module or a drive.
 */
extern const dmi_name_set_t dmi_hpe_encryption_names;

#endif // !OPENDMI_ENTITY_HPE_COMMON_INTERNAL_H
