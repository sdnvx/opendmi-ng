//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_PROCESSOR_EX_INTERNAL_H
#define OPENDMI_ENTITY_PROCESSOR_EX_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/processor-ex.h>

// Value names, see processor-ex-names.c
extern const dmi_name_set_t dmi_processor_arch_names;

// Operation handlers, see processor-ex-handlers.c
bool dmi_processor_ex_decode_data(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value);

#endif // !OPENDMI_ENTITY_PROCESSOR_EX_INTERNAL_H
