//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_SVT_H
#define OPENDMI_ENTITY_INTEL_SVT_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_intel_svt           dmi_intel_svt_t;
typedef struct dmi_intel_svt_milestone dmi_intel_svt_milestone_t;

/**
 * @brief Milestone of the boot, which Intel Silicon View Technology reports.
 */
struct dmi_intel_svt_milestone
{
    /**
     * @brief Code of the milestone, e.g. `0x10` for the end of the memory
     * initialization.
     */
    uint8_t code;

    /**
     * @brief Name of the milestone, e.g. `Memory Init Complete`.
     */
    const char *name;
};

/**
 * @brief Intel Silicon View Technology milestones (type 222).
 *
 * Lists the milestones of the boot the firmware reports to the debug and
 * validation tools of Intel Silicon View Technology (SVT).
 */
struct dmi_intel_svt
{
    /**
     * @brief Version of the structure.
     */
    uint8_t version;

    /**
     * @brief Value whose meaning is not established, `0x0099` in all known
     * data.
     */
    uint16_t parameter;

    /**
     * @brief Number of milestones.
     */
    size_t milestone_count;

    /**
     * @brief Milestones.
     */
    dmi_intel_svt_milestone_t *milestones;
};

/**
 * @brief Intel Silicon View Technology milestones entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_svt_spec;

/**
 * @brief Intel Silicon View Technology milestones entity specification, for
 * the structures laid out with the parameter aligned to two bytes.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_svt_aligned_spec;

#endif // !OPENDMI_ENTITY_INTEL_SVT_H
