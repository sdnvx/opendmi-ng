//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/internal.h>
#include <opendmi/utils/string.h>

static void test_string_tolower(void **pstate);
static void test_string_toupper(void **pstate);
static void test_string_case_non_ascii(void **pstate);
static void test_asprintf_failure(void **pstate);
static void test_string_set(void **pstate);
static void test_string_from_bytes(void **pstate);
static void test_string_set_bytes(void **pstate);
static void test_string_arguments(void **pstate);

static int test_vasprintf(char **strp, const char *format, ...);

// Signature padded with spaces, and the bytes which spell no text
static const uint8_t test_bytes[]  = { ' ', '_', 'S', 'M', '_', ' ' };
static const uint8_t test_binary[] = { '_', 'S', 0x01, '_' };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_string_tolower),
        cmocka_unit_test(test_string_toupper),
        cmocka_unit_test(test_string_case_non_ascii),
        cmocka_unit_test(test_asprintf_failure),
        cmocka_unit_test(test_string_set),
        cmocka_unit_test(test_string_from_bytes),
        cmocka_unit_test(test_string_set_bytes),
        cmocka_unit_test(test_string_arguments)
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

static void test_string_from_bytes(void **pstate)
{
    dmi_unused(pstate);

    char buffer[sizeof(test_bytes) + 1];

    // Printable bytes are copied as they are, and the text is terminated
    memset(buffer, 'x', sizeof(buffer));
    assert_ptr_equal(dmi_string_from_bytes(test_bytes, sizeof(test_bytes), buffer, false), buffer);
    assert_string_equal(buffer, " _SM_ ");

    // Spaces are left out on request
    memset(buffer, 'x', sizeof(buffer));
    assert_ptr_equal(dmi_string_from_bytes(test_bytes, sizeof(test_bytes), buffer, true), buffer);
    assert_string_equal(buffer, "_SM_");

    // Spaces alone leave no text once they are left out
    const uint8_t spaces[] = { ' ', ' ' };

    assert_null(dmi_string_from_bytes(spaces, sizeof(spaces), buffer, true));

    // Bytes which are not all printable spell no text
    assert_null(dmi_string_from_bytes(test_binary, sizeof(test_binary), buffer, false));

    // No bytes spell no text either
    assert_null(dmi_string_from_bytes(test_bytes, 0, buffer, false));
}

static void test_string_set_bytes(void **pstate)
{
    dmi_unused(pstate);

    char *string = nullptr;

    // Printable bytes become a string
    assert_true(dmi_string_set_bytes(nullptr, &string, test_bytes, sizeof(test_bytes), false));
    assert_non_null(string);
    assert_string_equal(string, " _SM_ ");

    // Previous string is replaced, and spaces are left out on request
    assert_true(dmi_string_set_bytes(nullptr, &string, test_bytes, sizeof(test_bytes), true));
    assert_non_null(string);
    assert_string_equal(string, "_SM_");

    // Bytes which are not all printable leave no string, which is not a
    // failure
    assert_true(dmi_string_set_bytes(nullptr, &string, test_binary, sizeof(test_binary), false));
    assert_null(string);

    // Longest text is taken as a whole
    uint8_t data[DMI_STRING_BYTES_MAX];

    memset(data, 'A', sizeof(data));
    assert_true(dmi_string_set_bytes(nullptr, &string, data, sizeof(data), false));
    assert_non_null(string);
    assert_int_equal(strlen(string), sizeof(data));

    // No bytes leave no string either
    assert_true(dmi_string_set_bytes(nullptr, &string, data, 0, false));
    assert_null(string);
}

static void test_string_arguments(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);

    // Formatted string has nowhere to be stored
    assert_int_equal(dmi_asprintf(nullptr, "text"), -1);
    assert_int_equal(test_vasprintf(nullptr, "text"), -1);

    // Result is not left undefined without a format
    const char *volatile format = nullptr;
    char *result = (char *)&result;

    assert_int_equal(test_vasprintf(&result, format), -1);
    assert_null(result);

    // Nothing is converted without a string
    dmi_string_tolower(nullptr);
    dmi_string_toupper(nullptr);

    dmi_error_clear(context);
    assert_false(dmi_string_set(context, nullptr, "value"));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_ARGUMENT_NULL);

    assert_false(dmi_string_is_placeholder(nullptr));

    // Bytes longer than the longest text are rejected, and the string is kept
    uint8_t data[DMI_STRING_BYTES_MAX + 1];
    char   *string = nullptr;

    memset(data, 'A', sizeof(data));
    assert_true(dmi_string_set(context, &string, "value"));

    dmi_error_clear(context);
    assert_false(dmi_string_set_bytes(context, &string, data, sizeof(data), false));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_ARGUMENT_INVALID);
    assert_string_equal(string, "value");

    assert_true(dmi_string_set(context, &string, nullptr));

    dmi_error_clear(context);
    assert_false(dmi_string_set_bytes(context, nullptr, test_bytes, sizeof(test_bytes), false));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_ARGUMENT_NULL);

    // Bytes which are not there spell nothing to replace the string with,
    // unlike no bytes at all, which leave no string
    assert_true(dmi_string_set(context, &string, "value"));

    dmi_error_clear(context);
    assert_false(dmi_string_set_bytes(context, &string, nullptr, sizeof(test_bytes), false));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_ARGUMENT_NULL);
    assert_string_equal(string, "value");

    assert_true(dmi_string_set_bytes(context, &string, nullptr, 0, false));
    assert_null(string);

    // Buffer is left as it is without the bytes, and is required itself
    char buffer[sizeof(test_bytes) + 1] = "text";

    assert_null(dmi_string_from_bytes(nullptr, sizeof(test_bytes), buffer, false));
    assert_string_equal(buffer, "text");
    assert_null(dmi_string_from_bytes(test_bytes, sizeof(test_bytes), nullptr, false));

    dmi_destroy(context);
}

static int test_vasprintf(char **strp, const char *format, ...)
{
    int rv;
    va_list args;

    va_start(args, format);
    rv = dmi_vasprintf(strp, format, args);
    va_end(args);

    return rv;
}
