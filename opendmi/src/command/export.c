//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/format.h>
#include <opendmi/internal.h>
#include <opendmi/utils/file.h>
#include <opendmi/utils/tty.h>
#include <opendmi/utils/locale.h>

#include <opendmi/command/common.h>
#include <opendmi/command/export.h>

typedef struct dmi_export_config
{
    char *output_path;
    const dmi_format_t *output_format;
    dmi_format_options_t options;
    bool force;
} dmi_export_config_t;

static void dmi_export_usage(void);
static bool dmi_export_set_format(dmi_context_t *context, const char *value);
static int dmi_export_main(dmi_context_t *context, int argc, char *argv[]);

static dmi_export_config_t dmi_export_config =
{
    .output_path   = nullptr,
    .output_format = nullptr,
    .options       = {},
    .force         = false
};

static const dmi_option_set_t dmi_export_options =
{
    .name    = "Command options",
    .options = (const dmi_option_t[]){
        {
            .short_names = "?h",
            .long_names  = (const char *[]){ "help", nullptr },
            .description = "Print this help and exit",
            .value       = &dmi_command_config.show_usage,
        },
        {
            .short_names = "o",
            .long_names  = (const char *[]){ "output", nullptr },
            .description = "Set output file path (default: stdout)",
            .value       = &dmi_export_config.output_path,
            .argument    = {
                .name     = "path",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "f",
            .long_names  = (const char *[]){ "format", nullptr },
            .description = "Set output format (default: yaml)",
            .handler     = dmi_export_set_format,
            .argument    = {
                .name     = "format",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "D",
            .long_names  = (const char *[]){ "dump", nullptr },
            .description = "Do not decode the entries",
            .value       = &dmi_export_config.options.dump,
        },
        {
            .short_names = "p",
            .long_names  = (const char *[]){ "pretty", nullptr },
            .description = "Enable pretty output",
            .value       = &dmi_export_config.options.pretty
        },
        {
            .short_names = "F",
            .long_names  = (const char *[]){ "force", nullptr },
            .description = "Overwrite existing files",
            .value       = &dmi_export_config.force
        },
        {}
    }
};

const dmi_command_t dmi_export_command =
{
    .name        = "export",
    .description = "Export SMBIOS data to external format",
    .options     = dmi_options(&dmi_export_options, &dmi_filter_options),
    .handlers    = {
        .usage = dmi_export_usage,
        .main  = dmi_export_main
    }
};

static void dmi_export_usage(void)
{
    dmi_command_usage(&dmi_export_command);

    dmi_tty_header("%s:", dmi_tool_string("Supported output formats"));

    for (const dmi_format_t **pformat = dmi_formats; *pformat != nullptr; pformat++) {
        dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "%4s%-5s", "", (*pformat)->code);
        dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, " %s\n", dmi_tool_string((*pformat)->name));
    }
    printf("\n");
}

static bool dmi_export_set_format(dmi_context_t *context, const char *value)
{
    const dmi_format_t *format;

    dmi_unused(context);
    assert(value != nullptr);

    format = dmi_format_get(value);
    if (format == nullptr) {
        dmi_command_message("Invalid output format: %s", value);
        return false;
    }

    dmi_export_config.output_format = format;

    return true;
}

static int dmi_export_main(dmi_context_t *context, int argc, char *argv[])
{
    const char *mode;
    FILE *out;

    assert(context != nullptr);
    dmi_unused(argc);
    dmi_unused(argv);

    if (dmi_export_config.output_format == nullptr)
        dmi_export_config.output_format = dmi_format_default;

    if (dmi_export_config.output_path != nullptr) {
        if (dmi_export_config.force)
            mode = "w";
        else
            mode = "wx";

        out = fopen(dmi_export_config.output_path, mode);
        if (out == nullptr) {
            dmi_command_message_ex(
                    &dmi_export_command, "Unable to open output file: %s: %s",
                    dmi_export_config.output_path, strerror(errno));
            return EXIT_FAILURE;
        }
    } else {
        out = stdout;
    }

    bool success = dmi_print_all(context, out, dmi_export_config.output_format,
                                 &dmi_export_config.options);
    if (not success)
        dmi_command_trace(context);

    if (dmi_export_config.output_path != nullptr) {
        // Only regular files are removed on errors, not devices or pipes
        dmi_file_stat_t st;
        bool is_regular = (dmi_file_stat(fileno(out), &st) == 0) and S_ISREG(st.st_mode);

        if ((fclose(out) != 0) and success) {
            dmi_command_message_ex(
                    &dmi_export_command, "Unable to write output file: %s: %s",
                    dmi_export_config.output_path, strerror(errno));
            success = false;
        }

        // Do not leave incomplete output file behind
        if ((not success) and is_regular)
            remove(dmi_export_config.output_path);
    }

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
