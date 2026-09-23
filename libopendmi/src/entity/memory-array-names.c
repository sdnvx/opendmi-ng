//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <inttypes.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/reader.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/memory-device.h>

#include <opendmi/entity/memory-array-internal.h>

const dmi_name_set_t dmi_memory_array_location_names =
{
    .code  = "memory-array-location",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MEMORY_ARRAY_LOCATION_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_ARRAY_LOCATION_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_ARRAY_LOCATION_UNKNOWN),
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_MOTHERBOARD,
            .code = "motherboard",
            .name = "System board or motherboard"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_ISA,
            .code = "isa",
            .name = "ISA add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_EISA,
            .code = "eisa",
            .name = "EISA add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PCI,
            .code = "pci",
            .name = "PCI add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_MCA,
            .code = "mca",
            .name = "MCA add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PCMCIA,
            .code = "pcmcia",
            .name = "PCMCIA add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PROPRIETARY,
            .code = "proprietary",
            .name = "Proprietary add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_NUBUS,
            .code = "nubus",
            .name = "NuBus"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PC_98_C20,
            .code = "pc-98-c20",
            .name = "PC-98/C20 add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PC_98_C24,
            .code = "pc-98-c24",
            .name = "PC-98/C24 add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PC_98_E,
            .code = "pc-98-e",
            .name = "PC-98/E add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_PC_98_LOCAL_BUS,
            .code = "pc-98-local-bus",
            .name = "PC-98/Local bus add-on card"
        },
        {
            .id   = DMI_MEMORY_ARRAY_LOCATION_CXL,
            .code = "cxl",
            .name = "CXL add-on card"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_array_usage_names =
{
    .code  = "memory-array-usage",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MEMORY_ARRAY_USAGE_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_ARRAY_USAGE_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_ARRAY_USAGE_UNKNOWN),
        {
            .id   = DMI_MEMORY_ARRAY_USAGE_SYSTEM,
            .code = "system",
            .name = "System memory"
        },
        {
            .id   = DMI_MEMORY_ARRAY_USAGE_VIDEO,
            .code = "video",
            .name = "Video memory"
        },
        {
            .id   = DMI_MEMORY_ARRAY_USAGE_FLASH,
            .code = "flash",
            .name = "Flash memory"
        },
        {
            .id   = DMI_MEMORY_ARRAY_USAGE_NVRAM,
            .code = "nvram",
            .name = "Non-volatile RAM"
        },
        {
            .id   = DMI_MEMORY_ARRAY_USAGE_CACHE,
            .code = "cache",
            .name = "Cache memory"
        },
        {}
    })
};

const char *dmi_memory_array_location_name(dmi_memory_array_location_t value)
{
    return dmi_name_lookup(&dmi_memory_array_location_names, (int)value);
}

const char *dmi_memory_array_usage_name(dmi_memory_array_usage_t value)
{
    return dmi_name_lookup(&dmi_memory_array_usage_names, (int)value);
}
