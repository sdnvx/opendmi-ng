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
#include <opendmi/utils/locale.h>
#include <opendmi/utils/vector.h>

#include <opendmi/command/common.h>
#include <opendmi/command/lint.h>

/**
 * @brief Widths of the columns of the rule listing, in characters.
 */
#define DMI_LINT_CODE_WIDTH     32
#define DMI_LINT_SEVERITY_WIDTH 16

/**
 * @brief State of the report being printed.
 */
typedef struct dmi_lint_report
{
    dmi_context_t *context;

    size_t total;
    size_t counts[DMI_LINT_SEVERITY_ERROR + 1];
} dmi_lint_report_t;

typedef struct dmi_lint_config
{
    bool check_all;
    bool strict;
    bool quiet;
    bool list_rules;
    bool producer;

    // Code names of the rules to enable or to disable, as given on the
    // command line
    dmi_vector_t enabled;
    dmi_vector_t disabled;
} dmi_lint_config_t;

static void dmi_lint_usage(void);
static int dmi_lint_main(dmi_context_t *context, int argc, char *argv[]);

static bool dmi_lint_config_enable(dmi_context_t *context, const char *value);
static bool dmi_lint_config_disable(dmi_context_t *context, const char *value);

static bool dmi_lint_rule_filter(void *data, const dmi_lint_rule_t *rule);
static bool dmi_lint_rule_matches(const dmi_vector_t *codes, const dmi_lint_rule_t *rule);
static void dmi_lint_issue_print(void *data, const dmi_lint_issue_t *issue);
static void dmi_lint_summary(const dmi_lint_report_t *report);
static void dmi_lint_pad(const char *text, size_t width);
static void dmi_lint_rule_print(const dmi_lint_rule_t *rule);
static void dmi_lint_rules_print(dmi_context_t *context);
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
    dmi_unused(argv);

    if (argc > 0) {
        dmi_command_message_ex(&dmi_lint_command, "Too many arguments");
        return EXIT_USAGE;
    }

    if (dmi_lint_config.list_rules) {
        dmi_lint_rules_print(context);
        return EXIT_SUCCESS;
    }

    const dmi_lint_options_t options =
    {
        .profile = dmi_lint_config.producer
                ? DMI_LINT_PROFILE_PRODUCER : DMI_LINT_PROFILE_READER,
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

//
// Rules are enabled and disabled by their code names, and a name without a
// dot stands for the whole group, e.g. "entry" for every "entry.*" rule.
//
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

//
// Print an issue the way a compiler prints a diagnostic, so that the output
// is readable by the tools which are used to that format. The data being
// checked is not named, since there is only one of it.
//
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
        spec = dmi_type_spec(report->context, issue->type);

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

    dmi_tty_cprintf(DMI_TTY_COLOR_NONE, ": %s ", issue->message);
    dmi_tty_cprintf(DMI_TTY_COLOR_GREY, "[%s]\n", issue->rule->code);
}

//
// Print the number of the issues of every severity, which is the summary a
// build log is grepped for.
//
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

//
// Pad a string to a width, counting the characters rather than the bytes,
// since a translated name holds more bytes than it takes on the screen.
//
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

static void dmi_lint_rule_print(const dmi_lint_rule_t *rule)
{
    dmi_lint_severity_t severity = dmi_lint_rule_severity(rule, DMI_LINT_PROFILE_READER);
    const char *name = dmi_name_lookup(&dmi_lint_severity_names, severity);

    dmi_tty_cprintf(DMI_TTY_COLOR_YELLOW, "%4s%s", "", rule->code);
    dmi_lint_pad(rule->code, DMI_LINT_CODE_WIDTH);

    dmi_tty_cprintf(dmi_lint_severity_color(severity), "%s", name);
    dmi_lint_pad(name, DMI_LINT_SEVERITY_WIDTH);

    dmi_tty_cprintf(DMI_TTY_COLOR_WHITE, "%s", dmi_lint_rule_name(rule));

    if (rule->optional)
        dmi_tty_cprintf(DMI_TTY_COLOR_GREY, " (%s)", dmi_tool_string("all checks only"));

    printf("\n");
}

static void dmi_lint_rules_print(dmi_context_t *context)
{
    dmi_tty_header("%s:", dmi_tool_string("Rules"));

    for (const dmi_lint_rule_t *const *rule = dmi_lint_rules(); *rule != nullptr; rule++)
        dmi_lint_rule_print(*rule);

    printf("\n");

    // Rules of the structure types are provided by their specifications, so
    // the listing depends on the modules which are enabled
    dmi_tty_header("%s:", dmi_tool_string("Rules of the structure types"));

    for (dmi_type_t type = 0; type <= DMI_TYPE_MAX; type++) {
        const dmi_entity_spec_t *spec = dmi_type_spec(context, type);

        if ((spec == nullptr) or (spec->lint_rules == nullptr))
            continue;

        for (const dmi_lint_rule_t *const *rule = spec->lint_rules; *rule != nullptr; rule++)
            dmi_lint_rule_print(*rule);
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
