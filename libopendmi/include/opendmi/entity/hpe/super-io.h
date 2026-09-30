//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_SUPER_IO_H
#define OPENDMI_ENTITY_HPE_SUPER_IO_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_hpe_super_io dmi_hpe_super_io_t;

/**
 * @brief HP/HPE Super I/O enable/disable indicator (type 194).
 *
 * Tells which ports of the Super I/O controller are enabled.
 */
struct dmi_hpe_super_io
{
    /**
     * @brief Whether serial port A is enabled.
     */
    bool is_serial_a_enabled;

    /**
     * @brief Whether serial port B is enabled.
     */
    bool is_serial_b_enabled;

    /**
     * @brief Whether the parallel port is enabled.
     */
    bool is_parallel_enabled;

    /**
     * @brief Whether the floppy disk port is enabled.
     */
    bool is_floppy_enabled;

    /**
     * @brief Whether the virtual serial port is enabled.
     */
    bool is_virtual_serial_enabled;
};

/**
 * @brief HP/HPE Super I/O enable/disable indicator entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_hpe_super_io_spec;

#endif // !OPENDMI_ENTITY_HPE_SUPER_IO_H
