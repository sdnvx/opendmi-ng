//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <opendmi/context.h>
#include <opendmi/lint.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/tty.h>
#include <opendmi/utils/utf8.h>
#include <opendmi/utils/locale.h>
#include <opendmi/utils/vector.h>

#include <opendmi/command/common.h>
#include <opendmi/command/lint.h>

/**
 * @internal
 * @brief Widths of the columns of the rule listing, in characters.
 */
#define DMI_LINT_CODE_WIDTH     32
#define DMI_LINT_SEVERITY_WIDTH 16

/**
 * @internal
 * @brief State of the report being printed.
 */
typedef struct dmi_lint_report
{
    dmi_context_t *context;

    size_t total;
    size_t counts[DMI_LINT_SEVERITY_ERROR + 1];
} dmi_lint_report_t;

/**
 * @internal
 * @brief Configuration of the command, as given on the command line.
 */
typedef struct dmi_lint_config
{
    bool check_all;
    bool strict;
    bool quiet;
    bool list_rules;
    bool producer;

    /**
     * @brief Code names of the rules to enable, as given on the command line.
     */
    dmi_vector_t enabled;

    /**
     * @brief Code names of the rules to disable, as given on the command
     * line.
     */
    dmi_vector_t disabled;
} dmi_lint_config_t;

/**
 * @internal
 * @brief Print the usage of the command.
 */
static void dmi_lint_usage(void);

/**
 * @internal
 * @brief Check the SMBIOS data against the lint rules and report the issues
 * found, or list the rules.
 *
 * @param[in] context DMI context.
 * @param[in] argc    Number of the arguments.
 * @param[in] argv    Arguments of the command, unused.
 *
 * @return Exit code of the command.
 */
static int dmi_lint_main(dmi_context_t *context, int argc, char *argv[]);

/**
 * @internal
 * @brief Tell whether the command needs no SMBIOS data.
 *
 * @details Listing the rules needs no data, since the rules are the ones of
 * the library and of the modules which are enabled.
 *
 * @return `true` if the rules are to be listed, `false` otherwise.
 */
static bool dmi_lint_detached(void);

/**
 * @internal
 * @brief Handle the `--enable` option: enable a rule or a group of rules.
 *
 * @param[in] context DMI context, unused.
 * @param[in] value   Code name of the rule or of the group.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_lint_config_enable(dmi_context_t *context, const char *value);

/**
 * @internal
 * @brief Handle the `--disable` option: disable a rule or a group of rules.
 *
 * @param[in] context DMI context, unused.
 * @param[in] value   Code name of the rule or of the group.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_lint_config_disable(dmi_context_t *context, const char *value);

/**
 * @internal
 * @brief Tell whether a rule is to be checked, according to the rules
 * enabled and disabled on the command line.
 *
 * @param[in] data Configuration of the command.
 * @param[in] rule Rule to check.
 *
 * @return `true` if the rule is to be checked, `false` otherwise.
 */
static bool dmi_lint_rule_filter(void *data, const dmi_lint_rule_t *rule);

/**
 * @internal
 * @brief Tell whether a rule matches one of the code names given.
 *
 * @details Rules are enabled and disabled by their code names, and a name
 * without a dot stands for the whole group, e.g. "entry" for every "entry.*"
 * rule.
 *
 * @param[in] codes Code names of the rules or of the groups.
 * @param[in] rule  Rule to match.
 *
 * @return `true` if the rule matches one of the names, `false` otherwise.
 */
static bool dmi_lint_rule_matches(const dmi_vector_t *codes, const dmi_lint_rule_t *rule);

/**
 * @internal
 * @brief Count an issue and print it, unless it is below an error in quiet
 * mode.
 *
 * @details An issue is printed the way a compiler prints a diagnostic, so that
 * the output is readable by the tools which are used to that format. The data
 * being checked is not named, since there is only one of it.
 *
 * @param[in,out] data  Report being printed.
 * @param[in]     issue Issue to print.
 */
static void dmi_lint_issue_print(void *data, const dmi_lint_issue_t *issue);

/**
 * @internal
 * @brief Print the number of the issues of every severity, which is the
 * summary a build log is grepped for.
 *
 * @param[in] report Report being printed.
 */
static void dmi_lint_summary(const dmi_lint_report_t *report);

/**
 * @internal
 * @brief Pad a string to a width, counting the characters rather than the
 * bytes, since a translated name holds more bytes than it takes on the screen.
 *
 * @param[in] text  String which has been printed.
 * @param[in] width Width to pad the string to.
 */
static void dmi_lint_pad(const char *text, size_t width);

/**
 * @internal
 * @brief Print a line of the rule listing: the code name, the severity and
 * the name of a rule.
 *
 * @param[in] rule    Rule to print.
 * @param[in] profile Profile the severity is given for.
 */
static void dmi_lint_rule_print(const dmi_lint_rule_t *rule, dmi_lint_profile_t profile);

/**
 * @internal
 * @brief Print the rules of the library, and then the ones of the structure
 * types.
 *
 * @param[in] context DMI context.
 * @param[in] profile Profile the severities are given for.
 */
static void dmi_lint_rules_print(dmi_context_t *context, dmi_lint_profile_t profile);

/**
 * @internal
 * @brief Get the color an issue of a severity is printed in.
 *
 * @param[in] severity Severity of the issue.
 *
 * @return Color of the severity.
 */
static dmi_tty_color_t dmi_lint_severity_color(dmi_lint_severity_t severity);

static dmi_lint_config_t dmi_lint_config;

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
            .description = "Enable the checks which are disabled by default",
            .value       = &dmi_lint_config.check_all
        },
        {
            .short_names = "e",
            .long_names  = (const char *[]){ "enable", nullptr },
            .description = "Enable the rule or the group of rules",
            .handler     = dmi_lint_config_enable,
            .argument    = {
                .name     = "rule",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "d",
            .long_names  = (const char *[]){ "disable", nullptr },
            .description = "Disable the rule or the group of rules",
            .handler     = dmi_lint_config_disable,
            .argument    = {
                .name     = "rule",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "P",
            .long_names  = (const char *[]){ "producer", nullptr },
            .description = "Check the data as if it was produced, not read",
            .value       = &dmi_lint_config.producer
        },
        {
            .short_names = "S",
            .long_names  = (const char *[]){ "strict", nullptr },
            .description = "Treat warnings as errors",
            .value       = &dmi_lint_config.strict
        },
        {
            .short_names = "q",
            .long_names  = (const char *[]){ "quiet", nullptr },
            .description = "Report errors only",
            .value       = &dmi_lint_config.quiet
        },
        {
            .short_names = "R",
            .long_names  = (const char *[]){ "list-rules", nullptr },
            .description = "List the rules and exit",
            .value       = &dmi_lint_config.list_rules
        },
        {}
    }
};

const dmi_command_t dmi_lint_command =
{
    .name        = "lint",
    .description = "Check SMBIOS structures for errors",
    .options     = dmi_options(&dmi_lint_options),
    .flags       = DMI_COMMAND_FLAG_PAGER,
    .handlers    = {
        .usage    = dmi_lint_usage,
        .main     = dmi_lint_main,
        .detached = dmi_lint_detached
    }
};

static void dmi_lint_usage(void)
{
    dmi_command_usage(&dmi_lint_command);
}

static int dmi_lint_main(dmi_context_t *context, int argc, char *argv[])
{
    dmi_unused(argv);

    if (argc > 0) {
        dmi_command_message_ex(&dmi_lint_command, "Too many arguments");
        return EXIT_USAGE;
    }

    dmi_lint_profile_t profile = dmi_lint_config.producer
            ? DMI_LINT_PROFILE_PRODUCER : DMI_LINT_PROFILE_READER;

    if (dmi_lint_config.list_rules) {
        dmi_lint_rules_print(context, profile);
        return EXIT_SUCCESS;
    }

    const dmi_lint_options_t options =
    {
        .profile          = profile,
        .all              = dmi_lint_config.check_all,
        .rule_filter      = dmi_lint_rule_filter,
        .rule_filter_data = &dmi_lint_config
    };

    dmi_lint_report_t report = { .context = context };

    if (not dmi_lint(context, &options, dmi_lint_issue_print, &report)) {
        dmi_command_trace(context);
        return EXIT_FAILURE;
    }

    dmi_lint_summary(&report);

    // Data is clean if nothing worth an error has been found, and warnings
    // count as errors in strict mode
    if (report.counts[DMI_LINT_SEVERITY_ERROR] > 0)
        return EXIT_FAILURE;

    if (dmi_lint_config.strict and (report.counts[DMI_LINT_SEVERITY_WARNING] > 0))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

static bool dmi_lint_detached(void)
{
    return dmi_lint_config.list_rules;
}

static bool dmi_lint_config_enable(dmi_context_t *context, const char *value)
{
    dmi_unused(context);

    return dmi_vector_push(&dmi_lint_config.enabled, (uintptr_t)value);
}

static bool dmi_lint_config_disable(dmi_context_t *context, const char *value)
{
    dmi_unused(context);

    return dmi_vector_push(&dmi_lint_config.disabled, (uintptr_t)value);
}

static bool dmi_lint_rule_matches(const dmi_vector_t *codes, const dmi_lint_rule_t *rule)
{
    uintptr_t value;

    for (size_t i = 0; dmi_vector_get(codes, i, &value); i++) {
        const char *code = (const char *)value;
        size_t length = strlen(code);

        if (strcmp(rule->code, code) == 0)
            return true;

        if ((strncmp(rule->code, code, length) == 0) and (rule->code[length] == '.'))
            return true;
    }

    return false;
}

static bool dmi_lint_rule_filter(void *data, const dmi_lint_rule_t *rule)
{
    const dmi_lint_config_t *config = data;

    if (dmi_lint_rule_matches(&config->disabled, rule))
        return false;

    // Naming the rules to check leaves the rest of them out, while the ones
    // named explicitly are checked even if they are disabled by default
    if (dmi_vector_length(&config->enabled) > 0)
        return dmi_lint_rule_matches(&config->enabled, rule);

    return true;
}

static void dmi_lint_issue_print(void *data, const dmi_lint_issue_t *issue)
{
    dmi_lint_report_t *report = data;

    report->total++;
    report->counts[issue->severity]++;

    if (dmi_lint_config.quiet and (issue->severity < DMI_LINT_SEVERITY_ERROR))
        return;

    // Issues of the entry point and of the table have no offset of their own
    if (issue->offset != DMI_LINT_NO_OFFSET)
        dmi_tty_cprintf(DMI_TTY_COLOR_NONE, "0x%04zX: ", issue->offset);

    dmi_tty_cprintf(dmi_lint_severity_color(issue->severity), "%s: ",
                    dmi_name_lookup(&dmi_lint_severity_names, issue->severity));

    // Structures are named by their code, so that the location is machine
    // readable, while the translated name follows it in parentheses
    const dmi_entity_spec_t *spec = nullptr;

    if (issue->handle != DMI_HANDLE_INVALID) {
        // Structures of a type told apart by their signatures are named by
        // the specification they have been decoded by
        const dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(report->context),
                                                         issue->handle, DMI_TYPE_ANY, true);

        spec = ((entity != nullptr) and (entity->spec != nullptr))
             ? entity->spec
             : dmi_type_spec(report->context, issue->type);

        if (spec != nullptr)
            dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "%s@0x%04X", spec->code, (unsigned)issue->handle);
        else
            dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "type-%d@0x%04X",
                            (int)issue->type, (unsigned)issue->handle);
    } else if (issue->rule->scope == DMI_LINT_SCOPE_ENTRY) {
        dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "%s", dmi_tool_text("value", "entry", "<entry>"));
    } else {
        dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "%s", dmi_tool_text("value", "table", "<table>"));
    }

    if (issue->attribute != nullptr)
        dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, ".%s", issue->attribute);

    if (spec != nullptr)
        dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, " (%s)", dmi_spec_name(spec));

    // Messages may quote strings of the data, which are not trusted, so
    // control characters are escaped
    char *message = dmi_utf8_escape(report->context, issue->message);

    dmi_tty_cprintf(DMI_TTY_COLOR_NONE, ": %s ",
                    (message != nullptr) ? message : dmi_tool_text("value", "error", "<error>"));
    dmi_tty_cprintf(DMI_TTY_COLOR_GREY, "[%s]\n", issue->rule->code);
    dmi_free(message);
}

static void dmi_lint_summary(const dmi_lint_report_t *report)
{
    if (report->total == 0) {
        char *text = dmi_tool_message("lint", "clean", "no issues found", nullptr, 0);

        if (text != nullptr) {
            printf("%s\n", text);
            dmi_free(text);
        }

        return;
    }

    const dmi_message_arg_t args[] =
    {
        DMI_MESSAGE_NUMBER(report->total),
        DMI_MESSAGE_NUMBER(report->counts[DMI_LINT_SEVERITY_ERROR]),
        DMI_MESSAGE_NUMBER(report->counts[DMI_LINT_SEVERITY_WARNING]),
        DMI_MESSAGE_NUMBER(report->counts[DMI_LINT_SEVERITY_NOTE])
    };

    char *text = dmi_tool_message("lint", "summary",
                                  "{0} issues: {1} errors, {2} warnings, {3} notes",
                                  args, countof(args));
    if (text == nullptr)
        return;

    printf("%s\n", text);
    dmi_free(text);
}

static void dmi_lint_pad(const char *text, size_t width)
{
    size_t length = 0;

    for (const char *ptr = text; *ptr != 0; ptr++) {
        // Continuation bytes of a character take no place of their own
        if ((*ptr & 0xC0) != 0x80)
            length++;
    }

    while (length++ < width)
        printf(" ");
}

static void dmi_lint_rule_print(const dmi_lint_rule_t *rule, dmi_lint_profile_t profile)
{
    dmi_lint_severity_t severity = dmi_lint_rule_severity(rule, profile);
    const char *name = dmi_name_lookup(&dmi_lint_severity_names, severity);

    dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "%4s%s", "", rule->code);
    dmi_lint_pad(rule->code, DMI_LINT_CODE_WIDTH);

    dmi_tty_cprintf(dmi_lint_severity_color(severity), "%s", name);
    dmi_lint_pad(name, DMI_LINT_SEVERITY_WIDTH);

    dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, "%s", dmi_lint_rule_name(rule));

    if (rule->params.optional)
        dmi_tty_cprintf(DMI_TTY_COLOR_GREY, " (%s)", dmi_tool_string("all checks only"));

    printf("\n");
}

static void dmi_lint_rules_print(dmi_context_t *context, dmi_lint_profile_t profile)
{
    dmi_tty_header("%s:", dmi_tool_string("Rules"));

    for (const dmi_lint_rule_t *const *rule = dmi_lint_rules(); *rule != nullptr; rule++)
        dmi_lint_rule_print(*rule, profile);

    printf("\n");

    // Rules of the structure types are provided by their specifications, so
    // the listing depends on the modules which are enabled
    dmi_tty_header("%s:", dmi_tool_string("Rules of the structure types"));

    for (dmi_type_id_t type = 0; type <= DMI_TYPE_ID_MAX; type++) {
        const dmi_entity_spec_t *spec = dmi_type_spec(context, type);

        if ((spec == nullptr) or (spec->lint_rules == nullptr))
            continue;

        for (const dmi_lint_rule_t *rule = spec->lint_rules; rule->code != nullptr; rule++)
            dmi_lint_rule_print(rule, profile);
    }

    printf("\n");
}

static dmi_tty_color_t dmi_lint_severity_color(dmi_lint_severity_t severity)
{
    switch (severity) {
        case DMI_LINT_SEVERITY_ERROR:
            return DMI_TTY_COLOR_RED;
        case DMI_LINT_SEVERITY_WARNING:
            return DMI_TTY_COLOR_OLIVE;
        case DMI_LINT_SEVERITY_NOTE:
            return DMI_TTY_COLOR_GREY;
        default:
            return DMI_TTY_COLOR_NONE;
    }
}
