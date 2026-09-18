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
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>
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
static void test_registry_decode_malformed(void **pstate);
static void test_registry_decode_malformed_strict(void **pstate);
static void test_registry_resolve(void **pstate);
static void test_registry_resolve_strict(void **pstate);

static void test_resolve(dmi_registry_t *registry, dmi_handle_t handle, dmi_type_t type,
                         bool success, dmi_type_t found, dmi_error_code_t reason);

static dmi_context_t *test_registry_open(unsigned int flags, dmi_data_t *table, size_t size);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

// SMBIOS table with malformed memory device referenced by memory channel
static dmi_data_t test_malformed_table[] = {
    // Memory device, handle 0x0010
    17, 0x15, 0x10, 0x00,
    0x01, 0x00, 0xFE, 0xFF, 0x40, 0x00, 0x40, 0x00, 0x00, 0x20,
    0x09, 0x00, 0x00, 0x00, 0x1A, 0x80, 0x00,
    0x00, 0x00,

    // Memory device, handle 0x0011, shorter than the minimum length
    17, 0x10, 0x11, 0x00,
    0x01, 0x00, 0xFE, 0xFF, 0x40, 0x00, 0x40, 0x00, 0x00, 0x20,
    0x09, 0x00,
    0x00, 0x00,

    // Memory channel, handle 0x0040, devices: 0x0010 and 0x0011
    37, 0x0D, 0x40, 0x00,
    0x03, 0x04, 0x02, 0x02, 0x10, 0x00, 0x02, 0x11, 0x00,
    0x00, 0x00,

    // End of table
    127, 0x04, 0xFF, 0x00,
    0x00, 0x00
};

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

// SMBIOS table with a structure at handle 0x0000, which some vendors use as
// unspecified handle value
static dmi_data_t test_resolve_table[] = {
    // OEM strings, handle 0x0000
    11, 0x05, 0x00, 0x00,
    0x01, 'O', 'E', 'M', 0x00,
    0x00,

    // Physical memory array, handle 0x0001
    16, 0x0F, 0x01, 0x00,
    0x03, 0x03, 0x03, 0x00, 0x00, 0x40, 0x00, 0xFE, 0xFF, 0x02, 0x00,
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
        cmocka_unit_test(test_registry_link_unset_handles),
        cmocka_unit_test(test_registry_decode_malformed),
        cmocka_unit_test(test_registry_decode_malformed_strict),
        cmocka_unit_test(test_registry_resolve),
        cmocka_unit_test(test_registry_resolve_strict)
    };

    return cmocka_run_group_tests(tests, test_registry_setup, test_registry_teardown);
}

static int test_registry_setup(void **pstate)
{
    dmi_context_t *context;

    context = test_registry_open(DMI_CONTEXT_FLAG_LINK, test_table, sizeof(test_table));
    if (context == nullptr)
        return -1;

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

    const dmi_entity_t *entity = dmi_registry_get(registry, 0x0011, DMI_TYPE(MEMORY_DEVICE), false);
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

    const dmi_entity_t *entity = dmi_registry_get_any(registry, 0x0010, types, false);
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

    const dmi_entity_t *entity = dmi_registry_get_first(registry, DMI_TYPE(MEMORY_DEVICE), false);
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

    const dmi_entity_t *device_a = dmi_registry_get(registry, 0x0010, DMI_TYPE(MEMORY_DEVICE), false);
    const dmi_entity_t *device_b = dmi_registry_get(registry, 0x0011, DMI_TYPE(MEMORY_DEVICE), false);
    const dmi_entity_t *channel  = dmi_registry_get(registry, 0x0040, DMI_TYPE(MEMORY_CHANNEL), false);
    const dmi_entity_t *array    = dmi_registry_get(registry, 0x0001, DMI_TYPE(MEMORY_ARRAY), false);

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
    const dmi_entity_t *array_addr_1 = dmi_registry_get(registry, 0x0020, DMI_TYPE(MEMORY_ARRAY_ADDR), false);
    const dmi_entity_t *array_addr_2 = dmi_registry_get(registry, 0x0021, DMI_TYPE(MEMORY_ARRAY_ADDR), false);
    assert_non_null(array_addr_1);
    assert_non_null(array_addr_2);

    const dmi_memory_array_addr_t *array_addr_1_info = dmi_entity_info(array_addr_1, DMI_TYPE(MEMORY_ARRAY_ADDR));
    const dmi_memory_array_addr_t *array_addr_2_info = dmi_entity_info(array_addr_2, DMI_TYPE(MEMORY_ARRAY_ADDR));
    assert_ptr_equal(array_addr_1_info->array, array);
    assert_null(array_addr_2_info->array);

    // Device mapped address without array mapped address reference
    const dmi_entity_t *device_addr = dmi_registry_get(registry, 0x0030, DMI_TYPE(MEMORY_DEVICE_ADDR), false);
    assert_non_null(device_addr);

    const dmi_memory_device_addr_t *device_addr_info = dmi_entity_info(device_addr, DMI_TYPE(MEMORY_DEVICE_ADDR));
    assert_ptr_equal(device_addr_info->device, device_b);
    assert_null(device_addr_info->array_addr);
}

static void test_registry_decode_malformed(void **pstate)
{
    dmi_unused(pstate);

    // Malformed structure does not prevent the rest of the table from being used
    dmi_context_t *context = test_registry_open(DMI_CONTEXT_FLAG_LINK, test_malformed_table,
                                                sizeof(test_malformed_table));
    assert_non_null(context);

    dmi_registry_t *registry = context->state.registry;
    assert_true(registry->status & DMI_REGISTRY_STATUS_DECODED);
    assert_true(registry->status & DMI_REGISTRY_STATUS_LINKED);

    const dmi_entity_t *device_a = dmi_registry_get(registry, 0x0010, DMI_TYPE(MEMORY_DEVICE), false);
    const dmi_entity_t *device_b = dmi_registry_get(registry, 0x0011, DMI_TYPE(MEMORY_DEVICE), false);
    const dmi_entity_t *channel  = dmi_registry_get(registry, 0x0040, DMI_TYPE(MEMORY_CHANNEL), false);

    if ((device_a == nullptr) or (device_b == nullptr) or (channel == nullptr)) {
        dmi_destroy(context);
        fail_msg("Entity not found");
    }

    // Malformed entity is left undecoded
    assert_false(device_b->state & DMI_ENTITY_STATE_DECODED);
    assert_null(dmi_entity_info(device_b, DMI_TYPE(MEMORY_DEVICE)));

    // Other entities are decoded and linked, including references to the malformed one
    assert_true(device_a->state & DMI_ENTITY_STATE_DECODED);
    assert_true(channel->state & DMI_ENTITY_STATE_LINKED);

    const dmi_memory_channel_t *channel_info = dmi_entity_info(channel, DMI_TYPE(MEMORY_CHANNEL));
    const dmi_memory_device_t *device_a_info = dmi_entity_info(device_a, DMI_TYPE(MEMORY_DEVICE));

    if ((channel_info == nullptr) or (device_a_info == nullptr)) {
        dmi_destroy(context);
        fail_msg("Entity is not decoded");
    }

    assert_int_equal(channel_info->device_count, 2);
    assert_ptr_equal(channel_info->devices[0].device, device_a);
    assert_ptr_equal(channel_info->devices[1].device, device_b);
    assert_ptr_equal(device_a_info->channel, channel);

    dmi_destroy(context);
}

static void test_registry_decode_malformed_strict(void **pstate)
{
    dmi_unused(pstate);

    // Malformed structure is an error in strict mode
    const dmi_context_t *context = test_registry_open(DMI_CONTEXT_FLAG_LINK | DMI_CONTEXT_FLAG_STRICT,
                                                test_malformed_table, sizeof(test_malformed_table));
    assert_null(context);
}

static void test_registry_resolve(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = test_registry_open(DMI_CONTEXT_FLAG_LINK, test_resolve_table,
                                                sizeof(test_resolve_table));
    assert_non_null(context);

    dmi_registry_t *registry = context->state.registry;

    // Reference which is not set is resolved to nothing
    test_resolve(registry, DMI_HANDLE_INVALID, DMI_TYPE(MEMORY_ARRAY), true, DMI_TYPE_INVALID, DMI_ERROR_NONE);
    test_resolve(registry, DMI_HANDLE_UNSUPPORTED, DMI_TYPE(MEMORY_ARRAY), true, DMI_TYPE_INVALID, DMI_ERROR_NONE);

    // Valid references, with and without type check
    test_resolve(registry, 0x0001, DMI_TYPE(MEMORY_ARRAY), true, DMI_TYPE(MEMORY_ARRAY), DMI_ERROR_NONE);
    test_resolve(registry, 0x0001, DMI_TYPE_INVALID, true, DMI_TYPE(MEMORY_ARRAY), DMI_ERROR_NONE);
    test_resolve(registry, 0x0000, DMI_TYPE(OEM_STRINGS), true, DMI_TYPE(OEM_STRINGS), DMI_ERROR_NONE);

    // Broken references
    test_resolve(registry, 0x0001, DMI_TYPE(CACHE), false, DMI_TYPE_INVALID, DMI_ERROR_INVALID_ENTITY_TYPE);
    test_resolve(registry, 0x0999, DMI_TYPE(MEMORY_ARRAY), false, DMI_TYPE_INVALID, DMI_ERROR_ENTITY_NOT_FOUND);

    // Handle 0x0000 of unexpected type means unspecified value in relaxed mode
    test_resolve(registry, 0x0000, DMI_TYPE(MEMORY_ARRAY), true, DMI_TYPE_INVALID, DMI_ERROR_NONE);

    // Any of the expected types is accepted
    static const dmi_type_t valid_types[]   = { DMI_TYPE(CACHE), DMI_TYPE(MEMORY_ARRAY), DMI_TYPE_INVALID };
    static const dmi_type_t invalid_types[] = { DMI_TYPE(CACHE), DMI_TYPE(PROCESSOR), DMI_TYPE_INVALID };

    dmi_entity_t *entity = nullptr;

    assert_true(dmi_registry_resolve_any(registry, 0x0001, valid_types, &entity));
    assert_non_null(entity);

    dmi_error_clear(context);
    assert_false(dmi_registry_resolve_any(registry, 0x0001, invalid_types, &entity));
    assert_null(entity);
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_INVALID_ENTITY_TYPE);
    assert_non_null(strstr(dmi_error_peek_last(context)->message, "unexpected"));

    // The only expected type is named in the error message
    dmi_error_clear(context);
    assert_false(dmi_registry_resolve(registry, 0x0001, DMI_TYPE(CACHE), &entity));
    assert_non_null(strstr(dmi_error_peek_last(context)->message, "instead of"));

    dmi_destroy(context);
}

static void test_registry_resolve_strict(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = test_registry_open(DMI_CONTEXT_FLAG_LINK | DMI_CONTEXT_FLAG_STRICT,
                                                test_resolve_table, sizeof(test_resolve_table));
    assert_non_null(context);

    // Handle 0x0000 is not treated as unspecified value in strict mode
    test_resolve(context->state.registry, 0x0000, DMI_TYPE(MEMORY_ARRAY), false, DMI_TYPE_INVALID,
                 DMI_ERROR_INVALID_ENTITY_TYPE);

    dmi_destroy(context);
}

//
// Resolve reference and check the result, the resolved entity type and the
// raised error
//
static void test_resolve(dmi_registry_t *registry, dmi_handle_t handle, dmi_type_t type,
                         bool success, dmi_type_t found, dmi_error_code_t reason)
{
    dmi_context_t *context = registry->context;
    dmi_entity_t *entity = (dmi_entity_t *)(uintptr_t)1;

    dmi_error_clear(context);
    assert_int_equal(dmi_registry_resolve(registry, handle, type, &entity), success);

    if (found == DMI_TYPE_INVALID) {
        assert_null(entity);
    } else {
        assert_non_null(entity);
        assert_int_equal(entity->type, found);
    }

    const dmi_error_t *error = dmi_error_peek_last(context);
    if (reason == DMI_ERROR_NONE)
        assert_null(error);
    else
        assert_int_equal(error->reason, reason);
}

static dmi_context_t *test_registry_open(unsigned int flags, dmi_data_t *table, size_t size)
{
    dmi_context_t *context;

    context = dmi_create(flags);
    if (context == nullptr)
        return nullptr;

    dmi_set_logger(context, &test_logger);

    context->state.smbios_version = DMI_VERSION(2, 7, 0);
    context->state.table_data     = table;
    context->state.table_size     = size;

    context->state.registry = dmi_registry_create(context, 0);

    bool success =
        (context->state.registry != nullptr) and
        dmi_registry_scan(context->state.registry) and
        dmi_registry_decode(context->state.registry) and
        dmi_registry_link(context->state.registry);

    if (not success) {
        dmi_destroy(context);
        return nullptr;
    }

    return context;
}
