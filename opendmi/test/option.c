//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/command.h>
#include <opendmi/option.h>
#include <opendmi/internal.h>
#include <opendmi/test/helpers.h>

static int test_option_setup(void **pstate);
static int test_option_teardown(void **pstate);

static void test_option_parse_empty(void **pstate);
static void test_option_parse_short_flags(void **pstate);
static void test_option_parse_short_value(void **pstate);
static void test_option_parse_long(void **pstate);
static void test_option_parse_optional(void **pstate);
static void test_option_parse_terminators(void **pstate);
static void test_option_parse_errors(void **pstate);
static void test_option_find(void **pstate);
static void test_option_not_implemented(void **pstate);
static void test_option_duplicates(void **pstate);

static dmi_context_t *test_context = nullptr;

static struct {
    bool is_all;
    bool is_brief;
    bool is_verbose;
    bool is_quiet;
    const char *file;
    const char *log;
    bool is_log;
    int messages;
} test_config;

static bool test_set_log(dmi_context_t *context, const char *value);

static const dmi_option_set_t test_options = {
    .name    = "Test options",
    .options = (const dmi_option_t[]){
        {
            .short_names = "a",
            .long_names  = (const char *[]){ "all", nullptr },
            .value       = &test_config.is_all
        },
        {
            .short_names = "b",
            .long_names  = (const char *[]){ "brief", nullptr },
            .value       = &test_config.is_brief
        },
        {
            .long_names  = (const char *[]){ "verbose", nullptr },
            .value       = &test_config.is_verbose
        },
        {
            .short_names = "q",
            .value       = &test_config.is_quiet
        },
        {
            .short_names = "n",
            .long_names  = (const char *[]){ "none", nullptr }
        },
        {
            .short_names = "N",
            .long_names  = (const char *[]){ "none-value", nullptr },
            .argument    = {
                .name     = "value",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "i",
            .long_names  = (const char *[]){ "file", nullptr },
            .value       = &test_config.file,
            .argument    = {
                .name     = "path",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = true
            }
        },
        {
            .short_names = "l",
            .long_names  = (const char *[]){ "log", nullptr },
            .handler     = test_set_log,
            .argument    = {
                .name     = "path",
                .type     = DMI_ARGUMENT_TYPE_STRING,
                .required = false
            }
        },
        {}
    }
};

static bool test_set_log(dmi_context_t *context, const char *value)
{
    dmi_unused(context);

    test_config.is_log = true;
    test_config.log    = value;

    return true;
}

// Stub for command.c
void dmi_command_message(const char *format, ...)
{
    dmi_unused(format);
    test_config.messages++;
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_option_parse_empty),
        cmocka_unit_test(test_option_parse_short_flags),
        cmocka_unit_test(test_option_parse_short_value),
        cmocka_unit_test(test_option_parse_long),
        cmocka_unit_test(test_option_parse_optional),
        cmocka_unit_test(test_option_parse_terminators),
        cmocka_unit_test(test_option_parse_errors),
        cmocka_unit_test(test_option_find),
        cmocka_unit_test(test_option_not_implemented),
        cmocka_unit_test(test_option_duplicates)
    };

    return cmocka_run_group_tests(tests, test_option_setup, test_option_teardown);
}

static int test_option_setup(void **pstate)
{
    dmi_unused(pstate);

    test_context = dmi_create(0);
    if (test_context == nullptr)
        return -1;

    return 0;
}

static int test_option_teardown(void **pstate)
{
    dmi_unused(pstate);

    dmi_destroy(test_context);
    test_context = nullptr;

    return 0;
}

// Parse argument list and return number of processed arguments
static int parse(int argc, const char *args[])
{
    static char buffer[16][64];
    char *argv[16];

    assert_true(argc <= 16);

    memset(&test_config, 0, sizeof(test_config));

    // Parser may modify arguments, so copy them
    for (int i = 0; i < argc; i++) {
        strncpy(buffer[i], args[i], sizeof(buffer[i]) - 1);
        argv[i] = buffer[i];
    }

    return dmi_option_parse(test_context, dmi_options(&test_options), argc, argv);
}

#define parse_args(...) \
    parse(countof(((const char *[]){ __VA_ARGS__ })), (const char *[]){ __VA_ARGS__ })

static void test_option_parse_empty(void **pstate)
{
    dmi_unused(pstate);

    assert_int_equal(parse(0, (const char *[]){ nullptr }), 0);
    assert_int_equal(parse_args("show"), 0);
    assert_int_equal(parse_args("show", "-a"), 0);
    assert_false(test_config.is_all);
}

static void test_option_parse_short_flags(void **pstate)
{
    dmi_unused(pstate);

    assert_int_equal(parse_args("-a", "show"), 1);
    assert_true(test_config.is_all);
    assert_false(test_config.is_brief);

    // Several flags in a single argument
    assert_int_equal(parse_args("-ab", "show"), 1);
    assert_true(test_config.is_all);
    assert_true(test_config.is_brief);

    assert_int_equal(parse_args("-a", "-b", "show"), 2);
    assert_true(test_config.is_all);
    assert_true(test_config.is_brief);
}

static void test_option_parse_short_value(void **pstate)
{
    dmi_unused(pstate);

    // Value in the next argument
    assert_int_equal(parse_args("-i", "dump.bin", "show"), 2);
    assert_string_equal(test_config.file, "dump.bin");

    // Value in the same argument
    assert_int_equal(parse_args("-idump.bin", "show"), 1);
    assert_string_equal(test_config.file, "dump.bin");

    // Value starting with dash is accepted, as getopt does
    assert_int_equal(parse_args("-i", "-a", "show"), 2);
    assert_string_equal(test_config.file, "-a");
    assert_false(test_config.is_all);

    // Flags followed by option with value
    assert_int_equal(parse_args("-abi", "dump.bin", "show"), 2);
    assert_true(test_config.is_all);
    assert_true(test_config.is_brief);
    assert_string_equal(test_config.file, "dump.bin");

    assert_int_equal(parse_args("-a", "-i", "dump.bin", "-b", "show"), 4);
    assert_true(test_config.is_brief);
}

static void test_option_parse_long(void **pstate)
{
    dmi_unused(pstate);

    // Options with only long or only short names
    assert_int_equal(parse_args("--verbose", "-q", "show"), 2);
    assert_true(test_config.is_verbose);
    assert_true(test_config.is_quiet);

    assert_int_equal(parse_args("--all", "show"), 1);
    assert_true(test_config.is_all);

    assert_int_equal(parse_args("--file", "dump.bin", "show"), 2);
    assert_string_equal(test_config.file, "dump.bin");

    assert_int_equal(parse_args("--file=dump.bin", "show"), 1);
    assert_string_equal(test_config.file, "dump.bin");

    assert_int_equal(parse_args("--file", "--all", "show"), 2);
    assert_string_equal(test_config.file, "--all");
    assert_false(test_config.is_all);

    assert_int_equal(parse_args("--all", "--file", "dump.bin", "--brief", "show"), 4);
    assert_true(test_config.is_all);
    assert_true(test_config.is_brief);
    assert_string_equal(test_config.file, "dump.bin");
}

static void test_option_parse_optional(void **pstate)
{
    dmi_unused(pstate);

    // Short form of option with optional value is just a flag
    assert_int_equal(parse_args("-l", "show"), 1);
    assert_true(test_config.is_log);
    assert_null(test_config.log);

    assert_int_equal(parse_args("-la", "show"), 1);
    assert_true(test_config.is_log);
    assert_true(test_config.is_all);

    assert_int_equal(parse_args("-al", "show"), 1);
    assert_true(test_config.is_log);
    assert_true(test_config.is_all);

    // Attached value is not accepted in short form
    assert_int_equal(parse_args("-llog.txt"), -1);

    // Long form accepts value only after equal sign
    assert_int_equal(parse_args("--log", "show"), 1);
    assert_true(test_config.is_log);
    assert_null(test_config.log);

    assert_int_equal(parse_args("--log=log.txt", "show"), 1);
    assert_true(test_config.is_log);
    assert_string_equal(test_config.log, "log.txt");

    assert_int_equal(parse_args("--log=", "show"), 1);
    assert_string_equal(test_config.log, "");
}

static void test_option_parse_terminators(void **pstate)
{
    dmi_unused(pstate);

    // Double dash is consumed and terminates options
    assert_int_equal(parse_args("-a", "--", "-b"), 2);
    assert_true(test_config.is_all);
    assert_false(test_config.is_brief);

    // Single dash is not an option
    assert_int_equal(parse_args("-a", "-", "-b"), 1);
    assert_false(test_config.is_brief);
}

static void test_option_parse_errors(void **pstate)
{
    dmi_unused(pstate);

    assert_int_equal(parse_args("-x"), -1);
    assert_int_equal(test_config.messages, 1);

    assert_int_equal(parse_args("--unknown"), -1);
    assert_int_equal(parse_args("-i"), -1);
    assert_int_equal(parse_args("--file"), -1);
    assert_int_equal(parse_args("--all=yes"), -1);
}

static void test_option_find(void **pstate)
{
    dmi_unused(pstate);

    const dmi_option_t *option;

    option = dmi_option_find_long(&test_options, "verbose");
    assert_non_null(option);
    assert_ptr_equal(option->value, &test_config.is_verbose);

    option = dmi_option_find_long(&test_options, "file");
    assert_non_null(option);
    assert_ptr_equal(option->value, &test_config.file);

    assert_null(dmi_option_find_long(&test_options, "q"));
    assert_null(dmi_option_find_long(&test_options, "unknown"));

    option = dmi_option_find_short(&test_options, 'q');
    assert_non_null(option);
    assert_ptr_equal(option->value, &test_config.is_quiet);

    assert_null(dmi_option_find_short(&test_options, 'v'));
}

static void test_option_not_implemented(void **pstate)
{
    dmi_unused(pstate);

    // Options without both handler and value are reported as errors
    assert_int_equal(parse_args("-n"), -1);
    assert_int_equal(test_config.messages, 1);

    assert_int_equal(parse_args("--none"), -1);
    assert_int_equal(parse_args("-N", "value"), -1);
    assert_int_equal(parse_args("--none-value=value"), -1);
}

static void test_option_duplicates(void **pstate)
{
    dmi_unused(pstate);

    const dmi_option_set_t unique_options = {
        .name    = "Unique options",
        .options = (const dmi_option_t[]){
            {
                .short_names = "xy",
                .long_names  = (const char *[]){ "extra", nullptr },
                .value       = &test_config.is_all
            },
            {
                .long_names  = (const char *[]){ "extra-long", nullptr },
                .value       = &test_config.is_all
            },
            {}
        }
    };

    const dmi_option_set_t duplicate_options = {
        .name    = "Duplicate options",
        .options = (const dmi_option_t[]){
            {
                .short_names = "za",
                .long_names  = (const char *[]){ "zzz", nullptr },
                .value       = &test_config.is_all
            },
            {
                .short_names = "Z",
                .long_names  = (const char *[]){ "brief", nullptr },
                .value       = &test_config.is_all
            },
            {
                .short_names = "z",
                .long_names  = (const char *[]){ "zzz", nullptr },
                .value       = &test_config.is_all
            },
            {}
        }
    };

    assert_int_equal(dmi_test_option_duplicates(dmi_options(&test_options)), 0);
    assert_int_equal(dmi_test_option_duplicates(dmi_options(&test_options, &unique_options)), 0);

    // Duplicates in the same set: -z and --zzz
    assert_int_equal(dmi_test_option_duplicates(dmi_options(&duplicate_options, &unique_options)), 2);

    // Duplicates in different sets: -a and --brief, in addition to the above
    assert_int_equal(dmi_test_option_duplicates(dmi_options(&test_options, &duplicate_options)), 4);
}
