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

#include <opendmi/entity/oem-strings.h>

static bool dmi_oem_strings_decode(dmi_entity_t *entity);
static void dmi_oem_strings_cleanup(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_oem_strings_spec =
{
    .code            = "oem-strings",
    .name            = "OEM strings",
    .description     = (const char *[]){
        "This structure contains free-form strings defined by the OEM. "
        "Examples are part numbers for system reference documents, contact "
        "information for the manufacturer, and so on.",
        //
        nullptr
    },
    .type            = DMI_TYPE(OEM_STRINGS),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x05,
    .decoded_length  = sizeof(dmi_oem_strings_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_oem_strings_t, string_count, INTEGER, {
            .code = "string-count",
            .name = "String count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_oem_strings_t, strings, string_count, STRING, {
            .code    = "strings",
            .name    = "Strings"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_oem_strings_decode,
        .cleanup = dmi_oem_strings_cleanup
    }
};

static bool dmi_oem_strings_decode(dmi_entity_t *entity)
{
    dmi_oem_strings_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(OEM_STRINGS));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    if (not dmi_stream_decode(stream, dmi_byte_t, &info->string_count))
        return false;

    info->strings = dmi_alloc_array(context, sizeof(const char *), info->string_count);
    if (info->strings == nullptr)
        return false;

    for (size_t i = 0; i < info->string_count; i++) {
        info->strings[i] = dmi_entity_string(entity, (dmi_string_t)(i + 1));
    }

    return true;
}

static void dmi_oem_strings_cleanup(dmi_entity_t *entity)
{
    dmi_oem_strings_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(OEM_STRINGS));
    if (info == nullptr)
        return;

    dmi_free(info->strings);
}
