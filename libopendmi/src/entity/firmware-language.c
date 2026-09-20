//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-language.h>

static bool dmi_firmware_language_decode(dmi_entity_t *entity);
static void dmi_firmware_language_cleanup(dmi_entity_t *entity);

static const dmi_name_set_t dmi_firmware_language_flag_names =
{
    .code = "firmware-language-flags",
    .names = (const dmi_name_t[]){
        {
            .id   = 0,
            .code = "is-abbreviated",
            .name = "Abbreviated"
        },
        {}
    }
};

const dmi_entity_spec_t dmi_firmware_language_spec =
{
    .code            = "firmware-language",
    .name            = "Firmware language information",
    .description     = (const char *[]){
        "The information in this structure defines the installable language "
        "attributes of the platform firmware.",
        //
        nullptr
    },
    .type            = DMI_TYPE(FIRMWARE_LANGUAGE),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x16,
    .decoded_length  = sizeof(dmi_firmware_language_t),
    .attributes      = (const dmi_attribute_t[]) {
        DMI_ATTRIBUTE_ARRAY(dmi_firmware_language_t, languages, language_count, STRING, {
            .code   = "languages",
            .name   = "Languages"
        }),
        DMI_ATTRIBUTE(dmi_firmware_language_t, flags, SET, {
            .code   = "flags",
            .name   = "Flags",
            .values = &dmi_firmware_language_flag_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_language_t, current_language, STRING, {
            .code   = "current-language",
            .name   = "Current language"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_firmware_language_decode,
        .cleanup = dmi_firmware_language_cleanup
    }
};

static bool dmi_firmware_language_decode(dmi_entity_t *entity)
{
    dmi_firmware_language_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_LANGUAGE));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &info->language_count) and
        dmi_stream_decode(stream, dmi_byte_t, &info->flags.__value) and
        dmi_stream_skip(stream, 15) and
        dmi_stream_decode_str(stream, &info->current_language);
    if (not status)
        return false;

    if (info->language_count > 0) {
        info->languages = dmi_alloc_array(context, sizeof(const char *), info->language_count);
        if (info->languages == nullptr)
            return false;

        for (size_t i = 0; i < info->language_count; i++) {
            info->languages[i] = dmi_entity_string(entity, (dmi_string_t)(i + 1));
        }
    }

    return true;
}

static void dmi_firmware_language_cleanup(dmi_entity_t *entity)
{
    dmi_firmware_language_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_LANGUAGE));
    if (info == nullptr)
        return;

    dmi_free(info->languages);
}
