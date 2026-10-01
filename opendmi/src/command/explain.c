//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/locale.h>
#include <opendmi/utils/tty.h>
#include <opendmi/utils/locale.h>
#include <opendmi/command/explain.h>

/**
 * @internal
 * @brief Print the usage of the command.
 */
static void dmi_explain_usage(void);

/**
 * @internal
 * @brief Print the explanation of the structure type named by the argument.
 *
 * @param[in] context DMI context.
 * @param[in] argc    Number of the arguments.
 * @param[in] argv    Arguments of the command.
 *
 * @return Exit code of the command.
 */
static int dmi_explain_main(dmi_context_t *context, int argc, char *argv[]);

/**
 * @internal
 * @brief Find the specification of a structure type.
 *
 * @details Types are named either by their code or by their number, the way
 * the filter options of the other commands take them.
 *
 * @param[in] context DMI context.
 * @param[in] code    Code or number of the type.
 *
 * @return Specification of the type, or `nullptr` if there is none.
 */
static const dmi_entity_spec_t *dmi_explain_find_entity(dmi_context_t *context, const char *code);

/**
 * @internal
 * @brief Look up the explanation of the structure type in the resources of
 * the tool.
 *
 * @param[in] code Code of the type.
 *
 * @return Translated explanation, or `nullptr` if there is none.
 */
static const char *dmi_explain_text(const char *code);

static const dmi_option_set_t dmi_explain_options =
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

const dmi_command_t dmi_explain_command =
{
    .name        = "explain",
    .description = "Explain SMBIOS structure or type",
    .options     = dmi_options(&dmi_explain_options),
    .flags       = DMI_COMMAND_FLAG_DETACHED | DMI_COMMAND_FLAG_PAGER,
    .arguments   = (const dmi_argument_t[]){
        {
            .name     = "type",
            .type     = DMI_ARGUMENT_TYPE_STRING,
            .required = true
        },
        {}
    },
    .handlers    = {
        .usage = dmi_explain_usage,
        .main  = dmi_explain_main
    }
};

static void dmi_explain_usage(void)
{
    dmi_command_usage(&dmi_explain_command);
}

static int dmi_explain_main(dmi_context_t *context, int argc, char *argv[])
{
    assert(context != nullptr);
    assert(argc >= 0);
    assert(argv != nullptr);

    if (argc < 1) {
        dmi_command_message_ex(&dmi_explain_command, "No type specified");
        return EXIT_USAGE;
    }

    if (argc > 1) {
        dmi_command_message_ex(&dmi_explain_command, "Too many arguments");
        return EXIT_USAGE;
    }

    const dmi_entity_spec_t *spec = dmi_explain_find_entity(context, *argv);
    if (spec == nullptr) {
        dmi_command_message_ex(&dmi_explain_command, "Unknown type specified: %s", *argv);
        return EXIT_FAILURE;
    }

    // Name of the type is translated, while the code is machine-readable
    dmi_tty_header("%s, type %d\n%s", spec->code, (int)spec->type->id, dmi_spec_name(spec));

    // Explanations are translated, and the built-in ones are used if there is
    // no translation, or if the tool is built without ICU4C support
    const char *text = dmi_explain_text(spec->code);

    if (text != nullptr) {
        printf("%s\n\n", text);
    } else if (spec->description != nullptr) {
        const char **para;

        for (para = spec->description; *para != nullptr; para++) {
            printf("%s\n\n", *para);
        }
    }

    return EXIT_SUCCESS;
}

static const char *dmi_explain_text(const char *code)
{
    return dmi_resource_string(dmi_tool_resource(), code, "description");
}

static const dmi_entity_spec_t *dmi_explain_find_entity(dmi_context_t *context, const char *code)
{
    assert(context != nullptr);
    assert(code != nullptr);

    // Types are named either by their code or by their number, the way the
    // filter options of the other commands take them
    dmi_type_id_t type = dmi_type_find(context, code);

    if (type == DMI_TYPE_ID_INVALID) {
        char *end;
        long value;

        errno = 0;
        value = strtol(code, &end, 10);

        if ((*code == 0) or (*end != 0) or (errno != 0) or
            (value < 0) or (value > DMI_TYPE_ID_MAX))
            return nullptr;

        type = (dmi_type_id_t)value;
    }

    return dmi_type_spec(context, type);
}
