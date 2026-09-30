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
#include <opendmi/internal.h>
#include <opendmi/module.h>
#include <opendmi/module/intel.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/hpe/proliant-info.h>

#include <opendmi/entity/intel/mei.h>

static int test_mei_setup(void **pstate);
static int test_mei_teardown(void **pstate);

static void test_mei_decode(void **pstate);
static void test_mei_absent(void **pstate);
static void test_mei_names(void **pstate);
static void test_mei_corporate(void **pstate);
static void test_mei_dell(void **pstate);
static void test_mei_proliant(void **pstate);

static const char *test_elitebook_path = OPENDMI_TEST_DATA "/hp/elitebook-820-g4-x3t22av.bin";
static const char *test_dell_path = OPENDMI_TEST_DATA "/dell/g15-5510.bin";
static const char *test_proliant_path = OPENDMI_TEST_DATA "/hp/proliant-dl360-g6-484184-b21.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_mei_decode, test_mei_setup, test_mei_teardown),
        cmocka_unit_test_setup_teardown(test_mei_absent, test_mei_setup, test_mei_teardown),
        cmocka_unit_test(test_mei_names),
        cmocka_unit_test_setup_teardown(test_mei_corporate, test_mei_setup, test_mei_teardown),
        cmocka_unit_test_setup_teardown(test_mei_dell, test_mei_setup, test_mei_teardown),
        cmocka_unit_test_setup_teardown(test_mei_proliant, test_mei_setup, test_mei_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_mei_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_mei_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_mei_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_intel_module));

    // Management Engine interfaces of Asus Zenbook UX425JA, of which the
    // second one is absent
    static const uint8_t data[] = {
        219, 0x38, 0x6C, 0x00,
        0x01, 0x02,
        0x01,
        0x45, 0x02, 0x00, 0xA0, 0x06, 0x01, 0x85, 0x36,
        0x20, 0x00, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00,
        0x03, 0x1F, 0x00, 0x00, 0xC9, 0x0B, 0x40, 0x44,
        0x02,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        'M', 'E', 'I', '1', 0x00,
        'M', 'E', 'I', '2', 0x00,
        0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_mei_t *info = dmi_entity_info(entity, DMI_TYPE(intel_mei));
    assert_non_null(info);
    assert_int_equal(info->version, 1);
    assert_int_equal(info->device_count, 2);

    assert_string_equal(info->devices[0].name, "MEI1");
    assert_true(info->devices[0].is_present);
    assert_int_equal(info->devices[0].hfsts[0], 0xA0000245);
    assert_int_equal(info->devices[0].hfsts[1], 0x36850106);
    assert_int_equal(info->devices[0].hfsts[2], 0x00000020);
    assert_int_equal(info->devices[0].hfsts[3], 0x00004004);
    assert_int_equal(info->devices[0].hfsts[4], 0x00001F03);
    assert_int_equal(info->devices[0].hfsts[5], 0x44400BC9);

    assert_string_equal(info->devices[1].name, "MEI2");
    assert_false(info->devices[1].is_present);

    // State of the firmware is told by the first interface
    assert_int_equal(info->state, DMI_INTEL_ME_STATE_NORMAL);
    assert_int_equal(info->mode, DMI_INTEL_ME_MODE_NORMAL);
    assert_int_equal(info->error_code, 0);
    assert_true(info->is_init_complete);
    assert_false(info->is_manufacturing);
    assert_int_equal(info->sku, DMI_INTEL_ME_SKU_CONSUMER);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_mei_absent(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_intel_module));

    // State of the firmware is unknown when the first interface is absent
    static const uint8_t data[] = {
        219, 0x1F, 0x6C, 0x00,
        0x01, 0x01,
        0x01,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        'M', 'E', 'I', '1', 0x00,
        0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_mei_t *info = dmi_entity_info(entity, DMI_TYPE(intel_mei));
    assert_non_null(info);
    assert_false(info->devices[0].is_present);
    assert_int_equal(info->state, DMI_INTEL_ME_STATE_UNSPEC);
    assert_int_equal(info->mode, DMI_INTEL_ME_MODE_UNSPEC);
    assert_int_equal(info->error_code, UINT8_MAX);
    assert_int_equal(info->sku, DMI_INTEL_ME_SKU_UNSPEC);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_mei_names(void **pstate)
{
    dmi_unused(pstate);

    assert_string_equal(dmi_intel_me_state_name(DMI_INTEL_ME_STATE_NORMAL), "Normal");
    assert_string_equal(dmi_intel_me_mode_name(DMI_INTEL_ME_MODE_OVERRIDE_JUMPER), "Security override by jumper");
    assert_string_equal(dmi_intel_me_sku_name(DMI_INTEL_ME_SKU_CORPORATE), "Corporate");
}

static void test_mei_corporate(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Firmware of vPro platforms is of the corporate SKU
    assert_true(dmi_load(context, test_elitebook_path));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_mei), false);
    assert_non_null(entity);

    const dmi_intel_mei_t *info = dmi_entity_info(entity, DMI_TYPE(intel_mei));
    assert_non_null(info);
    assert_int_equal(info->device_count, 3);
    assert_int_equal(info->state, DMI_INTEL_ME_STATE_NORMAL);
    assert_int_equal(info->sku, DMI_INTEL_ME_SKU_CORPORATE);
}

static void test_mei_dell(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Dell places the structure at type 203
    assert_true(dmi_load(context, test_dell_path));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_mei), false);
    assert_non_null(entity);
    assert_int_equal(dmi_entity_type_id(entity), 203);
    assert_ptr_equal(entity->spec, &dmi_intel_mei_spec);

    const dmi_intel_mei_t *info = dmi_entity_info(entity, DMI_TYPE(intel_mei));
    assert_non_null(info);
    assert_int_equal(info->device_count, 4);
    assert_int_equal(info->sku, DMI_INTEL_ME_SKU_CONSUMER);
}

static void test_mei_proliant(void **pstate)
{
    dmi_context_t *context = *pstate;

    // HP servers give type 219 to a structure of their own
    assert_true(dmi_load(context, test_proliant_path));

    dmi_entity_t *entity = dmi_registry_lookup_first_id(dmi_get_registry(context), DMI_TYPE_ID(INTEL_MEI), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_hpe_proliant_info_spec);
}
