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

#include <opendmi/entity/dell/token-refs.h>
#include <opendmi/entity/hpe/nic.h>

#include <opendmi/entity/intel/fvi.h>

static int test_fvi_setup(void **pstate);
static int test_fvi_teardown(void **pstate);

static void test_fvi_decode(void **pstate);
static void test_fvi_unspecified(void **pstate);
static void test_fvi_truncated(void **pstate);
static void test_fvi_platform(void **pstate);
static void test_fvi_dell(void **pstate);
static void test_fvi_proliant(void **pstate);

static const char *test_asrock_path = OPENDMI_TEST_DATA "/asrock/b460-pro4.bin";
static const char *test_dell_path = OPENDMI_TEST_DATA "/dell/g15-5510.bin";
static const char *test_proliant_path = OPENDMI_TEST_DATA "/hp/proliant-dl360-g6-484184-b21.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_fvi_decode, test_fvi_setup, test_fvi_teardown),
        cmocka_unit_test_setup_teardown(test_fvi_unspecified, test_fvi_setup, test_fvi_teardown),
        cmocka_unit_test_setup_teardown(test_fvi_truncated, test_fvi_setup, test_fvi_teardown),
        cmocka_unit_test_setup_teardown(test_fvi_platform, test_fvi_setup, test_fvi_teardown),
        cmocka_unit_test_setup_teardown(test_fvi_dell, test_fvi_setup, test_fvi_teardown),
        cmocka_unit_test_setup_teardown(test_fvi_proliant, test_fvi_setup, test_fvi_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_fvi_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_fvi_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_fvi_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_intel_module));

    // Management Engine versions of Acer Nitro AN515-31
    static const uint8_t data[] = {
        221, 0x1A, 0x19, 0x00,
        0x03,
        0x01, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x04, 0x0B, 0x08, 0x32, 0x61, 0x0D,
        'R', 'e', 'f', 'e', 'r', 'e', 'n', 'c', 'e', ' ', 'C', 'o', 'd', 'e', ' ',
        '-', ' ', 'M', 'E', ' ', '1', '1', '.', '0', 0x00,
        'M', 'E', 'B', 'x', ' ', 'v', 'e', 'r', 's', 'i', 'o', 'n', 0x00,
        'M', 'E', ' ', 'F', 'i', 'r', 'm', 'w', 'a', 'r', 'e', ' ',
        'V', 'e', 'r', 's', 'i', 'o', 'n', 0x00,
        'C', 'o', 'n', 's', 'u', 'm', 'e', 'r', ' ', 'S', 'K', 'U', 0x00,
        0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_fvi_t *info = dmi_entity_info(entity, DMI_TYPE(intel_fvi));
    assert_non_null(info);
    assert_int_equal(info->item_count, 3);

    // First item is the version of the reference code module
    assert_string_equal(info->items[0].component, "Reference Code - ME 11.0");
    assert_null(info->items[0].version_string);
    assert_int_equal(info->items[0].major, 2);
    assert_int_equal(info->items[0].minor, 2);
    assert_int_equal(info->items[0].revision, 0);
    assert_int_equal(info->items[0].build, 0);

    assert_string_equal(info->items[1].component, "MEBx version");

    // Version string tells the SKU of the firmware
    assert_string_equal(info->items[2].component, "ME Firmware Version");
    assert_string_equal(info->items[2].version_string, "Consumer SKU");
    assert_int_equal(info->items[2].major, 11);
    assert_int_equal(info->items[2].minor, 8);
    assert_int_equal(info->items[2].revision, 50);
    assert_int_equal(info->items[2].build, 3425);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_fvi_unspecified(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_intel_module));

    // Components carrying a state or a single number leave the rest of the
    // version unspecified
    static const uint8_t data[] = {
        221, 0x13, 0x10, 0x00,
        0x02,
        0x01, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x03, 0x00, 0xFF, 0xFF, 0xFF, 0x21, 0x00,
        'P', 'C', 'H', '-', 'C', 'R', 'I', 'D', ' ', 'S', 't', 'a', 't', 'u', 's', 0x00,
        'D', 'i', 's', 'a', 'b', 'l', 'e', 'd', 0x00,
        'P', 'C', 'H', '-', 'C', 'R', 'I', 'D', ' ', 'O', 'r', 'i', 'g', 'i', 'n', 'a', 'l', ' ',
        'V', 'a', 'l', 'u', 'e', 0x00,
        0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_fvi_t *info = dmi_entity_info(entity, DMI_TYPE(intel_fvi));
    assert_non_null(info);
    assert_int_equal(info->item_count, 2);

    assert_string_equal(info->items[0].version_string, "Disabled");
    assert_int_equal(info->items[0].major, UINT8_MAX);
    assert_int_equal(info->items[0].minor, UINT8_MAX);
    assert_int_equal(info->items[0].revision, UINT8_MAX);
    assert_int_equal(info->items[0].build, UINT16_MAX);

    assert_int_equal(info->items[1].major, UINT8_MAX);
    assert_int_equal(info->items[1].build, 0x21);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_fvi_truncated(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_intel_module));

    // Two items are declared, but only one is present
    static const uint8_t data[] = {
        221, 0x0C, 0x10, 0x00,
        0x02,
        0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
        'F', 'S', 'P', ' ', 'B', 'i', 'n', 'a', 'r', 'y', ' ',
        'V', 'e', 'r', 's', 'i', 'o', 'n', 0x00,
        0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    const dmi_intel_fvi_t *info = dmi_entity_info(entity, DMI_TYPE(intel_fvi));
    assert_non_null(info);
    assert_int_equal(info->item_count, 1);
    assert_string_equal(info->items[0].component, "FSP Binary Version");
    assert_int_equal(info->items[0].major, 4);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_fvi_platform(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Structures of the Intel reference code are decoded whatever the vendor
    // of the firmware is
    assert_true(dmi_load(context, test_asrock_path));
    assert_true(dmi_has_extension(context, &dmi_intel_module));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_fvi), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_intel_fvi_spec);

    const dmi_intel_fvi_t *info = dmi_entity_info(entity, DMI_TYPE(intel_fvi));
    assert_non_null(info);
    assert_int_equal(info->item_count, 3);
    assert_string_equal(info->items[0].component, "Reference Code - CPU");
    assert_string_equal(info->items[1].component, "uCode Version");
    assert_int_equal(info->items[1].build, 0xCC);
}

static void test_fvi_dell(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Dell places the structure at type 205, and gives type 221 to a structure
    // of its own
    assert_true(dmi_load(context, test_dell_path));

    dmi_registry_t *registry = dmi_get_registry(context);

    dmi_entity_t *entity = dmi_registry_lookup_first(registry, DMI_TYPE(intel_fvi), false);
    assert_non_null(entity);
    assert_int_equal(dmi_entity_type_id(entity), 205);
    assert_ptr_equal(entity->spec, &dmi_intel_fvi_spec);

    const dmi_intel_fvi_t *info = dmi_entity_info(entity, DMI_TYPE(intel_fvi));
    assert_non_null(info);
    assert_int_equal(info->item_count, 1);
    assert_string_equal(info->items[0].component, "BIOS Guard");

    entity = dmi_registry_lookup_first_id(registry, DMI_TYPE_ID(INTEL_FVI), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_dell_token_refs_2_spec);
}

static void test_fvi_proliant(void **pstate)
{
    dmi_context_t *context = *pstate;

    // HP servers give type 221 to a structure of their own
    assert_true(dmi_load(context, test_proliant_path));
    assert_true(dmi_has_extension(context, &dmi_intel_module));
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE_ID(INTEL_FVI)), &dmi_hpe_iscsi_nic_spec);

    dmi_entity_t *entity = dmi_registry_lookup_first_id(dmi_get_registry(context), DMI_TYPE_ID(INTEL_FVI), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_hpe_iscsi_nic_spec);
}
