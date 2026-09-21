//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-module-internal.h>

const dmi_name_set_t dmi_memory_module_type_names =
{
    .code  = "memory-module-type",
    .names = DMI_NAMES({
        DMI_NAME_OTHER(0),
        DMI_NAME_UNKNOWN(1),
        {
            .id   = 2,
            .code = "standard",
            .name = "Standard"
        },
        {
            .id   = 3,
            .code = "fpm",
            .name = "Fast page mode"
        },
        {
            .id   = 4,
            .code = "edo",
            .name = "EDO"
        },
        {
            .id   = 5,
            .code = "parity",
            .name = "Parity"
        },
        {
            .id   = 6,
            .code = "ecc",
            .name = "ECC"
        },
        {
            .id   = 7,
            .code = "simm",
            .name = "SIMM"
        },
        {
            .id   = 8,
            .code = "dimm",
            .name = "DIMM"
        },
        {
            .id   = 9,
            .code = "burst-edo",
            .name = "Burst EDO"
        },
        {
            .id   = 10,
            .code = "sdram",
            .name = "SDRAM"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_module_size_status_names =
{
    .code = "memory-module-size-status",
    .names = DMI_NAMES({
        {
            .id   = DMI_MEMORY_MODULE_SIZE_STATUS_INVALID,
            .code = "invalid",
            .name = "Invalid"
        },
        {
            .id   = DMI_MEMORY_MODULE_SIZE_STATUS_PRESENT,
            .code = "present",
            .name = "Present"
        },
        {
            .id   = DMI_MEMORY_MODULE_SIZE_STATUS_NOT_DETERMINABLE,
            .code = "not-determinable",
            .name = "Not determinable"
        },
        {
            .id   = DMI_MEMORY_MODULE_SIZE_STATUS_NOT_ENABLED,
            .code = "not-enabled",
            .name = "Not enabled"
        },
        {
            .id   = DMI_MEMORY_MODULE_SIZE_STATUS_NOT_INSTALLED,
            .code = "not-installed",
            .name = "Not installed"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_module_error_names =
{
    .code  = "memory-module-error",
    .names = DMI_NAMES({
        {
            .id   = 0,
            .code = "uncorrectable",
            .name = "Uncorrectable"
        },
        {
            .id   = 1,
            .code = "correctable",
            .name = "Correctable"
        },
        {
            .id   = 2,
            .code = "event-log",
            .name = "Event log"
        },
        {}
    })
};

const char *dmi_memory_module_size_status_name(dmi_memory_module_size_status_t value)
{
    return dmi_name_lookup(&dmi_memory_module_size_status_names, (int)value);
}
