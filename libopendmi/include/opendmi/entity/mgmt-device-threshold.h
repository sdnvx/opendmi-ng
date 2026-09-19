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
__dmi_api void dmi_mgmt_device_threshold_set_component(dmi_entity_t *entity, dmi_type_t type);

__END_DECLS

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
     * management device components, `DMI_TYPE_INVALID` if the thresholds are
     * not used, or are used by components of different types.
     */
    dmi_type_t component_type;

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
