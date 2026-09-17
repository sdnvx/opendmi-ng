//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>

#include <opendmi/entry.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/tty.h>

#include <opendmi/command/entry.h>
#include <opendmi/format/text/handlers.h>
#include <opendmi/format/text/helpers.h>

static void dmi_entry_usage(void);
static int dmi_entry_main(dmi_context_t *context, int argc, char *argv[]);

static const dmi_option_set_t dmi_entry_options =
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

const dmi_command_t dmi_entry_command =
{
    .name        = "entry",
    .description = "Show SMBIOS entry point data",
    .options     = dmi_options(&dmi_entry_options),
    .flags       = DMI_COMMAND_FLAG_PAGER,
    .handlers    = {
        .usage = dmi_entry_usage,
        .main  = dmi_entry_main
    }
};

static void dmi_entry_usage(void)
{
    dmi_command_usage(&dmi_entry_command);
}

static int dmi_entry_main(dmi_context_t *context, int argc, char *argv[])
{
    assert(context != nullptr);
    dmi_unused(argc);
    dmi_unused(argv);

    const dmi_entry_spec_t *spec = context->state.entry_spec;
    dmi_text_session_t *session = nullptr;
    char *smbios_version = nullptr;
    char *entry_version = nullptr;
    bool success = false;

    // Some backends (e.g. Windows) do not provide entry point data
    if (spec == nullptr) {
        dmi_command_message_ex(&dmi_entry_command, "Entry point data is not available");
        return EXIT_FAILURE;
    }

    do {
        session = dmi_text_initialize(context, stdout, nullptr);
        if (session == nullptr)
            break;

        smbios_version = dmi_version_format(context->state.smbios_version);
        if (smbios_version == nullptr) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            break;
        }

        entry_version = dmi_version_format(context->state.entry_version);
        if (entry_version == nullptr) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            break;
        }

        dmi_tty_header("%s", spec->name);

        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "Anchor: %s\n", spec->anchor);
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "EPS version: %s\n", entry_version);
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "SMBIOS version: %s\n", smbios_version);
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "Address size: %zu bits\n", context->state.address_size << 3);
        dmi_text_printf(session, DMI_TTY_COLOR_NONE, "\n");

        for (const dmi_attribute_t *attr = spec->attributes; attr->params.name; attr++) {
            const dmi_data_t *value = dmi_member_ptr(context, attr->value, dmi_data_t);

            dmi_text_printf(session, DMI_TTY_COLOR_NONE, "%s: ", attr->params.name);
            dmi_text_entity_attr_value(session, attr, value, nullptr);
        }

        if ((fflush(stdout) != 0) or ferror(stdout)) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s", strerror(errno));
            break;
        }

        success = true;
    } while (false);

    if (session != nullptr)
        dmi_text_finalize(session);
    dmi_free(smbios_version);
    dmi_free(entry_version);

    if (not success) {
        dmi_command_trace(context);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
