//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_AMI_FIREWIRE_GUID_H
#define OPENDMI_ENTITY_AMI_FIREWIRE_GUID_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_AMI_FIREWIRE_GUID_T
#   define DMI_AMI_FIREWIRE_GUID_T
    typedef struct dmi_ami_firewire_guid dmi_ami_firewire_guid_t;
#endif // !DMI_AMI_FIREWIRE_GUID_T

/**
 * @brief AMI FireWire GUID (type 139).
 *
 * Keeps the data holding the GUID of the IEEE 1394 (FireWire) controller of
 * the board, under the name `V1394GUID`. Reverse engineered from the data
 * corpus, which holds a single structure of the kind: the layout of the data
 * is not established, so the data is kept as a whole, and is replaced when
 * the table is anonymized.
 */
struct dmi_ami_firewire_guid
{
    /**
     * @brief Signature, `FE DC BA 98 76 54 32 10`, which the structure is
     * told by.
     */
    dmi_binary_t signature;

    /**
     * @brief Data holding the GUID of the controller, 40 bytes, which begin
     * like the bus information block of the configuration ROM of IEEE 1394.
     */
    dmi_binary_t data;

    /**
     * @brief Value whose meaning is not established, 0 in all known data.
     */
    uint8_t unknown;

    /**
     * @brief Name of the data, `V1394GUID`.
     */
    const char *name;
};

/**
 * @brief AMI FireWire GUID entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_ami_firewire_guid_spec;

#endif // !OPENDMI_ENTITY_AMI_FIREWIRE_GUID_H
