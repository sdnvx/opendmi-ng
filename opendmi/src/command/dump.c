//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/command/dump.h>

typedef struct dmi_dump_config
{
    char *output_path;
    bool  force;
} dmi_dump_config_t;

static void dmi_dump_usage(void);
static int dmi_dump_main(dmi_context_t *context, int argc, char *argv[]);

static dmi_dump_config_t dmi_dump_config =
{
    .output_path = "smbios.bin",
    .force       = false
};

static const dmi_option_set_t dmi_dump_options =
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
            .short_names = "o",
            .long_names  = (const char *[]){ "output", nullptr },
            .description = "Set output file path (default: smbios.bin)",
            .value       = &dmi_dump_config.output_path,
            .argument    = {
                .name     = "path",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "F",
            .long_names  = (const char *[]){ "force", nullptr },
            .description = "Overwrite existing files",
            .value       = &dmi_dump_config.force
        },
        {}
    }
};

const dmi_command_t dmi_dump_command =
{
    .name        = "dump",
    .description = "Dump entire SMBIOS table to file",
    .options     = dmi_options(&dmi_dump_options),
    .handlers    = {
        .usage = dmi_dump_usage,
        .main  = dmi_dump_main
    }
};

static void dmi_dump_usage(void)
{
    dmi_command_usage(&dmi_dump_command);
}

static int dmi_dump_main(dmi_context_t *context, int argc, char *argv[])
{
    dmi_unused(context);
    dmi_unused(argc);
    dmi_unused(argv);

    if (not dmi_save(context, dmi_dump_config.output_path, dmi_dump_config.force)) {
        dmi_command_trace(context);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
