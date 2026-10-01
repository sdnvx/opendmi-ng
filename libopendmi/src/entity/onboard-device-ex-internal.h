//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_ONBOARD_DEVICE_EX_INTERNAL_H
#define OPENDMI_ENTITY_ONBOARD_DEVICE_EX_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/onboard-device-ex.h>

/**
 * @internal
 * @brief Check that no other device of the same type shares the instance of
 * the device.
 *
 * @details Devices of the same type are told apart by their instances, so no
 * two of them share one.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_onboard_device_ex_lint_instance(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_ONBOARD_DEVICE_EX_INTERNAL_H
