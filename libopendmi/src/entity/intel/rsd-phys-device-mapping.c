//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-phys-device-mapping-internal.h>

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
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x06,
        .decoded_length  = sizeof(dmi_intel_rsd_phys_device_mapping_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_rsd_phys_device_mapping_t, device_type, BYTE),

        // Reserved byte
        DMI_FIELD_SKIP(sizeof(dmi_byte_t)),

        // Devices run to the end of the structure, which carries no number of
        // them of its own
        DMI_FIELD_ARRAY(dmi_intel_rsd_phys_device_mapping_t, devices, device_count,
            .stride = DMI_INTEL_RSD_PHYS_DEVICE_SIZE,
            .fields = DMI_FIELDS({
                DMI_FIELD(dmi_intel_rsd_phys_device_t, handle, WORD),
                DMI_FIELD_BINARY(dmi_intel_rsd_phys_device_t, data, 2 * sizeof(dmi_byte_t)),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
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
        {}
    }),

    .handlers = {
        .derive  = dmi_intel_rsd_phys_device_mapping_derive,
        .cleanup = dmi_intel_rsd_phys_device_mapping_cleanup
    }
};
