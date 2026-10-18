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

#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/base64.h>

typedef struct test_vector test_vector_t;

struct test_vector
{
    dmi_data_t  decoded[6];
    size_t      length;
    const char *encoded;
};

static void test_base64_encode(void **pstate);
static void test_base64_decode(void **pstate);
static void test_base64_roundtrip(void **pstate);
static void test_base64_arguments(void **pstate);

// Vectors of RFC 4648, which end with two, one and no padding characters,
// and the bytes which take the first and the last characters of the alphabet
static const test_vector_t test_data[] =
{
    { { 'f' },                          1, "Zg=="     },
    { { 'f', 'o' },                     2, "Zm8="     },
    { { 'f', 'o', 'o' },                3, "Zm9v"     },
    { { 'f', 'o', 'o', 'b' },           4, "Zm9vYg==" },
    { { 'f', 'o', 'o', 'b', 'a' },      5, "Zm9vYmE=" },
    { { 'f', 'o', 'o', 'b', 'a', 'r' }, 6, "Zm9vYmFy" },
    { { 0x00 },                         1, "AA=="     },
    { { 0xFB, 0xFF, 0xBF },             3, "+/+/"     }
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_base64_encode),
        cmocka_unit_test(test_base64_decode),
        cmocka_unit_test(test_base64_roundtrip),
        cmocka_unit_test(test_base64_arguments)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_base64_encode(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < countof(test_data); i++) {
        size_t length = 0;

        char *text = dmi_base64_encode(test_data[i].decoded, test_data[i].length, &length);

        assert_non_null(text);
        assert_string_equal(text, test_data[i].encoded);
        assert_int_equal(length, strlen(test_data[i].encoded));

        dmi_free(text);
    }

    // Length is optional
    char *text = dmi_base64_encode(test_data[0].decoded, test_data[0].length, nullptr);

    assert_non_null(text);
    assert_string_equal(text, test_data[0].encoded);

    dmi_free(text);
}

static void test_base64_decode(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < countof(test_data); i++) {
        const char *text = test_data[i].encoded;
        size_t length = 0;

        dmi_data_t *data = dmi_base64_decode(text, strlen(text), &length);

        assert_non_null(data);
        assert_int_equal(length, test_data[i].length);
        assert_memory_equal(data, test_data[i].decoded, length);

        dmi_free(data);
    }

    // Length is optional
    const char *text = test_data[0].encoded;
    dmi_data_t *data = dmi_base64_decode(text, strlen(text), nullptr);

    assert_non_null(data);
    assert_memory_equal(data, test_data[0].decoded, test_data[0].length);

    dmi_free(data);

    // Text is made of whole groups of four characters, so the one with a
    // group cut short, or with the padding left out, is rejected
    size_t length = SIZE_MAX;

    assert_null(dmi_base64_decode("Zm9vY", 5, &length));
    assert_null(dmi_base64_decode("Zm9vYg", 6, &length));
    assert_null(dmi_base64_decode("Zm8", 3, &length));

    // Characters are of the alphabet of RFC 4648 alone, which neither the
    // ones of its variant for file names nor the white space are
    assert_null(dmi_base64_decode("Zm9*", 4, &length));
    assert_null(dmi_base64_decode("Zm9v\n+/8", 8, &length));
    assert_null(dmi_base64_decode("-_-_", 4, &length));
    assert_null(dmi_base64_decode("Zm9v\0m9v", 8, &length));

    // Padding stands for the bytes the last group is short of, so it is found
    // at the end of the text alone, and takes two characters at most
    assert_null(dmi_base64_decode("Zg==Zg==", 8, &length));
    assert_null(dmi_base64_decode("Zm=v", 4, &length));
    assert_null(dmi_base64_decode("=m9v", 4, &length));
    assert_null(dmi_base64_decode("Z===", 4, &length));
    assert_null(dmi_base64_decode("====", 4, &length));

    assert_uint_equal(length, SIZE_MAX);
}

static void test_base64_roundtrip(void **pstate)
{
    dmi_unused(pstate);

    // Every value of a byte is at every position of a group of three bytes
    dmi_data_t data[3 * (UINT8_MAX + 1)];

    for (size_t i = 0; i < sizeof(data); i++)
        data[i] = (dmi_data_t)i;

    // Data ends at every position of a group as well
    for (size_t length = sizeof(data) - 2; length <= sizeof(data); length++) {
        size_t text_length = 0;
        size_t data_length = 0;

        char *text = dmi_base64_encode(data, length, &text_length);

        assert_non_null(text);
        assert_int_equal(text_length, strlen(text));

        dmi_data_t *result = dmi_base64_decode(text, text_length, &data_length);

        assert_non_null(result);
        assert_int_equal(data_length, length);
        assert_memory_equal(result, data, length);

        dmi_free(result);
        dmi_free(text);
    }
}

static void test_base64_arguments(void **pstate)
{
    dmi_unused(pstate);

    const dmi_data_t data[] = { 'f', 'o', 'o' };
    const char *text = "Zm9v";

    // Length is left as it is, since nothing is encoded or decoded
    size_t length = SIZE_MAX;

    assert_null(dmi_base64_encode(nullptr, sizeof(data), &length));
    assert_null(dmi_base64_decode(nullptr, strlen(text), &length));

    // Empty data is not encoded, nor is it decoded from an empty text
    assert_null(dmi_base64_encode(data, 0, &length));
    assert_null(dmi_base64_decode(text, 0, &length));

    assert_uint_equal(length, SIZE_MAX);
}
