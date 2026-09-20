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
#include <opendmi/module/sun.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/sun/processor-ex.h>
#include <opendmi/entity/sun/port-ex.h>
#include <opendmi/entity/sun/pcie-root-complex.h>
#include <opendmi/entity/sun/memory-array-ex.h>
#include <opendmi/entity/sun/memory-device-ex.h>

static int test_sun_setup(void **pstate);
static int test_sun_teardown(void **pstate);

static void test_sun_processor_ex_decode(void **pstate);
static void test_sun_port_ex_decode(void **pstate);
static void test_sun_pcie_root_complex_decode(void **pstate);
static void test_sun_memory_array_ex_decode(void **pstate);
static void test_sun_memory_device_ex_decode(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_sun_processor_ex_decode, test_sun_setup, test_sun_teardown),
        cmocka_unit_test_setup_teardown(test_sun_port_ex_decode, test_sun_setup, test_sun_teardown),
        cmocka_unit_test_setup_teardown(test_sun_pcie_root_complex_decode, test_sun_setup, test_sun_teardown),
        cmocka_unit_test_setup_teardown(test_sun_memory_array_ex_decode, test_sun_setup, test_sun_teardown),
        cmocka_unit_test_setup_teardown(test_sun_memory_device_ex_decode, test_sun_setup, test_sun_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_sun_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_add_extension(context, dmi_module_find("sun"))) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_sun_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_sun_processor_ex_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Three APIC IDs are specified, but only two are present
    static const uint8_t data[] = {
        132, 0x0C, 0x00, 0x84,
        0x04, 0x00, 0x01, 0x03,
        0x00, 0x00, 0x01, 0x00,
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    const dmi_sun_processor_ex_t *info = dmi_entity_info(entity, DMI_TYPE(SUN_PROCESSOR_EX));
    assert_non_null(info);

    assert_int_equal(info->processor_handle, 0x0004);
    assert_int_equal(info->fru, 1);
    assert_int_equal(info->apic_id_total, 3);
    assert_int_equal(info->apic_id_count, 2);
    assert_int_equal(info->apic_ids[0], 0);
    assert_int_equal(info->apic_ids[1], 1);

    dmi_entity_destroy(entity);
}

static void test_sun_port_ex_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t data[] = {
        136, 0x0C, 0x00, 0x88,
        0x03, 0x00, 0x08, 0x00, 0x02, 0x10, 0x00, 0x05,
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_sun_port_ex_t *info = dmi_entity_info(entity, DMI_TYPE(SUN_PORT_EX));
    assert_non_null(info);

    assert_int_equal(info->chassis_handle, 0x0003);
    assert_int_equal(info->port_handle, 0x0008);
    assert_int_equal(info->device_type, 2);
    assert_int_equal(info->device_handle, 0x0010);
    assert_int_equal(info->phy, 5);

    dmi_entity_destroy(entity);
}

static void test_sun_pcie_root_complex_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Bus 3Ah, device 3, function 1
    static const uint8_t data[] = {
        138, 0x08, 0x00, 0x8A,
        0x02, 0x00, 0x19, 0x3A,
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_sun_pcie_root_complex_t *info = dmi_entity_info(entity, DMI_TYPE(SUN_PCIE_ROOT_COMPLEX));
    assert_non_null(info);

    assert_int_equal(info->baseboard_handle, 0x0002);
    assert_int_equal(info->bus_number, 0x3A);
    assert_int_equal(info->device_number, 3);
    assert_int_equal(info->function_number, 1);

    dmi_entity_destroy(entity);
}

static void test_sun_memory_array_ex_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t data[] = {
        144, 0x0A, 0x00, 0x90,
        0x10, 0x00, 0x04, 0x00, 0xC2, 0x00,
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_sun_memory_array_ex_t *info = dmi_entity_info(entity, DMI_TYPE(SUN_MEMORY_ARRAY_EX));
    assert_non_null(info);

    assert_int_equal(info->memory_array_handle, 0x0010);
    assert_int_equal(info->component_handle, 0x0004);
    assert_int_equal(info->bus_number, 0x00);
    assert_int_equal(info->device_number, 0x18);
    assert_int_equal(info->function_number, 2);

    dmi_entity_destroy(entity);
}

static void test_sun_memory_device_ex_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t data[] = {
        145, 0x0A, 0x00, 0x91,
        0x11, 0x00, 0x01, 0x02, 0x00, 0x01,
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    const dmi_sun_memory_device_ex_t *info = dmi_entity_info(entity, DMI_TYPE(SUN_MEMORY_DEVICE_EX));
    assert_non_null(info);

    assert_int_equal(info->memory_device_handle, 0x0011);
    assert_int_equal(info->dram_channel, 1);
    assert_int_equal(info->chip_select_total, 2);
    assert_int_equal(info->chip_select_count, 2);
    assert_int_equal(info->chip_selects[0], 0);
    assert_int_equal(info->chip_selects[1], 1);

    dmi_entity_destroy(entity);
}
