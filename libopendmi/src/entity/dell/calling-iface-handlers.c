//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/calling-iface-internal.h>

const dmi_attribute_t dmi_dell_calling_iface_token_attrs[] =
{
    DMI_ATTRIBUTE(dmi_dell_calling_iface_token_t, id, INTEGER, {
        .code  = "id",
        .name  = "Token ID",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_dell_calling_iface_token_t, location, INTEGER, {
        .code  = "location",
        .name  = "Location",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_dell_calling_iface_token_t, value, INTEGER, {
        .code  = "value",
        .name  = "Value",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    {}
};

bool dmi_dell_calling_iface_decode(dmi_entity_t *entity)
{
    dmi_dell_calling_iface_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_CALLING_IFACE));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    bool status =
        dmi_stream_decode(stream, dmi_word_t, &info->cmd_io_address) and
        dmi_stream_decode(stream, dmi_byte_t, &info->cmd_io_code) and
        dmi_stream_decode(stream, dmi_dword_t, &info->supported_cmds);
    if (not status)
        return false;

    // Tokens are terminated by the end-of-table marker, which may be
    // truncated itself
    size_t capacity = dmi_stream_remaining(stream) / DMI_DELL_CALLING_IFACE_TOKEN_SIZE;
    if (capacity > 0) {
        info->tokens = dmi_alloc_array(context, sizeof(*info->tokens), capacity);
        if (info->tokens == nullptr)
            return false;
    }

    while (true) {
        dmi_word_t id = 0;

        if (not dmi_stream_decode(stream, dmi_word_t, &id))
            return dmi_entity_incomplete(entity);

        if (id == DMI_DELL_TOKEN_EOT) {
            dmi_stream_skip(stream, dmi_stream_remaining(stream));
            break;
        }

        dmi_dell_calling_iface_token_t token = { .id = id };

        status =
            dmi_stream_decode(stream, dmi_word_t, &token.location) and
            dmi_stream_decode(stream, dmi_word_t, &token.value);
        if (not status)
            return dmi_entity_incomplete(entity);

        if (id != DMI_DELL_TOKEN_UNUSED)
            info->tokens[info->token_count++] = token;
    }

    return true;
}

void dmi_dell_calling_iface_cleanup(dmi_entity_t *entity)
{
    dmi_dell_calling_iface_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_CALLING_IFACE));
    if (info == nullptr)
        return;

    dmi_free(info->tokens);
}
