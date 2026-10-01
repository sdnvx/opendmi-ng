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
#include <opendmi/registry.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel/platform.h>

static int test_setup(void **pstate);
static int test_teardown(void **pstate);

static void test_platform_bay_trail(void **pstate);
static void test_platform_server(void **pstate);

static const char *test_ideapad_path = OPENDMI_TEST_DATA "/lenovo/ideapad-100s-11iby-80r2.bin";
static const char *test_server_path = OPENDMI_TEST_DATA "/intel/s2600wtt.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_platform_bay_trail, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_platform_server, test_setup, test_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_platform_bay_trail(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_ideapad_path));

    dmi_registry_t *registry = dmi_get_registry(context);
    const dmi_entity_t *entity = dmi_registry_lookup(registry, 0x0022, DMI_TYPE(intel_platform), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_intel_platform_spec);

    const dmi_intel_platform_t *info = dmi_entity_info(entity, DMI_TYPE(intel_platform));
    assert_non_null(info);

    // Versions of the components of Intel Atom Z3735F, in the order of the
    // reference code
    assert_string_equal(info->gop_version, "7.2.1013");
    assert_string_equal(info->microcode_version, "832");
    assert_string_equal(info->mrc_version, "1.02");
    assert_string_equal(info->sec_version, "1.2.0.1149");
    assert_string_equal(info->ulpmc_version, "Non ULPMC!!");
    assert_string_equal(info->pmc_version, "0x4_45");
    assert_string_equal(info->punit_version, "0x27");
    assert_string_equal(info->soc_version, "0F (C0 Stepping)");
    assert_string_equal(info->board_version, "BAY LAKE CR (6)");
    assert_string_equal(info->cpu_flavor, "VLV-QC Notebook (3)");
    assert_string_equal(info->bios_version, "E2CN13WW");
    assert_string_equal(info->pmic_version, "41.01");
    assert_string_equal(info->touch_version, "NA");

    // Settings follow the versions
    assert_string_equal(info->secure_boot, "0");
    assert_string_equal(info->max_cstate, "3");
    assert_string_equal(info->rc6, "1");
}

//
// Intel server boards give type 148 to structures of another layout
//
static void test_platform_server(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_server_path));

    dmi_registry_t *registry = dmi_get_registry(context);
    const dmi_entity_t *entity = dmi_registry_lookup_first_id(registry, 148, false);
    assert_non_null(entity);
    assert_int_equal(entity->body_length, 0x30);
    assert_null(entity->spec);
}
