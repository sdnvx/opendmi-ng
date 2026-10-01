//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils/tty.h>
#include <opendmi/utils/locale.h>
#include <opendmi/utils/vector.h>

#include <opendmi/command/types.h>

typedef struct dmi_types_config
{
    bool show_core;
    bool show_all;
    bool show_raw;
    dmi_vector_t show_modules;
} dmi_types_config_t;

/**
 * @internal
 * @brief Print the usage of the command.
 */
static void dmi_types_usage(void);

/**
 * @internal
 * @brief Handle the `--module` option, which lists the types provided by a
 * module.
 *
 * @param[in] context DMI context, unused.
 * @param[in] value   Name of the module.
 *
 * @return `true` on success, `false` if the module is unknown or cannot be
 * added to the list.
 */
static bool dmi_types_add_module(dmi_context_t *context, const char *value);

/**
 * @internal
 * @brief Handle the `--all-modules` option, which lists the types provided by
 * all modules.
 *
 * @param[in] context DMI context, unused.
 * @param[in] value   Value of the option, unused.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_types_add_all_modules(dmi_context_t *context, const char *value);

/**
 * @internal
 * @brief Add a module to the ones whose types are listed, instead of the
 * core types.
 *
 * @param[in] module Module to add.
 *
 * @return `true` on success, `false` if the module cannot be added.
 */
static bool dmi_types_show_module_types(const dmi_module_t *module);

/**
 * @internal
 * @brief Match a module of the list against a module code.
 *
 * @param[in] entry Module of the list, `dmi_module_t`.
 * @param[in] key   Module code to match.
 *
 * @return `true` if the module has the code, `false` otherwise.
 */
static bool dmi_types_match_module(uintptr_t entry, uintptr_t key);

/**
 * @internal
 * @brief List the structure types the options have selected.
 *
 * @param[in] context DMI context.
 * @param[in] argc    Number of the arguments, unused.
 * @param[in] argv    Arguments of the command, unused.
 *
 * @return Exit code of the command.
 */
static int  dmi_types_main(dmi_context_t *context, int argc, char *argv[]);

/**
 * @internal
 * @brief List the core structure types, which the specification defines.
 *
 * @param[in] context DMI context.
 */
static void dmi_types_show_core(dmi_context_t *context);

/**
 * @internal
 * @brief List the structure types provided by a module.
 *
 * @param[in] context DMI context, unused.
 * @param[in] module  Module to list the types of.
 */
static void dmi_types_show_module(dmi_context_t *context, const dmi_module_t *module);

/**
 * @internal
 * @brief Print a line describing a structure type, either raw or formatted
 * for the terminal.
 *
 * @param[in] context DMI context, unused.
 * @param[in] module  Module providing the type, or `nullptr` for a core type.
 * @param[in] spec    Specification of the type.
 */
static void dmi_types_show_type(
        dmi_context_t           *context,
        const dmi_module_t      *module,
        const dmi_entity_spec_t *spec);

/**
 * @internal
 * @brief Free the list of the modules whose types are listed.
 *
 * @param[in] context DMI context, unused.
 */
static void dmi_types_cleanup(dmi_context_t *context);

static dmi_types_config_t dmi_types_config =
{
    .show_core = true,
    .show_all  = false,
    .show_raw  = false,
    .show_modules = {
        .matcher = dmi_types_match_module
    }
};

static const dmi_option_set_t dmi_types_options =
{
    .name    = "Command options",
    .options = (const dmi_option_t[]){
        {
            .short_names = "?h",
            .long_names  = (const char *[]){ "help", nullptr },
            .description = "Print this help and exit",
            .value       = &dmi_command_config.show_usage
        },
        {
            .short_names = "m",
            .long_names  = (const char *[]){ "module", nullptr },
            .description = "List types provided by module",
            .handler     = dmi_types_add_module,
            .argument    = {
                .name     = "module",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "M",
            .long_names  = (const char *[]){ "all-modules", nullptr },
            .description = "List types provided by all modules",
            .handler     = dmi_types_add_all_modules
        },
        {
            .short_names = "a",
            .long_names  = (const char *[]){ "all", nullptr },
            .description = "List all available types",
            .value       = &dmi_types_config.show_all
        },
        {
            .short_names = "r",
            .long_names  = (const char *[]){ "raw", nullptr },
            .description = "Raw output (default if stdout is a pipe)",
            .value       = &dmi_types_config.show_raw
        },
        {}
    }
};

const dmi_command_t dmi_types_command =
{
    .name        = "types",
    .description = "List SMBIOS structure types",
    .options     = dmi_options(&dmi_types_options),
    .flags       = DMI_COMMAND_FLAG_DETACHED | DMI_COMMAND_FLAG_PAGER,
    .handlers    = {
        .usage   = dmi_types_usage,
        .main    = dmi_types_main,
        .cleanup = dmi_types_cleanup
    }
};

static void dmi_types_usage(void)
{
    dmi_command_usage(&dmi_types_command);
}

static bool dmi_types_add_module(dmi_context_t *context, const char *value)
{
    dmi_unused(context);

    assert(value != nullptr);

    const dmi_module_t *module = dmi_module_find(value);
    if (module == nullptr) {
        dmi_command_message_ex(&dmi_types_command, "Unknown module name: %s", value);
        return false;
    }

    return dmi_types_show_module_types(module);
}

static bool dmi_types_add_all_modules(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    for (const dmi_module_t *module = dmi_module_next(nullptr); module != nullptr; module = dmi_module_next(module)) {
        if (not dmi_types_show_module_types(module))
            return false;
    }

    return true;
}

static bool dmi_types_show_module_types(const dmi_module_t *module)
{
    dmi_types_config.show_core = false;

    if (dmi_vector_exists(&dmi_types_config.show_modules, (uintptr_t)module->code))
        return true;

    if (not dmi_vector_push(&dmi_types_config.show_modules, (uintptr_t)module)) {
        dmi_command_message_ex(&dmi_types_command, "Internal error: %s", strerror(errno));
        return false;
    }

    return true;
}

static bool dmi_types_match_module(uintptr_t entry, uintptr_t key)
{
    dmi_module_t *module = dmi_cast(module, entry);
    const char   *code   = dmi_cast(code, key);

    return strcmp(module->code, code) == 0;
}

static int dmi_types_main(dmi_context_t *context, int argc, char *argv[])
{
    const dmi_module_t *module = nullptr;

    assert(context != nullptr);
    dmi_unused(argc);
    dmi_unused(argv);

    dmi_types_config.show_raw = dmi_command_is_raw(dmi_types_config.show_raw);

    if (not dmi_types_config.show_raw)
        dmi_command_banner();

    if (dmi_types_config.show_core)
        dmi_types_show_core(context);

    if (dmi_types_config.show_all) {
        for (module = dmi_module_next(nullptr); module != nullptr; module = dmi_module_next(module)) {
            dmi_types_show_module(context, module);
        }
    } else if (not dmi_vector_is_empty(&dmi_types_config.show_modules)) {
        size_t nmodules = dmi_vector_length(&dmi_types_config.show_modules);

        for (size_t i = 0; i < nmodules; i++) {
            if (not dmi_vector_get(&dmi_types_config.show_modules, i, (uintptr_t *)&module))
                break;

            dmi_types_show_module(context, module);
        }
    }

    return EXIT_SUCCESS;
}

static void dmi_types_show_core(dmi_context_t *context)
{
    dmi_unused(context);

    if (not dmi_types_config.show_raw)
        dmi_tty_header("%s:", dmi_tool_string("Core types"));

    for (int type = 0; type < __DMI_TYPE_ID_OEM_START; type++) {
        const dmi_entity_spec_t *spec = dmi_type_spec(context, (dmi_type_id_t)type);

        if (spec == nullptr)
            continue;

        dmi_types_show_type(context, nullptr, spec);
    }

    if (not dmi_types_config.show_raw)
        printf("\n");
}

static void dmi_types_show_module(dmi_context_t *context, const dmi_module_t *module)
{
    const dmi_entity_spec_t **pspec = nullptr;

    dmi_unused(context);
    assert(module != nullptr);

    if ((module->entities == nullptr) || (*module->entities == nullptr))
        return;

    if (not dmi_types_config.show_raw)
        dmi_tty_header("%s:", dmi_tool_string(module->name));

    for (pspec = module->entities; *pspec != nullptr; pspec++) {
        dmi_types_show_type(context, module, *pspec);
    }

    if (not dmi_types_config.show_raw)
        printf("\n");
}

static void dmi_types_show_type(
        dmi_context_t           *context,
        const dmi_module_t      *module,
        const dmi_entity_spec_t *spec)
{
    dmi_unused(context);
    assert(spec != nullptr);

    if (dmi_types_config.show_raw) {
        const char *module_name;

        if (module != nullptr)
            module_name = module->code;
        else
            module_name = "core";

        printf("%d\t%s\t%s\t%s\n", spec->type->id, module_name, spec->code, spec->name);
    } else {
        dmi_tty_cprintf(DMI_TTY_COLOR_NAVY, "%4s%-3d", "", spec->type->id);
        dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "  %-30s", spec->code);
        dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, "  %s\n", dmi_spec_name(spec));
    }
}

static void dmi_types_cleanup(dmi_context_t *context)
{
    dmi_unused(context);
    dmi_vector_clear(&dmi_types_config.show_modules);
}
