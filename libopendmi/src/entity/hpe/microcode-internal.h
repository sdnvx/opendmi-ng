//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_MICROCODE_INTERNAL_H
#define OPENDMI_ENTITY_HPE_MICROCODE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/microcode.h>

/**
 * @internal
 * @brief Decode the processor signatures and the release dates of the
 * microcode patches.
 *
 * @details AMD platforms leave the base family out of the signature, which is
 * put back in place. Release dates are carried in BCD.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_microcode_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_MICROCODE_INTERNAL_H
