//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>
#include <opendmi/entity/intel/rsd-memory-device.h>

static bool dmi_intel_rsd_memory_device_decode(dmi_entity_t *entity);

const dmi_name_set_t dmi_intel_rsd_memory_type_names =
{
    .code  = "intel-rsd-memory-types",
    .names = (const dmi_name_t[]){
        {
            .id   = DMI_INTEL_RSD_MEMORY_TYPE_DIMM,
            .code = "dimm",
            .name = "DIMM"
        },
        {
            .id   = DMI_INTEL_RSD_MEMORY_TYPE_NVDIMM_N,
            .code = "nvdimm-n",
            .name = "NVDIMM-N (Byte accessible persistent memory)"
        },
        {
            .id   = DMI_INTEL_RSD_MEMORY_TYPE_NVDIMM_F,
            .code = "nvdimm-f",
            .name = "NVDIMM-F (Block accessible persistent memory)"
        },
        {
            .id   = DMI_INTEL_RSD_MEMORY_TYPE_NVDIMM_P,
            .code = "nvdimm-p",
            .name = "NVDIMM-P"
        },
        {}
    }
};

const dmi_name_set_t dmi_intel_rsd_memory_media_names =
{
    .code  = "intel-rsd-memory-medias",
    .names = (const dmi_name_t[]){
        {
            .id   = DMI_INTEL_RSD_MEMORY_MEDIA_DRAM,
            .code = "dram",
            .name = "DRAM"
        },
        {
            .id   = DMI_INTEL_RSD_MEMORY_MEDIA_NAND,
            .code = "nand",
            .name = "NAND"
        },
        {
            .id   = DMI_INTEL_RSD_MEMORY_MEDIA_PROPRIETARY,
            .code = "proprietary",
            .name = "Proprietary"
        },
        {}
    }
};

const dmi_entity_spec_t dmi_intel_rsd_memory_device_spec =
{
    .code            = "intel-rsd-memory-device-ex",
    .name            = "Intel RSD memory device extended information",
    .type            = DMI_TYPE(INTEL_RSD_MEMORY_DEVICE),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x0F,
    .decoded_length  = sizeof(dmi_intel_rsd_memory_device_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_intel_rsd_memory_device_t, device_handle, HANDLE, {
            .code = "device-handle",
            .name = "Device handle"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_memory_device_t, memory_type, ENUM, {
            .code   = "memory-type",
            .name   = "Memory type",
            .values = &dmi_intel_rsd_memory_type_names
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_memory_device_t, memory_media, ENUM, {
            .code   = "memory-media",
            .name   = "Memory media",
            .values = &dmi_intel_rsd_memory_media_names
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_memory_device_t, firmware_revision, STRING, {
            .code   = "firmware-revision",
            .name   = "Firmware revision"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_memory_device_t, firmware_api_version, STRING, {
            .code   = "firmware-api-version",
            .name   = "Firmware API version"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_memory_device_t, maximum_tdp, DECIMAL, {
            .code   = "maximum-tdp",
            .name   = "Maximum TDP",
            .scale  = 3,
            .unit   = DMI_UNIT_WATT
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_memory_device_t, smbus_address, INTEGER, {
            .code   = "smbus-address",
            .name   = "SMBus address"
        }),
        {}
    },
    .handlers = {
        .decode = dmi_intel_rsd_memory_device_decode
    }
};

const char *dmi_intel_rsd_memory_type_name(dmi_intel_rsd_memory_type_t value)
{
    return dmi_name_lookup(&dmi_intel_rsd_memory_type_names, (int)value);
}

const char *dmi_intel_rsd_memory_media_name(dmi_intel_rsd_memory_media_t value)
{
    return dmi_name_lookup(&dmi_intel_rsd_memory_media_names, (int)value);
}

static bool dmi_intel_rsd_memory_device_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_memory_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_MEMORY_DEVICE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    return
        dmi_stream_decode(stream, dmi_word_t, &info->device_handle) and
        dmi_stream_decode(stream, dmi_byte_t, &info->memory_type) and
        dmi_stream_decode(stream, dmi_byte_t, &info->memory_media) and
        dmi_stream_decode_str(stream, &info->firmware_revision) and
        dmi_stream_decode_str(stream, &info->firmware_api_version) and
        dmi_stream_decode(stream, dmi_dword_t, &info->maximum_tdp) and
        dmi_stream_decode(stream, dmi_byte_t, &info->smbus_address);
}
