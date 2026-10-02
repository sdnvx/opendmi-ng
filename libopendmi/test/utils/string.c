//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>
#include <cmocka.h>

#include <opendmi/internal.h>
#include <opendmi/utils/string.h>

static void test_string_tolower(void **pstate);
static void test_string_toupper(void **pstate);
static void test_string_case_non_ascii(void **pstate);
static void test_asprintf_failure(void **pstate);
static void test_string_set(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_string_tolower),
        cmocka_unit_test(test_string_toupper),
        cmocka_unit_test(test_string_case_non_ascii),
        cmocka_unit_test(test_asprintf_failure),
        cmocka_unit_test(test_string_set)
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

static void test_string_case_non_ascii(void **pstate)
{
    dmi_unused(pstate);

    // Bytes above 0x7F are negative as plain characters, and are left as
    // they are by the C locale
    char lower[] = "\xC3\x84\xFF" "A";
    char upper[] = "\xC3\xA4\xFF" "a";

    dmi_string_tolower(lower);
    dmi_string_toupper(upper);

    assert_string_equal(lower, "\xC3\x84\xFF" "a");
    assert_string_equal(upper, "\xC3\xA4\xFF" "A");
}

static void test_asprintf_failure(void **pstate)
{
    dmi_unused(pstate);

#if defined(_WIN32)
    // Conversion of wide characters is not known to fail the same way there
    skip();
#endif

    // Wide character which the C locale cannot represent makes formatting
    // fail, and the result is not left undefined then
    char *string = (char *)&string;

    assert_true(dmi_asprintf(&string, "%ls", L"\x1234") < 0);
    assert_null(string);

    // Result is not left undefined without a format either
    const char *volatile format = nullptr;

    string = (char *)&string;
    assert_true(dmi_asprintf(&string, format) < 0);
    assert_null(string);
}

static void test_string_set(void **pstate)
{
    dmi_unused(pstate);

    char *string = nullptr;
    char value[] = "value";

    // Value is copied
    assert_true(dmi_string_set(nullptr, &string, value));
    assert_non_null(string);
    assert_ptr_not_equal(string, value);
    assert_string_equal(string, "value");

    // Previous string is replaced
    assert_true(dmi_string_set(nullptr, &string, "other"));
    assert_string_equal(string, "other");

    // String is freed by nullptr
    assert_true(dmi_string_set(nullptr, &string, nullptr));
    assert_null(string);
}
