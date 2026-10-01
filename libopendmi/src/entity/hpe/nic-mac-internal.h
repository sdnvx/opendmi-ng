//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_NIC_MAC_INTERNAL_H
#define OPENDMI_ENTITY_HPE_NIC_MAC_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/nic-mac.h>

/**
 * @internal
 * @brief Derive the state of a NIC from its PCI location.
 *
 * @details NIC whose bus and device and function are all zero is disabled,
 * one whose bus and device and function have all bits set is not installed,
 * and any other one is installed.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_nic_mac_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_NIC_MAC_INTERNAL_H
