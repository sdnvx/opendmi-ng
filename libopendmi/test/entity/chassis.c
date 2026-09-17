//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/chassis.h>

void test_chassis_type_name(void **pstate);
void test_chassis_security_status_name(void **pstate);
static int test_chassis_setup(void **pstate);
static int test_chassis_teardown(void **pstate);
static void test_chassis_decode_v20(void **pstate);
static void test_chassis_decode_oem_defined(void **pstate);
static void test_chassis_decode_elements(void **pstate);
static void test_chassis_decode_elements_overflow(void **pstate);
static void test_chassis_decode_short_elements(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_chassis_type_name),
        cmocka_unit_test(test_chassis_security_status_name),
        cmocka_unit_test_setup_teardown(test_chassis_decode_v20, test_chassis_setup, test_chassis_teardown),
        cmocka_unit_test_setup_teardown(test_chassis_decode_oem_defined, test_chassis_setup, test_chassis_teardown),
        cmocka_unit_test_setup_teardown(test_chassis_decode_elements, test_chassis_setup, test_chassis_teardown),
        cmocka_unit_test_setup_teardown(test_chassis_decode_elements_overflow, test_chassis_setup, test_chassis_teardown),
        cmocka_unit_test_setup_teardown(test_chassis_decode_short_elements, test_chassis_setup, test_chassis_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

void test_chassis_type_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_chassis_type_name(__DMI_CHASSIS_TYPE_COUNT));

    for (int i = 0; i < __DMI_CHASSIS_TYPE_COUNT; i++) {
        assert_non_null(dmi_chassis_type_name(i));
    }
}

void test_chassis_security_status_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_chassis_security_status_name(__DMI_CHASSIS_SECURITY_STATUS_COUNT));

    for (int i = 0; i < __DMI_CHASSIS_SECURITY_STATUS_COUNT; i++) {
        assert_non_null(dmi_chassis_security_status_name(i));
    }
}

static int test_chassis_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_chassis_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

// Decode chassis structure with given formatted area, followed by strings
// "Vendor" and "SKU"
static dmi_entity_t *decode_chassis(dmi_context_t *context, const uint8_t *body, size_t length)
{
    static uint8_t data[512];
    static const uint8_t strings[] = { 'V', 'e', 'n', 'd', 'o', 'r', 0, 'S', 'K', 'U', 0, 0 };

    assert_true(4 + length + sizeof(strings) <= sizeof(data));

    data[0] = 3;
    data[1] = (uint8_t)(4 + length);
    data[2] = 0x00;
    data[3] = 0x10;
    memcpy(data + 4, body, length);
    memcpy(data + 4 + length, strings, sizeof(strings));

    dmi_entity_t *entity = dmi_entity_create(context, data, 4 + length + sizeof(strings));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    return entity;
}

static void test_chassis_decode_v20(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_entity_t *entity = decode_chassis(context, (const uint8_t[]){ 0x01, 0x17, 0x00, 0x00, 0x00 }, 5);

    const dmi_chassis_t *info = dmi_entity_info(entity, DMI_TYPE(CHASSIS));
    assert_non_null(info);

    assert_string_equal(info->vendor, "Vendor");
    assert_int_equal(info->type, DMI_CHASSIS_TYPE_RACK_MOUNT);
    assert_int_equal(entity->level, DMI_VERSION(2, 0, 0));
    assert_int_equal(info->element_count, 0);
    assert_null(info->elements);
    assert_null(info->sku_number);

    dmi_entity_destroy(entity);
}

static void test_chassis_decode_oem_defined(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t body[] = {
        0x01, 0x17, 0x00, 0x00, 0x00,       // Strings and type
        0x03, 0x03, 0x03, 0x03,             // States
        0x78, 0x56, 0x34, 0x12              // OEM-defined
    };

    // OEM-defined field is not read partially
    dmi_entity_t *entity = decode_chassis(context, body, 0x0F - 4);
    const dmi_chassis_t *info = dmi_entity_info(entity, DMI_TYPE(CHASSIS));
    assert_int_equal(entity->level, DMI_VERSION(2, 3, 0));
    assert_int_equal(info->oem_defined, 0);
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    dmi_entity_destroy(entity);

    entity = decode_chassis(context, body, sizeof(body));
    info = dmi_entity_info(entity, DMI_TYPE(CHASSIS));
    assert_int_equal(entity->level, DMI_VERSION(2, 3, 0));
    assert_int_equal(info->oem_defined, 0x12345678);
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    dmi_entity_destroy(entity);
}

static void test_chassis_decode_elements(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t body[] = {
        0x01, 0x17, 0x00, 0x00, 0x00,       // Strings and type
        0x03, 0x03, 0x03, 0x03,             // States
        0x00, 0x00, 0x00, 0x00,             // OEM-defined
        0x02, 0x02,                         // Height, power cords
        0x02, 0x03,                         // Element count and size
        0x0A, 0x01, 0x02,                   // Baseboard: server blade
        0x91, 0x00, 0x04,                   // SMBIOS structure: memory device
        0x02,                               // SKU number
        0x01, 0x10                          // Rack type and height
    };

    dmi_entity_t *entity = decode_chassis(context, body, sizeof(body));

    const dmi_chassis_t *info = dmi_entity_info(entity, DMI_TYPE(CHASSIS));
    assert_non_null(info);

    assert_int_equal(entity->level, DMI_VERSION(3, 9, 0));
    assert_int_equal(info->height, 2);
    assert_int_equal(info->power_cord_count, 2);
    assert_int_equal(info->element_count, 2);
    assert_int_equal(info->element_size, 3);

    assert_int_equal(info->elements[0].type, DMI_TYPE_INVALID);
    assert_int_equal(info->elements[0].board_type, 0x0A);
    assert_int_equal(info->elements[0].minimum_count, 1);
    assert_int_equal(info->elements[0].maximum_count, 2);

    assert_int_equal(info->elements[1].type, DMI_TYPE(MEMORY_DEVICE));
    assert_int_equal(info->elements[1].minimum_count, 0);
    assert_int_equal(info->elements[1].maximum_count, 4);

    assert_string_equal(info->sku_number, "SKU");
    assert_int_equal(info->rack_type, DMI_RACK_TYPE_OPEN);
    assert_int_equal(info->rack_height, 0x10);

    dmi_entity_destroy(entity);
}

static void test_chassis_decode_elements_overflow(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Elements (255 x 255 bytes) do not fit into the structure
    static const uint8_t body[] = {
        0x01, 0x17, 0x00, 0x00, 0x00,
        0x03, 0x03, 0x03, 0x03,
        0x00, 0x00, 0x00, 0x00,
        0x02, 0x02,
        0xFF, 0xFF,
        0x02
    };

    dmi_entity_t *entity = decode_chassis(context, body, sizeof(body));

    const dmi_chassis_t *info = dmi_entity_info(entity, DMI_TYPE(CHASSIS));
    assert_non_null(info);

    // No element is completely present
    assert_int_equal(info->power_cord_count, 2);
    assert_int_equal(info->element_count, 0);
    assert_null(info->sku_number);
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    dmi_entity_destroy(entity);
}

static void test_chassis_decode_short_elements(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Element records are shorter than 3 bytes
    static const uint8_t body[] = {
        0x01, 0x17, 0x00, 0x00, 0x00,
        0x03, 0x03, 0x03, 0x03,
        0x00, 0x00, 0x00, 0x00,
        0x02, 0x02,
        0x02, 0x02,
        0x91, 0x00, 0x91, 0x00,
        0x02
    };

    dmi_entity_t *entity = decode_chassis(context, body, sizeof(body));

    const dmi_chassis_t *info = dmi_entity_info(entity, DMI_TYPE(CHASSIS));
    assert_non_null(info);

    assert_int_equal(info->element_count, 0);
    assert_null(info->elements);
    assert_int_equal(info->element_size, 2);
    assert_string_equal(info->sku_number, "SKU");
    assert_int_equal(entity->level, DMI_VERSION(2, 7, 0));

    dmi_entity_destroy(entity);
}
