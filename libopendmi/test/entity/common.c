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
#include <opendmi/entity/common.h>

static void test_status_name(void **pstate);
static void test_error_correct_type_name(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_status_name),
        cmocka_unit_test(test_error_correct_type_name)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_status_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_status_name(__DMI_STATUS_COUNT));

    for (int i = 0; i < __DMI_STATUS_COUNT; i++) {
        assert_non_null(dmi_status_name(i));
    }
}

static void test_error_correct_type_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_error_correct_type_name(__DMI_ERROR_CORRECT_TYPE_COUNT));

    for (int i = 0; i < __DMI_ERROR_CORRECT_TYPE_COUNT; i++) {
        assert_non_null(dmi_error_correct_type_name(i));
    }
}
