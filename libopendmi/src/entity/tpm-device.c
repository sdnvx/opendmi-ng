//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/stream.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/tpm-device.h>

static bool dmi_tpm_device_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_tpm_device_feature_names =
{
    .code  = "tpm-device-features",
    .names = (dmi_name_t[]){
        {
            .id   = 2,
            .code = "is-unsupported",
            .name = "Characteristics are not supported"
        },
        {
            .id   = 3,
            .code = "is-update-configurable",
            .name = "Configurable via firmware update"
        },
        {
            .id   = 4,
            .code = "is-software-configurable",
            .name = "Configurable via platform software support"
        },
        {
            .id = 5,
            .code = "is-proprietary-configurable",
            .name = "Configurable via OEM proprietary mechanism"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_tpm_device_spec =
{
    .code            = "tpm-device",
    .name            = "TPM device",
    .type            = DMI_TYPE(TPM_DEVICE),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x1F,
    .decoded_length  = sizeof(dmi_tpm_device_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_tpm_device_t, spec_version, VERSION, {
            .code   = "specification-version",
            .name   = "Specification version",
            .scale  = 2
        }),
        DMI_ATTRIBUTE(dmi_tpm_device_t, firmware_version, INTEGER, {
            .code   = "firmware-version",
            .name   = "Firmware version",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_tpm_device_t, description, STRING, {
            .code   = "description",
            .name   = "Description"
        }),
        DMI_ATTRIBUTE(dmi_tpm_device_t, features, SET, {
            .code   = "characteristics",
            .name   = "Characteristics",
            .values = &dmi_tpm_device_feature_names
        }),
        DMI_ATTRIBUTE(dmi_tpm_device_t, oem_defined, INTEGER, {
            .code   = "oem-defined",
            .name   = "OEM-defined",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_tpm_device_decode
    }
};

static bool dmi_tpm_device_decode(dmi_entity_t *entity)
{
    dmi_tpm_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(TPM_DEVICE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    uint8_t spec_version_major;
    uint8_t spec_version_minor;
    uint32_t firmware_version_1;
    uint32_t firmware_version_2;

    bool status =
        dmi_stream_read_data(&entity->stream, info->vendor_id, sizeof(info->vendor_id) - 1) and
        dmi_stream_decode(stream, dmi_byte_t, &spec_version_major) and
        dmi_stream_decode(stream, dmi_byte_t, &spec_version_minor) and
        dmi_stream_decode(stream, dmi_dword_t, &firmware_version_1) and
        dmi_stream_decode(stream, dmi_dword_t, &firmware_version_2) and
        dmi_stream_decode_str(stream, &info->description) and
        dmi_stream_decode(stream, dmi_qword_t, &info->features.__value) and
        dmi_stream_decode(stream, dmi_dword_t, &info->oem_defined);
    if (not status)
        return false;

    // Terminate vendor identifier
    info->vendor_id[sizeof(info->vendor_id) - 1] = 0;

    // Decode specification version
    info->spec_version = dmi_version(spec_version_major, spec_version_minor, 0);

    // Decode firmware version
    if (dmi_version_major(info->spec_version) > 1) {
        info->firmware_version = ((uint64_t)firmware_version_1 << 32) |
                                 (uint64_t)firmware_version_2;
    } else {
        info->firmware_version = firmware_version_1;
    }

    return true;
}
