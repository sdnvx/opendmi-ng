//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_STRING_PROPERTY_INTERNAL_H
#define OPENDMI_ENTITY_STRING_PROPERTY_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/string-property.h>

// Operation handlers, see string-property-handlers.c
bool dmi_string_property_link(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_STRING_PROPERTY_INTERNAL_H
