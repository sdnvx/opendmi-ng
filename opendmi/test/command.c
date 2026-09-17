//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cmocka.h>

#include <opendmi/command.h>
#include <opendmi/test/helpers.h>

static int test_command_setup(void **pstate);

static void test_command_names(void **pstate);
static void test_command_find(void **pstate);
static void test_command_global_options(void **pstate);
static void test_command_options(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_command_names),
        cmocka_unit_test(test_command_find),
        cmocka_unit_test(test_command_global_options),
        cmocka_unit_test(test_command_options)
    };

    return cmocka_run_group_tests(tests, test_command_setup, nullptr);
}

static int test_command_setup(void **pstate)
{
    dmi_unused(pstate);

    dmi_command_init("opendmi-command-test");

    return 0;
}

static void test_command_names(void **pstate)
{
    dmi_unused(pstate);

    for (const dmi_command_t **pcommand = dmi_commands; *pcommand != nullptr; pcommand++) {
        assert_non_null((*pcommand)->name);
        assert_non_null((*pcommand)->description);
        assert_non_null((*pcommand)->handlers.usage);
        assert_non_null((*pcommand)->handlers.main);

        for (const dmi_command_t **pnext = pcommand + 1; *pnext != nullptr; pnext++) {
            if (strcmp((*pcommand)->name, (*pnext)->name) == 0)
                fail_msg("Duplicate command: %s", (*pcommand)->name);
        }
    }
}

static void test_command_find(void **pstate)
{
    dmi_unused(pstate);

    for (const dmi_command_t **pcommand = dmi_commands; *pcommand != nullptr; pcommand++)
        assert_ptr_equal(dmi_command_find((*pcommand)->name), *pcommand);

    assert_null(dmi_command_find("unknown"));
}

static void test_command_global_options(void **pstate)
{
    dmi_unused(pstate);

    assert_int_equal(dmi_test_option_duplicates(dmi_options(&dmi_global_options)), 0);
}

static void test_command_options(void **pstate)
{
    dmi_unused(pstate);

    // Command options are parsed separately from global ones, so only option
    // sets of the same command must not have duplicate names
    for (const dmi_command_t **pcommand = dmi_commands; *pcommand != nullptr; pcommand++) {
        const dmi_command_t *command = *pcommand;

        if (command->options == nullptr)
            continue;

        if (dmi_test_option_duplicates(command->options) > 0)
            fail_msg("Command %s has duplicate options", command->name);
    }
}
