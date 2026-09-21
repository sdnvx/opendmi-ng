//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_REVISIONS_INTERNAL_H
#define OPENDMI_ENTITY_REVISIONS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/revisions.h>

// Operation handlers, see revisions-handlers.c
uintmax_t dmi_dell_revisions_convert_version(uintmax_t raw);

#endif // !OPENDMI_ENTITY_REVISIONS_INTERNAL_H
