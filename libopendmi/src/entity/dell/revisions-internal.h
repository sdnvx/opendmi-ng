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
bool dmi_dell_revisions_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);
bool dmi_dell_revisions_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

#endif // !OPENDMI_ENTITY_REVISIONS_INTERNAL_H
