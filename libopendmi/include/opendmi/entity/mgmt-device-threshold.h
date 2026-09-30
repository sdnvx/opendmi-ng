//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MGMT_DEVICE_THRESHOLD_H
#define OPENDMI_ENTITY_MGMT_DEVICE_THRESHOLD_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_mgmt_device_threshold dmi_mgmt_device_threshold_t;

__BEGIN_DECLS

/**
 * @internal
 * @brief Set type of the component using the thresholds.
 *
 * Units of threshold values are defined by the component type. If the
 * thresholds are used by components of different types, the units are
 * ambiguous, and the values are shown as they are stored.
 *
 * @param[in] entity Management device threshold data entity.
 * @param[in] type   Component type.
 */
__dmi_api void dmi_mgmt_device_threshold_set_component(dmi_entity_t *entity, dmi_type_id_t type);

__END_DECLS

/**
 * @brief Management device threshold data structure (type 36).
 *
 * Holds the thresholds of a probe or cooling device of a management device.
 * The units of the values are the ones of the component using them:
 * millivolts, milliamperes, tenths of degree Celsius, or revolutions per
 * minute. A value is `SHRT_MIN` if the threshold is not available.
 */
struct dmi_mgmt_device_threshold
{
    /**
     * @brief Lower non-critical threshold for this component.
     */
    short lower_non_critical;

    /**
     * @brief Upper non-critical threshold for this component.
     */
    short upper_non_critical;

    /**
     * @brief Lower critical threshold for this component.
     */
    short lower_critical;

    /**
     * @brief Upper critical threshold for this component.
     */
    short upper_critical;

    /**
     * @brief Lower non-recoverable threshold for this component.
     */
    short lower_non_recoverable;

    /**
     * @brief Upper non-recoverable threshold for this component.
     */
    short upper_non_recoverable;

    /**
     * @brief Type of the component (probe or cooling device) using the
     * thresholds, which defines units of the values. Set when linking
     * management device components, `DMI_TYPE_ID_INVALID` if the thresholds are
     * not used, or are used by components of different types.
     */
    dmi_type_id_t component_type;

    /**
     * @brief Set if the thresholds are used by components of different types.
     */
    bool is_ambiguous;
};

/**
 * @brief Management device threshold data entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_mgmt_device_threshold_spec;

#endif // !OPENDMI_ENTITY_MGMT_DEVICE_THRESHOLD_H
