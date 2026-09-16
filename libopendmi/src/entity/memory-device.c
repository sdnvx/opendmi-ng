//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-device.h>

static bool dmi_memory_device_decode(dmi_entity_t *entity);
static bool dmi_memory_device_link(dmi_entity_t *entity);

static const dmi_name_set_t dmi_memory_device_type_names =
{
    .code  = "memory-device-types",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_memory_device_type_detail_names =
{
    .code  = "memory-device-type-details",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_memory_device_form_factor_names =
{
    .code  = "memory-device-form-factors",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_memory_device_tech_names =
{
    .code  = "memory-device-technologies",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_memory_device_spec =
{
    .code            = "memory-device",
    .name            = "Memory device",
    .description     = (const char *[]){
        "This structure describes a single memory device that is part of a "
        "larger Physical Memory Array (Type 16) structure.",
        //
        "Note: If a system includes memory-device sockets, the SMBIOS "
        "implementation includes a Memory Device structure instance for each "
        "slot, whether the socket is currently populated.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MEMORY_DEVICE),
    .minimum_version = DMI_VERSION(2, 1, 0),
    .minimum_length  = 0x15,
    .decoded_length  = sizeof(dmi_memory_device_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_memory_device_t, array_handle, HANDLE, {
            .code    = "array-handle",
            .name    = "Memory array handle",
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, error_info_handle, HANDLE, {
            .code    = "error-info-handle",
            .name    = "Memory error information handle"
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, total_width, INTEGER, {
            .code    = "total-width",
            .name    = "Total width",
            .unit    = DMI_UNIT_BIT,
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, data_width, INTEGER, {
            .code    = "data-width",
            .name    = "Data width",
            .unit    = DMI_UNIT_BIT,
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, size, SIZE, {
            .code    = "size",
            .name    = "Size",
            .unknown = dmi_value_ptr((dmi_size_t)UINT64_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, form_factor, ENUM, {
            .code    = "form-factor",
            .name    = "Form-factor",
            .unspec  = dmi_value_ptr(DMI_MEMORY_DEVICE_FORM_FACTOR_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_DEVICE_FORM_FACTOR_UNKNOWN),
            .values  = &dmi_memory_device_form_factor_names
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, device_set, INTEGER, {
            .code    = "device-set",
            .name    = "Device set",
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, device_locator, STRING, {
            .code    = "device-locator",
            .name    = "Device locator"
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, bank_locator, STRING, {
            .code    = "bank-locator",
            .name    = "Bank locator"
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, memory_type, ENUM, {
            .code    = "memory-type",
            .name    = "Memory type",
            .unspec  = dmi_value_ptr(DMI_MEMORY_DEVICE_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_DEVICE_TYPE_UNKNOWN),
            .values  = &dmi_memory_device_type_names
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, memory_type_detail, SET, {
            .code    = "memory-type-details",
            .name    = "Memory type details",
            .values  = &dmi_memory_device_type_detail_names
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, maximum_speed, INTEGER, {
            .code    = "maximum-speed",
            .name    = "Maximum speed",
            .unit    = DMI_UNIT_MEGAXA_SECOND,
            .unknown = dmi_value_ptr((unsigned long)0),
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Manufacturer",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, asset_tag, STRING, {
            .code    = "asset-tag",
            .name    = "Asset tag",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, part_number, STRING, {
            .code    = "part-number",
            .name    = "Part number",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, rank, INTEGER, {
            .code    = "rank",
            .name    = "Rank",
            .unknown = dmi_value_ptr((unsigned short)0),
            .level   = DMI_VERSION(2, 6, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, configured_speed, INTEGER, {
            .code    = "configured-speed",
            .name    = "Configured speed",
            .unit    = DMI_UNIT_MEGAXA_SECOND,
            .unknown = dmi_value_ptr((unsigned long)0),
            .level   = DMI_VERSION(2, 7, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, minimum_voltage, DECIMAL, {
            .code    = "minimum-voltage",
            .name    = "Minimum voltage",
            .unit    = DMI_UNIT_VOLT,
            .scale   = 3,
            .unknown = dmi_value_ptr((unsigned short)0),
            .level   = DMI_VERSION(2, 8, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, maximum_voltage, DECIMAL, {
            .code    = "maximum-voltage",
            .name    = "Maximum voltage",
            .unit    = DMI_UNIT_VOLT,
            .scale   = 3,
            .unknown = dmi_value_ptr((unsigned short)0),
            .level   = DMI_VERSION(2, 8, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, configured_voltage, DECIMAL, {
            .code    = "configured-voltage",
            .name    = "Configured voltage",
            .unit    = DMI_UNIT_VOLT,
            .scale   = 3,
            .unknown = dmi_value_ptr((unsigned short)0),
            .level   = DMI_VERSION(2, 8, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, memory_tech, ENUM, {
            .code    = "memory-technology",
            .name    = "Memory technology",
            .unspec  = dmi_value_ptr(DMI_MEMORY_DEVICE_TECH_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_DEVICE_TECH_UNKNOWN),
            .values  = &dmi_memory_device_tech_names,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, memory_mode_caps, INTEGER, {
            .code    = "memory-mode-caps",
            .name    = "Memory operating mode capabilities",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, firmware_version, STRING, {
            .code    = "firmware-version",
            .name    = "Firmware version",
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, module_vendor_id, INTEGER, {
            .code    = "module-vendor-id",
            .name    = "Module manufacturer ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, module_product_id, INTEGER, {
            .code    = "module-product-id",
            .name    = "Module product ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, controller_vendor_id, INTEGER, {
            .code    = "controller-vendor-id",
            .name    = "Memory subsystem controller manufacturer ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, controller_product_id, INTEGER, {
            .code    = "controller-product-id",
            .name    = "Memory subsystem controller product ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, non_volatile_size, SIZE, {
            .code    = "non-volatile-size",
            .name    = "Non-volatile size",
            .unknown = dmi_value_ptr((dmi_size_t)UINT64_MAX),
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, volatile_size, SIZE, {
            .code    = "volatile-size",
            .name    = "Volatile size",
            .unknown = dmi_value_ptr((dmi_size_t)UINT64_MAX),
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, cache_size, SIZE, {
            .code    = "cache-size",
            .name    = "Cache size",
            .unknown = dmi_value_ptr((dmi_size_t)UINT64_MAX),
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, logical_size, SIZE, {
            .code    = "logical-size",
            .name    = "Logical size",
            .unknown = dmi_value_ptr((dmi_size_t)UINT64_MAX),
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, pmic0_vendor_id, INTEGER, {
            .code    = "pmi0-vendor-id",
            .name    = "PMIC0 manufacturer ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 7, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, pmic0_revision, INTEGER, {
            .code    = "pmic0-revision",
            .name    = "PMIC0 revision number",
            .unknown = dmi_value_ptr((uint16_t)0xFF00u),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 7, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, rcd_vendor_id, INTEGER, {
            .code    = "rcd-vendor-id",
            .name    = "RCD manufacturer ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 7, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, rcd_revision, INTEGER, {
            .code    = "rcd-revision",
            .name    = "RCD revision number",
            .unknown = dmi_value_ptr((uint16_t)0xFF00u),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 7, 0)
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_memory_device_decode,
        .link   = dmi_memory_device_link,
    }
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

dmi_size_t dmi_memory_device_size(uint16_t value)
{
    dmi_size_t size = value & 0x7FFFu;

    if (value & 0x8000u)
        size <<= 10; // Granularity is 1 Kb
    else
        size <<= 20; // Granularity is 1 Mb

    return size;
}

dmi_size_t dmi_memory_device_size_ex(uint32_t value)
{
    if (value & 0x80000000u)
        return SIZE_MAX;

    return (dmi_size_t)(value & 0x7FFFFFFFu) << 20; // Granularity is 1 Mb
}

static bool dmi_memory_device_decode(dmi_entity_t *entity)
{
    dmi_memory_device_t *info;
    const dmi_memory_device_data_t *data;

    data = dmi_entity_data(entity, DMI_TYPE(MEMORY_DEVICE));
    if (data == nullptr)
        return false;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_DEVICE));
    if (info == nullptr)
        return false;

    info->array_handle      = dmi_decode(data->array_handle);
    info->error_info_handle = dmi_decode(data->error_info_handle);

    info->total_width = dmi_decode(data->total_width);
    if (info->total_width == 0xFFFFu)
        info->total_width = USHRT_MAX;

    info->data_width = dmi_decode(data->data_width);
    if (info->data_width == 0xFFFFu)
        info->data_width = USHRT_MAX;

    uint16_t size = dmi_decode(data->size);
    if (size != 0xFFFFu)
        info->size = dmi_memory_device_size(size);
    else
        info->size = UINT64_MAX;

    info->form_factor = data->form_factor;

    if (data->device_set != 0xFFu)
        info->device_set = data->device_set;
    else
        info->device_set = USHRT_MAX;

    info->device_locator             = dmi_entity_string(entity, data->device_locator);
    info->bank_locator               = dmi_entity_string(entity, data->bank_locator);
    info->memory_type                = dmi_decode(data->memory_type);
    info->memory_type_detail.__value = dmi_decode(data->memory_type_detail);

    //
    // SMBIOS 2.3 features
    //

    if (entity->body_length > 0x15) {
        entity->level = dmi_version(2, 3, 0);
        info->maximum_speed = dmi_decode(data->maximum_speed);
    }

    if (entity->body_length > 0x17)
        info->vendor = dmi_entity_string(entity, data->vendor);
    if (entity->body_length > 0x18)
        info->serial_number = dmi_entity_string(entity, data->serial_number);
    if (entity->body_length > 0x19)
        info->asset_tag = dmi_entity_string(entity, data->asset_tag);
    if (entity->body_length > 0x1A)
        info->part_number = dmi_entity_string(entity, data->part_number);

    //
    // SMBIOS 2.6 features
    //

    if (entity->body_length > 0x1B) {
        entity->level = dmi_version(2, 6, 0);
        info->rank = data->rank;
    }

    //
    // SMBIOS 2.7 features
    //

    if (entity->body_length > 0x1C) {
        entity->level = dmi_version(2, 7, 0);

        if (size == 0x7FFFu)
            info->size = dmi_memory_device_size_ex(size);
    }

    if (entity->body_length > 0x20)
        info->configured_speed = dmi_decode(data->configured_speed);

    //
    // SMBIOS 2.8 features
    //

    if (entity->body_length > 0x22) {
        entity->level = dmi_version(2, 8, 0);
        info->minimum_voltage = dmi_decode(data->minimum_voltage);
    }

    if (entity->body_length > 0x24)
        info->maximum_voltage = dmi_decode(data->maximum_voltage);
    if (entity->body_length > 0x26)
        info->configured_voltage = dmi_decode(data->configured_voltage);

    //
    // SMBIOS 3.2 features
    //

    if (entity->body_length > 0x28) {
        entity->level = dmi_version(3, 2, 0);
        info->memory_tech = dmi_decode(data->memory_tech);
    }

    if (entity->body_length > 0x29)
        info->memory_mode_caps = dmi_decode(data->memory_mode_caps);

    if (entity->body_length > 0x2B)
        info->firmware_version = dmi_entity_string(entity, data->firmware_version);

    if (entity->body_length > 0x2C)
        info->module_vendor_id = dmi_decode(data->module_vendor_id);
    if (entity->body_length > 0x2E)
        info->module_product_id = dmi_decode(data->module_product_id);

    if (entity->body_length > 0x30)
        info->controller_vendor_id = dmi_decode(data->controller_vendor_id);
    if (entity->body_length > 0x32)
        info->controller_product_id = dmi_decode(data->controller_product_id);

    if (entity->body_length > 0x34)
        info->non_volatile_size = dmi_decode(data->non_volatile_size);
    if (entity->body_length > 0x3C)
        info->volatile_size = dmi_decode(data->volatile_size);
    if (entity->body_length > 0x44)
        info->cache_size = dmi_decode(data->cache_size);
    if (entity->body_length > 0x4C)
        info->logical_size = dmi_decode(data->logical_size);

    //
    // SMBIOS 3.3 features
    //

    if (entity->body_length > 0x54) {
        entity->level = dmi_version(3, 3, 0);
        if (data->maximum_speed == 0xFFFFu)
            info->maximum_speed = dmi_decode(data->maximum_speed_ex);
    }

    if (entity->body_length > 0x58) {
        if (data->configured_speed == 0xFFFFu)
            info->configured_speed = dmi_decode(data->configured_speed_ex);
    }

    // SMBIOS 3.7 features
    if (entity->body_length > 0x5C) {
        entity->level = dmi_version(3, 7, 0);
        info->pmic0_vendor_id = dmi_decode(data->pmic0_vendor_id);
    }
    if (entity->body_length > 0x5E)
        info->pmic0_revision = dmi_decode(data->pmic0_revision);

    if (entity->body_length > 0x60)
        info->rcd_vendor_id = dmi_decode(data->rcd_vendor_id);
    if (entity->body_length > 0x62)
        info->rcd_revision = dmi_decode(data->rcd_revision);

    return true;
}

static bool dmi_memory_device_link(dmi_entity_t *entity)
{
    static const dmi_type_t error_types[] = {
        DMI_TYPE(MEMORY_ERROR_32),
        DMI_TYPE(MEMORY_ERROR_64),
        DMI_TYPE_INVALID
    };

    dmi_memory_device_t *info;
    dmi_registry_t *registry;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_DEVICE));
    if (info == nullptr)
        return false;

    registry = entity->context->state.registry;

    if (info->array_handle != DMI_HANDLE_INVALID) {
        info->array = dmi_registry_get(registry, info->array_handle, DMI_TYPE(MEMORY_ARRAY), false);
        if (info->array == nullptr) {
            dmi_error_raise_ex(entity->context, DMI_ERROR_ENTITY_NOT_FOUND,
                               "Memory array: 0x%04x", info->array_handle);
        }
    }

    if ((info->error_info_handle != DMI_HANDLE_INVALID) and
        (info->error_info_handle != DMI_HANDLE_UNSUPPORTED))
    {
        info->error_info = dmi_registry_get_any(registry, info->error_info_handle, error_types, false);
    }

    return true;
}
