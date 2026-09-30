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

bool dmi_hpe_nic_mac_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_NIC_MAC_INTERNAL_H
