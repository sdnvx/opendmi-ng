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
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/processor-ex.h>

static void test_processor_ex_decode(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_processor_ex_decode)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_processor_ex_decode(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    static const struct {
        uint8_t              length;
        uint8_t              block_length;
        dmi_processor_arch_t arch;
        size_t               data_length;
        bool                 incomplete;
    } test_cases[] = {
        { 0x0C, 4, DMI_PROCESSOR_ARCH_RV64,    4, false },
        { 0x09, 1, DMI_PROCESSOR_ARCH_AMD64,   1, false },
        { 0x08, 0, DMI_PROCESSOR_ARCH_AARCH64, 0, false },
        // Structure without processor-specific block
        { 0x06, 0, DMI_PROCESSOR_ARCH_RESERVED, 0, false },
        // Processor-specific data does not fit into the structure
        { 0x0A, 4, DMI_PROCESSOR_ARCH_RV64,    2, true  }
    };

    for (size_t i = 0; i < countof(test_cases); i++) {
        uint8_t data[0x0C + 2] = {
            44, test_cases[i].length, 0x0E, 0x00,       // Header
            0x0D, 0x00,                                 // Processor handle
            test_cases[i].block_length, test_cases[i].arch,
            0x11, 0x22, 0x33, 0x44                      // Processor-specific data
        };

        data[test_cases[i].length]     = 0;
        data[test_cases[i].length + 1] = 0;

        dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

        dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, test_cases[i].length + 2);
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));
        assert_int_equal((entity->state & DMI_ENTITY_STATE_INCOMPLETE) != 0, test_cases[i].incomplete);

        const dmi_processor_ex_t *info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR_EX));
        assert_non_null(info);
        assert_int_equal(info->processor_handle, 0x000D);
        assert_int_equal(info->arch, test_cases[i].arch);
        assert_int_equal(info->data.length, test_cases[i].data_length);

        if (test_cases[i].data_length > 0) {
            assert_ptr_equal(info->data.data,
                             dmi_buffer_at(entity->buffer, entity->offset, entity->total_length) + 0x08);
            assert_int_equal(info->data.data[0], 0x11);
        } else {
            assert_null(info->data.data);
        }

        dmi_entity_destroy(entity);

        dmi_buffer_destroy(entity_buffer);
    }

    dmi_destroy(context);
}
