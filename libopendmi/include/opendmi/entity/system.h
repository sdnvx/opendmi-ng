//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_SYSTEM_H
#define OPENDMI_ENTITY_SYSTEM_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/utils/uuid.h>

typedef struct dmi_system dmi_system_t;

typedef enum dmi_system_wakeup_type
{
    DMI_SYSTEM_WAKEUP_TYPE_RESERVED      = 0x00, ///< Reserved
    DMI_SYSTEM_WAKEUP_TYPE_OTHER         = 0x01, ///< Other
    DMI_SYSTEM_WAKEUP_TYPE_UNKNOWN       = 0x02, ///< Unknown
    DMI_SYSTEM_WAKEUP_TYPE_APM_TIMER     = 0x03, ///< APM Timer
    DMI_SYSTEM_WAKEUP_TYPE_MODEM_RING    = 0x04, ///< Modem Ring
    DMI_SYSTEM_WAKEUP_TYPE_LAN_REMOTE    = 0x05, ///< LAN Remote
    DMI_SYSTEM_WAKEUP_TYPE_POWER_SWITCH  = 0x06, ///< Power Switch
    DMI_SYSTEM_WAKEUP_TYPE_PCI_PME       = 0x07, ///< PCI PME#
    DMI_SYSTEM_WAKEUP_TYPE_POWER_RESTORE = 0x08, ///< AC Power Restored
} dmi_system_wakeup_type_t;

/**
 * @brief System information structure (type 1).
 */
struct dmi_system
{
    /**
     * @brief Manufacturer name.
     */
    const char *vendor;

    /**
     * @brief Product name.
     */
    const char *product;

    /**
     * @brief Product version.
     */
    const char *version;

    /**
     * @brief Serial number.
     */
    const char *serial_number;

    /**
     * @brief UUID.
     */
    dmi_uuid_t uuid;

    /**
     * @brief Identifies the event that caused the system to power up.
     */
    dmi_system_wakeup_type_t wakeup_type;

    /**
     * @brief SKU number.
     *
     * This text string identifies a particular computer configuration for
     * sale. It is sometimes also called a product ID or purchase order number.
     * This number is frequently found in existing fields, but there is no
     * standard format. Typically for a given system board from a given OEM,
     * there are tens of unique processor, memory, hard drive, and optical
     * drive configurations.
     */
    const char *sku_number;

    /**
     * @brief Family name.
     *
     * This text string identifies the family to which a particular computer
     * belongs. A family refers to a set of computers that are similar but not
     * identical from a hardware or software point of view. Typically, a family
     * is composed of different computer models, which have different
     * configurations and pricing points. Computers in the same family often
     * have similar branding and cosmetic features.
     */
    const char *family;
};

/**
 * @brief System information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_system_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_system_wakeup_type_name(dmi_system_wakeup_type_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_SYSTEM_H
