//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/dell.h>
#include <opendmi/entity/dell/calling-iface.h>

/**
 * @brief Token identifier of unused tokens.
 */
#define DMI_DELL_TOKEN_UNUSED 0x0000u

/**
 * @brief Token identifier of the end-of-table marker.
 */
#define DMI_DELL_TOKEN_EOT 0xFFFFu

/**
 * @brief Size of the token in the structure.
 */
#define DMI_DELL_CALLING_IFACE_TOKEN_SIZE 6

static bool dmi_dell_calling_iface_decode(dmi_entity_t *entity);
static void dmi_dell_calling_iface_cleanup(dmi_entity_t *entity);

static const dmi_attribute_t dmi_dell_calling_iface_token_attrs[] =
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
    DMI_ATTRIBUTE_NULL
};

const dmi_entity_spec_t dmi_dell_calling_iface_spec =
{
    .type            = DMI_TYPE(DELL_CALLING_IFACE),
    .code            = "dell-calling-iface",
    .name            = "Dell calling interface",
    .description     = (const char *[]){
        "Describes the interface for issuing commands to the firmware through "
        "an I/O port, and tokens, which are stored in non-volatile storage "
        "and accessed with these commands.",
        //
        nullptr
    },
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x0B,
    .decoded_length  = sizeof(dmi_dell_calling_iface_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_dell_calling_iface_t, cmd_io_address, INTEGER, {
            .code   = "command-io-address",
            .name   = "Command I/O address",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_calling_iface_t, cmd_io_code, INTEGER, {
            .code   = "command-io-code",
            .name   = "Command I/O code",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_dell_calling_iface_t, supported_cmds, INTEGER, {
            .code   = "supported-commands",
            .name   = "Supported commands",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_dell_calling_iface_t, tokens, token_count, STRUCT, {
            .code   = "tokens",
            .name   = "Tokens",
            .attrs  = dmi_dell_calling_iface_token_attrs
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_dell_calling_iface_decode,
        .cleanup = dmi_dell_calling_iface_cleanup
    }
};

static bool dmi_dell_calling_iface_decode(dmi_entity_t *entity)
{
    dmi_dell_calling_iface_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_CALLING_IFACE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

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
        info->tokens = dmi_alloc_array(entity->context, sizeof(*info->tokens), capacity);
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

static void dmi_dell_calling_iface_cleanup(dmi_entity_t *entity)
{
    dmi_dell_calling_iface_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(DELL_CALLING_IFACE));
    if (info == nullptr)
        return;

    dmi_free(info->tokens);
}
