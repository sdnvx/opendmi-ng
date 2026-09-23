//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/reader.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/tpm-device-internal.h>

//
// Vendor identifier is four bytes of text, which some firmware stores as a
// little-endian double word, so that it starts with the terminating zero,
// e.g. "\0XFI" for "IFX". Only printable characters are kept, and the
// identifier ends at the first other.
//
bool dmi_tpm_device_decode_vendor_id(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    dmi_unused(field);

    char *id = value;

    for (size_t i = 0; i < 4; i++)
        id[i] = (char)((data->number >> (i * CHAR_BIT)) & 0xFFu);

    id[4] = 0;

    if ((id[0] == 0) and (id[3] != 0)) {
        if (data->entity != nullptr) {
            dmi_context_t *context = dmi_entity_context(data->entity);

            dmi_log_notice(context, "Handle 0x%04hx (%s): Vendor ID bytes are reversed",
                           dmi_entity_handle(data->entity),
                           dmi_type_name(context, dmi_entity_type(data->entity)));
        }

        for (size_t i = 0; i < 2; i++) {
            char c    = id[i];
            id[i]     = id[3 - i];
            id[3 - i] = c;
        }
    }

    size_t length = 0;
    while ((length < 4) and (id[length] >= 0x20) and (id[length] < 0x7F))
        length++;

    id[length] = 0;

    return true;
}

//
// Specification version is one byte of major and one of minor.
//
bool dmi_tpm_device_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, dmi_version((unsigned int)(data->number & 0xFFu), (unsigned int)((data->number >> 8) & 0xFFu), 0));
}

//
// Firmware version is two double words, of which the first one is the more
// significant half of the number they spell together.
//
bool dmi_tpm_device_decode_firmware_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, ((data->number & 0xFFFFFFFFu) << 32) | ((data->number >> 32) & 0xFFFFFFFFu));
}

//
// Vendor and firmware version mean what the version of the specification says
// they do, so they are read once the fields are there.
//
bool dmi_tpm_device_derive(dmi_entity_t *entity)
{
    dmi_tpm_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(TPM_DEVICE));
    if (info == nullptr)
        return false;

    info->vendor = (info->vendor_id[0] != 0) ? info->vendor_id : nullptr;

    uint32_t firmware_version_1 = (uint32_t)(info->firmware_version >> 32);
    uint32_t firmware_version_2 = (uint32_t)(info->firmware_version & 0xFFFFFFFFu);

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

bool dmi_tpm_device_encode_vendor_id(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    dmi_unused(field);

    const char *vendor_id = value;

    data->number = 0;

    for (size_t i = 0; i < 4; i++)
        data->number |= (uintmax_t)(dmi_byte_t)vendor_id[i] << (i * CHAR_BIT);

    return true;
}

bool dmi_tpm_device_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    dmi_version_t version = (dmi_version_t)dmi_field_get(field, value);

    data->number = dmi_version_major(version) | (dmi_version_minor(version) << 8);

    return true;
}

bool dmi_tpm_device_encode_firmware_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    uintmax_t version = dmi_field_get(field, value);

    data->number = ((version & 0xFFFFFFFFu) << 32) | ((version >> 32) & 0xFFFFFFFFu);

    return true;
}
