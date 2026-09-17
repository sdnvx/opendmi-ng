//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/format/text.h>

#include <opendmi/command/common.h>
#include <opendmi/command/show.h>

typedef struct dmi_show_config
{
    dmi_format_options_t options;
} dmi_show_config_t;

static void dmi_show_usage(void);
static bool dmi_show_set_quiet(dmi_context_t *context, const char *value);
static bool dmi_show_set_verbose(dmi_context_t *context, const char *value);
static int dmi_show_main(dmi_context_t *context, int argc, char *argv[]);

static dmi_show_config_t dmi_show_config =
{
    .options = {
        .mode = DMI_FORMAT_MODE_NORMAL,
        .dump = false
    }
};

const dmi_option_set_t dmi_show_options =
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
            .short_names = "q",
            .long_names  = (const char *[]){ "quiet", nullptr },
            .description = "Hide meta-data and handle references",
            .handler     = dmi_show_set_quiet
        },
        {
            .short_names = "V",
            .long_names  = (const char *[]){ "verbose", nullptr },
            .description = "Show structure versions and states",
            .handler     = dmi_show_set_verbose
        },
        {
            .short_names = "D",
            .long_names  = (const char *[]){ "dump", nullptr },
            .description = "Do not decode the entries",
            .value       = &dmi_show_config.options.dump
        },
        {}
    }
};

const dmi_command_t dmi_show_command =
{
    .name        = "show",
    .description = "Show SMBIOS structures data",
    .options     = dmi_options(&dmi_show_options, &dmi_filter_options),
    .flags       = DMI_COMMAND_FLAG_PAGER,
    .handlers    = {
        .usage = dmi_show_usage,
        .main  = dmi_show_main
    }
};

static void dmi_show_usage(void)
{
    dmi_command_usage(&dmi_show_command);
}

static bool dmi_show_set_quiet(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_show_config.options.mode = DMI_FORMAT_MODE_QUIET;

    return true;
}

static bool dmi_show_set_verbose(dmi_context_t *context, const char *value)
{
    dmi_unused(context);
    dmi_unused(value);

    dmi_show_config.options.mode = DMI_FORMAT_MODE_VERBOSE;

    return true;
}

static int dmi_show_main(dmi_context_t *context, int argc, char *argv[])
{
    dmi_unused(argc);
    dmi_unused(argv);

    if (not dmi_print_all(context, stdout, &dmi_text_format, &dmi_show_config.options)) {
        dmi_command_trace(context);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
