//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_TYPE_221_INTERNAL_H
#define OPENDMI_ENTITY_TYPE_221_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/ami/type-221.h>

// Operation handlers, see type-221-handlers.c
void dmi_ami_type_221_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_TYPE_221_INTERNAL_H
