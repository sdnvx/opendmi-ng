//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_ASF_H
#define OPENDMI_ENTITY_INTEL_ASF_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_INTEL_ASF_T
#   define DMI_INTEL_ASF_T
    typedef struct dmi_intel_asf dmi_intel_asf_t;
#endif // !DMI_INTEL_ASF_T

/**
 * @brief Intel Alert Standard Format information (type 129).
 *
 * Names the Alert Standard Format (ASF) support of the platform. Reverse
 * engineered from the data corpus: the meaning of the fields other than the
 * strings is not established. The structure is told by the bytes which begin
 * it, since other vendors give type 129 to structures of their own.
 */
struct dmi_intel_asf
{
    /**
     * @brief Version of the structure, presumably, 1 in all known data.
     */
    uint8_t version;

    /**
     * @brief Name of the ASF support, e.g. `Intel_ASF` or `Insyde_ASF_001`.
     */
    const char *name;

    /**
     * @brief Identifier of the ASF support, e.g. `Intel_ASF_001` or
     * `Dell_ASF_002`.
     */
    const char *identifier;

    /**
     * @brief Value whose meaning is not established, 1 in most known data
     * and 0 on ThinkPad T61p.
     */
    uint8_t parameter;

    /**
     * @brief Bytes which follow in the longer structures, whose meaning is
     * not established, e.g. on Acer Aspire 3680.
     */
    uint8_t extra[8];

    /**
     * @brief Whether the structure holds `extra`, which the structures of 8
     * bytes leave out.
     */
    bool has_extra;
};

/**
 * @brief Intel Alert Standard Format information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_intel_asf_spec;

#endif // !OPENDMI_ENTITY_INTEL_ASF_H
