//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <opendmi/module.h>
#include <opendmi/context.h>
#include <opendmi/utils.h>
#include <opendmi/utils/string.h>
#include <opendmi/utils/tty.h>

#include <opendmi/command/modules.h>

typedef struct dmi_modules_config
{
    bool show_raw;
} dmi_modules_config_t;

static void dmi_modules_usage(void);
static int dmi_modules_main(dmi_context_t *context, int argc, char *argv[]);
static int dmi_modules_comparator(const void *lhs, const void *rhs);

static dmi_modules_config_t dmi_modules_config =
{
    .show_raw = false
};

static const dmi_option_set_t dmi_modules_options =
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
            .short_names = "r",
            .long_names  = (const char *[]){ "raw", nullptr },
            .description = "Raw output (default if stdout is a pipe)",
            .value       = &dmi_modules_config.show_raw
        },
        {}
    }
};

const dmi_command_t dmi_modules_command =
{
    .name        = "modules",
    .description = "List available modules",
    .options     = dmi_options(&dmi_modules_options),
    .flags       = DMI_COMMAND_FLAG_DETACHED,
    .handlers    = {
        .usage = dmi_modules_usage,
        .main  = dmi_modules_main
    }
};

static void dmi_modules_usage(void)
{
    dmi_command_usage(&dmi_modules_command);
}

static int dmi_modules_main(dmi_context_t *context, int argc, char *argv[])
{
    dmi_unused(argc);
    dmi_unused(argv);

    if (dmi_tty_is_stdout()) {
        dmi_command_banner();
        dmi_tty_header("Available modules:");
    } else {
        dmi_modules_config.show_raw = true;
    }

    int rv = EXIT_FAILURE;
    size_t width = 0;
    size_t count = 0;
    size_t index = 0;
    char *format = nullptr;
    const dmi_module_t **modules = nullptr;

    for (const dmi_module_t *module = dmi_modules; module != nullptr; module = module->next) {
        size_t name_width = strlen(module->code);
        if (width < name_width)
            width = name_width;
        count++;
    }

    do {
        if (dmi_asprintf(&format, "%%4s%%-%us", width + 2) < 0) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            break;
        }

        modules = dmi_alloc_array(context, sizeof(dmi_module_t *), count);
        if (modules == nullptr)
            break;

        for (const dmi_module_t *module = dmi_modules; module != nullptr; module = module->next, index++) {
            modules[index] = module;
        }

        qsort(modules, count, sizeof(dmi_module_t *), dmi_modules_comparator);

        for (index = 0; index < count; index++) {
            const dmi_module_t *module = modules[index];

            if (dmi_modules_config.show_raw) {
                printf("%s\t%s\n", module->code, module->name);
            } else {
                dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, format, "", module->code);
                dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, "%s\n", module->name);
            }
        }

        if (not dmi_modules_config.show_raw)
            printf("\n");

        rv = EXIT_SUCCESS;
    } while (false);

    if (rv != EXIT_SUCCESS)
        dmi_command_trace(context);

    dmi_free(modules);
    dmi_free(format);

    return rv;
}

static int dmi_modules_comparator(const void *lhs, const void *rhs)
{
    const dmi_module_t *lhs_module = dmi_deref(dmi_module_t *, lhs);
    const dmi_module_t *rhs_module = dmi_deref(dmi_module_t *, rhs);

    return strcmp(lhs_module->code, rhs_module->code);
}
