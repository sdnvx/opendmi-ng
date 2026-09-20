//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/stream.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/tpm-device.h>

static bool dmi_tpm_device_decode(dmi_entity_t *entity);
static void dmi_tpm_device_decode_vendor(dmi_entity_t *entity, dmi_tpm_device_t *info);

static const dmi_name_set_t dmi_tpm_device_feature_names =
{
    .code  = "tpm-device-feature",
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
        DMI_ATTRIBUTE(dmi_tpm_device_t, vendor, STRING, {
            .code   = "vendor-id",
            .name   = "Vendor ID"
        }),
        DMI_ATTRIBUTE(dmi_tpm_device_t, spec_version, VERSION, {
            .code   = "specification-version",
            .name   = "Specification version",
            .scale  = 2
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_tpm_device_t, firmware_version_format, {
            .code     = "firmware-version",
            .name     = "Firmware version",
            .variants = (const dmi_attribute_variant_t[]){
                DMI_VARIANT(DMI_TPM_FIRMWARE_VERSION_FORMAT_TPM_1, dmi_tpm_device_t, firmware_revision, VERSION, {
                    .scale = 2
                }),
                DMI_VARIANT(DMI_TPM_FIRMWARE_VERSION_FORMAT_TPM_2, dmi_tpm_device_t, firmware_version_2, STRUCT, {
                    .attrs = (const dmi_attribute_t[]){
                        DMI_ATTRIBUTE(dmi_tpm_firmware_version_t, major, INTEGER, {
                            .code = "major",
                            .name = "Major"
                        }),
                        DMI_ATTRIBUTE(dmi_tpm_firmware_version_t, minor, INTEGER, {
                            .code = "minor",
                            .name = "Minor"
                        }),
                        DMI_ATTRIBUTE(dmi_tpm_firmware_version_t, vendor_specific, INTEGER, {
                            .code  = "vendor-specific",
                            .name  = "Vendor-specific",
                            .flags = DMI_ATTRIBUTE_FLAG_HEX
                        }),
                        DMI_ATTRIBUTE_NULL
                    }
                }),
                DMI_VARIANT_DEFAULT(dmi_tpm_device_t, firmware_version, INTEGER, {
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_VARIANT_NULL
            }
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

    dmi_stream_t *stream = dmi_entity_stream(entity);

    uint8_t spec_version_major;
    uint8_t spec_version_minor;
    uint32_t firmware_version_1;
    uint32_t firmware_version_2;

    bool status =
        dmi_stream_read_data(stream, info->vendor_id, sizeof(info->vendor_id) - 1) and
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

    dmi_tpm_device_decode_vendor(entity, info);

    // Decode specification version
    info->spec_version = dmi_version(spec_version_major, spec_version_minor, 0);

    // Firmware version is kept as stored, and parsed according to the TPM
    // version
    info->firmware_version = ((uint64_t)firmware_version_1 << 32) | (uint64_t)firmware_version_2;

    switch (dmi_version_major(info->spec_version)) {
    case 1:
        // TCPA_VERSION structure: major, minor, revMajor and revMinor
        info->firmware_version_format = DMI_TPM_FIRMWARE_VERSION_FORMAT_TPM_1;
        info->firmware_revision       = dmi_version((firmware_version_1 >> 16) & 0xFFu,
                                                    (firmware_version_1 >> 24) & 0xFFu, 0);
        break;

    case 2:
        info->firmware_version_format            = DMI_TPM_FIRMWARE_VERSION_FORMAT_TPM_2;
        info->firmware_version_2.major           = (uint16_t)(firmware_version_1 >> 16);
        info->firmware_version_2.minor           = (uint16_t)(firmware_version_1 & 0xFFFFu);
        info->firmware_version_2.vendor_specific = firmware_version_2;
        break;

    default:
        info->firmware_version_format = DMI_TPM_FIRMWARE_VERSION_FORMAT_RAW;
        break;
    }

    return true;
}

static void dmi_tpm_device_decode_vendor(dmi_entity_t *entity, dmi_tpm_device_t *info)
{
    char *id = info->vendor_id;

    dmi_context_t *context = dmi_entity_context(entity);

    // Some firmware stores vendor identifier as a little-endian double word,
    // so that it starts with the terminating zero, e.g. "\0XFI" for "IFX"
    if ((id[0] == 0) and (id[3] != 0)) {
        dmi_log_notice(context,
                       "Handle 0x%04hx (%s): Vendor ID bytes are reversed",
                       dmi_entity_handle(entity), dmi_type_name(context, entity->type));

        for (size_t i = 0; i < 2; i++) {
            char c = id[i];
            id[i]     = id[3 - i];
            id[3 - i] = c;
        }
    }

    // Only printable characters are kept, identifier ends at the first other
    size_t length = 0;
    while ((length < 4) and (id[length] >= 0x20) and (id[length] < 0x7F))
        length++;

    id[length] = 0;

    info->vendor = (length > 0) ? id : nullptr;
}

