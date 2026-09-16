//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/registry.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/memory-array-addr.h>
#include <opendmi/entity/memory-channel.h>
#include <opendmi/entity/memory-device.h>
#include <opendmi/entity/memory-device-addr.h>

static int test_registry_setup(void **pstate);
static int test_registry_teardown(void **pstate);

static void test_registry_get(void **pstate);
static void test_registry_get_reserved_handles(void **pstate);
static void test_registry_get_any(void **pstate);
static void test_registry_get_first(void **pstate);
static void test_registry_link_unset_handles(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

// SMBIOS table with unset (0xFFFF) references
static dmi_data_t test_table[] = {
    // Physical memory array, handle 0x0001
    16, 0x0F, 0x01, 0x00,
    0x03, 0x03, 0x03, 0x00, 0x00, 0x40, 0x00, 0xFE, 0xFF, 0x02, 0x00,
    0x00, 0x00,

    // Memory device A, handle 0x0010 (not referenced by channel and mapping)
    17, 0x15, 0x10, 0x00,
    0x01, 0x00, 0xFE, 0xFF, 0x40, 0x00, 0x40, 0x00, 0x00, 0x20,
    0x09, 0x00, 0x00, 0x00, 0x1A, 0x80, 0x00,
    0x00, 0x00,

    // Memory device B, handle 0x0011
    17, 0x15, 0x11, 0x00,
    0x01, 0x00, 0xFE, 0xFF, 0x40, 0x00, 0x40, 0x00, 0x00, 0x20,
    0x09, 0x00, 0x00, 0x00, 0x1A, 0x80, 0x00,
    0x00, 0x00,

    // Memory array mapped address, handle 0x0020, array 0x0001
    19, 0x0F, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x3F, 0x00, 0x01, 0x00, 0x02,
    0x00, 0x00,

    // Memory array mapped address, handle 0x0021, array is not set
    19, 0x0F, 0x21, 0x00,
    0x00, 0x00, 0x40, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0xFF, 0xFF, 0x02,
    0x00, 0x00,

    // Memory device mapped address, handle 0x0030, device 0x0011, array mapping is not set
    20, 0x13, 0x30, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x1F, 0x00, 0x11, 0x00, 0xFF, 0xFF,
    0xFF, 0x00, 0x00,
    0x00, 0x00,

    // Memory channel, handle 0x0040, devices: unset and 0x0011
    37, 0x0D, 0x40, 0x00,
    0x03, 0x04, 0x02, 0x02, 0xFF, 0xFF, 0x02, 0x11, 0x00,
    0x00, 0x00,

    // End of table
    127, 0x04, 0xFF, 0x00,
    0x00, 0x00
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_registry_get),
        cmocka_unit_test(test_registry_get_reserved_handles),
        cmocka_unit_test(test_registry_get_any),
        cmocka_unit_test(test_registry_get_first),
        cmocka_unit_test(test_registry_link_unset_handles)
    };

    return cmocka_run_group_tests(tests, test_registry_setup, test_registry_teardown);
}

static int test_registry_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(DMI_CONTEXT_FLAG_LINK);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    context->state.smbios_version  = DMI_VERSION(2, 7, 0);
    context->state.table_data      = test_table;
    context->state.table_area_size = sizeof(test_table);

    context->state.registry = dmi_registry_create(context, 0);

    bool success =
        (context->state.registry != nullptr) and
        dmi_registry_scan(context->state.registry) and
        dmi_registry_decode(context->state.registry) and
        dmi_registry_link(context->state.registry);

    if (not success) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_registry_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_registry_get(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_registry_t *registry = context->state.registry;

    dmi_entity_t *entity = dmi_registry_get(registry, 0x0011, DMI_TYPE(MEMORY_DEVICE), false);
    assert_non_null(entity);
    assert_int_equal(entity->handle, 0x0011);

    // Missing handle
    dmi_error_clear(context);
    assert_null(dmi_registry_get(registry, 0x1234, DMI_TYPE_INVALID, false));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_ENTITY_NOT_FOUND);

    dmi_error_clear(context);
    assert_null(dmi_registry_get(registry, 0x1234, DMI_TYPE_INVALID, true));
    assert_null(dmi_error_peek_last(context));

    // Type mismatch
    dmi_error_clear(context);
    assert_null(dmi_registry_get(registry, 0x0011, DMI_TYPE(MEMORY_ARRAY), false));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_INVALID_ENTITY_TYPE);
}

static void test_registry_get_reserved_handles(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_registry_t *registry = context->state.registry;

    // Reserved handles never match any entity and are not errors
    dmi_error_clear(context);

    assert_null(dmi_registry_get(registry, DMI_HANDLE_INVALID, DMI_TYPE(MEMORY_DEVICE), false));
    assert_null(dmi_registry_get(registry, DMI_HANDLE_INVALID, DMI_TYPE_INVALID, false));
    assert_null(dmi_registry_get(registry, DMI_HANDLE_UNSUPPORTED, DMI_TYPE(MEMORY_DEVICE), false));

    assert_null(dmi_error_peek_last(context));
}

static void test_registry_get_any(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_registry_t *registry = context->state.registry;

    static const dmi_type_t types[] = {
        DMI_TYPE(MEMORY_ARRAY),
        DMI_TYPE(MEMORY_DEVICE),
        DMI_TYPE_INVALID
    };

    dmi_entity_t *entity = dmi_registry_get_any(registry, 0x0010, types, false);
    assert_non_null(entity);
    assert_int_equal(entity->handle, 0x0010);

    dmi_error_clear(context);
    assert_null(dmi_registry_get_any(registry, DMI_HANDLE_INVALID, types, false));
    assert_null(dmi_registry_get_any(registry, DMI_HANDLE_UNSUPPORTED, types, false));
    assert_null(dmi_error_peek_last(context));

    assert_null(dmi_registry_get_any(registry, 0x0040, types, false));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_INVALID_ENTITY_TYPE);
}

static void test_registry_get_first(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_registry_t *registry = context->state.registry;

    dmi_entity_t *entity = dmi_registry_get_first(registry, DMI_TYPE(MEMORY_DEVICE), false);
    assert_non_null(entity);
    assert_int_equal(entity->handle, 0x0010);

    dmi_error_clear(context);
    assert_null(dmi_registry_get_first(registry, DMI_TYPE(PROCESSOR), true));
    assert_null(dmi_error_peek_last(context));

    assert_null(dmi_registry_get_first(registry, DMI_TYPE(PROCESSOR), false));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_ENTITY_NOT_FOUND);

    dmi_error_clear(context);
    assert_null(dmi_registry_get_first(registry, DMI_TYPE_INVALID, false));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_INVALID_ARGUMENT);
}

static void test_registry_link_unset_handles(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_registry_t *registry = context->state.registry;

    dmi_entity_t *device_a = dmi_registry_get(registry, 0x0010, DMI_TYPE(MEMORY_DEVICE), false);
    dmi_entity_t *device_b = dmi_registry_get(registry, 0x0011, DMI_TYPE(MEMORY_DEVICE), false);
    dmi_entity_t *channel  = dmi_registry_get(registry, 0x0040, DMI_TYPE(MEMORY_CHANNEL), false);
    dmi_entity_t *array    = dmi_registry_get(registry, 0x0001, DMI_TYPE(MEMORY_ARRAY), false);

    assert_non_null(device_a);
    assert_non_null(device_b);
    assert_non_null(channel);
    assert_non_null(array);

    // Memory channel is bound only to the referenced device
    const dmi_memory_channel_t *channel_info = dmi_entity_info(channel, DMI_TYPE(MEMORY_CHANNEL));
    assert_int_equal(channel_info->device_count, 2);
    assert_null(channel_info->devices[0].device);
    assert_ptr_equal(channel_info->devices[1].device, device_b);

    const dmi_memory_device_t *device_a_info = dmi_entity_info(device_a, DMI_TYPE(MEMORY_DEVICE));
    const dmi_memory_device_t *device_b_info = dmi_entity_info(device_b, DMI_TYPE(MEMORY_DEVICE));
    assert_null(device_a_info->channel);
    assert_ptr_equal(device_b_info->channel, channel);

    // Array mapped address without array reference
    dmi_entity_t *array_addr_1 = dmi_registry_get(registry, 0x0020, DMI_TYPE(MEMORY_ARRAY_ADDR), false);
    dmi_entity_t *array_addr_2 = dmi_registry_get(registry, 0x0021, DMI_TYPE(MEMORY_ARRAY_ADDR), false);
    assert_non_null(array_addr_1);
    assert_non_null(array_addr_2);

    const dmi_memory_array_addr_t *array_addr_1_info = dmi_entity_info(array_addr_1, DMI_TYPE(MEMORY_ARRAY_ADDR));
    const dmi_memory_array_addr_t *array_addr_2_info = dmi_entity_info(array_addr_2, DMI_TYPE(MEMORY_ARRAY_ADDR));
    assert_ptr_equal(array_addr_1_info->array, array);
    assert_null(array_addr_2_info->array);

    // Device mapped address without array mapped address reference
    dmi_entity_t *device_addr = dmi_registry_get(registry, 0x0030, DMI_TYPE(MEMORY_DEVICE_ADDR), false);
    assert_non_null(device_addr);

    const dmi_memory_device_addr_t *device_addr_info = dmi_entity_info(device_addr, DMI_TYPE(MEMORY_DEVICE_ADDR));
    assert_ptr_equal(device_addr_info->device, device_b);
    assert_null(device_addr_info->array_addr);
}
