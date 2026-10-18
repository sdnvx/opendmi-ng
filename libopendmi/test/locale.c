//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cmocka.h>

#include <opendmi/internal.h>
#include <opendmi/locale.h>
#include <opendmi/utils.h>

static void test_message_text(void **pstate);
static void test_message_number(void **pstate);
static void test_message_brace(void **pstate);
static void test_message_invalid(void **pstate);
static void test_locale_name_copied(void **pstate);
static void test_locale_arguments(void **pstate);
static void test_locale_library(void **pstate);
static void test_resource_arguments(void **pstate);

#ifdef ENABLE_ICU
// Resource data of the library, which the static library does not hide
extern const char libopendmi_dat[];
#endif

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_message_text),
        cmocka_unit_test(test_message_number),
        cmocka_unit_test(test_message_brace),
        cmocka_unit_test(test_message_invalid),
        cmocka_unit_test(test_locale_name_copied),
        cmocka_unit_test(test_locale_arguments),
        cmocka_unit_test(test_locale_library),
        cmocka_unit_test(test_resource_arguments)
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

static void test_locale_name_copied(void **pstate)
{
    dmi_unused(pstate);

    char name[] = "ru_RU";

    // Locale is set only by the library built with ICU4C support
    if (not dmi_set_locale(name))
        skip();

    // Name is kept by the library, not by the caller
    memset(name, 'x', sizeof(name) - 1);

    const char *locale = dmi_get_locale();
    assert_non_null(locale);
    assert_true(strncmp(locale, "ru", 2) == 0);

    assert_true(dmi_set_locale(nullptr));
}

static void test_locale_arguments(void **pstate)
{
    dmi_unused(pstate);

    // Data is not looked into without the name of its package
    const char data[] = "";

    // Library built without ICU4C support opens no resources and finds no
    // strings whatever it is given, so the results are the same there
    assert_null(dmi_resource_open(nullptr, data));
    assert_null(dmi_resource_open("test", nullptr));

    // No resource holds no strings, which is not an error. The table and the
    // key are checked only once there is a resource to look in.
    assert_null(dmi_resource_string(nullptr, "value", "unknown-error"));

    assert_null(dmi_locale_string(nullptr, "unknown-error"));
    assert_null(dmi_locale_string("value", nullptr));
}

//
// Library has translations for some locales only, and its resources are
// opened on the first lookup, so they are looked for again once the locale
// has changed, and their absence is a failure of neither the lookup nor the
// change.
//
static void test_locale_library(void **pstate)
{
    dmi_unused(pstate);

    // Locale is set only by the library built with ICU4C support
    if (not dmi_set_locale("en_US"))
        skip();

    assert_null(dmi_locale_string("access-method", "oem-specific"));

    assert_true(dmi_set_locale("ru_RU"));
    assert_non_null(dmi_locale_string("access-method", "oem-specific"));

    // Strings are dropped along with the locale they are of
    assert_true(dmi_set_locale("en_US"));
    assert_null(dmi_locale_string("access-method", "oem-specific"));

    assert_true(dmi_set_locale(nullptr));
}

static void test_resource_arguments(void **pstate)
{
    dmi_unused(pstate);

#ifdef ENABLE_ICU
    // Package of the library has no root bundle, so it is opened with the
    // locale it has a bundle for
    assert_true(dmi_set_locale("ru_RU"));

    dmi_resource_t *resource = dmi_resource_open("libopendmi", libopendmi_dat);
    assert_non_null(resource);

    // Table and the key are required once there is a resource to look in
    assert_non_null(dmi_resource_string(resource, "access-method", "oem-specific"));
    assert_null(dmi_resource_string(resource, nullptr, "oem-specific"));
    assert_null(dmi_resource_string(resource, "access-method", nullptr));

    // Resource is closed before the locale is put back, since it has no
    // bundle to be reopened with for the one of the environment
    dmi_resource_close(resource);

    assert_true(dmi_set_locale(nullptr));
#else
    // Resources are opened only by the library built with ICU4C support
    skip();
#endif
}
