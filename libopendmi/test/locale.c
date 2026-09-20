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
#include <opendmi/locale.h>
#include <opendmi/utils.h>

static void test_message_text(void **pstate);
static void test_message_number(void **pstate);
static void test_message_brace(void **pstate);
static void test_message_invalid(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_message_text),
        cmocka_unit_test(test_message_number),
        cmocka_unit_test(test_message_brace),
        cmocka_unit_test(test_message_invalid)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void assert_message(const char *pattern, const dmi_message_arg_t *args, size_t count, const char *result)
{
    char *text = dmi_message_format(nullptr, pattern, args, count);

    if (result == nullptr) {
        assert_null(text);
        return;
    }

    assert_non_null(text);
    assert_string_equal(text, result);

    dmi_free(text);
}

static void test_message_text(void **pstate)
{
    dmi_unused(pstate);

    const dmi_message_arg_t args[] = {
        DMI_MESSAGE_TEXT("Insyde Corp."),
        DMI_MESSAGE_TEXT("3.0")
    };

    assert_message("SMBIOS vendor: {0}", args, countof(args), "SMBIOS vendor: Insyde Corp.");
    assert_message("{1}, {0}", args, countof(args), "3.0, Insyde Corp.");
    assert_message("{0}{1}", args, countof(args), "Insyde Corp.3.0");
    assert_message("no arguments", args, countof(args), "no arguments");
    assert_message("", args, countof(args), "");
}

static void test_message_number(void **pstate)
{
    dmi_unused(pstate);

    const dmi_message_arg_t args[] = {
        DMI_MESSAGE_NUMBER(31),
        DMI_MESSAGE_NUMBER(-1)
    };

    assert_message("{0} structures", args, countof(args), "31 structures");
    assert_message("{1}", args, countof(args), "-1");
}

static void test_message_brace(void **pstate)
{
    dmi_unused(pstate);

    const dmi_message_arg_t args[] = { DMI_MESSAGE_NUMBER(7) };

    assert_message("{{0}", args, countof(args), "{0}");
    assert_message("{{{0}", args, countof(args), "{7");
}

static void test_message_invalid(void **pstate)
{
    dmi_unused(pstate);

    const dmi_message_arg_t args[] = { DMI_MESSAGE_TEXT("text") };

    // Placeholder without an argument, or one which is not closed
    assert_message("{1}", args, countof(args), nullptr);
    assert_message("{0", args, countof(args), nullptr);
    assert_message("{x}", args, countof(args), nullptr);

    // Plural forms need resources, which the test has none of
    assert_message("{0:byte}", args, countof(args), nullptr);

    assert_message(nullptr, args, countof(args), nullptr);
}
