//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_INTEL_RSD_FPGA_INTERNAL_H
#define OPENDMI_ENTITY_INTEL_RSD_FPGA_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel-rsd/fpga.h>

/**
 * @internal
 * @brief Names of the FPGA types.
 */
extern const dmi_name_set_t dmi_intel_rsd_fpga_type_names;

/**
 * @internal
 * @brief Names of the FPGA status values.
 */
extern const dmi_name_set_t dmi_intel_rsd_fpga_status_names;

/**
 * @internal
 * @brief Names of the instruction set architectures of the FPGA hard processor
 * subsystem (HPS).
 */
extern const dmi_name_set_t dmi_intel_rsd_fpga_hps_isa_names;

/**
 * @internal
 * @brief Names of the FPGA high-speed serial interface (HSSI) configurations.
 */
extern const dmi_name_set_t dmi_intel_rsd_fpga_hssi_config_names;

/**
 * @internal
 * @brief Names of the technologies of the memory attached to the FPGA.
 */
extern const dmi_name_set_t dmi_intel_rsd_fpga_memory_tech_names;

#endif // !OPENDMI_ENTITY_INTEL_RSD_FPGA_INTERNAL_H
