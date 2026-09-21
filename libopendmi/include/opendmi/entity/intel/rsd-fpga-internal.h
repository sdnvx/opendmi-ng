//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_RSD_FPGA_INTERNAL_H
#define OPENDMI_ENTITY_RSD_FPGA_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/intel/rsd-fpga.h>

// Value names, see rsd-fpga-names.c
extern const dmi_name_set_t dmi_intel_rsd_fpga_type_names;
extern const dmi_name_set_t dmi_intel_rsd_fpga_status_names;
extern const dmi_name_set_t dmi_intel_rsd_fpga_hps_isa_names;
extern const dmi_name_set_t dmi_intel_rsd_fpga_hssi_config_names;
extern const dmi_name_set_t dmi_intel_rsd_fpga_memory_tech_names;

#endif // !OPENDMI_ENTITY_RSD_FPGA_INTERNAL_H
