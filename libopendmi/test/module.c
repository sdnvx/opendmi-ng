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
#include <opendmi/module.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-network-card.h>
#include <opendmi/entity/intel/rsd-processor-cpuid.h>

static void test_module_unique_types(void **pstate);
static void test_module_intel_types(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_module_unique_types),
        cmocka_unit_test(test_module_intel_types)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
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
