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

#include <opendmi/entity/hpe/common-internal.h>
#include <opendmi/entity/hpe/drive-internal.h>

const dmi_entity_spec_t dmi_hpe_drive_spec =
{
    .type        = DMI_TYPE(HPE_DRIVE),
    .code        = "hpe-drive",
    .name        = "HP/HPE hard drive inventory record",
    .description = (const char *[]){
        "Describes an NVMe or SATA drive attached to the system directly, "
        "which the firmware has found at boot.",
        //
        nullptr
    },
    .params = {
        .generations    = { .minimum = DMI_HPE_GEN10 },
        .minimum_length = 0x2A,
        .decoded_length = sizeof(dmi_hpe_drive_t)
    },

    // Power-on hours take 16 bytes, of which the low 8 are read
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_drive_t, correlation_handle, dmi_word_t),
        DMI_FIELD(dmi_hpe_drive_t, drive_type,         dmi_byte_t),
        DMI_FIELD(dmi_hpe_drive_t, unique_id,          dmi_qword_t),
        DMI_FIELD(dmi_hpe_drive_t, legacy_capacity,    dmi_dword_t),
        DMI_FIELD(dmi_hpe_drive_t, power_on_hours,     dmi_qword_t),
        DMI_FIELD_SKIP(8),
        DMI_FIELD_SKIP(1),
        DMI_FIELD(dmi_hpe_drive_t, power,       dmi_byte_t),
        DMI_FIELD(dmi_hpe_drive_t, form_factor, dmi_byte_t),
        DMI_FIELD(dmi_hpe_drive_t, health,      dmi_byte_t),
        DMI_FIELD_STRING(dmi_hpe_drive_t, serial_number),
        DMI_FIELD_STRING(dmi_hpe_drive_t, model_number),
        DMI_FIELD_STRING(dmi_hpe_drive_t, firmware_revision),

        DMI_FIELD_GROUP(),
        DMI_FIELD_STRING(dmi_hpe_drive_t, location),
        DMI_FIELD(dmi_hpe_drive_t, encryption, dmi_byte_t,
                  .absent = dmi_value_ptr((dmi_hpe_encryption_t)UINT8_MAX)),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_drive_t, capacity,         dmi_qword_t),
        DMI_FIELD(dmi_hpe_drive_t, block_size,       dmi_dword_t),
        DMI_FIELD(dmi_hpe_drive_t, rotation_speed,   dmi_word_t),
        DMI_FIELD(dmi_hpe_drive_t, negotiated_speed, dmi_word_t),
        DMI_FIELD(dmi_hpe_drive_t, capable_speed,    dmi_word_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_drive_t, correlation_handle, HANDLE, {
            .code    = "correlation-handle",
            .name    = "Device correlation handle",
            .targets = dmi_types(DMI_TYPE(HPE_DEVICE_CORRELATION))
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, drive_type, ENUM, {
            .code   = "drive-type",
            .name   = "Hard drive type",
            .values = &dmi_hpe_drive_type_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, unique_id, INTEGER, {
            .code  = "unique-id",
            .name  = "Unique ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, legacy_capacity, INTEGER, {
            .code = "legacy-capacity",
            .name = "Capacity in megabytes",
            .unit = DMI_UNIT_MEGABYTE
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, capacity, SIZE, {
            .code   = "capacity",
            .name   = "Capacity",
            .unspec = dmi_value_ptr((uint64_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, power_on_hours, INTEGER, {
            .code = "power-on-hours",
            .name = "Power-on time",
            .unit = DMI_UNIT_HOUR
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, power, INTEGER, {
            .code   = "power",
            .name   = "Power",
            .unit   = DMI_UNIT_WATT,
            .unspec = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, form_factor, ENUM, {
            .code   = "form-factor",
            .name   = "Form factor",
            .values = &dmi_hpe_drive_form_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, health, ENUM, {
            .code    = "health",
            .name    = "Health status",
            .unknown = dmi_value_ptr(DMI_HPE_DRIVE_HEALTH_UNKNOWN),
            .values  = &dmi_hpe_drive_health_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, serial_number, STRING, {
            .code = "serial-number",
            .name = "Serial number"
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, model_number, STRING, {
            .code = "model-number",
            .name = "Model number"
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, firmware_revision, STRING, {
            .code = "firmware-revision",
            .name = "Firmware revision"
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, location, STRING, {
            .code = "location",
            .name = "Location"
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, encryption, ENUM, {
            .code   = "encryption",
            .name   = "Encryption status",
            .unspec = dmi_value_ptr((dmi_hpe_encryption_t)UINT8_MAX),
            .values = &dmi_hpe_encryption_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, block_size, INTEGER, {
            .code   = "block-size",
            .name   = "Block size",
            .unit   = DMI_UNIT_BYTE,
            .unspec = dmi_value_ptr((uint32_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, rotation_speed, INTEGER, {
            .code    = "rotation-speed",
            .name    = "Rotational speed",
            .unit    = DMI_UNIT_REVOLUTION,
            .unspec  = dmi_value_ptr((uint16_t)1),
            .unknown = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, negotiated_speed, INTEGER, {
            .code    = "negotiated-speed",
            .name    = "Negotiated speed",
            .unit    = DMI_UNIT_GIGABIT_SECOND,
            .unknown = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_drive_t, capable_speed, INTEGER, {
            .code    = "capable-speed",
            .name    = "Capable speed",
            .unit    = DMI_UNIT_GIGABIT_SECOND,
            .unknown = dmi_value_ptr((uint16_t)0)
        }),
        {}
    })
};

const dmi_name_set_t dmi_hpe_drive_type_names =
{
    .code  = "hpe-drive-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_DRIVE_TYPE_UNDETERMINED,
            .code = "undetermined",
            .name = "Undetermined"
        },
        {
            .id   = DMI_HPE_DRIVE_TYPE_NVME_SSD,
            .code = "nvme-ssd",
            .name = "NVMe SSD"
        },
        {
            .id   = DMI_HPE_DRIVE_TYPE_SATA,
            .code = "sata",
            .name = "SATA"
        },
        {
            .id   = DMI_HPE_DRIVE_TYPE_SAS,
            .code = "sas",
            .name = "SAS"
        },
        {
            .id   = DMI_HPE_DRIVE_TYPE_SATA_SSD,
            .code = "sata-ssd",
            .name = "SATA SSD"
        },
        {
            .id   = DMI_HPE_DRIVE_TYPE_NVME_VROC,
            .code = "nvme-vroc",
            .name = "NVMe managed by VROC/VMD"
        },
        {}
    })
};

const char *dmi_hpe_drive_type_name(dmi_hpe_drive_type_t value)
{
    return dmi_name_lookup(&dmi_hpe_drive_type_names, (int)value);
}

const dmi_name_set_t dmi_hpe_drive_form_names =
{
    .code  = "hpe-drive-form",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_DRIVE_FORM_3_5,
            .code = "3.5-inch",
            .name = "3.5-inch form factor"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_2_5,
            .code = "2.5-inch",
            .name = "2.5-inch form factor"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_1_8,
            .code = "1.8-inch",
            .name = "1.8-inch form factor"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_SMALLER,
            .code = "smaller",
            .name = "Smaller than 1.8-inch form factor"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_MSATA,
            .code = "msata",
            .name = "mSATA"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_M2,
            .code = "m2",
            .name = "M.2"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_MICRO_SSD,
            .code = "micro-ssd",
            .name = "MicroSSD"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_CFAST,
            .code = "cfast",
            .name = "CFast"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_EDSFF,
            .code = "edsff",
            .name = "EDSFF of unknown form factor"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_EDSFF_1U_S,
            .code = "edsff-1u-s",
            .name = "EDSFF 1U short"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_EDSFF_1U_L,
            .code = "edsff-1u-l",
            .name = "EDSFF 1U long"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_EDSFF_E3_S,
            .code = "edsff-e3-s",
            .name = "EDSFF E3 short"
        },
        {
            .id   = DMI_HPE_DRIVE_FORM_EDSFF_E3_L,
            .code = "edsff-e3-l",
            .name = "EDSFF E3 long"
        },
        {}
    })
};

const char *dmi_hpe_drive_form_name(dmi_hpe_drive_form_t value)
{
    return dmi_name_lookup(&dmi_hpe_drive_form_names, (int)value);
}

const dmi_name_set_t dmi_hpe_drive_health_names =
{
    .code  = "hpe-drive-health",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_DRIVE_HEALTH_OK,
            .code = "ok",
            .name = "OK"
        },
        {
            .id   = DMI_HPE_DRIVE_HEALTH_WARNING,
            .code = "warning",
            .name = "Warning"
        },
        {
            .id   = DMI_HPE_DRIVE_HEALTH_CRITICAL,
            .code = "critical",
            .name = "Critical"
        },
        {
            .id   = DMI_HPE_DRIVE_HEALTH_UNKNOWN,
            .code = "unknown",
            .name = "Unknown"
        },
        {}
    })
};

const char *dmi_hpe_drive_health_name(dmi_hpe_drive_health_t value)
{
    return dmi_name_lookup(&dmi_hpe_drive_health_names, (int)value);
}
