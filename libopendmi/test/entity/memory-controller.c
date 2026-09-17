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
#include <opendmi/entity/memory-controller.h>

static void test_memory_interleave_name(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_memory_interleave_name)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_memory_interleave_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_memory_interleave_name(__DMI_MEMORY_INTERLEAVE_COUNT));

    for (int i = 0; i < __DMI_MEMORY_INTERLEAVE_COUNT; i++) {
        assert_non_null(dmi_memory_interleave_name(i));
    }
}
