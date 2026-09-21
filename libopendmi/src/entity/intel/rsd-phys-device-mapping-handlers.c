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

// Location numbers are named according to the device type, and the data of
// unknown device types is shown as stored
const dmi_attribute_t dmi_intel_rsd_phys_device_attrs[] =
{
    DMI_ATTRIBUTE(dmi_intel_rsd_phys_device_t, handle, HANDLE, {
        .code = "handle",
        .name = "Structure handle"
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_phys_device_t, type, {
        .code     = "socket-number",
        .name     = "Socket number",
        .variants = DMI_VARIANTS({
            DMI_VARIANT(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PROCESSOR,
                        dmi_intel_rsd_phys_device_t, primary_number, INTEGER, {}),
            DMI_VARIANT(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY,
                        dmi_intel_rsd_phys_device_t, primary_number, INTEGER, {}),
            {}
        })
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_phys_device_t, type, {
        .code     = "riser-number",
        .name     = "Riser number",
        .variants = DMI_VARIANTS({
            DMI_VARIANT(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PCIE_SLOT,
                        dmi_intel_rsd_phys_device_t, primary_number, INTEGER, {}),
            {}
        })
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_phys_device_t, type, {
        .code     = "slot-number",
        .name     = "Slot number",
        .variants = DMI_VARIANTS({
            DMI_VARIANT(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PCIE_SLOT,
                        dmi_intel_rsd_phys_device_t, secondary_number, INTEGER, {}),
            DMI_VARIANT(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY,
                        dmi_intel_rsd_phys_device_t, secondary_number, INTEGER, {}),
            {}
        })
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_phys_device_t, is_raw, {
        .code     = "data",
        .name     = "Location data",
        .variants = DMI_VARIANTS({
            DMI_VARIANT(true, dmi_intel_rsd_phys_device_t, data, BINARY, {}),
            {}
        })
    }),
    {}
};

//
// Every device of a structure is of the type the structure declares, and the
// location data means what that type says it does.
//
bool dmi_intel_rsd_phys_device_mapping_derive(dmi_entity_t *entity)
{
    dmi_intel_rsd_phys_device_mapping_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PHYS_DEVICE_MAPPING));
    if (info == nullptr)
        return false;

    for (size_t i = 0; i < info->device_count; i++) {
        dmi_intel_rsd_phys_device_t *device = &info->devices[i];

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
    }

    return true;
}

void dmi_intel_rsd_phys_device_mapping_cleanup(dmi_entity_t *entity)
{
    dmi_intel_rsd_phys_device_mapping_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PHYS_DEVICE_MAPPING));
    if (info == nullptr)
        return;

    dmi_free(info->devices);
}
