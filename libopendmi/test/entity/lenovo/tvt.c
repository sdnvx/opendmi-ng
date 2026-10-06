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
#include <opendmi/module/lenovo.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel/vpro.h>
#include <opendmi/entity/lenovo/tvt.h>

static int test_tvt_setup(void **pstate);
static int test_tvt_teardown(void **pstate);

static void test_tvt_shared_type(void **pstate);
static void test_tvt_unknown(void **pstate);
static void test_tvt_diagnostics(void **pstate);

static const char *test_x280_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-x280-20kf.bin";
static const char *test_t61p_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-t61p-6460-6xg.bin";
static const char *test_m720q_path = OPENDMI_TEST_DATA "/lenovo/thinkcentre-m720q-10t8.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_tvt_shared_type, test_tvt_setup, test_tvt_teardown),
        cmocka_unit_test_setup_teardown(test_tvt_unknown, test_tvt_setup, test_tvt_teardown),
        cmocka_unit_test_setup_teardown(test_tvt_diagnostics, test_tvt_setup, test_tvt_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_tvt_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_tvt_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

// Structures of Intel and of Lenovo share type 131 in the same table, and
// each is decoded by its own specification
static void test_tvt_shared_type(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_x280_path));

    dmi_registry_iter_t iter;
    dmi_registry_iter_initialize(&iter, dmi_get_registry(context), nullptr);

    const dmi_entity_t *tvt_entity  = nullptr;
    const dmi_entity_t *vpro_entity = nullptr;

    dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        if (entity->type_id != 131)
            continue;

        if (entity->spec == &dmi_lenovo_tvt_spec)
            tvt_entity = entity;
        else if (entity->spec == &dmi_intel_vpro_spec)
            vpro_entity = entity;
        else
            fail_msg("Structure 0x%04x of type 131 is not decoded", entity->handle);
    }

    const dmi_lenovo_tvt_t *tvt = dmi_entity_info(tvt_entity, DMI_TYPE(lenovo_tvt));
    assert_non_null(tvt);
    assert_int_equal(tvt->version, 1);
    assert_string_equal(tvt->signature, "TVT-Enablement");
    assert_int_equal(tvt->features.length, 16);
    assert_false(tvt->is_diagnostics);

    const dmi_intel_vpro_t *vpro = dmi_entity_info(vpro_entity, DMI_TYPE(intel_vpro));
    assert_non_null(vpro);
    assert_int_equal(vpro->me_version.major, 11);
}

// Structures no signature matches are left undecoded
static void test_tvt_unknown(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_t61p_path));

    dmi_registry_iter_t iter;
    dmi_registry_iter_initialize(&iter, dmi_get_registry(context), nullptr);

    size_t tvt     = 0;
    size_t unknown = 0;

    const dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        if (entity->type_id != 131)
            continue;

        if (entity->spec == &dmi_lenovo_tvt_spec)
            tvt++;
        else if (entity->spec == nullptr)
            unknown++;
    }

    assert_int_equal(tvt, 1);
    assert_int_equal(unknown, 1);
}

// Bit 127 of the features tells whether the diagnostics are available
static void test_tvt_diagnostics(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_m720q_path));

    dmi_registry_iter_t iter;
    dmi_registry_iter_initialize(&iter, dmi_get_registry(context), nullptr);

    const dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        if (entity->spec != &dmi_lenovo_tvt_spec)
            continue;

        const dmi_lenovo_tvt_t *info = dmi_entity_info(entity, DMI_TYPE(lenovo_tvt));
        assert_non_null(info);
        assert_true(info->is_diagnostics);
        return;
    }

    fail_msg("Structure is not decoded");
}
