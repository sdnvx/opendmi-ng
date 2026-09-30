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

bool dmi_hpe_proliant_info_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_PROLIANT_INFO_INTERNAL_H
