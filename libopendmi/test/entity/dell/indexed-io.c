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
#include <opendmi/module.h>
#include <opendmi/module/dell.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/dell/indexed-io.h>

static int test_dell_indexed_io_setup(void **pstate);
static int test_dell_indexed_io_teardown(void **pstate);

static void test_dell_indexed_io_decode(void **pstate);
static void test_dell_indexed_io_decode_unterminated(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_dell_indexed_io_decode, test_dell_indexed_io_setup, test_dell_indexed_io_teardown),
        cmocka_unit_test_setup_teardown(test_dell_indexed_io_decode_unterminated, test_dell_indexed_io_setup, test_dell_indexed_io_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_dell_indexed_io_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_add_extension(context, dmi_module_find("dell"))) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_dell_indexed_io_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_dell_indexed_io_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Tokens of Dell PowerEdge 1800, with unused and string tokens added
    static const uint8_t data[] = {
        212, 0x23, 0x00, 0xD4,          // Header
        0x70, 0x00, 0x71, 0x00,         // Index and data ports
        0x00, 0x10, 0x2D, 0x2E,         // Checksum type, range and index
        0x03, 0x00, 0x11, 0x7F, 0x80,   // Boolean token
        0x00, 0x00, 0x00, 0x00, 0x00,   // Unused token
        0x04, 0x00, 0x11, 0x7F, 0x00,   // Boolean token
        0x5C, 0x00, 0x40, 0x00, 0x08,   // String token
        0xFF, 0xFF, 0x00,               // Truncated end-of-table marker
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    const dmi_dell_indexed_io_t *info = dmi_entity_info(entity, DMI_TYPE(DELL_INDEXED_IO));
    assert_non_null(info);

    assert_int_equal(info->index_port, 0x70);
    assert_int_equal(info->data_port, 0x71);
    assert_int_equal(info->check_type, DMI_DELL_CHECK_TYPE_WORD_CHECKSUM);
    assert_int_equal(info->check_start, 0x10);
    assert_int_equal(info->check_end, 0x2D);
    assert_int_equal(info->check_index, 0x2E);

    // Unused token is skipped
    assert_int_equal(info->token_count, 3);

    assert_int_equal(info->tokens[0].id, 0x0003);
    assert_int_equal(info->tokens[0].location, 0x11);
    assert_int_equal(info->tokens[0].and_mask, 0x7F);
    assert_false(info->tokens[0].is_string);
    assert_int_equal(info->tokens[0].or_value, 0x80);

    assert_int_equal(info->tokens[1].id, 0x0004);
    assert_int_equal(info->tokens[1].or_value, 0x00);

    assert_int_equal(info->tokens[2].id, 0x005C);
    assert_int_equal(info->tokens[2].location, 0x40);
    assert_true(info->tokens[2].is_string);
    assert_int_equal(info->tokens[2].string_length, 8);

    dmi_entity_destroy(entity);
}

static void test_dell_indexed_io_decode_unterminated(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Token table without the end-of-table marker
    static const uint8_t data[] = {
        212, 0x11, 0x00, 0xD4,
        0x70, 0x00, 0x71, 0x00,
        0x01, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x11, 0x7F, 0x80,
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    const dmi_dell_indexed_io_t *info = dmi_entity_info(entity, DMI_TYPE(DELL_INDEXED_IO));
    assert_non_null(info);

    assert_int_equal(info->check_type, DMI_DELL_CHECK_TYPE_BYTE_CHECKSUM);
    assert_int_equal(info->token_count, 1);
    assert_int_equal(info->tokens[0].id, 0x0003);

    dmi_entity_destroy(entity);
}
