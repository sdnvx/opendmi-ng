//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_CURRENT_PROBE_H
#define OPENDMI_ENTITY_CURRENT_PROBE_H

#pragma once

#include <opendmi/entity/probe.h>

/**
 * @brief Electrical current probe structure (type 29), decoded into
 * `dmi_probe_t`, which all the probe types share.
 */
#ifndef DMI_CURRENT_PROBE_T
#   define DMI_CURRENT_PROBE_T
    typedef struct dmi_probe dmi_current_probe_t;
#endif // !DMI_CURRENT_PROBE_T

/**
 * @brief Electrical current probe entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_current_probe_spec;

#endif // !OPENDMI_ENTITY_CURRENT_PROBE_H
