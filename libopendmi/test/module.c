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
#include <opendmi/module.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-network-card.h>
#include <opendmi/entity/intel/rsd-processor-cpuid.h>

static void test_module_builtin(void **pstate);
static void test_module_register(void **pstate);
static void test_module_unique_types(void **pstate);
static void test_module_intel_types(void **pstate);
static void test_module_unique_attribute_codes(void **pstate);

static void assert_unique_attribute_codes(const char *spec_code, const dmi_attribute_t *attrs);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_module_builtin),
        cmocka_unit_test(test_module_register),
        cmocka_unit_test(test_module_unique_types),
        cmocka_unit_test(test_module_intel_types),
        cmocka_unit_test(test_module_unique_attribute_codes)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_module_builtin(void **pstate)
{
    dmi_unused(pstate);

    static const char *codes[] = {
        "acer", "ami", "apple", "dell", "hpe", "intel", "lenovo", "sun"
    };

    size_t count = 0;
    while (dmi_builtin_modules[count] != nullptr)
        count++;

    assert_int_equal(count, countof(codes));

    // All built-in modules are available, and their codes are unique
    for (size_t i = 0; i < countof(codes); i++) {
        const dmi_module_t *module = dmi_module_find(codes[i]);

        assert_non_null(module);
        assert_string_equal(module->code, codes[i]);
        assert_ptr_equal(module, dmi_builtin_modules[i]);
    }

    assert_null(dmi_module_find("unknown"));
}

static void test_module_register(void **pstate)
{
    dmi_unused(pstate);

    static dmi_module_t external_1 = { .code = "external-1", .name = "External module 1" };
    static dmi_module_t external_2 = { .code = "external-2", .name = "External module 2" };
    static dmi_module_t duplicate  = { .code = "intel",      .name = "Duplicate module"  };

    assert_true(dmi_module_register(&external_1));
    assert_true(dmi_module_register(&external_2));

    // Modules with duplicate codes are not registered
    assert_false(dmi_module_register(&duplicate));
    assert_false(dmi_module_register(&external_1));

    assert_ptr_equal(dmi_module_find("external-1"), &external_1);
    assert_ptr_equal(dmi_module_find("external-2"), &external_2);
    assert_ptr_equal(dmi_module_find("intel"), &dmi_intel_module);

    // Registered modules follow built-in ones in the order of registration
    const dmi_module_t *module = dmi_module_next(nullptr);

    for (size_t i = 0; dmi_builtin_modules[i] != nullptr; i++) {
        assert_ptr_equal(module, dmi_builtin_modules[i]);
        module = dmi_module_next(module);
    }

    assert_ptr_equal(module, &external_1);
    module = dmi_module_next(module);
    assert_ptr_equal(module, &external_2);
    assert_null(dmi_module_next(module));
}

static void test_module_unique_types(void **pstate)
{
    dmi_unused(pstate);

    assert_non_null(dmi_module_next(nullptr));

    for (const dmi_module_t *module = dmi_module_next(nullptr); module != nullptr; module = dmi_module_next(module)) {
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

static void test_module_unique_attribute_codes(void **pstate)
{
    dmi_unused(pstate);

    // Core entities
    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);

    for (size_t type = 0; type <= DMI_TYPE_MAX; type++) {
        const dmi_entity_spec_t *spec = context->type_map[type];
        if (spec != nullptr)
            assert_unique_attribute_codes(spec->code, spec->attributes);
    }

    dmi_destroy(context);

    // Module entities
    for (const dmi_module_t *module = dmi_module_next(nullptr); module != nullptr; module = dmi_module_next(module)) {
        if (module->entities == nullptr)
            continue;

        for (const dmi_entity_spec_t **pspec = module->entities; *pspec != nullptr; pspec++)
            assert_unique_attribute_codes((*pspec)->code, (*pspec)->attributes);
    }
}

static void assert_unique_attribute_codes(const char *spec_code, const dmi_attribute_t *attrs)
{
    if (attrs == nullptr)
        return;

    for (const dmi_attribute_t *attr = attrs; attr->params.name != nullptr; attr++) {
        if (attr->params.code == nullptr)
            fail_msg("%s: attribute %s has no code", spec_code, attr->params.name);

        for (const dmi_attribute_t *next = attr + 1; next->params.name != nullptr; next++) {
            if ((next->params.code != nullptr) and (strcmp(attr->params.code, next->params.code) == 0))
                fail_msg("%s: duplicate attribute code %s", spec_code, attr->params.code);
        }

        // Structure attributes have their own namespace
        assert_unique_attribute_codes(spec_code, attr->params.attrs);
    }
}
