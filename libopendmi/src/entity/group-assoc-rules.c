//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/module.h>
#include <opendmi/utils.h>

#include <opendmi/entity/group-assoc-internal.h>

/**
 * @internal
 * @brief Find the specification the members of a group are decoded by, among
 * the groups of the enabled modules.
 */
static const dmi_entity_spec_t *dmi_group_assoc_member_spec(dmi_context_t *context, const char *name);

//
// Members of a group of a well-known name are decoded by the specification the
// group lists, and the ones which are not are placed by the vendor at a type
// number the module does not know of, or listed by mistake.
//
void dmi_group_assoc_lint_member(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_group_assoc_t *info = dmi_entity_info(entity, DMI_TYPE(group_assoc));
    if ((info == nullptr) or (info->group_name == nullptr))
        return;

    dmi_context_t *context = dmi_entity_context(entity);

    const dmi_entity_spec_t *spec = dmi_group_assoc_member_spec(context, info->group_name);
    if (spec == nullptr)
        return;

    dmi_registry_t *registry = dmi_get_registry(context);

    for (size_t i = 0; i < info->item_count; i++) {
        const dmi_group_assoc_item_t *item = &info->items[i];

        // Members of another type than the item declares, e.g. the ones of a
        // placeholder handle, are the business of the links
        const dmi_entity_t *member = dmi_registry_lookup(registry, item->handle, DMI_TYPE_ANY, true);
        if ((member == nullptr) or (dmi_entity_type_id(member) != item->type))
            continue;

        // Structures of the same type are members whatever the layout they
        // are decoded by
        if (dmi_entity_type(member) == spec->type)
            continue;

        dmi_lint_issue(lint, entity, "items", dmi_lint_entity_offset(lint, entity),
                       "member 0x%04x of group \"%s\" is %s of type %d, while the group lists %s",
                       item->handle, info->group_name,
                       (member->spec != nullptr) ? member->spec->code : "a structure",
                       (int)item->type, spec->code);
    }
}

static const dmi_entity_spec_t *dmi_group_assoc_member_spec(dmi_context_t *context, const char *name)
{
    for (const dmi_module_t *module = dmi_module_next(nullptr); module != nullptr; module = dmi_module_next(module)) {
        if ((module->groups == nullptr) or not dmi_has_extension(context, module))
            continue;

        for (const dmi_module_group_t *group = module->groups; group->name != nullptr; group++) {
            if (strcmp(group->name, name) == 0)
                return group->spec;
        }
    }

    return nullptr;
}
