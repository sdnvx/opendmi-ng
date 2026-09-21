//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/lint.h>
#include <opendmi/log.h>
#include <opendmi/entry.h>
#include <opendmi/utils.h>
#include <opendmi/utils/file.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

static int test_lint_setup(void **pstate);
static int test_lint_teardown(void **pstate);

static void test_lint_rules(void **pstate);
static void test_lint_clean_dump(void **pstate);
static void test_lint_trailing_data(void **pstate);
static void test_lint_profile(void **pstate);
static void test_lint_rule_filter(void **pstate);
static void test_lint_closed_context(void **pstate);
static void test_lint_raw_data(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Dump which has no issues worth an error
static const char *test_clean_path = OPENDMI_TEST_DATA "/acer/nitro-an515-31.bin";

// Dump with 290 bytes past the end-of-table structure
static const char *test_trailing_path = OPENDMI_TEST_DATA "/lenovo/thinkcentre-m700-10hy.bin";

// Dump with an IPMI device structure, and the copy of it the test breaks
static const char *test_ipmi_path = OPENDMI_TEST_DATA "/asus/rs100-x7.bin";
static const char *test_broken_path = "lint-test.bin";

// Offset of the revision of the IPMI specification within the structure
static const size_t test_ipmi_revision = 0x05;

typedef struct test_lint_report
{
    size_t total;
    size_t counts[DMI_LINT_SEVERITY_ERROR + 1];

    // Rule and severity of the last issue reported
    const dmi_lint_rule_t *rule;
    dmi_lint_severity_t    severity;
} test_lint_report_t;

typedef struct test_lint_state
{
    dmi_context_t *context;
} test_lint_state_t;

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_lint_rules),
        cmocka_unit_test(test_lint_clean_dump),
        cmocka_unit_test(test_lint_trailing_data),
        cmocka_unit_test(test_lint_profile),
        cmocka_unit_test(test_lint_rule_filter),
        cmocka_unit_test(test_lint_closed_context),
        cmocka_unit_test(test_lint_raw_data)
    };

    return cmocka_run_group_tests(tests, test_lint_setup, test_lint_teardown);
}

static int test_lint_setup(void **pstate)
{
    test_lint_state_t *state = calloc(1, sizeof(*state));
    if (state == nullptr)
        return -1;

    *pstate = state;

    state->context = dmi_create(DMI_CONTEXT_FLAG_LINK | DMI_CONTEXT_FLAG_RELAXED);
    if (state->context == nullptr)
        return -1;

    dmi_set_logger(state->context, &test_logger);
    dmi_set_log_level(state->context, DMI_LOG_ERROR);

    return 0;
}

static int test_lint_teardown(void **pstate)
{
    test_lint_state_t *state = *pstate;

    if (state != nullptr) {
        dmi_destroy(state->context);
        free(state);
    }

    return 0;
}

static void test_lint_handler(void *data, const dmi_lint_issue_t *issue)
{
    test_lint_report_t *report = data;

    assert_non_null(issue->rule);
    assert_non_null(issue->message);

    report->total++;
    report->counts[issue->severity]++;
    report->rule     = issue->rule;
    report->severity = issue->severity;
}

//
// Load a dump and check it, so that every test starts with the data it needs.
//
static void test_lint_check(
        test_lint_state_t        *state,
        const char               *path,
        const dmi_lint_options_t *options,
        test_lint_report_t       *report)
{
    dmi_close(state->context);

    assert_true(dmi_load(state->context, path));
    assert_true(dmi_lint(state->context, options, test_lint_handler, report));
}

//
// Rule the check is limited to, so that the tests do not depend on the other
// rules, which the dumps of the test data may grow issues of over time.
//
static const dmi_lint_rule_t *test_lint_only_rule;

static bool test_lint_accept_only(void *data, const dmi_lint_rule_t *rule)
{
    dmi_unused(data);

    return rule == test_lint_only_rule;
}

//
// Check a dump against a single rule, and report how many issues it has found
// and how severe they are.
//
static size_t test_lint_count_rule(
        test_lint_state_t   *state,
        const char          *path,
        const char          *code,
        dmi_lint_profile_t   profile,
        dmi_lint_severity_t *severity)
{
    test_lint_only_rule = dmi_lint_rule_find(state->context, code);
    assert_non_null(test_lint_only_rule);

    const dmi_lint_options_t options =
    {
        .profile     = profile,
        .all         = true,
        .rule_filter = test_lint_accept_only
    };

    test_lint_report_t report = {};

    test_lint_check(state, path, &options, &report);

    if ((severity != nullptr) and (report.total > 0))
        *severity = report.severity;

    return report.total;
}

static void test_lint_rules(void **pstate)
{
    dmi_unused(pstate);

    const dmi_lint_rule_t *const *rules = dmi_lint_rules();
    assert_non_null(rules);

    size_t count = 0;

    for (const dmi_lint_rule_t *const *rule = rules; *rule != nullptr; rule++) {
        assert_non_null((*rule)->code);
        assert_non_null((*rule)->params.name);

        // Every rule is reachable by its code name, and the codes are unique
        assert_ptr_equal(dmi_lint_rule_find(nullptr, (*rule)->code), *rule);
        count++;
    }

    assert_true(count > 0);

    assert_null(dmi_lint_rule_find(nullptr, "no.such.rule"));
    assert_null(dmi_lint_rule_find(nullptr, nullptr));
    assert_null(dmi_lint_rule_name(nullptr));

    // Issues of the producer profile are never less severe than the ones of
    // the reader profile
    for (const dmi_lint_rule_t *const *rule = rules; *rule != nullptr; rule++) {
        assert_true(dmi_lint_rule_severity(*rule, DMI_LINT_PROFILE_PRODUCER) >=
                    dmi_lint_rule_severity(*rule, DMI_LINT_PROFILE_READER));
    }

    assert_int_equal(dmi_lint_rule_severity(nullptr, DMI_LINT_PROFILE_READER),
                     DMI_LINT_SEVERITY_NONE);
}

static void test_lint_clean_dump(void **pstate)
{
    test_lint_state_t *state = *pstate;
    test_lint_report_t report = {};

    const dmi_lint_options_t options = { .all = true };

    test_lint_check(state, test_clean_path, &options, &report);

    // Notes and warnings depend on the rules the library has, while an error
    // means the data is broken, and the dump is not
    assert_int_equal(report.counts[DMI_LINT_SEVERITY_ERROR], 0);
}

static void test_lint_trailing_data(void **pstate)
{
    test_lint_state_t *state = *pstate;
    dmi_lint_severity_t severity = DMI_LINT_SEVERITY_NONE;

    size_t count = test_lint_count_rule(state, test_trailing_path, "table.trailing-data",
                                        DMI_LINT_PROFILE_READER, &severity);

    assert_int_equal(count, 1);
    assert_int_equal(severity, DMI_LINT_SEVERITY_NOTE);
}

static void test_lint_profile(void **pstate)
{
    test_lint_state_t *state = *pstate;
    dmi_lint_severity_t severity = DMI_LINT_SEVERITY_NONE;

    // The same data is an error for the data being produced
    size_t count = test_lint_count_rule(state, test_trailing_path, "table.trailing-data",
                                        DMI_LINT_PROFILE_PRODUCER, &severity);

    assert_int_equal(count, 1);
    assert_int_equal(severity, DMI_LINT_SEVERITY_ERROR);
}

static bool test_lint_reject_all(void *data, const dmi_lint_rule_t *rule)
{
    dmi_unused(data);
    dmi_unused(rule);

    return false;
}

static void test_lint_rule_filter(void **pstate)
{
    test_lint_state_t *state = *pstate;

    const dmi_lint_options_t options =
    {
        .all         = true,
        .rule_filter = test_lint_reject_all
    };

    test_lint_report_t report = {};

    test_lint_check(state, test_trailing_path, &options, &report);

    // Filtering every rule out leaves nothing to report
    assert_int_equal(report.total, 0);
}

//
// Break the binary-coded decimal of an IPMI device structure, and check that
// the rule reading the raw data of the structures finds it.
//
static void test_lint_raw_data(void **pstate)
{
    test_lint_state_t *state = *pstate;

    size_t size = 0;
    dmi_data_t *data = dmi_file_get(state->context, test_ipmi_path, -1, &size);

    assert_non_null(data);

    // Structures follow the entry point, which is no longer than its maximum
    size_t offset = DMI_ENTRY_MAX_SIZE;
    bool broken = false;

    while ((offset + 4) < size) {
        dmi_byte_t type   = data[offset];
        dmi_byte_t length = data[offset + 1];

        if (type == DMI_TYPE_IPMI_DEVICE) {
            data[offset + test_ipmi_revision] = 0x1A;
            broken = true;
            break;
        }

        // Strings of a structure end with a pair of zeroes
        size_t end = offset + length;

        while (((end + 1) < size) and not ((data[end] == 0) and (data[end + 1] == 0)))
            end++;

        offset = end + 2;
    }

    assert_true(broken);

    FILE *file = fopen(test_broken_path, "wb");

    assert_non_null(file);
    assert_int_equal(fwrite(data, 1, size, file), size);
    assert_int_equal(fclose(file), 0);

    dmi_free(data);

    dmi_lint_severity_t severity = DMI_LINT_SEVERITY_NONE;
    size_t count = test_lint_count_rule(state, test_broken_path, "ipmi-device.revision",
                                        DMI_LINT_PROFILE_READER, &severity);

    remove(test_broken_path);

    assert_int_equal(count, 1);
    assert_int_equal(severity, DMI_LINT_SEVERITY_WARNING);
}

static void test_lint_closed_context(void **pstate)
{
    test_lint_state_t *state = *pstate;
    test_lint_report_t report = {};

    dmi_close(state->context);

    assert_false(dmi_lint(state->context, nullptr, test_lint_handler, &report));
    assert_int_equal(report.total, 0);

    dmi_error_t *error = dmi_error_peek_last(state->context);

    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_STATE);

    assert_false(dmi_lint(nullptr, nullptr, nullptr, nullptr));
}
