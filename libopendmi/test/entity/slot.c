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
#include <opendmi/entity/slot.h>

static void test_slot_usage_name(void **pstate);
static void test_slot_width_name(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_slot_usage_name),
        cmocka_unit_test(test_slot_width_name)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_slot_usage_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_slot_usage_name(__DMI_SLOT_USAGE_COUNT));

    for (int i = 0; i < __DMI_SLOT_USAGE_COUNT; i++) {
        assert_non_null(dmi_slot_usage_name(i));
    }
}

static void test_slot_width_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_slot_width_name(__DMI_SLOT_WIDTH_COUNT));

    for (int i = 0; i < __DMI_SLOT_WIDTH_COUNT; i++) {
        assert_non_null(dmi_slot_width_name(i));
    }
}
