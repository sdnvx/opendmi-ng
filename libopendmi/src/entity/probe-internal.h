//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_PROBE_INTERNAL_H
#define OPENDMI_ENTITY_PROBE_INTERNAL_H

#pragma once

#include <limits.h>

#include <opendmi/field.h>
#include <opendmi/lint.h>

#include <opendmi/entity/probe.h>

/**
 * @internal
 * @brief Fields of a probe structure.
 *
 * @details Voltage, temperature and current probes are described by one
 * structure, so they are laid out the same way and differ only in the units
 * of the values they carry.
 */
#define dmi_probe_fields(__entity)                                  \
    DMI_FIELDS({                                                    \
        DMI_FIELD_STRING(__entity, description),                   \
                                                                    \
        DMI_FIELD_BITS(__entity, location, 5),                      \
        DMI_FIELD_BITS(__entity, status,   3),                      \
        DMI_FIELD_PAD(dmi_byte_t),                                  \
                                                                    \
        DMI_FIELD(__entity, maximum_value, dmi_word_t),             \
        DMI_FIELD(__entity, minimum_value, dmi_word_t),             \
        DMI_FIELD(__entity, resolution,    dmi_word_t),             \
        DMI_FIELD(__entity, tolerance,     dmi_word_t),             \
        DMI_FIELD(__entity, accuracy,      dmi_word_t),             \
        DMI_FIELD(__entity, oem_defined,   dmi_dword_t),            \
                                                                    \
        /* Probes which read nothing of their own carry no value */ \
        DMI_FIELD_PRESET(__entity, nominal_value, (short)SHRT_MIN), \
        DMI_FIELD_GROUP(),                                          \
        DMI_FIELD(__entity, nominal_value, dmi_word_t),             \
        {}                                                          \
    })

/**
 * @internal
 * @brief Check that the minimum value of a probe is not above the maximum
 * one, and that the nominal value is within them.
 *
 * @details Values which are unknown are left out of the check. The rule is
 * shared by the voltage, temperature and current probes, which are decoded
 * into the same structure.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked.
 */
void dmi_probe_lint_range(dmi_lint_t *lint, const dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_PROBE_INTERNAL_H
