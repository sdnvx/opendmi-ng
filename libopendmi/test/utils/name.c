//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/utils/name.h>
#include <opendmi/internal.h>

static void test_name_lookup(void **pstate);
static void test_name_lookup_ex(void **pstate);
static void test_name_lookup_rev(void **pstate);

// Exact entry within a range takes precedence over the range
static const dmi_name_set_t test_names =
{
    .code  = "test-names",
    .names = (const dmi_name_t[]){
        { 1,    "first",  "First"  },
        { 0x81, "inside", "Inside" },
        DMI_NAME_NULL
    },
    .ranges = (const dmi_name_range_t[]){
        { 0x80, 0xFF, "range", "Range" },
        DMI_NAME_RANGE_NULL
    }
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_name_lookup),
        cmocka_unit_test(test_name_lookup_ex),
        cmocka_unit_test(test_name_lookup_rev)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_name_lookup(void **pstate)
{
    dmi_unused(pstate);

    assert_string_equal(dmi_code_lookup(&test_names, 1), "first");
    assert_string_equal(dmi_name_lookup(&test_names, 1), "First");

    assert_string_equal(dmi_code_lookup(&test_names, 0x81), "inside");
    assert_string_equal(dmi_name_lookup(&test_names, 0x81), "Inside");

    assert_string_equal(dmi_code_lookup(&test_names, 0x80), "range");
    assert_string_equal(dmi_name_lookup(&test_names, 0xFF), "Range");

    assert_null(dmi_code_lookup(&test_names, 2));
    assert_null(dmi_name_lookup(&test_names, 0x100));
    assert_null(dmi_code_lookup(&test_names, -1));
    assert_null(dmi_name_lookup(nullptr, 1));
}

static void test_name_lookup_ex(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        int              id;
        const char      *code;
        const char      *name;
        dmi_name_type_t  type;
    } cases[] = {
        { 1,     "first",  "First",  DMI_NAME_TYPE_EXACT },
        { 0x81,  "inside", "Inside", DMI_NAME_TYPE_EXACT },
        { 0x80,  "range",  "Range",  DMI_NAME_TYPE_RANGE },
        { 0xFF,  "range",  "Range",  DMI_NAME_TYPE_RANGE },
        { 2,     nullptr,  nullptr,  DMI_NAME_TYPE_NONE  },
        { 0x100, nullptr,  nullptr,  DMI_NAME_TYPE_NONE  },
        { -1,    nullptr,  nullptr,  DMI_NAME_TYPE_NONE  }
    };

    for (size_t i = 0; i < countof(cases); i++) {
        dmi_name_type_t type = (dmi_name_type_t)-1;
        const char *code = dmi_code_lookup_ex(&test_names, cases[i].id, &type);

        if (cases[i].code != nullptr)
            assert_string_equal(code, cases[i].code);
        else
            assert_null(code);
        assert_int_equal(type, cases[i].type);

        type = (dmi_name_type_t)-1;
        const char *name = dmi_name_lookup_ex(&test_names, cases[i].id, &type);

        if (cases[i].name != nullptr)
            assert_string_equal(name, cases[i].name);
        else
            assert_null(name);
        assert_int_equal(type, cases[i].type);
    }

    // Type is optional
    assert_string_equal(dmi_code_lookup_ex(&test_names, 0x80, nullptr), "range");
    assert_string_equal(dmi_name_lookup_ex(&test_names, 1, nullptr), "First");

    // Missing dictionary is not found
    dmi_name_type_t type = DMI_NAME_TYPE_EXACT;

    assert_null(dmi_name_lookup_ex(nullptr, 1, &type));
    assert_int_equal(type, DMI_NAME_TYPE_NONE);
}

static void test_name_lookup_rev(void **pstate)
{
    dmi_unused(pstate);

    // Range codes are not searched
    assert_int_equal(dmi_code_lookup_rev(&test_names, "inside"), 0x81);
    assert_int_equal(dmi_code_lookup_rev(&test_names, "range"), -1);
    assert_int_equal(dmi_code_lookup_rev(&test_names, "missing"), -1);
}
