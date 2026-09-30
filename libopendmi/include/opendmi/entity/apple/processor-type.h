//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_APPLE_PROCESSOR_TYPE_H
#define OPENDMI_ENTITY_APPLE_PROCESSOR_TYPE_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_apple_processor_type dmi_apple_processor_type_t;

/**
 * @brief Class of processors, the major type of a processor type.
 */
typedef enum dmi_apple_processor_class
{
    DMI_APPLE_PROCESSOR_CLASS_UNKNOWN      = 0x00, ///< Unknown
    DMI_APPLE_PROCESSOR_CLASS_CORE         = 0x02, ///< Intel Core
    DMI_APPLE_PROCESSOR_CLASS_CORE_2       = 0x03, ///< Intel Core 2
    DMI_APPLE_PROCESSOR_CLASS_XEON_PENRYN  = 0x04, ///< Intel Xeon (Penryn)
    DMI_APPLE_PROCESSOR_CLASS_XEON_NEHALEM = 0x05, ///< Intel Xeon (Nehalem)
    DMI_APPLE_PROCESSOR_CLASS_CORE_I5      = 0x06, ///< Intel Core i5
    DMI_APPLE_PROCESSOR_CLASS_CORE_I7      = 0x07, ///< Intel Core i7
    DMI_APPLE_PROCESSOR_CLASS_CORE_I3      = 0x09, ///< Intel Core i3
    DMI_APPLE_PROCESSOR_CLASS_XEON_E5      = 0x0A, ///< Intel Xeon E5
    DMI_APPLE_PROCESSOR_CLASS_CORE_M       = 0x0B, ///< Intel Core M
    DMI_APPLE_PROCESSOR_CLASS_CORE_M3      = 0x0C, ///< Intel Core m3
    DMI_APPLE_PROCESSOR_CLASS_CORE_M5      = 0x0D, ///< Intel Core m5
    DMI_APPLE_PROCESSOR_CLASS_CORE_M7      = 0x0E, ///< Intel Core m7
    DMI_APPLE_PROCESSOR_CLASS_XEON_W       = 0x0F, ///< Intel Xeon W
    DMI_APPLE_PROCESSOR_CLASS_CORE_I9      = 0x10  ///< Intel Core i9
} dmi_apple_processor_class_t;

/**
 * @brief Apple processor type information structure (type 131).
 *
 * Tells the type of the processor, by which macOS names it, e.g. `0x0701`
 * for Intel Core i7. Laid out as the `AppleSmBios.h` header of OpenCore
 * describes it.
 */
struct dmi_apple_processor_type
{
    /**
     * @brief Generation of the processor within its class, the minor type,
     * e.g. `1` for the first generation of Intel Core i7.
     */
    uint8_t generation;

    /**
     * @brief Class of the processor, the major type.
     */
    dmi_apple_processor_class_t processor_class;
};

/**
 * @brief Apple processor type information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_apple_processor_type_spec;

__BEGIN_DECLS

__dmi_api const char *dmi_apple_processor_class_name(dmi_apple_processor_class_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_APPLE_PROCESSOR_TYPE_H
