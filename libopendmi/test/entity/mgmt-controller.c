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
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/mgmt-controller.h>

static int test_mgmt_controller_setup(void **pstate);
static int test_mgmt_controller_teardown(void **pstate);

static void test_mgmt_controller_decode_v30(void **pstate);
static void test_mgmt_controller_decode_records(void **pstate);
static void test_mgmt_controller_decode_no_records(void **pstate);
static void test_mgmt_controller_decode_truncated(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_v30, test_mgmt_controller_setup, test_mgmt_controller_teardown),
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_records, test_mgmt_controller_setup, test_mgmt_controller_teardown),
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_no_records, test_mgmt_controller_setup, test_mgmt_controller_teardown),
        cmocka_unit_test_setup_teardown(test_mgmt_controller_decode_truncated, test_mgmt_controller_setup, test_mgmt_controller_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_mgmt_controller_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_mgmt_controller_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

// Create entity from structure body, followed by empty string set
static dmi_entity_t *create_entity(dmi_context_t *context, const uint8_t *body, size_t length)
{
    static uint8_t data[256];

    assert_true(length + 4 + 2 <= sizeof(data));

    data[0] = 42;
    data[1] = (uint8_t)(length + 4);
    data[2] = 0x00;
    data[3] = 0x10;
    memcpy(data + 4, body, length);
    data[length + 4] = 0;
    data[length + 5] = 0;

    dmi_entity_t *entity = dmi_entity_create(context, data, length + 6);
    assert_non_null(entity);

    return entity;
}

static void test_mgmt_controller_decode_v30(void **pstate)
{
    dmi_context_t *context = *pstate;

    // SMBIOS 3.0 structure without protocol records
    static const uint8_t body[] = {
        0x40, 0x03, 0xAA, 0xBB, 0xCC
    };

    dmi_entity_t *entity = create_entity(context, body, sizeof(body));
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_int_equal(info->if_type, DMI_MGMT_IF_TYPE_NETWORK_HOST_IF);
    assert_int_equal(info->if_data_length, 3);
    assert_memory_equal(info->if_data, body + 2, 3);
    assert_int_equal(info->proto_records_count, 0);
    assert_null(info->proto_records);

    dmi_entity_destroy(entity);
}

static void test_mgmt_controller_decode_records(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t body[] = {
        0x40, 0x02, 0x11, 0x22,         // Interface type and data
        0x02,                           // Number of protocol records
        0x04, 0x03, 0x01, 0x02, 0x03,   // Redfish over IP record
        0xF0, 0x00                      // OEM record without data
    };

    dmi_entity_t *entity = create_entity(context, body, sizeof(body));
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_int_equal(info->if_data_length, 2);
    assert_memory_equal(info->if_data, body + 2, 2);
    assert_int_equal(info->proto_records_count, 2);

    assert_int_equal(info->proto_records[0]->type, DMI_MGMT_PROTO_REDFISH_OVER_IP);
    assert_int_equal(info->proto_records[0]->length, 3);
    assert_memory_equal(info->proto_records[0]->data, body + 7, 3);

    assert_int_equal(info->proto_records[1]->type, DMI_MGMT_PROTO_OEM);
    assert_int_equal(info->proto_records[1]->length, 0);

    dmi_entity_destroy(entity);
}

static void test_mgmt_controller_decode_no_records(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Zero interface data length and zero protocol records
    static const uint8_t body[] = { 0x40, 0x00, 0x00 };

    dmi_entity_t *entity = create_entity(context, body, sizeof(body));
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_controller_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);

    assert_int_equal(info->if_data_length, 0);
    assert_null(info->if_data);
    assert_int_equal(info->proto_records_count, 0);

    dmi_entity_destroy(entity);
}

static void test_mgmt_controller_decode_truncated(void **pstate)
{
    dmi_context_t *context = *pstate;
    const dmi_mgmt_controller_t *info;
    dmi_entity_t *entity;

    // Interface data length exceeds structure length (seen in real firmware)
    entity = create_entity(context, (const uint8_t[]){ 0x02, 0xFF, 0x01, 0x02, 0xFF, 0xFF, 0xFF, 0xFF }, 8);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);
    assert_int_equal(info->if_type, DMI_MGMT_IF_TYPE_MCTP_KCS);
    assert_int_equal(info->if_data_length, 0);
    assert_null(info->if_data);
    assert_int_equal(info->proto_records_count, 0);

    dmi_entity_destroy(entity);

    // Number of protocol records exceeds structure length
    entity = create_entity(context, (const uint8_t[]){ 0x40, 0x00, 0xFF, 0x04, 0x00 }, 5);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);
    assert_int_equal(info->proto_records_count, 1);
    assert_int_equal(info->proto_records[0]->type, DMI_MGMT_PROTO_REDFISH_OVER_IP);
    assert_int_equal(info->proto_records[0]->length, 0);

    dmi_entity_destroy(entity);

    // Protocol record header is truncated
    entity = create_entity(context, (const uint8_t[]){ 0x40, 0x00, 0x01, 0x04 }, 4);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);
    assert_int_equal(info->proto_records_count, 0);

    dmi_entity_destroy(entity);

    // Protocol record data exceeds structure length
    entity = create_entity(context, (const uint8_t[]){
        0x40, 0x01, 0x11, 0x02, 0x04, 0x01, 0xAA, 0x03, 0xFF, 0xBB
    }, 10);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    assert_non_null(info);
    assert_int_equal(info->if_data_length, 1);
    assert_int_equal(info->proto_records_count, 1);
    assert_int_equal(info->proto_records[0]->type, DMI_MGMT_PROTO_REDFISH_OVER_IP);
    assert_int_equal(info->proto_records[0]->length, 1);
    assert_int_equal(info->proto_records[0]->data[0], 0xAA);

    dmi_entity_destroy(entity);
}
