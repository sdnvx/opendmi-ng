//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>

#include <opendmi/context.h>

#include <opendmi/command/common.h>
#include <opendmi/command/lint.h>

typedef struct dmi_lint_config
{
    bool check_all;
    bool check_links;
} dmi_lint_config_t;

static void dmi_lint_usage(void);
static int dmi_lint_main(dmi_context_t *context, int argc, char *argv[]);

static dmi_lint_config_t dmi_lint_config =
{
    .check_all   = false,
    .check_links = false
};

static const dmi_option_set_t dmi_lint_options =
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
            .short_names = "A",
            .long_names  = (const char *[]){ "all-checks", nullptr },
            .description = "Enable all checks",
            .value       = &dmi_lint_config.check_all
        },
        {
            .short_names = "l",
            .long_names  = (const char *[]){ "links", nullptr },
            .description = "Enable link checking",
            .value       = &dmi_lint_config.check_links
        },
        {}
    }
};

const dmi_command_t dmi_lint_command =
{
    .name        = "lint",
    .description = "Check SMBIOS structures for errors",
    .options     = dmi_options(&dmi_lint_options, &dmi_filter_options),
    .handlers    = {
        .usage = dmi_lint_usage,
        .main  = dmi_lint_main
    }
};

static void dmi_lint_usage(void)
{
    dmi_command_usage(&dmi_lint_command);
}

static int dmi_lint_main(dmi_context_t *context, int argc, char *argv[])
{
    dmi_unused(context);
    dmi_unused(argc);
    dmi_unused(argv);

    dmi_command_message_ex(&dmi_lint_command, "Not implemented yet");

    return EXIT_FAILURE;
}
