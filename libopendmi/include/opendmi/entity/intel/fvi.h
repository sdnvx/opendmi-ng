//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_FVI_H
#define OPENDMI_ENTITY_INTEL_FVI_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_intel_fvi      dmi_intel_fvi_t;
typedef struct dmi_intel_fvi_item dmi_intel_fvi_item_t;

/**
 * @brief Version of a firmware component, as listed by Intel firmware version
 * information.
 *
 * Each part of the version is unspecified on its own, and components which
 * carry a single number, e.g. a revision identifier, set that part only.
 */
struct dmi_intel_fvi_item
{
    /**
     * @brief Name of the component, e.g. `Reference Code - CPU`.
     */
    const char *component;

    /**
     * @brief Version of the component written as a string, or a state of the
     * component, e.g. `Disabled`. @c nullptr if the component has none.
     */
    const char *version_string;

    /**
     * @brief Major version. Set to `UINT8_MAX` when unspecified.
     */
    uint8_t major;

    /**
     * @brief Minor version. Set to `UINT8_MAX` when unspecified.
     */
    uint8_t minor;

    /**
     * @brief Revision. Set to `UINT8_MAX` when unspecified.
     */
    uint8_t revision;

    /**
     * @brief Build number. Set to `UINT16_MAX` when unspecified.
     */
    uint16_t build;
};

/**
 * @brief Intel firmware version information (type 221).
 *
 * The Intel reference code lists the versions of the firmware components of
 * a platform in several structures of this type, one for each module of the
 * reference code, e.g. the CPU, the Management Engine or the PCH, whose first
 * item is the version of the reference code module itself.
 */
struct dmi_intel_fvi
{
    /**
     * @brief Number of items.
     */
    size_t item_count;

    /**
     * @brief Versions of the firmware components.
     */
    dmi_intel_fvi_item_t *items;
};

/**
 * @brief Intel firmware version information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_fvi_spec;

#endif // !OPENDMI_ENTITY_INTEL_FVI_H
