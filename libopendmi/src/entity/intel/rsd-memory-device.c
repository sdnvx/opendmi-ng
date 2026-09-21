//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-memory-device-internal.h>

const dmi_entity_spec_t dmi_intel_rsd_memory_device_spec =
{
    .code            = "intel-rsd-memory-device-ex",
    .name            = "Intel RSD memory device extended information",
    .type            = DMI_TYPE(INTEL_RSD_MEMORY_DEVICE),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x0F,
        .decoded_length  = sizeof(dmi_intel_rsd_memory_device_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_rsd_memory_device_t, device_handle, dmi_word_t),
        DMI_FIELD(dmi_intel_rsd_memory_device_t, memory_type,   dmi_byte_t),
        DMI_FIELD(dmi_intel_rsd_memory_device_t, memory_media,  dmi_byte_t),
        DMI_FIELD_STRING(dmi_intel_rsd_memory_device_t, firmware_revision),
        DMI_FIELD_STRING(dmi_intel_rsd_memory_device_t, firmware_api_version),
        DMI_FIELD(dmi_intel_rsd_memory_device_t, maximum_tdp,   dmi_dword_t),
        DMI_FIELD(dmi_intel_rsd_memory_device_t, smbus_address, dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
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
    }),
};
