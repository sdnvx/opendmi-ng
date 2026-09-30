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
#include <opendmi/entry.h>
#include <opendmi/lint.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/module.h>
#include <opendmi/module/intel-rsd.h>
#include <opendmi/test/logger.h>

static int test_rsd_rules_setup(void **pstate);
static int test_rsd_rules_teardown(void **pstate);

static void test_rsd_rules_index(void **pstate);
static void test_rsd_rules_start_lane(void **pstate);
static void test_rsd_rules_cable_count(void **pstate);
static void test_rsd_rules_memory_handle(void **pstate);
static void test_rsd_rules_mapping_handle(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

static const char *test_dump_path = "rsd-rules-test.bin";

// Memory device information (type 17) of SMBIOS 2.1, with no strings
static const uint8_t test_memory_device[] = {
    17, 0x15, 0x11, 0x00,
    0x10, 0x00, 0xFE, 0xFF,
    0x40, 0x00, 0x40, 0x00,
    0x00, 0x40,
    0x09, 0x00,
    0x00, 0x00,
    0x1A, 0x80, 0x00,
    0, 0
};

// Intel RSD TPM information, whose handle a reference of another type is
// pointed at
static const uint8_t test_tpm[] = {
    195, 0x07, 0x12, 0x00,
    0x01, 0x01, 0x01,
    'T', 'P', 'M', ' ', '2', '.', '0', 0,
    0
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_rsd_rules_index),
        cmocka_unit_test(test_rsd_rules_start_lane),
        cmocka_unit_test(test_rsd_rules_cable_count),
        cmocka_unit_test(test_rsd_rules_memory_handle),
        cmocka_unit_test(test_rsd_rules_mapping_handle)
    };

    return cmocka_run_group_tests(tests, test_rsd_rules_setup, test_rsd_rules_teardown);
}

static int test_rsd_rules_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_add_extension(context, dmi_module_find("intel-rsd"))) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_rsd_rules_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

//
// Rule the check is limited to, so that the other rules, which the minimal
// tables of the tests break in plenty of ways, do not count.
//
static const dmi_lint_rule_t *test_only_rule;

static bool test_accept_only(void *data, const dmi_lint_rule_t *rule)
{
    dmi_unused(data);

    return rule == test_only_rule;
}

static void test_count_issue(void *data, const dmi_lint_issue_t *issue)
{
    dmi_unused(issue);

    (*(size_t *)data)++;
}

//
// Write a dump of an SMBIOS 3.0 table holding the given structures, which
// already end with their strings, and the end-of-table structure, and check it
// against a single rule.
//
static size_t test_lint(dmi_context_t *context, const char *code, const uint8_t *const *structures,
                        const size_t *sizes, size_t count)
{
    static const uint8_t end_of_table[] = { 127, 0x04, 0xFF, 0xFE, 0, 0 };

    uint8_t data[1024] = {};
    size_t  length = DMI_ENTRY_MAX_SIZE;

    for (size_t i = 0; i < count; i++) {
        assert_true(length + sizes[i] <= sizeof(data));
        memcpy(data + length, structures[i], sizes[i]);
        length += sizes[i];
    }

    memcpy(data + length, end_of_table, sizeof(end_of_table));
    length += sizeof(end_of_table);

    // Entry point of SMBIOS 3.0, with the table right after it
    size_t table_length = length - DMI_ENTRY_MAX_SIZE;

    memcpy(data, "_SM3_", 5);
    data[0x06] = 0x18;
    data[0x07] = 3;
    data[0x0A] = 0x01;
    data[0x0C] = (uint8_t)table_length;
    data[0x0D] = (uint8_t)(table_length >> 8);
    data[0x10] = DMI_ENTRY_MAX_SIZE;

    uint8_t sum = 0;
    for (size_t i = 0; i < 0x18; i++)
        sum += data[i];
    data[0x05] = (uint8_t)(0x100 - sum);

    FILE *file = fopen(test_dump_path, "wb");

    if (file != nullptr) {
        size_t written = fwrite(data, 1, length, file);
        int rv = fclose(file);

        assert_int_equal(written, length);
        assert_int_equal(rv, 0);
    } else {
        fail_msg("Unable to create file %s", test_dump_path);
    }

    dmi_close(context);
    assert_true(dmi_load(context, test_dump_path));
    remove(test_dump_path);

    test_only_rule = dmi_lint_rule_find(context, code);
    assert_non_null(test_only_rule);

    const dmi_lint_options_t options = {
        .profile     = DMI_LINT_PROFILE_READER,
        .all         = true,
        .rule_filter = test_accept_only
    };

    size_t issues = 0;
    assert_true(dmi_lint(context, &options, test_count_issue, &issues));

    return issues;
}

static size_t test_lint_one(dmi_context_t *context, const char *code, const uint8_t *data, size_t size)
{
    return test_lint(context, code, (const uint8_t *const[]){ data }, (const size_t[]){ size }, 1);
}

//
// Indices of TPM configurations and of FPGAs start at one.
//
static void test_rsd_rules_index(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t tpm[sizeof(test_tpm)];
    memcpy(tpm, test_tpm, sizeof(tpm));

    assert_int_equal(test_lint_one(context, "value.range", tpm, sizeof(tpm)), 0);

    tpm[0x04] = 0;
    assert_int_equal(test_lint_one(context, "value.range", tpm, sizeof(tpm)), 1);

    uint8_t fpga[0x24 + 2] = { 198, 0x24, 0x13, 0x00, 0x01 };

    assert_int_equal(test_lint_one(context, "value.range", fpga, sizeof(fpga)), 0);

    fpga[0x04] = 0;
    assert_int_equal(test_lint_one(context, "value.range", fpga, sizeof(fpga)), 1);
}

//
// Cable indices start at the lanes 0, 4, 8 and 12 only.
//
static void test_rsd_rules_start_lane(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const struct {
        uint8_t first;
        uint8_t second;
        size_t  issues;
    } test_cases[] = {
        { 0,  4,  0 },
        { 8,  12, 0 },
        { 0,  6,  1 },
        { 16, 2,  2 }
    };

    for (size_t i = 0; i < countof(test_cases); i++) {
        uint8_t data[] = {
            199, 0x0C, 0x14, 0x00,
            0x02, 0x00, 0x08, 0x02,
            0x00, test_cases[i].first,
            0x01, test_cases[i].second,
            0, 0
        };

        assert_int_equal(test_lint_one(context, "intel-rsd-cabled-pcie.start-lane", data, sizeof(data)),
                         test_cases[i].issues);
    }
}

//
// Ports have four cable indices at most.
//
static void test_rsd_rules_cable_count(void **pstate)
{
    dmi_context_t *context = *pstate;

    for (uint8_t count = 4; count <= 5; count++) {
        uint8_t data[0x08 + 2 * 5 + 2] = { 199, (uint8_t)(0x08 + 2 * count), 0x15, 0x00, 0x02, 0x00, 0x10, count };

        for (uint8_t i = 0; i < count; i++) {
            data[0x08 + 2 * i]     = i;
            data[0x08 + 2 * i + 1] = (uint8_t)(4 * (i % 4));
        }

        size_t size = 0x08 + 2 * count + 2;

        assert_int_equal(test_lint_one(context, "intel-rsd-cabled-pcie.cable-count", data, size),
                         count - 4);
    }
}

//
// Memory device extended information refers to a memory device.
//
static void test_rsd_rules_memory_handle(void **pstate)
{
    dmi_context_t *context = *pstate;

    for (uint8_t target = 0x11; target <= 0x12; target++) {
        const uint8_t extended[] = {
            197, 0x0F, 0x16, 0x00,
            target, 0x00, 0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0xA0,
            0, 0
        };

        const uint8_t *structures[] = { test_memory_device, test_tpm, extended };
        const size_t   sizes[]      = { sizeof(test_memory_device), sizeof(test_tpm), sizeof(extended) };

        assert_int_equal(test_lint(context, "link.wrong-type", structures, sizes, countof(sizes)),
                         (target == 0x11) ? 0 : 1);
    }
}

//
// Physical device mappings refer to the structures of the type their device
// type names.
//
static void test_rsd_rules_mapping_handle(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const struct {
        uint8_t device_type;
        uint8_t target;
        size_t  issues;
    } test_cases[] = {
        { 0x03, 0x11, 0 },  // Memory device
        { 0x03, 0x12, 1 },  // Memory device mapped to a TPM
        { 0x01, 0x11, 1 },  // Processor mapped to a memory device
        { 0x02, 0x11, 1 },  // System slot mapped to a memory device
        { 0x07, 0x12, 0 }   // Unknown device type refers to anything
    };

    for (size_t i = 0; i < countof(test_cases); i++) {
        const uint8_t mapping[] = {
            200, 0x0A, 0x17, 0x00,
            test_cases[i].device_type, 0x00,
            test_cases[i].target, 0x00, 0x00, 0x00,
            0, 0
        };

        const uint8_t *structures[] = { test_memory_device, test_tpm, mapping };
        const size_t   sizes[]      = { sizeof(test_memory_device), sizeof(test_tpm), sizeof(mapping) };

        assert_int_equal(test_lint(context, "link.wrong-type", structures, sizes, countof(sizes)),
                         test_cases[i].issues);
    }
}
