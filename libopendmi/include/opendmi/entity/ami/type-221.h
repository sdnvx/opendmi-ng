//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_AMI_TYPE_221_H
#define OPENDMI_ENTITY_AMI_TYPE_221_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_ami_type_221 dmi_ami_type_221_t;
typedef struct dmi_ami_type_221_item dmi_ami_type_221_item_t;

struct dmi_ami_type_221_item
{
    /**
     * @brief Item name.
     */
    const char *name;

    /**
     * @brief String value.
     */
    const char *string;

    /**
     * @brief Unknown field 1. Set to `UINT8_MAX` when unspecified.
     */
    uint8_t unknown_1;

    /**
     * @brief Unknown field 2. Set to `UINT16_MAX` when unspecified.
     */
    uint16_t unknown_2;

    /**
     * @brief Unknown field 3. Set to `UINT16_MAX` when unspecified.
     */
    uint16_t unknown_3;
};

/**
 * @brief AMI type 221 structure.
 */
struct dmi_ami_type_221
{
    size_t item_count;
    dmi_ami_type_221_item_t *items;
};

/**
 * @brief AMI type 221 entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_ami_type_221_spec;

#endif // !OPENDMI_ENTITY_AMI_TYPE_221_H
