//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/lenovo.h>

#include <opendmi/entity/lenovo/oem-internal.h>

//
// Structures of both types begin with a signature, followed by the offset,
// the number and the revision of the OEM structure they hold. The ones of
// the OEM structures of known layouts are told by the signature together
// with the number and the revision, and the rest by the signature alone
//

#define DMI_LENOVO_OEM_ATTRIBUTES(__type)             \
    DMI_ATTRIBUTE(__type, number, INTEGER, {          \
        .code = "number",                             \
        .name = "OEM structure number"                \
    }),                                               \
    DMI_ATTRIBUTE(__type, revision, INTEGER, {        \
        .code = "revision",                           \
        .name = "OEM structure revision"              \
    })

const dmi_entity_spec_t dmi_lenovo_mobile_oem_spec =
{
    .type        = DMI_TYPE(LENOVO_MOBILE_OEM),
    .code        = "lenovo-mobile-oem",
    .name        = "Lenovo mobile PC OEM data",
    .description = (const char *[]){
        "Holds an OEM structure of a Lenovo laptop whose layout is not known.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x09,
        .decoded_length = sizeof(dmi_lenovo_oem_t),
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = "TP",
            .size   = 2
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BINARY(dmi_lenovo_oem_t, signature, 2),
        DMI_FIELD(dmi_lenovo_oem_t, offset,   dmi_byte_t),
        DMI_FIELD(dmi_lenovo_oem_t, number,   dmi_byte_t),
        DMI_FIELD(dmi_lenovo_oem_t, revision, dmi_byte_t),
        DMI_FIELD_BINARY(dmi_lenovo_oem_t, data, DMI_FIELD_LENGTH_REST),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_LENOVO_OEM_ATTRIBUTES(dmi_lenovo_oem_t),
        DMI_ATTRIBUTE(dmi_lenovo_oem_t, data, BINARY, {
            .code = "data",
            .name = "Data"
        }),
        {}
    })
};

const dmi_entity_spec_t dmi_lenovo_oem_spec =
{
    .type        = DMI_TYPE(LENOVO_OEM),
    .code        = "lenovo-oem",
    .name        = "Lenovo OEM data",
    .description = (const char *[]){
        "Holds an OEM structure of a Lenovo system whose layout is not known.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0D,
        .decoded_length = sizeof(dmi_lenovo_oem_t),
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = "LENOVO",
            .size   = 6
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BINARY(dmi_lenovo_oem_t, signature, 6),
        DMI_FIELD(dmi_lenovo_oem_t, offset,   dmi_byte_t),
        DMI_FIELD(dmi_lenovo_oem_t, number,   dmi_byte_t),
        DMI_FIELD(dmi_lenovo_oem_t, revision, dmi_byte_t),
        DMI_FIELD_BINARY(dmi_lenovo_oem_t, data, DMI_FIELD_LENGTH_REST),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_LENOVO_OEM_ATTRIBUTES(dmi_lenovo_oem_t),
        DMI_ATTRIBUTE(dmi_lenovo_oem_t, data, BINARY, {
            .code = "data",
            .name = "Data"
        }),
        {}
    })
};

const dmi_entity_spec_t dmi_lenovo_device_presence_spec =
{
    .type        = DMI_TYPE(LENOVO_MOBILE_OEM),
    .code        = "lenovo-device-presence",
    .name        = "Lenovo ThinkPad device presence detection",
    .description = (const char *[]){
        "Tells which devices of a ThinkPad are present, of which only the "
        "fingerprint reader is known.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0A,
        .decoded_length = sizeof(dmi_lenovo_device_presence_t),
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = "TP\x07\x03\x01",
            .size   = 5
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BINARY(dmi_lenovo_device_presence_t, signature, 2),
        DMI_FIELD(dmi_lenovo_device_presence_t, offset,   dmi_byte_t),
        DMI_FIELD(dmi_lenovo_device_presence_t, number,   dmi_byte_t),
        DMI_FIELD(dmi_lenovo_device_presence_t, revision, dmi_byte_t),
        DMI_FIELD(dmi_lenovo_device_presence_t, devices,  dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_LENOVO_OEM_ATTRIBUTES(dmi_lenovo_device_presence_t),
        DMI_ATTRIBUTE(dmi_lenovo_device_presence_t, devices, INTEGER, {
            .code  = "devices",
            .name  = "Device presence bits",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_lenovo_device_presence_t, is_fingerprint_reader, BOOL, {
            .code = "is-fingerprint-reader",
            .name = "Fingerprint reader present"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_lenovo_device_presence_derive
    }
};

const dmi_entity_spec_t dmi_lenovo_bay_io_spec =
{
    .type        = DMI_TYPE(LENOVO_MOBILE_OEM),
    .code        = "lenovo-bay-io",
    .name        = "Lenovo bay I/O",
    .description = (const char *[]){
        "Describes the bays of a laptop, whose layout is not known past the "
        "version.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x11,
        .decoded_length = sizeof(dmi_lenovo_bay_io_t),
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = "TP\x07\x02" "BAY I/O ",
            .size   = 12
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BINARY(dmi_lenovo_bay_io_t, signature, 2),
        DMI_FIELD(dmi_lenovo_bay_io_t, offset, dmi_byte_t),
        DMI_FIELD(dmi_lenovo_bay_io_t, number, dmi_byte_t),
        DMI_FIELD_BINARY(dmi_lenovo_bay_io_t, bay_signature, 8),
        DMI_FIELD(dmi_lenovo_bay_io_t, version, dmi_byte_t),
        DMI_FIELD_BINARY(dmi_lenovo_bay_io_t, data, DMI_FIELD_LENGTH_REST),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_lenovo_bay_io_t, number, INTEGER, {
            .code = "number",
            .name = "OEM structure number"
        }),
        DMI_ATTRIBUTE(dmi_lenovo_bay_io_t, version, INTEGER, {
            .code = "version",
            .name = "Version"
        }),
        DMI_ATTRIBUTE(dmi_lenovo_bay_io_t, data, BINARY, {
            .code = "data",
            .name = "Data"
        }),
        {}
    })
};

const dmi_entity_spec_t dmi_lenovo_ecp_spec =
{
    .type        = DMI_TYPE(LENOVO_OEM),
    .code        = "lenovo-ecp",
    .name        = "Lenovo ThinkPad embedded controller program",
    .description = (const char *[]){
        "Tells the version of the program of the embedded controller, which "
        "the README files of the firmware updates of Lenovo name.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0F,
        .decoded_length = sizeof(dmi_lenovo_ecp_t),
        .signature      = DMI_SIGNATURE({
            .offset = 0x04,
            .bytes  = "LENOVO\x0B\x07\x01",
            .size   = 9
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BINARY(dmi_lenovo_ecp_t, signature, 6),
        DMI_FIELD(dmi_lenovo_ecp_t, offset,   dmi_byte_t),
        DMI_FIELD(dmi_lenovo_ecp_t, number,   dmi_byte_t),
        DMI_FIELD(dmi_lenovo_ecp_t, revision, dmi_byte_t),
        DMI_FIELD_STRING(dmi_lenovo_ecp_t, version),
        DMI_FIELD_STRING(dmi_lenovo_ecp_t, release_date),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_LENOVO_OEM_ATTRIBUTES(dmi_lenovo_ecp_t),
        DMI_ATTRIBUTE(dmi_lenovo_ecp_t, version, STRING, {
            .code = "version",
            .name = "Version ID"
        }),
        DMI_ATTRIBUTE(dmi_lenovo_ecp_t, release_date, STRING, {
            .code = "release-date",
            .name = "Release date"
        }),
        {}
    })
};

bool dmi_lenovo_device_presence_derive(dmi_entity_t *entity)
{
    dmi_lenovo_device_presence_t *info = dmi_entity_info(entity, DMI_TYPE(LENOVO_MOBILE_OEM));
    if (info == nullptr)
        return false;

    info->is_fingerprint_reader = (info->devices & 0x01) != 0;

    return true;
}
