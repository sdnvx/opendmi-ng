//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/intel.h>
#include <opendmi/entity/intel/rsd-phys-device-mapping.h>

/**
 * @brief Size of the device location in the structure.
 */
#define DMI_INTEL_RSD_PHYS_DEVICE_SIZE 4

static bool dmi_intel_rsd_phys_device_mapping_decode(dmi_entity_t *entity);
static void dmi_intel_rsd_phys_device_mapping_cleanup(dmi_entity_t *entity);

const dmi_name_set_t dmi_intel_rsd_phys_device_type_names =
{
    .code  = "intel-rsd-phys-device-types",
    .names = (const dmi_name_t[]){
        {
            .id   = DMI_INTEL_RSD_PHYS_DEVICE_TYPE_INVALID,
            .code = "invalid",
            .name = "Undefined or invalid"
        },
        {
            .id   = DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PROCESSOR,
            .code = "processor",
            .name = "Processor"
        },
        {
            .id   = DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PCIE_SLOT,
            .code = "pcie-slot",
            .name = "PCIe system slot"
        },
        {
            .id   = DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY,
            .code = "memory-device",
            .name = "Memory device"
        },
        DMI_NAME_NULL
    }
};

// Location numbers are named according to the device type, and the data of
// unknown device types is shown as stored
static const dmi_attribute_t dmi_intel_rsd_phys_device_attrs[] =
{
    DMI_ATTRIBUTE(dmi_intel_rsd_phys_device_t, handle, HANDLE, {
        .code = "handle",
        .name = "Structure handle"
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_phys_device_t, type, {
        .code     = "socket-number",
        .name     = "Socket number",
        .variants = (const dmi_attribute_variant_t[]){
            DMI_VARIANT(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PROCESSOR,
                        dmi_intel_rsd_phys_device_t, primary_number, INTEGER, {}),
            DMI_VARIANT(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY,
                        dmi_intel_rsd_phys_device_t, primary_number, INTEGER, {}),
            DMI_VARIANT_NULL
        }
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_phys_device_t, type, {
        .code     = "riser-number",
        .name     = "Riser number",
        .variants = (const dmi_attribute_variant_t[]){
            DMI_VARIANT(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PCIE_SLOT,
                        dmi_intel_rsd_phys_device_t, primary_number, INTEGER, {}),
            DMI_VARIANT_NULL
        }
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_phys_device_t, type, {
        .code     = "slot-number",
        .name     = "Slot number",
        .variants = (const dmi_attribute_variant_t[]){
            DMI_VARIANT(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PCIE_SLOT,
                        dmi_intel_rsd_phys_device_t, secondary_number, INTEGER, {}),
            DMI_VARIANT(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY,
                        dmi_intel_rsd_phys_device_t, secondary_number, INTEGER, {}),
            DMI_VARIANT_NULL
        }
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_phys_device_t, is_raw, {
        .code     = "data",
        .name     = "Location data",
        .variants = (const dmi_attribute_variant_t[]){
            DMI_VARIANT(true, dmi_intel_rsd_phys_device_t, data, BINARY, {}),
            DMI_VARIANT_NULL
        }
    }),
    DMI_ATTRIBUTE_NULL
};

const dmi_entity_spec_t dmi_intel_rsd_phys_device_mapping_spec =
{
    .type            = DMI_TYPE(INTEL_RSD_PHYS_DEVICE_MAPPING),
    .code            = "intel-rsd-phys-device-mapping",
    .name            = "Intel RSD physical device mapping information",
    .description     = (const char *[]){
        "Declares the relationship between physical devices and their SMBIOS "
        "representation, allowing to link structures to physical locations "
        "of the devices in the system.",
        //
        "Each structure describes devices of one type only. There may be "
        "several structures describing devices of the same type, if they do "
        "not fit into a single structure.",
        //
        nullptr
    },
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x06,
    .decoded_length  = sizeof(dmi_intel_rsd_phys_device_mapping_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_intel_rsd_phys_device_mapping_t, device_type, ENUM, {
            .code   = "device-type",
            .name   = "Device type",
            .values = &dmi_intel_rsd_phys_device_type_names
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_intel_rsd_phys_device_mapping_t, devices, device_count, STRUCT, {
            .code   = "devices",
            .name   = "Devices",
            .attrs  = dmi_intel_rsd_phys_device_attrs
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_intel_rsd_phys_device_mapping_decode,
        .cleanup = dmi_intel_rsd_phys_device_mapping_cleanup
    }
};

static bool dmi_intel_rsd_phys_device_mapping_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_phys_device_mapping_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PHYS_DEVICE_MAPPING));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    dmi_byte_t device_type = 0;

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &device_type) and
        dmi_stream_skip(stream, 1);
    if (not status)
        return false;

    info->device_type = dmi_cast(info->device_type, device_type);

    size_t count = dmi_stream_remaining(stream) / DMI_INTEL_RSD_PHYS_DEVICE_SIZE;
    if (count == 0)
        return true;

    info->devices = dmi_alloc_array(context, sizeof(*info->devices), count);
    if (info->devices == nullptr)
        return false;

    for (size_t i = 0; i < count; i++) {
        dmi_intel_rsd_phys_device_t *device = &info->devices[i];

        status =
            dmi_stream_decode(stream, dmi_word_t, &device->handle) and
            dmi_stream_decode_bin(stream, 2, &device->data);
        if (not status)
            return false;

        device->type             = info->device_type;
        device->primary_number   = device->data.data[0];
        device->secondary_number = device->data.data[1];

        switch (device->type) {
        case DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PROCESSOR:
        case DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PCIE_SLOT:
        case DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY:
            break;
        default:
            device->is_raw = true;
            break;
        }

        info->device_count++;
    }

    // Trailing bytes do not form a complete device location
    if (not dmi_stream_is_done(stream))
        return dmi_entity_incomplete(entity);

    return true;
}

static void dmi_intel_rsd_phys_device_mapping_cleanup(dmi_entity_t *entity)
{
    dmi_intel_rsd_phys_device_mapping_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PHYS_DEVICE_MAPPING));
    if (info == nullptr)
        return;

    dmi_free(info->devices);
}
