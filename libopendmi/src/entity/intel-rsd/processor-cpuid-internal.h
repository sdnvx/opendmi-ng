//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_RSD_PROCESSOR_CPUID_INTERNAL_H
#define OPENDMI_ENTITY_INTEL_RSD_PROCESSOR_CPUID_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel-rsd/processor-cpuid.h>

/**
 * @brief Size of the leaf in the structure: EAX, EBX, ECX and EDX.
 */
#define DMI_INTEL_RSD_CPUID_LEAF_SIZE 16

extern const dmi_name_set_t dmi_intel_rsd_cpuid_subtype_names;

extern const dmi_attribute_t dmi_intel_rsd_cpuid_leaf_attrs[];

/**
 * @internal
 * @brief Decode the CPUID leaves of a processor.
 *
 * @details Leaves of the known subtypes are decoded in the order the subtype
 * lists them, and the data of the unknown ones is kept as it is stored.
 *
 * @param[in,out] decoder Decoder of the structure.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_intel_rsd_processor_cpuid_decode(dmi_decoder_t *decoder);

/**
 * @internal
 * @brief Encode the CPUID leaves of a processor.
 *
 * @details Leaves of the known subtypes are written in the order the subtype
 * lists them, and the data of the unknown ones as it is stored.
 *
 * @param[in,out] encoder Encoder of the structure.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_intel_rsd_processor_cpuid_encode(dmi_encoder_t *encoder);

/**
 * @internal
 * @brief Free the leaves of a decoded structure.
 *
 * @param[in,out] entity Structure being cleaned up.
 */
void dmi_intel_rsd_processor_cpuid_cleanup(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_INTEL_RSD_PROCESSOR_CPUID_INTERNAL_H
