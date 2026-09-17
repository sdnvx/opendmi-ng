//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/command/import.h>

static void dmi_import_usage(void);
static int dmi_import_main(dmi_context_t *context, int argc, char *argv[]);

static const dmi_option_set_t dmi_import_options =
{
    .name    = "Command options",
    .options = (const dmi_option_t[]){
        {
            .short_names = "?h",
            .long_names  = (const char *[]){ "help", nullptr },
            .description = "Print this help and exit",
            .value       = &dmi_command_config.show_usage
        },
        {}
    }
};

const dmi_command_t dmi_import_command =
{
    .name        = "import",
    .description = "Import SMBIOS data from external format",
    .options     = dmi_options(&dmi_import_options),
    .handlers    = {
        .usage = dmi_import_usage,
        .main  = dmi_import_main
    }
};

static void dmi_import_usage(void)
{
    dmi_command_usage(&dmi_import_command);
}

static int dmi_import_main(dmi_context_t *context, int argc, char *argv[])
{
    dmi_unused(context);
    dmi_unused(argc);
    dmi_unused(argv);

    dmi_command_message_ex(&dmi_import_command, "Not implemented yet");

    return EXIT_FAILURE;
}
