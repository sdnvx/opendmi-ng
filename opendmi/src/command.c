//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include "config.h"

#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/module.h>
#include <opendmi/pager.h>
#include <opendmi/internal.h>
#include <opendmi/utils/string.h>
#include <opendmi/utils/tty.h>

#include <opendmi/command.h>
#include <opendmi/command/dump.h>
#include <opendmi/command/entry.h>
#include <opendmi/command/list.h>
#include <opendmi/command/modules.h>
#include <opendmi/command/show.h>
#include <opendmi/command/explain.h>
#include <opendmi/command/lint.h>
#include <opendmi/command/export.h>
#include <opendmi/command/import.h>
#include <opendmi/command/types.h>

static bool dmi_command_set_log_file(dmi_context_t *context, const char *value);
static bool dmi_command_set_log_level(dmi_context_t *context, const char *value);
static bool dmi_command_add_module(dmi_context_t *context, const char *value);

#if defined(__linux__)
static bool dmi_command_disable_sysfs(dmi_context_t *context, const char *value);
#endif

dmi_global_config_t dmi_global_config =
{
    .show_version = false,
    .show_usage   = false,
    .log_enable   = false,
    .log_path     = nullptr,
    .log_level    = DMI_LOG_NOTICE,
    .device_path  = nullptr,
    .input_path   = nullptr,
};

dmi_command_config_t dmi_command_config =
{
    .show_usage = false
};

const dmi_option_set_t dmi_global_options =
{
    .name    = "Global options",
    .options = (const dmi_option_t[]){
        {
            .short_names = "v",
            .long_names  = (const char *[]){ "version", nullptr },
            .description = "Print version information and exit",
            .value       = &dmi_global_config.show_version
        },
        {
            .short_names = "?h",
            .long_names  = (const char *[]){ "help", nullptr },
            .description = "Print this help and exit",
            .value       = &dmi_global_config.show_usage
        },
        {
            .short_names = "l",
            .long_names  = (const char *[]){ "log", nullptr },
            .description = "Enable logging",
            .value       = &dmi_global_config.log_enable
        },
        {
            .long_names  = (const char *[]){ "log-file", nullptr },
            .description = "Enable logging to the specified file",
            .handler     = dmi_command_set_log_file,
            .argument    = {
                .name     = "path",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "L",
            .long_names  = (const char *[]){ "log-level", nullptr },
            .description = "Set logging level",
            .handler     = dmi_command_set_log_level,
            .argument    = {
                .name     = "level",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
#       if defined(__linux__)
            {
                .short_names = "S",
                .long_names  = (const char *[]){ "no-sysfs", nullptr },
                .description = "Do not attempt to read DMI data from SysFS",
                .handler     = dmi_command_disable_sysfs
            },
#       endif
#       if !defined(_WIN32)
            {
                .short_names = "d",
                .long_names  = (const char *[]){ "device", nullptr },
                .description = "Set path to memory device (default: /dev/mem)",
                .value       = &dmi_global_config.device_path,
                .argument    = {
                    .name     = "path",
                    .type     = DMI_ARGUMENT_TYPE_STRING,
                    .required = true
                },
            },
#       endif
        {
            .short_names = "i",
            .long_names  = (const char *[]){ "file", nullptr },
            .description = "Read the DMI data from a binary file",
            .value       = &dmi_global_config.input_path,
            .argument    = {
                .name     = "path",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "m",
            .long_names  = (const char *[]){ "module", nullptr },
            .description = "Enable specified module",
            .handler     = dmi_command_add_module,
            .argument    = {
                .name     = "module",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {}
    }
};

const dmi_command_t *dmi_commands[] =
{
    &dmi_dump_command,
    &dmi_entry_command,
    &dmi_list_command,
    &dmi_show_command,
    &dmi_explain_command,
    &dmi_lint_command,
    &dmi_export_command,
    &dmi_import_command,
    &dmi_types_command,
    &dmi_modules_command,
    nullptr
};

const char *dmi_process = nullptr;

void dmi_command_init(const char *process)
{
    dmi_process = process;

    dmi_tty_init();
}

void dmi_command_list(void)
{
    const dmi_command_t **pcommand;

    dmi_tty_header("Commands:");

    for (pcommand = dmi_commands; *pcommand != nullptr; pcommand++) {
        dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "%4s%-8s", "", (*pcommand)->name);
        dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, " %s\n", (*pcommand)->description);
    }
    printf("\n");
}

const dmi_command_t *dmi_command_find(const char *name)
{
    const dmi_command_t **pcommand;

    assert(name != nullptr);

    for (pcommand = dmi_commands; *pcommand != nullptr; pcommand++) {
        if (strcmp((*pcommand)->name, name) == 0)
            return *pcommand;
    }

    return nullptr;
}

void dmi_command_banner(void)
{
    dmi_tty_header("OpenDMI Framework, version %s (%s)",
                   OPENDMI_VERSION, OPENDMI_RELEASE_DATE);

    dmi_tty_cprintf(DMI_TTY_COLOR_GREY, "Copyright (c) 2025-2026, The OpenDMI contributors\n");
    dmi_tty_cprintf(DMI_TTY_COLOR_GREY, "Licensed under the BSD 3-Clause License\n\n");
}

void dmi_command_usage(const dmi_command_t *command)
{
    dmi_command_banner();

    if (command != nullptr) {
        dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, "%s\n\n", command->description);
    }

    dmi_tty_header("Usage:");

    if (command == nullptr) {
        printf("    %s [global options] <command> [command options] [--] [command args]\n\n", dmi_process);
        dmi_command_list();
    } else {
        printf("    %s [global options] %s", dmi_process, command->name);

        if (command->options != nullptr) {
            for (const dmi_option_set_t **set = command->options; *set != nullptr; set++) {
                size_t name_len = strlen((*set)->name) + 1;

                char name[name_len];
                memcpy(name, (*set)->name, name_len);
                dmi_string_tolower(name);

                printf(" [%s]", name);
            }
        }

        if (command->arguments != nullptr) {
            printf(" [--]");
            for (const dmi_argument_t *arg = command->arguments; arg->name != nullptr; arg++) {
                printf(arg->required ? " <%s>" : " [<%s>]", arg->name);
            }
        }

        printf("\n\n");
    }

    dmi_option_list(&dmi_global_options);

    if (command != nullptr) {
        for (const dmi_option_set_t **set = command->options; *set != nullptr; set++)
            dmi_option_list(*set);
    } else {
        printf("Use %s <command> --help for more information\n\n", dmi_process);
    }
}

void dmi_command_message(const char *format, ...)
{
    va_list args;

    assert(format != nullptr);

    va_start(args, format);

    fprintf(stderr, "%s: ", dmi_process);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);
}

void dmi_command_message_ex(const dmi_command_t *command, const char *format, ...)
{
    va_list args;

    assert(command != nullptr);
    assert(format != nullptr);

    va_start(args, format);

    fprintf(stderr, "%s: %s: ", dmi_process, command->name);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);
}

void dmi_command_trace(dmi_context_t *context)
{
    dmi_error_t *error;

    assert(context != nullptr);

    while ((error = dmi_error_get_first(context)) != nullptr) {
        const char *reason = dmi_error_message(error->reason);

        if (error->message != nullptr)
            dmi_command_message("%s: %s", reason, error->message);
        else
            dmi_command_message("%s", reason);
    }
}

int dmi_command_run(
        const dmi_command_t *command,
        dmi_context_t       *context,
        int                  argc,
        char                *argv[])
{
    int rv = EXIT_FAILURE;

    assert(command != nullptr);
    assert(context != nullptr);
    assert(argv != nullptr);

    do {
        // Parse command-specific options
        if (command->options != nullptr) {
            int nopts;

            nopts = dmi_option_parse(context, command->options, argc, argv);
            if (nopts < 0) {
                rv = EXIT_USAGE;
                break;
            }

            argc -= nopts;
            argv += nopts;
        }

        // Print command usage if requested
        if (dmi_command_config.show_usage) {
            rv = EXIT_SUCCESS;
            command->handlers.usage();
            break;
        }

        // Reject unexpected arguments, since they are likely to be mistakes,
        // e.g. value of an option with optional argument written separately
        if ((command->arguments == nullptr) and (argc > 0)) {
            rv = EXIT_USAGE;
            dmi_command_message_ex(command, "Unexpected argument: %s", argv[0]);
            break;
        }

        // Load SMBIOS data
        if ((command->flags & DMI_COMMAND_FLAG_DETACHED) == 0) {
            bool status;

            if (dmi_global_config.input_path != nullptr)
                status = dmi_dump_load(context, dmi_global_config.input_path);
            else
                status = dmi_open(context, dmi_global_config.device_path);

            if (not status) {
                dmi_command_trace(context);
                break;
            }
        }

        // Start pager
        if (dmi_tty_is_stdout() and (command->flags & DMI_COMMAND_FLAG_PAGER)) {
            if (not dmi_pager_start(context)) {
                dmi_command_trace(context);
                break;
            }
        }

        rv = command->handlers.main(context, argc, argv);

        // Commands may write to standard output directly, so check it here
        if ((rv == EXIT_SUCCESS) and ((fflush(stdout) != 0) or ferror(stdout))) {
            dmi_command_message_ex(command, "Unable to write output: %s", strerror(errno));
            rv = EXIT_FAILURE;
        }
    } while (false);

    // Cleanup
    if (command->handlers.cleanup != nullptr)
        command->handlers.cleanup(context);

    return rv;
}

static bool dmi_command_set_log_file(dmi_context_t *context, const char *value)
{
    dmi_unused(context);

    assert(value != nullptr);

    dmi_global_config.log_enable = true;
    dmi_global_config.log_path   = value;

    return true;
}

static bool dmi_command_set_log_level(dmi_context_t *context, const char *value)
{
    dmi_log_level_t level;

    dmi_unused(context);

    level = dmi_log_level_find(value);
    if (level == DMI_LOG_INVALID) {
        dmi_command_message("Invalid logging level: %s", value);
        return false;
    }

    dmi_global_config.log_level = level;
    return true;
}

static bool dmi_command_add_module(dmi_context_t *context, const char *value)
{
    assert(context != nullptr);
    assert(value != nullptr);

    const dmi_module_t *module = dmi_module_find(value);
    if (module == nullptr) {
        dmi_command_message("Unknown module: %s", value);
        return false;
    }

    if (not dmi_add_extension(context, module)) {
        dmi_command_message("Unable to enable module: %s", value);
        return false;
    }

    return true;
}

#if defined(__linux__)
static bool dmi_command_disable_sysfs(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    // Linux backend supports only SysFS for now
    dmi_command_message("Option --no-sysfs is not implemented yet");

    return false;
}
#endif
