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

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/bis-entry-point.h>

static void test_bis_entry_point_decode(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_bis_entry_point_decode)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_bis_entry_point_decode(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    for (int valid = 1; valid >= 0; valid--) {
        // BIS entry point structure without strings
        uint8_t data[0x1C + 2] = {
            31, 0x1C, 0x00, 0x31,               // Header
            0x00, 0x00, 0x00, 0x00,             // Checksum, reserved
            0x34, 0x12, 0x00, 0xF0,             // 16-bit entry point (F000:1234)
            0x00, 0x10, 0x0E, 0x00              // 32-bit entry point
        };

        // Checksum makes the sum of structure bytes zero
        data[0x04] = dmi_checksum_calc(data, 0x1C);
        if (not valid)
            data[0x04]++;

        dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

        dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, sizeof(data));
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_bis_entry_point_t *info = dmi_entity_info(entity, DMI_TYPE(BIS_ENTRY_POINT));
        assert_non_null(info);

        assert_int_equal(info->checksum, data[0x04]);
        assert_int_equal(info->entry_point_16.segment, 0xF000);
        assert_int_equal(info->entry_point_16.offset, 0x1234);
        assert_int_equal(info->entry_point_32, 0x000E1000);
        assert_int_equal(info->is_valid, valid);

        dmi_entity_destroy(entity);

        dmi_buffer_destroy(entity_buffer);
    }

    dmi_destroy(context);
}
