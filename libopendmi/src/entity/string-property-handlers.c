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

#include <opendmi/entity/string-property-internal.h>

bool dmi_string_property_link(dmi_entity_t *entity)
{
    dmi_string_property_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(STRING_PROPERTY));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    if ((info->parent_handle == DMI_HANDLE_INVALID) or (info->parent_handle == DMI_HANDLE_UNSUPPORTED)) {
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND,
                           "String property 0x%04x: parent is not specified", dmi_entity_handle(entity));
        return false;
    }

    dmi_entity_t *parent = dmi_registry_lookup(registry, info->parent_handle, DMI_TYPE_ANY, true);

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
