//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_POWER_CONTROLS_H
#define OPENDMI_ENTITY_POWER_CONTROLS_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_power_controls dmi_power_controls_t;

/**
 * @brief System power controls structure (type 25).
 *
 * This structure describes the attributes for controlling the main power
 * supply to the system.
 *
 * Software that interprets this structure uses the month, day, hour, minute,
 * and second values to determine the number of seconds until the next power-on
 * of the system. The presence of this structure implies that a timed power-on
 * facility is available for the system.
 */
struct dmi_power_controls
{
    /**
     * @brief The month on which the next scheduled power-on is to occur, in
     * the range `1` to `12`.
     */
    unsigned short poweron_month;

    /**
     * @brief The day-of-month on which the next scheduled power-on is to
     * occur, in the range `1` to `31`.
     */
    unsigned short poweron_day;

    /**
     * @brief The hour on which the next scheduled power-on is to occur, in
     * the range `0` to `23`.
     */
    unsigned short poweron_hour;

    /**
     * @brief The minute on which the next scheduled power-on is to occur, in
     * the range `0` to `59`.
     */
    unsigned short poweron_minute;

    /**
     * @brief The second on which the next scheduled power-on is to occur, in
     * the range `0` to `59`.
     */
    unsigned short poweron_second;
};

/**
 * @brief System power controls entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_power_controls_spec;

#endif // !OPENDMI_ENTITY_POWER_CONTROLS_H
