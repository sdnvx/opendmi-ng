//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/internal.h>
#include <opendmi/utils/string.h>

static void test_string_tolower(void **pstate);
static void test_string_toupper(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_string_tolower),
        cmocka_unit_test(test_string_toupper)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_string_tolower(void **pstate)
{
    dmi_unused(pstate);

    char string[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const char *result = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789";

    dmi_string_tolower(string);

    assert_string_equal(string, result);
}

static void test_string_toupper(void **pstate)
{
    dmi_unused(pstate);

    char string[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const char *result = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    dmi_string_toupper(string);

    assert_string_equal(string, result);
}
