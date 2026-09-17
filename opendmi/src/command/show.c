//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>

#include <opendmi/context.h>
#include <opendmi/format/text.h>

#include <opendmi/command/common.h>
#include <opendmi/command/show.h>

typedef struct dmi_show_config
{
    bool show_dump;
    bool quiet;
} dmi_show_config_t;

static void dmi_show_usage(void);
static int dmi_show_main(dmi_context_t *context, int argc, char *argv[]);

static dmi_show_config_t dmi_show_config =
{
    .show_dump = false,
    .quiet     = false
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
            .value       = &dmi_show_config.quiet
        },
        {
            .short_names = "D",
            .long_names  = (const char *[]){ "dump", nullptr },
            .description = "Do not decode the entries",
            .value       = &dmi_show_config.show_dump
        },
        {}
    }
};

const dmi_command_t dmi_show_command =
{
    .name        = "show",
    .description = "Show SMBIOS structures data",
    .options     = dmi_options(&dmi_show_options, &dmi_filter_options),
    .handlers    = {
        .usage = dmi_show_usage,
        .main  = dmi_show_main
    }
};

static void dmi_show_usage(void)
{
    dmi_command_usage(&dmi_show_command);
}

static int dmi_show_main(dmi_context_t *context, int argc, char *argv[])
{
    dmi_unused(argc);
    dmi_unused(argv);

    if (not dmi_print_all(context, stdout, &dmi_text_format, dmi_show_config.show_dump)) {
        dmi_command_trace(context);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
