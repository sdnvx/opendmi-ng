//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_PROLIANT_INFO_INTERNAL_H
#define OPENDMI_ENTITY_HPE_PROLIANT_INFO_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/proliant-info.h>

/**
 * @internal
 * @brief Tell the features of the platform from the bits of the
 * miscellaneous features.
 *
 * @details See the `is_icru` and `is_uefi` members of
 * `dmi_hpe_proliant_info_t` for the bits each feature is told by.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_proliant_info_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_PROLIANT_INFO_INTERNAL_H
