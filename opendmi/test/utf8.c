//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/utf8.h>
#include <opendmi/test/helpers.h>

static void test_utf8_is_valid(void **pstate);
static void test_utf8_repair(void **pstate);
static void test_utf8_filter(void **pstate);

static bool test_utf8_is_printable(uint32_t code);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_utf8_is_valid),
        cmocka_unit_test(test_utf8_repair),
        cmocka_unit_test(test_utf8_filter)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_utf8_is_valid(void **pstate)
{
    dmi_unused(pstate);

    // Valid strings
    assert_true(dmi_utf8_is_valid(""));
    assert_true(dmi_utf8_is_valid("ASCII"));
    assert_true(dmi_utf8_is_valid("\xC2\xA9"));             // U+00A9
    assert_true(dmi_utf8_is_valid("\xE2\x82\xAC"));         // U+20AC
    assert_true(dmi_utf8_is_valid("\xED\x9F\xBF"));         // U+D7FF
    assert_true(dmi_utf8_is_valid("\xEF\xBF\xBD"));         // U+FFFD
    assert_true(dmi_utf8_is_valid("\xF0\x9F\x98\x80"));     // U+1F600
    assert_true(dmi_utf8_is_valid("\xF4\x8F\xBF\xBF"));     // U+10FFFF

    // Invalid bytes
    assert_false(dmi_utf8_is_valid("\xFF"));
    assert_false(dmi_utf8_is_valid("A\x80"));
    assert_false(dmi_utf8_is_valid("\xF8\x88\x80\x80\x80"));

    // Truncated sequences
    assert_false(dmi_utf8_is_valid("\xC2"));
    assert_false(dmi_utf8_is_valid("\xE2\x82"));
    assert_false(dmi_utf8_is_valid("\xF0\x9F\x98"));
    assert_false(dmi_utf8_is_valid("\xE2\x82" "A"));

    // Overlong encodings
    assert_false(dmi_utf8_is_valid("\xC0\x80"));
    assert_false(dmi_utf8_is_valid("\xC1\xBF"));
    assert_false(dmi_utf8_is_valid("\xE0\x9F\xBF"));
    assert_false(dmi_utf8_is_valid("\xF0\x8F\xBF\xBF"));

    // Surrogates and out of range code points
    assert_false(dmi_utf8_is_valid("\xED\xA0\x80"));
    assert_false(dmi_utf8_is_valid("\xED\xBF\xBF"));
    assert_false(dmi_utf8_is_valid("\xF4\x90\x80\x80"));
}

static void test_utf8_repair(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        const char *input;
        const char *output;
    } cases[] = {
        { "",                   ""                                   },
        { "ASCII \xC2\xA9",     "ASCII \xC2\xA9"                     },
        { "\xFF",               "\xEF\xBF\xBD"                       },
        { "A\xFF" "B",          "A\xEF\xBF\xBD" "B"                  },
        { "\xE2\x82" "A",       "\xEF\xBF\xBD\xEF\xBF\xBD" "A"       },
        { "\xC0\x80",           "\xEF\xBF\xBD\xEF\xBF\xBD"           },
        { "\xE2\x82\xAC\xFF",   "\xE2\x82\xAC\xEF\xBF\xBD"           }
    };

    for (size_t i = 0; i < countof(cases); i++) {
        char *output = dmi_utf8_repair(nullptr, cases[i].input);
        assert_non_null(output);

        bool valid = dmi_utf8_is_valid(output);
        int  diff  = strcmp(output, cases[i].output);

        dmi_free(output);

        assert_true(valid);
        assert_int_equal(diff, 0);
    }
}

static void test_utf8_filter(void **pstate)
{
    dmi_unused(pstate);

    // Filter is applied to valid characters only
    assert_true(dmi_utf8_is_valid_ex("A\xC2\xA9", test_utf8_is_printable));
    assert_false(dmi_utf8_is_valid_ex("A\x01", test_utf8_is_printable));
    assert_false(dmi_utf8_is_valid_ex("A\xC2\x80", test_utf8_is_printable));
    assert_false(dmi_utf8_is_valid_ex("A\xFF", test_utf8_is_printable));

    static const struct {
        const char *input;
        const char *output;
    } cases[] = {
        { "A\x01" "B",          "A\xEF\xBF\xBD" "B"                    },
        // Rejected multibyte character is replaced as a whole
        { "A\xC2\x80" "B",      "A\xEF\xBF\xBD" "B"                    },
        { "\x01\xFF",           "\xEF\xBF\xBD\xEF\xBF\xBD"             },
        { "\xE2\x82\xAC",       "\xE2\x82\xAC"                          }
    };

    for (size_t i = 0; i < countof(cases); i++) {
        char *output = dmi_utf8_repair_ex(nullptr, cases[i].input, test_utf8_is_printable);
        assert_non_null(output);

        bool valid = dmi_utf8_is_valid_ex(output, test_utf8_is_printable);
        int  diff  = strcmp(output, cases[i].output);

        dmi_free(output);

        assert_true(valid);
        assert_int_equal(diff, 0);
    }
}

// Reject C0 and C1 control characters
static bool test_utf8_is_printable(uint32_t code)
{
    return (code >= 0x20u) and not ((code >= 0x7Fu) and (code < 0xA0u));
}
