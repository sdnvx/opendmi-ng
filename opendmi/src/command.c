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
#include <opendmi/utils/locale.h>

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

/**
 * @internal
 * @brief Handles the option enabling logging to a file.
 *
 * @param[in] context DMI context.
 * @param[in] value   Path of the log file.
 *
 * @return Always `true`.
 */
static bool dmi_command_set_log_file(dmi_context_t *context, const char *value);

/**
 * @internal
 * @brief Handles the option setting the logging level.
 *
 * @param[in] context DMI context.
 * @param[in] value   Name of the logging level.
 *
 * @return `true` on success, `false` if the level is unknown.
 */
static bool dmi_command_set_log_level(dmi_context_t *context, const char *value);

/**
 * @internal
 * @brief Handles the option enabling a module.
 *
 * @param[in] context DMI context.
 * @param[in] value   Name of the module.
 *
 * @return `true` on success, `false` if the module is unknown or cannot
 * be enabled.
 */
static bool dmi_command_add_module(dmi_context_t *context, const char *value);

/**
 * @internal
 * @brief Handles the option disabling the modules enabled automatically.
 *
 * @param[in] context DMI context.
 * @param[in] value   Unused.
 *
 * @return Always `true`.
 */
static bool dmi_command_disable_auto_modules(dmi_context_t *context, const char *value);

/**
 * @internal
 * @brief Handles the option enabling the overlay of additional information.
 *
 * @param[in] context DMI context.
 * @param[in] value   Unused.
 *
 * @return Always `true`.
 */
static bool dmi_command_enable_overlay(dmi_context_t *context, const char *value);

#if defined(__linux__)

/**
 * @internal
 * @brief Handles the option disabling SysFS, which is not implemented yet.
 *
 * @param[in] context DMI context.
 * @param[in] value   Unused.
 *
 * @return Always `false`, since Linux backend supports only SysFS for
 * now.
 */
static bool dmi_command_disable_sysfs(dmi_context_t *context, const char *value);

#endif

/**
 * @internal
 * @brief Prints usage of the tool: its usage line, the list of commands and
 * global options.
 */
static void dmi_command_usage_tool(void);

/**
 * @internal
 * @brief Prints usage line of a command, which names the option sets and the
 * arguments of the command.
 *
 * @param[in] command Command to print the usage line of.
 */
static void dmi_command_usage_line(const dmi_command_t *command);

/**
 * @internal
 * @brief Prints name of an option set in the usage line, lowercased.
 *
 * @param[in] set Option set to print the name of.
 */
static void dmi_command_usage_set(const dmi_option_set_t *set);

/**
 * @internal
 * @brief Tells whether a command needs no SMBIOS data, either at all or with
 * the options it has been given.
 *
 * @param[in] command Command to check.
 *
 * @return `true` if the command needs no SMBIOS data, `false` otherwise.
 */
static bool dmi_command_is_detached(const dmi_command_t *command);

/**
 * @internal
 * @brief Prepares to run a command: loads SMBIOS data, unless the command
 * needs none, and starts the pager if the command pages its output.
 *
 * @details SMBIOS data is loaded from the input file if one is given, or is
 * read from the device otherwise.
 *
 * @param[in] command Command to prepare for.
 * @param[in] context Context to load SMBIOS data into.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_command_setup(const dmi_command_t *command, dmi_context_t *context);

/**
 * @internal
 * @brief Flushes standard output once a command has succeeded, since
 * commands may write to it directly.
 *
 * @details Output is not an error if the pager has been quit before reading
 * all of it.
 *
 * @param[in] command Command, which has written the output.
 *
 * @return `EXIT_SUCCESS` if the output has been written, `EXIT_FAILURE`
 *         otherwise.
 */
static int dmi_command_flush_output(const dmi_command_t *command);

/**
 * @internal
 * @brief Translates the format string of a message.
 *
 * @details Messages are translated by their text, which is the key of the
 * translation in the resources of the tool, so that the format arguments stay
 * the same.
 *
 * @param[in] format Format string of the message.
 *
 * @return Translated format string, or `format` itself if there is no
 *         translation.
 */
static const char *dmi_command_message_text(const char *format);

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
        {
            .short_names = "M",
            .long_names  = (const char *[]){ "no-auto-modules", nullptr },
            .description = "Don't enable modules of the platform automatically",
            .handler     = dmi_command_disable_auto_modules
        },
        {
            .short_names = "O",
            .long_names  = (const char *[]){ "overlay", nullptr },
            .description = "Apply additional information entries (type 40) to structures",
            .handler     = dmi_command_enable_overlay
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

    dmi_tty_header("%s:", dmi_tool_string("Commands"));

    for (pcommand = dmi_commands; *pcommand != nullptr; pcommand++) {
        dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "%4s%-8s", "", (*pcommand)->name);
        dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, " %s\n", dmi_tool_string((*pcommand)->description));
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
    dmi_tty_header(dmi_tool_string("OpenDMI Framework, version %s (%s)"),
                   OPENDMI_VERSION, OPENDMI_RELEASE_DATE);

    dmi_tty_cprintf(DMI_TTY_COLOR_GREY, "Copyright (c) 2025-2026, The OpenDMI contributors\n");
    dmi_tty_cprintf(DMI_TTY_COLOR_GREY, "%s\n\n", dmi_tool_string("Licensed under the BSD 3-Clause License"));
}

void dmi_command_usage(const dmi_command_t *command)
{
    dmi_command_banner();

    if (command == nullptr) {
        dmi_command_usage_tool();
        return;
    }

    dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, "%s\n\n", dmi_tool_string(command->description));

    dmi_tty_header("%s:", dmi_tool_string("Usage"));
    dmi_command_usage_line(command);

    dmi_option_list(&dmi_global_options);

    if (command->options == nullptr)
        return;

    for (const dmi_option_set_t **set = command->options; *set != nullptr; set++)
        dmi_option_list(*set);
}

void dmi_command_message(const char *format, ...)
{
    va_list args;

    assert(format != nullptr);

    va_start(args, format);

    fprintf(stderr, "%s: ", dmi_process);
    vfprintf(stderr, dmi_command_message_text(format), args);
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
    vfprintf(stderr, dmi_command_message_text(format), args);
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

int dmi_command_flush(FILE *stream)
{
    assert(stream != nullptr);

    errno = 0;
    if ((fflush(stream) == 0) and not ferror(stream))
        return 0;

    return (errno != 0) ? errno : EIO;
}

bool dmi_command_is_raw(bool show_raw)
{
    return show_raw or not dmi_tty_is_stdout();
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
        int nopts = 0;
        if (command->options != nullptr)
            nopts = dmi_option_parse(context, command->options, argc, argv);

        if (nopts < 0) {
            rv = EXIT_USAGE;
            break;
        }

        argc -= nopts;
        argv += nopts;

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

        if (not dmi_command_setup(command, context)) {
            dmi_command_trace(context);
            break;
        }

        rv = command->handlers.main(context, argc, argv);
        if (rv == EXIT_SUCCESS)
            rv = dmi_command_flush_output(command);
    } while (false);

    // Cleanup
    if (command->handlers.cleanup != nullptr)
        command->handlers.cleanup(context);

    return rv;
}

static bool dmi_command_is_detached(const dmi_command_t *command)
{
    if (command->flags & DMI_COMMAND_FLAG_DETACHED)
        return true;

    return (command->handlers.detached != nullptr) and command->handlers.detached();
}

static bool dmi_command_setup(const dmi_command_t *command, dmi_context_t *context)
{
    if (not dmi_command_is_detached(command)) {
        bool status;

        if (dmi_global_config.input_path != nullptr)
            status = dmi_load(context, dmi_global_config.input_path);
        else
            status = dmi_open(context, dmi_global_config.device_path);

        if (not status)
            return false;
    }

    if (dmi_tty_is_stdout() and (command->flags & DMI_COMMAND_FLAG_PAGER))
        return dmi_pager_start(context);

    return true;
}

static int dmi_command_flush_output(const dmi_command_t *command)
{
    int error = dmi_command_flush(stdout);

    if ((error == 0) or dmi_pager_has_quit(stdout, error))
        return EXIT_SUCCESS;

    dmi_command_message_ex(command, "Unable to write output: %s", strerror(error));

    return EXIT_FAILURE;
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

static bool dmi_command_disable_auto_modules(dmi_context_t *context, const char *value)
{
    assert(context != nullptr);
    dmi_unused(value);

    dmi_set_flags(context, dmi_get_flags(context) & ~DMI_CONTEXT_FLAG_AUTO_MODULES);

    return true;
}

static bool dmi_command_enable_overlay(dmi_context_t *context, const char *value)
{
    assert(context != nullptr);
    dmi_unused(value);

    dmi_set_flags(context, dmi_get_flags(context) | DMI_CONTEXT_FLAG_OVERLAY);

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

static void dmi_command_usage_tool(void)
{
    dmi_tty_header("%s:", dmi_tool_string("Usage"));

    // Text of the line is not its own key, since resource keys may have
    // neither brackets nor escape sequences
    printf(dmi_tool_text("text", "usage-line",
                         "    %s [global options] <command> [command options] [--] [command args]"),
           dmi_process);
    printf("\n\n");

    dmi_command_list();
    dmi_option_list(&dmi_global_options);

    printf(dmi_tool_string("Use %s <command> --help for more information"), dmi_process);
    printf("\n\n");
}

static void dmi_command_usage_line(const dmi_command_t *command)
{
    printf("    %s [%s] %s", dmi_process, dmi_tool_string("global options"), command->name);

    if (command->options != nullptr) {
        for (const dmi_option_set_t **set = command->options; *set != nullptr; set++)
            dmi_command_usage_set(*set);
    }

    if (command->arguments != nullptr) {
        printf(" [--]");
        for (const dmi_argument_t *arg = command->arguments; arg->name != nullptr; arg++) {
            printf(arg->required ? " <%s>" : " [<%s>]", dmi_tool_string(arg->name));
        }
    }

    printf("\n\n");
}

static void dmi_command_usage_set(const dmi_option_set_t *set)
{
    size_t name_len = strlen(set->name) + 1;

    char name[name_len];
    memcpy(name, set->name, name_len);
    dmi_string_tolower(name);

    // Names are translated after they are lowercased, so that the
    // translation has the case it is printed with
    printf(" [%s]", dmi_tool_string(name));
}

static const char *dmi_command_message_text(const char *format)
{
    const char *text = dmi_resource_string(dmi_tool_resource(), "message", format);

    return (text != nullptr) ? text : format;
}
