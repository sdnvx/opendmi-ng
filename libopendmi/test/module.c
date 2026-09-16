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
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/module.h>
#include <opendmi/module/intel.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel/rsd-network-card.h>
#include <opendmi/entity/intel/rsd-processor-cpuid.h>

static int test_module_setup(void **pstate);
static int test_module_teardown(void **pstate);

static void test_module_unique_types(void **pstate);
static void test_module_intel_types(void **pstate);
static void test_module_add_extension(void **pstate);
static void test_module_add_extension_duplicate(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_module_unique_types),
        cmocka_unit_test(test_module_intel_types),
        cmocka_unit_test_setup_teardown(test_module_add_extension, test_module_setup, test_module_teardown),
        cmocka_unit_test_setup_teardown(test_module_add_extension_duplicate, test_module_setup, test_module_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_module_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_module_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_module_unique_types(void **pstate)
{
    dmi_unused(pstate);

    assert_non_null(dmi_modules);

    for (const dmi_module_t *module = dmi_modules; module != nullptr; module = module->next) {
        if (module->entities == nullptr)
            continue;

        for (const dmi_entity_spec_t **pspec = module->entities; *pspec != nullptr; pspec++) {
            for (const dmi_entity_spec_t **pnext = pspec + 1; *pnext != nullptr; pnext++) {
                if ((*pspec)->type == (*pnext)->type)
                    fail_msg("Module %s: %s and %s have the same type %d", module->code,
                             (*pspec)->code, (*pnext)->code, (int)(*pspec)->type);
            }
        }
    }
}

static void test_module_intel_types(void **pstate)
{
    dmi_unused(pstate);

    assert_int_equal(dmi_intel_rsd_network_card_spec.type, DMI_TYPE(INTEL_RSD_NETWORK_CARD));
    assert_int_equal(dmi_intel_rsd_processor_cpuid_spec.type, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID));
}

static void test_module_add_extension(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_module_t *module = dmi_module_find("intel");
    assert_non_null(module);

    assert_true(dmi_add_extension(context, module));
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE(INTEL_RSD_NETWORK_CARD)),
                     &dmi_intel_rsd_network_card_spec);
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID)),
                     &dmi_intel_rsd_processor_cpuid_spec);

    // The same module cannot be added twice
    dmi_error_clear(context);
    assert_false(dmi_add_extension(context, module));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_MODULE_CONFLICT);
}

static void test_module_add_extension_duplicate(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const dmi_entity_spec_t spec_1 = {
        .type = (dmi_type_t)250,
        .code = "test-1",
        .name = "Test 1"
    };
    static const dmi_entity_spec_t spec_2 = {
        .type = (dmi_type_t)250,
        .code = "test-2",
        .name = "Test 2"
    };

    const dmi_module_t module = {
        .code     = "test",
        .name     = "Test module",
        .entities = (const dmi_entity_spec_t *[]){ &spec_1, &spec_2, nullptr }
    };

    dmi_error_clear(context);
    assert_false(dmi_add_extension(context, &module));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_MODULE_CONFLICT);

    // Type map is not modified on conflicts
    assert_null(dmi_type_spec(context, (dmi_type_t)250));
}
