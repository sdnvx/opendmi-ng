//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/string-property.h>

static bool dmi_string_property_decode(dmi_entity_t *entity);
static bool dmi_string_property_link(dmi_entity_t *entity);


const dmi_entity_spec_t dmi_string_property_spec =
{
    .code            = "string-property",
    .name            = "String property",
    .description     = (const char *[]){
        "This structure defines a string property for another structure. This "
        "allows adding string properties that are common to several "
        "structures without having to modify the definitions of these "
        "structures. Multiple type 46 structures can add string properties "
        "to the same parent structure.",
        //
        nullptr
    },
    .type            = DMI_TYPE(STRING_PROPERTY),
    .minimum_version = DMI_VERSION(3, 5, 0),
    .minimum_length  = 0x09,
    .decoded_length  = sizeof(dmi_string_property_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_string_property_t, ident, ENUM, {
            .code   = "ident",
            .name   = "Identifier",
            .values = &dmi_property_names
        }),
        DMI_ATTRIBUTE(dmi_string_property_t, value, STRING, {
            .code = "value",
            .name = "Value"
        }),
        DMI_ATTRIBUTE(dmi_string_property_t, parent_handle, HANDLE, {
            .code = "parent-handle",
            .name = "Parent handle"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers    = {
        .decode = dmi_string_property_decode,
        .link   = dmi_string_property_link
    }
};

static bool dmi_string_property_decode(dmi_entity_t *entity)
{
    dmi_string_property_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(STRING_PROPERTY));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    return
        dmi_stream_decode(stream, dmi_word_t, &info->ident) and
        dmi_stream_decode_str(stream, &info->value) and
        dmi_stream_decode(stream, dmi_word_t, &info->parent_handle);
}

static bool dmi_string_property_link(dmi_entity_t *entity)
{
    dmi_string_property_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(STRING_PROPERTY));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_registry(context);

    if ((info->parent_handle == DMI_HANDLE_INVALID) or (info->parent_handle == DMI_HANDLE_UNSUPPORTED)) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND,
                           "String property 0x%04x: parent is not specified", dmi_entity_handle(entity));
        return false;
    }

    dmi_entity_t *parent = dmi_registry_get(registry, info->parent_handle, DMI_TYPE_ANY, true);

    if (parent == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND,
                           "String property 0x%04x: parent 0x%04x not found",
                           dmi_entity_handle(entity), info->parent_handle);
        return false;
    }

    if (parent->type == DMI_TYPE(STRING_PROPERTY)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ENTITY_TYPE,
                           "String property 0x%04x: parent 0x%04x is a string property",
                           dmi_entity_handle(entity), info->parent_handle);
        return false;
    }

    if (not dmi_entity_add_property(parent, info))
        return false;

    info->parent = parent;

    return true;
}
