//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/memory-device-internal.h>

const dmi_name_set_t dmi_memory_device_type_names =
{
    .code  = "memory-device-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MEMORY_DEVICE_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_DEVICE_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_DEVICE_TYPE_UNKNOWN),
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_DRAM,
            .code = "dram",
            .name = "DRAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_EDRAM,
            .code = "edram",
            .name = "EDRAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_VRAM,
            .code = "vram",
            .name = "VRAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_SRAM,
            .code = "sram",
            .name = "SRAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_RAM,
            .code = "ram",
            .name = "RAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_ROM,
            .code = "rom",
            .name = "ROM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_FLASH,
            .code = "flash",
            .name = "Flash"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_EEPROM,
            .code = "eeprom",
            .name = "EEPROM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_FEPROM,
            .code = "feprom",
            .name = "FEPROM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_EPROM,
            .code = "eprom",
            .name = "EPROM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_CDRAM,
            .code = "cdram",
            .name = "CDRAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_3DRAM,
            .code = "3dram",
            .name = "3DRAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_SDRAM,
            .code = "sdram",
            .name = "SDRAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_SGRAM,
            .code = "sgram",
            .name = "SGRAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_RDRAM,
            .code = "rdram",
            .name = "RDRAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_DDR,
            .code = "ddr",
            .name = "DDR"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_DDR2,
            .code = "ddr2",
            .name = "DDR2"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_DDR2_FBDIMM,
            .code = "ddr2-fbdimm",
            .name = "DDR2 FB-DIMM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_DDR3,
            .code = "ddr3",
            .name = "DDR3"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_FBD2,
            .code = "fbd2",
            .name = "FBD2"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_DDR4,
            .code = "ddr4",
            .name = "DDR4"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_LPDDR,
            .code = "lpddr",
            .name = "LPDDR"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_LPDDR2,
            .code = "lpddr2",
            .name = "LPDDR2"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_LPDDR3,
            .code = "lpddr3",
            .name = "LPDDR3"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_LPDDR4,
            .code = "lpddr4",
            .name = "LPDDR4"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_LOGICAL_NV,
            .code = "logical-nv",
            .name = "Logical non-volatile device"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_HBM,
            .code = "hbm",
            .name = "HBM (High Bandwidth Memory)"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_HBM2,
            .code = "hbm2",
            .name = "HBM2 (High Bandwidth Memory Generation 2)"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_DDR5,
            .code = "ddr5",
            .name = "DDR5"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_LPDDR5,
            .code = "lpddr5",
            .name = "LPDDR5"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_HBM3,
            .code = "hbm3",
            .name = "HBM3 (High Bandwidth Memory Generation 3)"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TYPE_MRDIMM,
            .code = "mrdimm",
            .name = "MRDIMM"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_device_type_detail_names =
{
    .code  = "memory-device-type-detail",
    .names = DMI_NAMES({
        DMI_NAME_RESERVED(0),
        DMI_NAME_OTHER(1),
        DMI_NAME_UNKNOWN(2),
        {
            .id   = 3,
            .code = "fast-paged",
            .name = "Fast-paged"
        },
        {
            .id   = 4,
            .code = "static-column",
            .name = "Static column"
        },
        {
            .id   = 5,
            .code = "pseudo-static",
            .name = "Pseudo-static"
        },
        {
            .id   = 6,
            .code = "rambus",
            .name = "RAMBUS"
        },
        {
            .id   = 7,
            .code = "synchronous",
            .name = "Synchronous"
        },
        {
            .id   = 8,
            .code = "cmos",
            .name = "CMOS"
        },
        {
            .id   = 9,
            .code = "edo",
            .name = "EDO"
        },
        {
            .id   = 10,
            .code = "window-dram",
            .name = "Window DRAM"
        },
        {
            .id   = 11,
            .code = "cache-dram",
            .name = "Cache DRAM"
        },
        {
            .id   = 12,
            .code = "non-volatile",
            .name = "Non-volatile"
        },
        {
            .id   = 13,
            .code = "registered",
            .name = "Registered (buffered)"
        },
        {
            .id   = 14,
            .code = "unbuffered",
            .name = "Unbuffered (unregistered)"
        },
        {
            .id   = 15,
            .code = "lrdimm",
            .name = "LRDIMM"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_device_form_factor_names =
{
    .code  = "memory-device-form-factor",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MEMORY_DEVICE_FORM_FACTOR_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_DEVICE_FORM_FACTOR_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_DEVICE_FORM_FACTOR_UNKNOWN),
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_SIMM,
            .code = "simm",
            .name = "SIMM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_SIP,
            .code = "sip",
            .name = "SIP"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_CHIP,
            .code = "chip",
            .name = "Chip"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_DIP,
            .code = "dip",
            .name = "DIP"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_ZIP,
            .code = "zip",
            .name = "ZIP"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_CARD,
            .code = "card",
            .name = "Proprietary card"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_DIMM,
            .code = "dimm",
            .name = "DIMM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_TSOP,
            .code = "tsop",
            .name = "TSOP"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_CHIP_ROW,
            .code = "chip-row",
            .name = "Row of chips"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_RIMM,
            .code = "rimm",
            .name = "RIMM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_SODIMM,
            .code = "sodimm",
            .name = "SODIMM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_SRIMM,
            .code = "srimm",
            .name = "SRIMM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_FBDIMM,
            .code = "fbdimm",
            .name = "FBDIMM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_DIE,
            .code = "die",
            .name = "Die"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_CAMM,
            .code = "camm",
            .name = "CAMM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_CUDIMM,
            .code = "cudimm",
            .name = "CUDIMM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_FORM_FACTOR_CSODIMM,
            .code = "csodimm",
            .name = "CSODIMM"
        },
        {}
    })
};

const dmi_name_set_t dmi_memory_device_tech_names =
{
    .code  = "memory-device-technology",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MEMORY_DEVICE_TECH_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_DEVICE_TECH_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_DEVICE_TECH_UNKNOWN),
        {
            .id   = DMI_MEMORY_DEVICE_TECH_DRAM,
            .code = "dram",
            .name = "DRAM"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TECH_NVDIMM_N,
            .code = "nvdimm-n",
            .name = "NVDIMM-N"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TECH_NVDIMM_F,
            .code = "nvdimm-f",
            .name = "NVDIMM-F"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TECH_NVDIMM_P,
            .code = "nvdimm-p",
            .name = "NVDIMM-P"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TECH_OPTANE,
            .code = "optane",
            .name = "Intel Optane persistent memory"
        },
        {
            .id   = DMI_MEMORY_DEVICE_TECH_MRDIMM,
            .code = "mrdimm",
            .name = "MRDIMM"
        },
        {}
    })
};

const char *dmi_memory_device_type_name(dmi_memory_device_type_t value)
{
    return dmi_name_lookup(&dmi_memory_device_type_names, (int)value);
}

const char *dmi_memory_device_form_factor_name(dmi_memory_device_form_factor_t value)
{
    return dmi_name_lookup(&dmi_memory_device_form_factor_names, (int)value);
}

const char *dmi_memory_device_tech_name(dmi_memory_device_tech_t value)
{
    return dmi_name_lookup(&dmi_memory_device_tech_names, (int)value);
}
