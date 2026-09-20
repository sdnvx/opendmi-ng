//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils/tty.h>

#include <opendmi/command/common.h>
#include <opendmi/command/list.h>

typedef struct dmi_list_config {
    bool show_raw;
} dmi_list_config_t;

static void dmi_list_usage(void);
static int dmi_list_main(dmi_context_t *context, int argc, char *argv[]);

static dmi_list_config_t dmi_list_config =
{
    .show_raw = false
};

static const dmi_option_set_t dmi_list_options =
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
            .value       = &dmi_list_config.show_raw
        },
        {}
    }
};

const dmi_command_t dmi_list_command =
{
    .name        = "list",
    .description = "List SMBIOS structures",
    .options     = dmi_options(&dmi_list_options, &dmi_filter_options),
    .flags       = DMI_COMMAND_FLAG_PAGER,
    .handlers    = {
        .usage = dmi_list_usage,
        .main  = dmi_list_main
    }
};

static void dmi_list_usage(void)
{
    dmi_command_usage(&dmi_list_command);
}

static int dmi_list_main(dmi_context_t *context, int argc, char *argv[])
{
    dmi_registry_iter_t iter;
    dmi_entity_t *entity;

    assert(context != nullptr);
    dmi_unused(argc);
    dmi_unused(argv);

    dmi_registry_t *registry = dmi_get_registry(context);
    dmi_registry_iter_init(&iter, registry, &dmi_filter_config.filter);

    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        if (dmi_list_config.show_raw) {
            printf("0x%04hX\t%d\t%s\n", entity->handle, (int)entity->type, dmi_entity_name(entity));
        } else {
            dmi_tty_cprintf(DMI_TTY_COLOR_NAVY, "0x%04hX", entity->handle);
            dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "  %-3u", entity->type);
            dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, "  %s\n", dmi_entity_name(entity));
        }
    }

    return EXIT_SUCCESS;
}
