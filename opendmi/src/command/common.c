//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/module.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>

#include <opendmi/command.h>
#include <opendmi/command/common.h>
#include <opendmi/format/iter.h>

static bool dmi_filter_config_add_handle(dmi_context_t *context, const char *value);
static bool dmi_filter_config_add_type(dmi_context_t *context, const char *value);

static bool dmi_filter_config_enable_std(dmi_context_t *context, const char *value);
static bool dmi_filter_config_disable_std(dmi_context_t *context, const char *value);
static bool dmi_filter_config_enable_oem(dmi_context_t *context, const char *value);
static bool dmi_filter_config_disable_oem(dmi_context_t *context, const char *value);
static bool dmi_filter_config_enable_inactive(dmi_context_t *context, const char *value);
static bool dmi_filter_config_disable_inactive(dmi_context_t *context, const char *value);
static bool dmi_filter_config_enable_unknown(dmi_context_t *context, const char *value);
static bool dmi_filter_config_disable_unknown(dmi_context_t *context, const char *value);
static bool dmi_filter_config_enable_all(dmi_context_t *context, const char *value);
static bool dmi_filter_config_add_module(dmi_context_t *context, const char *value);
static bool dmi_filter_config_add_all_modules(dmi_context_t *context, const char *value);

static bool dmi_filter_config_add_module_types(const dmi_module_t *module);

dmi_filter_config_t dmi_filter_config =
{
    .filter = {
        .mask = DMI_FILTER_MASK_DEFAULT
    }
};

const dmi_option_set_t dmi_filter_options =
{
    .name    = "Filter options",
    .options = (const dmi_option_t[]){
        {
            .short_names = "H",
            .long_names  = (const char *[]){ "handle", nullptr },
            .description = "Only display the entries of given handle(s)",
            .handler     = dmi_filter_config_add_handle,
            .argument    = {
                .name     = "HANDLE",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "t",
            .long_names  = (const char *[]){ "type", nullptr },
            .description = "Only display the entries of given type(s)",
            .handler     = dmi_filter_config_add_type,
            .argument    = {
                .name     = "TYPE",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "s",
            .long_names  = (const char *[]){ "standard", nullptr },
            .description = "Show standard entries",
            .handler     = dmi_filter_config_enable_std
        },
        {
            .short_names = "S",
            .long_names  = (const char *[]){ "no-standard", nullptr },
            .description = "Don't show standard entries",
            .handler     = dmi_filter_config_disable_std
        },
        {
            .short_names = "e",
            .long_names  = (const char *[]){ "oem", nullptr },
            .description = "Show OEM-specific entries",
            .handler     = dmi_filter_config_enable_oem
        },
        {
            .short_names = "E",
            .long_names  = (const char *[]){ "no-oem", nullptr },
            .description = "Don't show OEM-specific entries",
            .handler     = dmi_filter_config_disable_oem
        },
        {
            .short_names = "i",
            .long_names  = (const char *[]){ "inactive", nullptr },
            .description = "Show inactive entries",
            .handler     = dmi_filter_config_enable_inactive
        },
        {
            .short_names = "I",
            .long_names  = (const char *[]){ "no-inactive", nullptr },
            .description = "Don't show inactive entries",
            .handler     = dmi_filter_config_disable_inactive
        },
        {
            .short_names = "u",
            .long_names  = (const char *[]){ "unknown", nullptr },
            .description = "Show unknown entries",
            .handler     = dmi_filter_config_enable_unknown
        },
        {
            .short_names = "U",
            .long_names  = (const char *[]){ "no-unknown", nullptr },
            .description = "Don't show unknown entries",
            .handler     = dmi_filter_config_disable_unknown
        },
        {
            .short_names = "m",
            .long_names  = (const char *[]){ "module", nullptr },
            .description = "Show entries provided by enabled module",
            .handler     = dmi_filter_config_add_module,
            .argument    = {
                .name     = "module",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "M",
            .long_names  = (const char *[]){ "all-modules", nullptr },
            .description = "Show entries provided by all enabled modules",
            .handler     = dmi_filter_config_add_all_modules
        },
        {
            .short_names = "a",
            .long_names  = (const char *[]){ "all", nullptr },
            .description = "Show all entries",
            .handler     = dmi_filter_config_enable_all
        },
        {}
    }
};

dmi_handle_t dmi_parse_handle(const char *str)
{
    char *ep;
    unsigned long value;

    errno = 0;
    value = strtoul(str, &ep, 16);

    if ((*str == 0) or (*ep != 0)) {
        dmi_command_message("Invalid handle value: %s", str);
        return DMI_HANDLE_INVALID;
    }

    if (((errno == ERANGE) and (value == ULONG_MAX)) or (value >= DMI_HANDLE_INVALID)) {
        dmi_command_message("Handle is out of range: %s", str);
        return DMI_HANDLE_INVALID;
    }

    return (dmi_handle_t)value;
}

dmi_type_t dmi_parse_type(dmi_context_t *context, const char *str)
{
    char *ep;
    long value;

    if (*str == 0) {
        dmi_command_message("Empty type value");
        return DMI_TYPE_INVALID;
    }

    if ((*str == '+') or (*str == '-')) {
        dmi_command_message("Invalid type value: %s", str);
        return DMI_TYPE_INVALID;
    }

    errno = 0;
    value = strtol(str, &ep, 10);

    if (*ep != 0) {
        dmi_type_t type = dmi_type_find(context, str);
        if (type == DMI_TYPE_INVALID)
            dmi_command_message("Unknown type code: %s", str);

        return type;
    }

    if (((errno == ERANGE) and ((value == LONG_MIN) or (value == LONG_MAX))) or
        (value < 0) or (value > DMI_TYPE_MAX))
    {
        dmi_command_message("Type is out of range: %s", str);
        return DMI_TYPE_INVALID;
    }

    return (dmi_type_t)value;
}

bool dmi_print_all(
        dmi_context_t              *context,
        FILE                       *stream,
        const dmi_format_t         *format,
        const dmi_format_options_t *options)
{
    void *session;
    dmi_registry_iter_t iter;

    assert(context != nullptr);
    assert(stream != nullptr);
    assert(format != nullptr);

    session = format->handlers.initialize(context, stream, options);
    if (session == nullptr)
        return false;

    bool success = false;
    do {
        if ((format->handlers.dump_start != nullptr) and not format->handlers.dump_start(session))
            break;
        if (not format->handlers.entry(session))
            break;
        if ((format->handlers.table_start != nullptr) and not format->handlers.table_start(session))
            break;

        bool status = true;

        dmi_registry_t *registry = dmi_get_registry(context);
        dmi_registry_iter_init(&iter, registry, &dmi_filter_config.filter);

        const dmi_entity_t *entity;
        while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
            status = dmi_print_entity(format, entity, session, options);
            if (not status)
                break;
        }

        if (not status)
            break;

        if ((format->handlers.table_end != nullptr) and not format->handlers.table_end(session))
            break;
        if ((format->handlers.dump_end != nullptr) and not format->handlers.dump_end(session))
            break;

        success = true;
    } while (false);

    // Finalization flushes buffered output of some formats
    format->handlers.finalize(session);

    if ((fflush(stream) != 0) or ferror(stream)) {
        if (success)
            dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s", strerror(errno));
        success = false;
    }

    return success;
}

bool dmi_print_entity(
        const dmi_format_t         *format,
        const dmi_entity_t         *entity,
        void                       *session,
        const dmi_format_options_t *options)
{
    assert(format != nullptr);
    assert(entity != nullptr);
    assert(session != nullptr);

    const dmi_entity_spec_t *spec = entity->spec;
    const dmi_attribute_t   *attr = nullptr;

    if (not format->handlers.entity_start(session, entity))
        return false;

    bool dump = (options != nullptr) and options->dump;

    if (entity->info and not dump) {
        if ((format->handlers.entity_attrs_start != nullptr) and
            not format->handlers.entity_attrs_start(session, entity))
            return false;

        for (attr = spec->attributes; attr->params.name; attr++) {
            const dmi_data_t *value = dmi_member_ptr(entity->info, attr->value, dmi_data_t);

            // Check attribute level
            if (attr->params.level != DMI_VERSION_NONE) {
                if (entity->level < attr->params.level)
                    continue;
            }

            // Variant attribute has no value if no variant matches
            if (dmi_attribute_resolve(attr, entity->info) == nullptr)
                continue;

            if (not format->handlers.entity_attr(session, entity, attr, value))
                return false;
        }

        if ((format->handlers.entity_attrs_end != nullptr) and
            not format->handlers.entity_attrs_end(session, entity))
            return false;

        // String properties are added by other structures during linking
        if (not dmi_vector_is_empty(&entity->properties) and
            not format->handlers.entity_properties(session, entity))
            return false;

        // Additional information entries are applied only if requested
        if ((entity->overlays != nullptr) and (format->handlers.entity_overlays != nullptr) and
            not format->handlers.entity_overlays(session, entity))
            return false;
    } else if (entity->type != DMI_TYPE(END_OF_TABLE)) {
        if (not format->handlers.entity_data(session, entity))
            return false;
        if (not format->handlers.entity_strings(session, entity))
            return false;
    }

    return format->handlers.entity_end(session, entity);
}

static bool dmi_filter_config_add_handle(dmi_context_t *context, const char *value)
{
    dmi_handle_t handle;

    assert(value != nullptr);
    dmi_unused(context);

    handle = dmi_parse_handle(value);
    if (handle == DMI_HANDLE_INVALID)
        return false;

    if (not dmi_filter_add_handle(&dmi_filter_config.filter, handle))
        return false;

    return true;
}

static bool dmi_filter_config_add_type(dmi_context_t *context, const char *value)
{
    dmi_type_t type;

    assert(value != nullptr);

    type = dmi_parse_type(context, value);
    if (type == DMI_TYPE_INVALID)
        return false;

    if (not dmi_filter_add_type(&dmi_filter_config.filter, type))
        return false;

    return true;
}

static bool dmi_filter_config_enable_std(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_filter_config.filter.mask |= DMI_FILTER_MASK_COMMON;

    return true;
}

static bool dmi_filter_config_disable_std(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_filter_config.filter.mask &= ~DMI_FILTER_MASK_COMMON;

    return true;
}

static bool dmi_filter_config_enable_oem(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_filter_config.filter.mask |= DMI_FILTER_MASK_OEM;

    return true;
}

static bool dmi_filter_config_disable_oem(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_filter_config.filter.mask &= ~DMI_FILTER_MASK_OEM;

    return true;
}

static bool dmi_filter_config_enable_inactive(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_filter_config.filter.mask |= DMI_FILTER_MASK_INACTIVE;

    return true;
}

static bool dmi_filter_config_disable_inactive(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_filter_config.filter.mask &= ~DMI_FILTER_MASK_INACTIVE;

    return true;
}

static bool dmi_filter_config_enable_unknown(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_filter_config.filter.mask |= DMI_FILTER_MASK_UNKNOWN;

    return true;
}

static bool dmi_filter_config_disable_unknown(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_filter_config.filter.mask &= ~DMI_FILTER_MASK_UNKNOWN;

    return true;
}

static bool dmi_filter_config_enable_all(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_filter_config.filter.mask = DMI_FILTER_MASK_ALL;

    return true;
}

static bool dmi_filter_config_add_module(dmi_context_t *context, const char *value)
{
    assert(context != nullptr);
    assert(value != nullptr);

    const dmi_module_t *module = dmi_module_find(value);
    if (module == nullptr) {
        dmi_command_message("Unknown module: %s", value);
        return false;
    }

    // Entities provided by the module are decoded only if it is enabled
    if (not dmi_has_extension(context, module)) {
        dmi_command_message("Module %s is not enabled", value);
        return false;
    }

    return dmi_filter_config_add_module_types(module);
}

static bool dmi_filter_config_add_all_modules(dmi_context_t *context, const char *value)
{
    bool found = false;

    assert(context != nullptr);
    dmi_unused(value);

    for (const dmi_module_t *module = dmi_module_next(nullptr); module != nullptr; module = dmi_module_next(module)) {
        if ((module->entities == nullptr) or (*module->entities == nullptr))
            continue;
        if (not dmi_has_extension(context, module))
            continue;

        if (not dmi_filter_config_add_module_types(module))
            return false;

        found = true;
    }

    // Empty filter matches all entries, so this is an error
    if (not found) {
        dmi_command_message("No modules providing entries are enabled");
        return false;
    }

    return true;
}

static bool dmi_filter_config_add_module_types(const dmi_module_t *module)
{
    // Empty filter matches all entries, so this is an error
    if ((module->entities == nullptr) or (*module->entities == nullptr)) {
        dmi_command_message("Module %s does not provide any entries", module->code);
        return false;
    }

    for (const dmi_entity_spec_t **pspec = module->entities; *pspec != nullptr; pspec++) {
        if (not dmi_filter_add_type(&dmi_filter_config.filter, (*pspec)->type))
            return false;
    }

    return true;
}
