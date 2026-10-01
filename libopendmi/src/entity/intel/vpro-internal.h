//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_VPRO_INTERNAL_H
#define OPENDMI_ENTITY_INTEL_VPRO_INTERNAL_H

#pragma once

#include <opendmi/field.h>

#include <opendmi/entity/intel/vpro.h>

/**
 * @internal
 * @brief Derive the values of a vPro structure from its raw fields.
 *
 * @details TCG version and the version of the verified access are taken from
 * the capabilities, and the wireless network controller is present unless
 * its device ID is zero or has either byte of all bits set. Older layout
 * holds the capabilities of the memory controller hub in place of the
 * version of the BIOS extension, which is told by a zero major and a nonzero
 * minor part of the version.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_intel_vpro_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_INTEL_VPRO_INTERNAL_H
