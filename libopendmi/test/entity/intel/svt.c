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

#include <opendmi/entity/intel/svt.h>

static int test_svt_setup(void **pstate);
static int test_svt_teardown(void **pstate);

static void test_svt_decode(void **pstate);
static void test_svt_dell(void **pstate);
static void test_svt_aligned(void **pstate);
static void test_svt_proliant(void **pstate);

static const char *test_acer_path     = OPENDMI_TEST_DATA "/acer/nitro-an515-31.bin";
static const char *test_dell_path     = OPENDMI_TEST_DATA "/dell/g15-5510.bin";
static const char *test_xps_path      = OPENDMI_TEST_DATA "/dell/xps-13-9365.bin";
static const char *test_proliant_path = OPENDMI_TEST_DATA "/hp/proliant-bl460c-g1-416656-b21.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_svt_decode, test_svt_setup, test_svt_teardown),
        cmocka_unit_test_setup_teardown(test_svt_dell, test_svt_setup, test_svt_teardown),
        cmocka_unit_test_setup_teardown(test_svt_aligned, test_svt_setup, test_svt_teardown),
        cmocka_unit_test_setup_teardown(test_svt_proliant, test_svt_setup, test_svt_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_svt_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_svt_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_svt_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_acer_path));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_svt), false);
    assert_non_null(entity);

    const dmi_intel_svt_t *info = dmi_entity_info(entity, DMI_TYPE(intel_svt));
    assert_non_null(info);
    assert_int_equal(info->version, 1);
    assert_int_equal(info->parameter, 0x0099);
    assert_int_equal(info->milestone_count, 3);

    assert_int_equal(info->milestones[0].code, 0x10);
    assert_string_equal(info->milestones[0].name, "Memory Init Complete");
    assert_int_equal(info->milestones[1].code, 0x20);
    assert_string_equal(info->milestones[1].name, "End of DXE Phase");
    assert_int_equal(info->milestones[2].code, 0x30);
    assert_string_equal(info->milestones[2].name, "BIOS Boot Complete");
}

static void test_svt_dell(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Dell places the structure at type 206, and gives type 222 to a structure
    // of its own
    assert_true(dmi_load(context, test_dell_path));

    dmi_registry_t *registry = dmi_get_registry(context);

    dmi_entity_t *entity = dmi_registry_lookup_first(registry, DMI_TYPE(intel_svt), false);
    assert_non_null(entity);
    assert_int_equal(dmi_entity_type_id(entity), 206);
    assert_ptr_equal(entity->spec, &dmi_intel_svt_spec);

    entity = dmi_registry_lookup_first_id(registry, DMI_TYPE_ID(INTEL_SVT), false);
    assert_non_null(entity);
    assert_null(entity->spec);
}

static void test_svt_aligned(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Firmware which does not pack the structure aligns the parameter to two
    // bytes, and ends the structure with a byte which makes its length even
    assert_true(dmi_load(context, test_xps_path));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_svt), false);
    assert_non_null(entity);
    assert_int_equal(dmi_entity_type_id(entity), 206);
    assert_ptr_equal(entity->spec, &dmi_intel_svt_aligned_spec);

    const dmi_intel_svt_t *info = dmi_entity_info(entity, DMI_TYPE(intel_svt));
    assert_non_null(info);
    assert_int_equal(info->version, 1);
    assert_int_equal(info->parameter, 0x0099);
    assert_int_equal(info->milestone_count, 3);
    assert_int_equal(info->milestones[2].code, 0x30);
    assert_string_equal(info->milestones[2].name, "BIOS Boot Complete");
}

static void test_svt_proliant(void **pstate)
{
    dmi_context_t *context = *pstate;

    // HP servers give type 222 to a structure of their own
    assert_true(dmi_load(context, test_proliant_path));

    dmi_entity_t *entity = dmi_registry_lookup_first_id(dmi_get_registry(context), DMI_TYPE_ID(INTEL_SVT), false);
    assert_non_null(entity);
    assert_null(entity->spec);
}
