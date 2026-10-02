//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "device-correlation-internal.h"

const dmi_entity_spec_t dmi_hpe_device_correlation_spec =
{
    .type        = DMI_TYPE(hpe_device_correlation),
    .code        = "hpe-device-correlation",
    .name        = "HP/HPE device correlation record",
    .description = (const char *[]){
        "Correlates a device with the system slot (type 9) or the onboard "
        "device (type 41) it is found at, and with the UEFI boot entries "
        "naming it.",
        //
        nullptr
    },
    .params = {
        .generations    = { .minimum = DMI_HPE_GEN9 },
        .minimum_length = 0x1F,
        .decoded_length = sizeof(dmi_hpe_device_correlation_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_device_correlation_t, device_handle,    dmi_word_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, smbus_handle,     dmi_word_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, pci_vendor_id,    dmi_word_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, pci_device_id,    dmi_word_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, pci_subvendor_id, dmi_word_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, pci_subdevice_id, dmi_word_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, pci_class,        dmi_byte_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, pci_subclass,     dmi_byte_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, parent_handle,    dmi_word_t),

        DMI_FIELD_BITS(dmi_hpe_device_correlation_t, is_peer_bifurcated, 1),
        DMI_FIELD_BITS(dmi_hpe_device_correlation_t, is_upstream,        1),
        DMI_FIELD_PAD(dmi_word_t),

        DMI_FIELD(dmi_hpe_device_correlation_t, device_type,     dmi_byte_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, device_location, dmi_byte_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, instance,        dmi_byte_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, sub_instance,    dmi_byte_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, bay,             dmi_byte_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, enclosure,       dmi_byte_t),
        DMI_FIELD_STRING(dmi_hpe_device_correlation_t, uefi_device_path),
        DMI_FIELD_STRING(dmi_hpe_device_correlation_t, uefi_device_name),
        DMI_FIELD_STRING(dmi_hpe_device_correlation_t, device_name),

        DMI_FIELD_GROUP(),
        DMI_FIELD_STRING(dmi_hpe_device_correlation_t, uefi_location),
        DMI_FIELD(dmi_hpe_device_correlation_t, physical_handle, dmi_word_t,
                  .absent = dmi_value_ptr(DMI_HANDLE_INVALID)),

        DMI_FIELD_GROUP(),
        DMI_FIELD_STRING(dmi_hpe_device_correlation_t, part_number),
        DMI_FIELD_STRING(dmi_hpe_device_correlation_t, serial_number),

        DMI_FIELD_GROUP(.present = dmi_member(dmi_hpe_device_correlation_t, has_pci_location)),
        DMI_FIELD(dmi_hpe_device_correlation_t, segment, dmi_word_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, bus,     dmi_byte_t),
        DMI_FIELD(dmi_hpe_device_correlation_t, devfn,   dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, device_handle, HANDLE, {
            .code    = "device-handle",
            .name    = "Associated device handle",
            .unspec  = dmi_value_ptr((dmi_handle_t)0xFFFE),
            .targets = dmi_types(DMI_TYPE(system_slots), DMI_TYPE(onboard_device_ex))
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, smbus_handle, HANDLE, {
            .code   = "smbus-handle",
            .name   = "Associated SMBus segment handle",
            .unspec = dmi_value_ptr((dmi_handle_t)0xFFFE)
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, pci_vendor_id, INTEGER, {
            .code   = "pci-vendor-id",
            .name   = "PCI vendor ID",
            .unspec = dmi_value_ptr((uint16_t)UINT16_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, pci_device_id, INTEGER, {
            .code   = "pci-device-id",
            .name   = "PCI device ID",
            .unspec = dmi_value_ptr((uint16_t)UINT16_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, pci_subvendor_id, INTEGER, {
            .code   = "pci-subvendor-id",
            .name   = "PCI subsystem vendor ID",
            .unspec = dmi_value_ptr((uint16_t)UINT16_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, pci_subdevice_id, INTEGER, {
            .code   = "pci-subdevice-id",
            .name   = "PCI subsystem ID",
            .unspec = dmi_value_ptr((uint16_t)UINT16_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, pci_class, INTEGER, {
            .code   = "pci-class",
            .name   = "PCI class code",
            .unspec = dmi_value_ptr((uint8_t)UINT8_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, pci_subclass, INTEGER, {
            .code   = "pci-subclass",
            .name   = "PCI subclass code",
            .unspec = dmi_value_ptr((uint8_t)UINT8_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        // Structure the handle refers to is not documented, so it is not
        // checked
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, parent_handle, HANDLE, {
            .code   = "parent-handle",
            .name   = "Parent handle",
            .unspec = dmi_value_ptr((dmi_handle_t)0xFFFE)
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, is_peer_bifurcated, BOOL, {
            .code = "is-peer-bifurcated",
            .name = "Peer bifurcated device"
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, is_upstream, BOOL, {
            .code = "is-upstream",
            .name = "Upstream device"
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, device_type, ENUM, {
            .code   = "device-type",
            .name   = "Device type",
            .values = &dmi_hpe_device_type_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, device_location, ENUM, {
            .code   = "device-location",
            .name   = "Device location",
            .values = &dmi_hpe_device_location_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, instance, INTEGER, {
            .code = "instance",
            .name = "Device instance"
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, sub_instance, INTEGER, {
            .code = "sub-instance",
            .name = "Device sub-instance"
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, bay, INTEGER, {
            .code    = "bay",
            .name    = "Bay",
            .unspec  = dmi_value_ptr((uint8_t)UINT8_MAX),
            .unknown = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, enclosure, INTEGER, {
            .code    = "enclosure",
            .name    = "Enclosure",
            .unspec  = dmi_value_ptr((uint8_t)UINT8_MAX),
            .unknown = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, uefi_device_path, STRING, {
            .code = "uefi-device-path",
            .name = "UEFI device path"
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, uefi_device_name, STRING, {
            .code = "uefi-device-name",
            .name = "UEFI device structured name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, device_name, STRING, {
            .code = "device-name",
            .name = "Device name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, uefi_location, STRING, {
            .code = "uefi-location",
            .name = "UEFI location"
        }),
        // Slot is given for the peer bifurcated devices only
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_device_correlation_t, is_peer_bifurcated, {
            .code     = "physical-handle",
            .name     = "Physical slot handle",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_device_correlation_t, physical_handle, HANDLE, {
                    .code    = "physical-handle",
                    .name    = "Physical slot handle",
                    .targets = dmi_types(DMI_TYPE(system_slots))
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, part_number, STRING, {
            .code = "part-number",
            .name = "Part number"
        }),
        DMI_ATTRIBUTE(dmi_hpe_device_correlation_t, serial_number, STRING, {
            .code  = "serial-number",
            .name  = "Serial number",
            .flags = DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        // Structures shorter than 40 bytes leave the PCI location out
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_device_correlation_t, has_pci_location, {
            .code     = "segment",
            .name     = "PCI segment group",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_device_correlation_t, segment, INTEGER, {
                    .code  = "segment",
                    .name  = "PCI segment group",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_device_correlation_t, has_pci_location, {
            .code     = "bus",
            .name     = "PCI bus",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_device_correlation_t, bus, INTEGER, {
                    .code  = "bus",
                    .name  = "PCI bus",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_device_correlation_t, has_pci_location, {
            .code     = "devfn",
            .name     = "PCI device and function",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_device_correlation_t, devfn, INTEGER, {
                    .code  = "devfn",
                    .name  = "PCI device and function",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        {}
    })
};

const dmi_name_set_t dmi_hpe_device_type_names =
{
    .code  = "hpe-device-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_DEVICE_TYPE_UNKNOWN,
            .code = "unknown",
            .name = "Unknown"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_FLEXIBLE_LOM,
            .code = "flexible-lom",
            .name = "Flexible LOM"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_EMBEDDED_LOM,
            .code = "embedded-lom",
            .name = "Embedded LOM"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_SLOT_NIC,
            .code = "slot-nic",
            .name = "NIC in a slot"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_STORAGE,
            .code = "storage",
            .name = "Storage controller"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_SMART_ARRAY,
            .code = "smart-array",
            .name = "Smart Array storage controller"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_USB_DISK,
            .code = "usb-disk",
            .name = "USB hard disk"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_OTHER_PCI,
            .code = "other-pci",
            .name = "Other PCI device"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_RAM_DISK,
            .code = "ram-disk",
            .name = "RAM disk"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_FIRMWARE_VOLUME,
            .code = "firmware-volume",
            .name = "Firmware volume"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_UEFI_SHELL,
            .code = "uefi-shell",
            .name = "UEFI shell"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_USB_BOOT,
            .code = "usb-boot",
            .name = "Generic UEFI USB boot entry"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_DYNAMIC_ARRAY,
            .code = "dynamic-array",
            .name = "Dynamic Smart Array controller"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_FILE,
            .code = "file",
            .name = "File"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_NVME,
            .code = "nvme",
            .name = "NVMe hard drive"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_NVDIMM,
            .code = "nvdimm",
            .name = "NVDIMM"
        },
        {
            .id   = DMI_HPE_DEVICE_TYPE_EMBEDDED_GPU,
            .code = "embedded-gpu",
            .name = "Embedded GPU"
        },
        {}
    })
};

DMI_NAME_FUNCTION(dmi_hpe_device_type)

const dmi_name_set_t dmi_hpe_device_location_names =
{
    .code  = "hpe-device-location",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_DEVICE_LOCATION_UNKNOWN,
            .code = "unknown",
            .name = "Unknown"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_EMBEDDED,
            .code = "embedded",
            .name = "Embedded"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_ILO_MEDIA,
            .code = "ilo-media",
            .name = "iLO virtual media"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_FRONT_USB,
            .code = "front-usb",
            .name = "Front USB port"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_REAR_USB,
            .code = "rear-usb",
            .name = "Rear USB port"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_INTERNAL_USB,
            .code = "internal-usb",
            .name = "Internal USB"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_INTERNAL_SD,
            .code = "internal-sd",
            .name = "Internal SD card"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_VIRTUAL_USB,
            .code = "virtual-usb",
            .name = "Internal virtual USB (embedded NAND)"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_EMBEDDED_SATA,
            .code = "embedded-sata",
            .name = "Embedded SATA port"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_EMBEDDED_ARRAY,
            .code = "embedded-array",
            .name = "Embedded Smart Array"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_PCI_SLOT,
            .code = "pci-slot",
            .name = "PCI slot"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_RAM,
            .code = "ram",
            .name = "RAM memory"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_USB,
            .code = "usb",
            .name = "USB"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_DYNAMIC_ARRAY,
            .code = "dynamic-array",
            .name = "Dynamic Smart Array controller"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_URL,
            .code = "url",
            .name = "URL"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_NVME_BAY,
            .code = "nvme-bay",
            .name = "NVMe drive bay"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_NVDIMM_PROCESSOR,
            .code = "nvdimm-processor",
            .name = "NVDIMM processor"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_NVDIMM_BOARD,
            .code = "nvdimm-board",
            .name = "NVDIMM board"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_NVME_RISER,
            .code = "nvme-riser",
            .name = "NVMe riser"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_NVDIMM_NAMESPACE,
            .code = "nvdimm-namespace",
            .name = "NVDIMM namespace"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_VROC_SATA,
            .code = "vroc-sata",
            .name = "VROC SATA"
        },
        {
            .id   = DMI_HPE_DEVICE_LOCATION_VROC_NVME,
            .code = "vroc-nvme",
            .name = "VROC NVMe"
        },
        {}
    })
};

DMI_NAME_FUNCTION(dmi_hpe_device_location)
