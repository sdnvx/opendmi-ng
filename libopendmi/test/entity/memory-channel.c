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
#include <opendmi/entity/memory-channel.h>

static void test_memory_channel_type_name(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_memory_channel_type_name)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_memory_channel_type_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_memory_channel_type_name(__DMI_MEMORY_CHANNEL_TYPE_COUNT));

    for (int i = 0; i < __DMI_MEMORY_CHANNEL_TYPE_COUNT; i++) {
        assert_non_null(dmi_memory_channel_type_name(i));
    }
}
