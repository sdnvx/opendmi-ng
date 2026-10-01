//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <opendmi/command.h>
#include <opendmi/option.h>
#include <opendmi/internal.h>
#include <opendmi/utils/tty.h>
#include <opendmi/utils/locale.h>

/**
 * @internal
 * @brief Applies an option without argument: calls its handler, or sets
 * the flag it points to, or clears it for a reverse option.
 *
 * @param[in] context Context to pass to the handler.
 * @param[in] option  Option to apply.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_option_toggle(
        dmi_context_t      *context,
        const dmi_option_t *option);

/**
 * @internal
 * @brief Applies an option with argument: calls its handler, or stores the
 * value in the variable it points to.
 *
 * @param[in] context Context to pass to the handler.
 * @param[in] option  Option to apply.
 * @param[in] value   Value of the option, or `nullptr` if an optional value
 *                    is not given.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_option_set(
        dmi_context_t      *context,
        const dmi_option_t *option,
        const char         *value);

/**
 * @internal
 * @brief Arguments, which are left to parse.
 */
typedef struct dmi_option_cursor
{
    /**
     * @brief Number of arguments left.
     */
    int argc;

    /**
     * @brief Arguments left.
     */
    char **argv;
} dmi_option_cursor_t;

/**
 * @internal
 * @brief Prints short names of an option.
 *
 * @param[in] option Option to print the names of.
 * @param[in] count  Number of names printed before.
 *
 * @return Number of names printed, including the ones printed before.
 */
static int dmi_option_list_short(const dmi_option_t *option, int count);

/**
 * @internal
 * @brief Prints long names of an option.
 *
 * @param[in] option Option to print the names of.
 * @param[in] count  Number of names printed before.
 *
 * @return Number of names printed, including the ones printed before.
 */
static int dmi_option_list_long(const dmi_option_t *option, int count);

/**
 * @internal
 * @brief Takes the next argument as the value of an option, as getopt does.
 *
 * @param[in,out] cursor Arguments left to parse.
 *
 * @return Next argument, or `nullptr` if there are no arguments left.
 */
static char *dmi_option_next(dmi_option_cursor_t *cursor);

/**
 * @internal
 * @brief Parses an option in long form, `--name` or `--name=value`.
 *
 * @details Value of an option, which requires one, is taken from the next
 * argument if it is not given after `=`. Optional value is accepted only
 * after `=`.
 *
 * @param[in]     context Context to pass to option handlers.
 * @param[in]     options Option sets to look the option up in.
 * @param[in]     arg     Argument without leading dashes, which is split at
 *                        `=` in place.
 * @param[in,out] cursor  Arguments left to parse.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_option_parse_long(
        dmi_context_t           *context,
        const dmi_option_set_t **options,
        char                    *arg,
        dmi_option_cursor_t     *cursor);

/**
 * @internal
 * @brief Parses options in short form, which may be grouped as `-abc`.
 *
 * @details Value of an option, which requires one, is the rest of the
 * argument, or the next argument if the option is the last one. Optional
 * value is not accepted, so such option is just a flag.
 *
 * @param[in]     context Context to pass to option handlers.
 * @param[in]     options Option sets to look the options up in.
 * @param[in]     arg     Argument without the leading dash.
 * @param[in,out] cursor  Arguments left to parse.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_option_parse_short(
        dmi_context_t           *context,
        const dmi_option_set_t **options,
        char                    *arg,
        dmi_option_cursor_t     *cursor);

void dmi_option_list(const dmi_option_set_t *set)
{
    const dmi_option_t *option;

    assert(set != nullptr);

    dmi_tty_header("%s:", dmi_tool_string(set->name));

    for (option = set->options; option->short_names || option->long_names; option++) {
        if (option->flags & DMI_OPTION_FLAG_HIDDEN)
            continue;

        int count = dmi_option_list_short(option, 0);
        dmi_option_list_long(option, count);

        dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, "\n%8s%s\n", "", dmi_tool_string(option->description));
    }

    printf("\n");
}

const dmi_option_t *dmi_option_find_short(const dmi_option_set_t *set, char name)
{
    const dmi_option_t *option;

    assert(set != nullptr);
    assert(name != 0);

    for (option = set->options; option->short_names || option->long_names; option++) {
        if (!option->short_names)
            continue;

        for (const char *item = option->short_names; *item != 0; item++) {
            if (name == *item)
                return option;
        }
    }

    return nullptr;
}

const dmi_option_t *dmi_option_find_short_ex(const dmi_option_set_t **options, char name)
{
    assert(options != nullptr);
    assert(name != 0);

    while (*options) {
        const dmi_option_t *option = dmi_option_find_short(*options, name);

        if (option != nullptr)
            return option;

        options++;
    }

    return nullptr;
}

const dmi_option_t *dmi_option_find_long(const dmi_option_set_t *set, const char *name)
{
    const dmi_option_t *option;

    assert(set != nullptr);
    assert(name != nullptr);

    for (option = set->options; option->short_names || option->long_names; option++) {
        if (!option->long_names)
            continue;

        for (const char **item = option->long_names; *item != nullptr; item++) {
            if (strcmp(name, *item) == 0)
                return option;
        }
    }

    return nullptr;
}

const dmi_option_t *dmi_option_find_long_ex(const dmi_option_set_t **options, const char *name)
{
    assert(options != nullptr);
    assert(name != 0);

    while (*options) {
        const dmi_option_t *option = dmi_option_find_long(*options, name);

        if (option != nullptr)
            return option;

        options++;
    }

    return nullptr;
}

int dmi_option_parse(
        dmi_context_t           *context,
        const dmi_option_set_t **options,
        int                      argc,
        char                    *argv[])
{
    dmi_option_cursor_t cursor = { .argc = argc, .argv = argv };

    // Number of processed arguments is calculated as the difference between
    // the initial and the remaining number of arguments
    while (cursor.argc > 0) {
        char *arg = *cursor.argv;

        // Non-option argument, including single dash, stops parsing
        if ((arg[0] != '-') or (arg[1] == 0))
            break;

        cursor.argc--, cursor.argv++;

        // Double dash terminates options
        if (strcmp(arg, "--") == 0)
            break;

        bool success = (arg[1] == '-')
            ? dmi_option_parse_long(context, options, arg + 2, &cursor)
            : dmi_option_parse_short(context, options, arg + 1, &cursor);

        if (not success)
            return -1;
    }

    return argc - cursor.argc;
}

static bool dmi_option_toggle(
        dmi_context_t      *context,
        const dmi_option_t *option)
{
    assert(context != nullptr);
    assert(option != nullptr);

    if (option->handler != nullptr)
        return option->handler(context, nullptr);

    // Option without both handler and value is a mistake in options table
    if (option->value == nullptr) {
        dmi_command_message("Option is not implemented");
        return false;
    }

    bool *flag = dmi_cast(flag, option->value);
    if (option->flags & DMI_OPTION_FLAG_REVERSE)
        *flag = false;
    else
        *flag = true;

    return true;
}

static bool dmi_option_set(
        dmi_context_t      *context,
        const dmi_option_t *option,
        const char         *value)
{
    assert(context != nullptr);
    assert(option != nullptr);

    if (option->handler != nullptr)
        return option->handler(context, value);

    // Option without both handler and value is a mistake in options table
    if (option->value == nullptr) {
        dmi_command_message("Option is not implemented");
        return false;
    }

    switch (option->argument.type) {
    case DMI_ARGUMENT_TYPE_STRING:
        *((const char **)option->value) = value;
        break;

    default:
        assert(false);
    }

    return true;
}

static int dmi_option_list_short(const dmi_option_t *option, int count)
{
    const dmi_argument_t *arg = &option->argument;

    if (option->short_names == nullptr)
        return count;

    for (const char *name = option->short_names; *name != 0; name++, count++) {
        printf("%s", count > 0 ? ", " : "    ");

        dmi_tty_cprintf(DMI_TTY_COLOR_AQUA, "-%c", *name);
        if ((arg->type != DMI_ARGUMENT_TYPE_NONE) and arg->required) {
            printf(" ");
            dmi_tty_cprintf(DMI_TTY_COLOR_LIME, "<%s>", dmi_tool_string(arg->name));
        }
    }

    return count;
}

static int dmi_option_list_long(const dmi_option_t *option, int count)
{
    const dmi_argument_t *arg = &option->argument;

    if (option->long_names == nullptr)
        return count;

    for (const char **name = option->long_names; *name != nullptr; name++, count++) {
        printf("%s", count > 0 ? ", " : "    ");

        dmi_tty_cprintf(DMI_TTY_COLOR_AQUA, "--%s", *name);
        if (arg->type != DMI_ARGUMENT_TYPE_NONE) {
            printf(arg->required ? "=" : "[=");
            dmi_tty_cprintf(DMI_TTY_COLOR_LIME, "<%s>", dmi_tool_string(arg->name));
            printf(arg->required ? "" : "]");
        }
    }

    return count;
}

static char *dmi_option_next(dmi_option_cursor_t *cursor)
{
    if (cursor->argc == 0)
        return nullptr;

    cursor->argc--;

    return *cursor->argv++;
}

static bool dmi_option_parse_long(
        dmi_context_t           *context,
        const dmi_option_set_t **options,
        char                    *arg,
        dmi_option_cursor_t     *cursor)
{
    char *value = strchr(arg, '=');
    if (value != nullptr)
        *value++ = 0;

    const dmi_option_t *option = dmi_option_find_long_ex(options, arg);
    if (option == nullptr) {
        dmi_command_message("Unknown option: --%s", arg);
        return false;
    }

    if (option->argument.type == DMI_ARGUMENT_TYPE_NONE) {
        if (value != nullptr) {
            dmi_command_message("Option --%s doesn't have arguments", arg);
            return false;
        }

        return dmi_option_toggle(context, option);
    }

    // Optional value is accepted only in `--name=value` form
    if (option->argument.required and (value == nullptr)) {
        value = dmi_option_next(cursor);
        if (value == nullptr) {
            dmi_command_message("Option --%s requires an argument", arg);
            return false;
        }
    }

    return dmi_option_set(context, option, value);
}

static bool dmi_option_parse_short(
        dmi_context_t           *context,
        const dmi_option_set_t **options,
        char                    *arg,
        dmi_option_cursor_t     *cursor)
{
    for (; *arg != 0; arg++) {
        char flag = *arg;

        const dmi_option_t *option = dmi_option_find_short_ex(options, flag);
        if (option == nullptr) {
            dmi_command_message("Unknown option: -%c", flag);
            return false;
        }

        if (option->argument.type == DMI_ARGUMENT_TYPE_NONE) {
            if (not dmi_option_toggle(context, option))
                return false;

            continue;
        }

        // Optional value is accepted only in `--name=value` form, so short
        // form of the option is just a flag
        if (not option->argument.required) {
            if (not dmi_option_set(context, option, nullptr))
                return false;

            continue;
        }

        // Value is the rest of the argument, or the next argument
        char *value = arg + 1;
        if (*value == 0) {
            value = dmi_option_next(cursor);
            if (value == nullptr) {
                dmi_command_message("Option -%c requires an argument", flag);
                return false;
            }
        }

        return dmi_option_set(context, option, value);
    }

    return true;
}
