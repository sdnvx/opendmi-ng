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

#include <opendmi/entity/baseboard.h>

static int test_baseboard_setup(void **pstate);
static int test_baseboard_teardown(void **pstate);

static void test_baseboard_type_name(void **pstate);
static void test_baseboard_decode_chassis_handle(void **pstate);
static void test_baseboard_decode_objects(void **pstate);
static void test_baseboard_decode_no_objects(void **pstate);
static void test_baseboard_decode_objects_overflow(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_baseboard_type_name),
        cmocka_unit_test_setup_teardown(test_baseboard_decode_chassis_handle, test_baseboard_setup, test_baseboard_teardown),
        cmocka_unit_test_setup_teardown(test_baseboard_decode_objects, test_baseboard_setup, test_baseboard_teardown),
        cmocka_unit_test_setup_teardown(test_baseboard_decode_no_objects, test_baseboard_setup, test_baseboard_teardown),
        cmocka_unit_test_setup_teardown(test_baseboard_decode_objects_overflow, test_baseboard_setup, test_baseboard_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_baseboard_type_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_baseboard_type_name(__DMI_BASEBOARD_TYPE_COUNT));

    for (int i = 0; i < __DMI_BASEBOARD_TYPE_COUNT; i++) {
        assert_non_null(dmi_baseboard_type_name(i));
    }
}

static int test_baseboard_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_baseboard_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

// Decode baseboard structure with given formatted area, followed by strings
static dmi_entity_t *decode_baseboard(dmi_buffer_t *buffer, const uint8_t *body, size_t length)
{
    static uint8_t data[1024];
    static const uint8_t strings[] = { 'V', 'e', 'n', 'd', 'o', 'r', 0, 0 };

    assert_true(4 + length + sizeof(strings) <= sizeof(data));

    data[0] = 2;
    data[1] = (uint8_t)(4 + length);
    data[2] = 0x00;
    data[3] = 0x10;
    memcpy(data + 4, body, length);
    memcpy(data + 4 + length, strings, sizeof(strings));

    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, 4 + length + sizeof(strings));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    return entity;
}

static const uint8_t test_baseboard_body[] = {
    0x01, 0x00, 0x00, 0x00, 0x00,           // Strings
    0x01,                                   // Features
    0x00,                                   // Location
    0x03, 0x00,                             // Chassis handle
    0x0A,                                   // Board type (motherboard)
    0x02,                                   // Number of contained objects
    0x10, 0x00, 0x11, 0x00                  // Contained object handles
};

static void test_baseboard_decode_chassis_handle(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);
    dmi_entity_t *entity;
    const dmi_baseboard_t *info;

    // Chassis handle is not read partially
    for (uint8_t length = 0x0B; length < 0x0E; length++) {
        entity = decode_baseboard(entity_buffer, test_baseboard_body, length - 4);
        info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
        assert_string_equal(info->vendor, "Vendor");
        assert_int_equal(info->chassis_handle, (length == 0x0D) ? 0x0003 : DMI_HANDLE_INVALID);
        assert_int_equal(info->type, 0);
        assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
        dmi_entity_destroy(entity);
    }

    entity = decode_baseboard(entity_buffer, test_baseboard_body, 0x0E - 4);
    info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    assert_int_equal(info->chassis_handle, 0x0003);
    assert_int_equal(info->type, DMI_BASEBOARD_TYPE_MOTHERBOARD);
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    dmi_entity_destroy(entity);
    dmi_buffer_destroy(entity_buffer);
}

static void test_baseboard_decode_objects(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = decode_baseboard(entity_buffer, test_baseboard_body, sizeof(test_baseboard_body));

    const dmi_baseboard_t *info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    assert_non_null(info);

    assert_int_equal(info->type, DMI_BASEBOARD_TYPE_MOTHERBOARD);
    assert_int_equal(info->object_count, 2);
    assert_int_equal(info->object_handles[0], 0x0010);
    assert_int_equal(info->object_handles[1], 0x0011);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

static void test_baseboard_decode_no_objects(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    uint8_t body[sizeof(test_baseboard_body)];
    memcpy(body, test_baseboard_body, sizeof(body));
    body[0x0E - 4] = 0;

    dmi_entity_t *entity = decode_baseboard(entity_buffer, body, 0x0F - 4);

    const dmi_baseboard_t *info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    assert_non_null(info);

    assert_int_equal(info->object_count, 0);
    assert_null(info->object_handles);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

static void test_baseboard_decode_objects_overflow(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    uint8_t body[sizeof(test_baseboard_body)];
    memcpy(body, test_baseboard_body, sizeof(body));

    // Second handle is missing, the first one is decoded
    dmi_entity_t *entity = decode_baseboard(entity_buffer, body, sizeof(body) - 2);
    const dmi_baseboard_t *info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    assert_int_equal(info->type, DMI_BASEBOARD_TYPE_MOTHERBOARD);
    assert_int_equal(info->object_count, 1);
    assert_int_equal(info->object_handles[0], 0x0010);
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    dmi_entity_destroy(entity);

    // Number of handles is far beyond structure length
    body[0x0E - 4] = 0xFF;
    entity = decode_baseboard(entity_buffer, body, sizeof(body));
    info = dmi_entity_info(entity, DMI_TYPE(BASEBOARD));
    assert_int_equal(info->object_count, 2);
    assert_int_equal(info->object_handles[1], 0x0011);
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    dmi_entity_destroy(entity);
    dmi_buffer_destroy(entity_buffer);
}
