//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/registry.h>

#include <opendmi/command.h>
#include <opendmi/command/common.h>

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
            .description = "Show entries provided by module(s)",
            .argument = {
                .name     = "module",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = false
            }
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
        dmi_command_message("Invalid handle value: %s\n", str);
        return DMI_HANDLE_INVALID;
    }

    if (((errno == ERANGE) and (value == ULONG_MAX)) or (value >= DMI_HANDLE_INVALID)) {
        dmi_command_message("Handle is out of range: %s\n", str);
        return DMI_HANDLE_INVALID;
    }

    return (dmi_handle_t)value;
}

dmi_type_t dmi_parse_type(dmi_context_t *context, const char *str)
{
    char *ep;
    long value;

    if (*str == 0) {
        dmi_command_message("Empty type value: %s\n", str);
        return DMI_TYPE_INVALID;
    }

    if ((*str == '+') or (*str == '-')) {
        dmi_command_message("Invalid type value: %s\n", str);
        return DMI_TYPE_INVALID;
    }

    errno = 0;
    value = strtol(str, &ep, 10);

    if (*ep != 0) {
        dmi_type_t type = dmi_type_find(context, str);
        if (value == DMI_TYPE_INVALID)
            dmi_command_message("Unknown type code: %s\n", str);

        return type;
    }

    if (((errno == ERANGE) and ((value == LONG_MIN) or (value == LONG_MAX))) or
        (value < 0) or (value > DMI_TYPE_MAX))
    {
        dmi_command_message("Type is out of range: %s\n", str);
        return DMI_TYPE_INVALID;
    }

    return (dmi_type_t)value;
}

void dmi_print_all(
        dmi_context_t      *context,
        FILE               *stream,
        const dmi_format_t *format,
        bool                dump)
{
    void *session;
    dmi_registry_iter_t iter;
    const dmi_entity_t *entity;

    assert(context != nullptr);
    assert(stream != nullptr);
    assert(format != nullptr);

    session = format->handlers.initialize(context, stream);
    if (not session)
        return;

    if (format->handlers.dump_start != nullptr)
        format->handlers.dump_start(session);

    format->handlers.entry(session);

    if (format->handlers.table_start != nullptr)
        format->handlers.table_start(session);

    dmi_registry_iter_init(&iter, context->state.registry, &dmi_filter_config.filter);
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        dmi_print_entity(format, entity, session, dump);
    }

    if (format->handlers.table_end != nullptr)
        format->handlers.table_end(session);

    if (format->handlers.dump_end != nullptr)
        format->handlers.dump_end(session);

    format->handlers.finalize(session);
}

void dmi_print_entity(
        const dmi_format_t *format,
        const dmi_entity_t *entity,
        void               *session,
        bool                dump)
{
    assert(format != nullptr);
    assert(entity != nullptr);
    assert(session != nullptr);

    const dmi_entity_spec_t *spec = entity->spec;
    const dmi_attribute_t   *attr = nullptr;

    format->handlers.entity_start(session, entity);

    if (entity->info and not dump) {
        if (format->handlers.entity_attrs_start != nullptr)
            format->handlers.entity_attrs_start(session, entity);

        for (attr = spec->attributes; attr->params.name; attr++) {
            const dmi_data_t *value = dmi_member_ptr(entity->info, attr->value, dmi_data_t);

            // Check attribute level
            if (attr->params.level != DMI_VERSION_NONE) {
                if (entity->level < attr->params.level)
                    continue;
            }

            format->handlers.entity_attr(session, entity, attr, value);
        }

        if (format->handlers.entity_attrs_end != nullptr)
            format->handlers.entity_attrs_end(session, entity);
    } else if (entity->type != DMI_TYPE(END_OF_TABLE)) {
        format->handlers.entity_data(session, entity);
        format->handlers.entity_strings(session, entity);
    }

    format->handlers.entity_end(session, entity);
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
