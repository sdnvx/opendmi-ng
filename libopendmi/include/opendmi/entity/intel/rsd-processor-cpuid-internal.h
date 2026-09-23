//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_RSD_PROCESSOR_CPUID_INTERNAL_H
#define OPENDMI_ENTITY_RSD_PROCESSOR_CPUID_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel/rsd-processor-cpuid.h>

/**
 * @brief Size of the leaf in the structure: EAX, EBX, ECX and EDX.
 */
#define DMI_INTEL_RSD_CPUID_LEAF_SIZE 16

// Value names, see rsd-processor-cpuid-names.c
extern const dmi_name_set_t dmi_intel_rsd_cpuid_subtype_names;

// Operation handlers, see rsd-processor-cpuid-handlers.c
extern const dmi_attribute_t dmi_intel_rsd_cpuid_leaf_attrs[];
bool dmi_intel_rsd_processor_cpuid_decode(dmi_entity_t *entity);
bool dmi_intel_rsd_processor_cpuid_encode(dmi_writer_t *writer);
void dmi_intel_rsd_processor_cpuid_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_RSD_PROCESSOR_CPUID_INTERNAL_H
