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
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/tpm-device-internal.h>

static void dmi_tpm_device_decode_vendor(dmi_entity_t *entity, dmi_tpm_device_t *info);

//
// Vendor identifier is four bytes of text, which the specification does not
// terminate.
//
bool dmi_tpm_device_decode_vendor_id(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    char *vendor_id = value;

    if (not dmi_stream_read_data(dmi_entity_stream(entity), vendor_id, 4))
        return false;

    vendor_id[4] = 0;

    return true;
}

//
// Specification version is one byte of major and one of minor.
//
uintmax_t dmi_tpm_device_convert_version(uintmax_t raw)
{
    return dmi_version((unsigned int)(raw & 0xFFu), (unsigned int)((raw >> 8) & 0xFFu), 0);
}

//
// Firmware version is two double words, of which the first one is the more
// significant half of the number they spell together.
//
uintmax_t dmi_tpm_device_convert_firmware_version(uintmax_t raw)
{
    return ((raw & 0xFFFFFFFFu) << 32) | ((raw >> 32) & 0xFFFFFFFFu);
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

    dmi_tpm_device_decode_vendor(entity, info);

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
