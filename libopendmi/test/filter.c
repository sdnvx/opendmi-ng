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
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/filter.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>
#include <opendmi/module.h>
#include <opendmi/module/hpe.h>
#include <opendmi/module/intel.h>
#include <opendmi/module/lenovo.h>
#include <opendmi/test/logger.h>

static int test_filter_setup(void **pstate);
static int test_filter_teardown(void **pstate);

static void test_filter_empty(void **pstate);
static void test_filter_handle(void **pstate);
static void test_filter_type(void **pstate);
static void test_filter_module(void **pstate);
static void test_filter_arguments(void **pstate);

static const dmi_entity_t *test_filter_entity(dmi_context_t *context, dmi_handle_t handle,
                                              const dmi_type_t *type);
static size_t test_filter_count(dmi_context_t *context, dmi_filter_t *filter);

// ThinkPad with an Intel processor, which carries the structures of both
// vendors, two of them at the same type number
static const char *test_dump_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-t14-g3-21aj.bin";

// Handles of the structures of the dump. Intel vPro information and Lenovo
// ThinkVantage Technologies enablement are both found at type 131
static const dmi_handle_t test_device_handle   = 0x0002;
static const dmi_handle_t test_system_handle   = 0x001C;
static const dmi_handle_t test_inactive_handle = 0x0023;
static const dmi_handle_t test_vpro_handle     = 0x0038;
static const dmi_handle_t test_tvt_handle      = 0x0041;

// Number of memory devices in the dump
static const size_t test_device_count = 8;

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_filter_empty),
        cmocka_unit_test(test_filter_handle),
        cmocka_unit_test(test_filter_type),
        cmocka_unit_test(test_filter_module),
        cmocka_unit_test(test_filter_arguments)
    };

    return cmocka_run_group_tests(tests, test_filter_setup, test_filter_teardown);
}

static int test_filter_setup(void **pstate)
{
    dmi_context_t *context;

    // Structures of the vendors are decoded by the modules of the platform
    context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_load(context, test_dump_path)) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_filter_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_filter_empty(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_entity_t *system   = test_filter_entity(context, test_system_handle, DMI_TYPE(system));
    const dmi_entity_t *inactive = test_filter_entity(context, test_inactive_handle, DMI_TYPE(inactive));

    // Filter without constraints is empty, and matches every structure
    dmi_filter_t *filter = dmi_filter_create(context);

    assert_non_null(filter);
    assert_true(dmi_filter_is_empty(filter));
    assert_true(dmi_filter_match(filter, system));
    assert_true(dmi_filter_match(filter, inactive));
    assert_int_equal(test_filter_count(context, filter), dmi_get_registry(context)->count);

    // Mask is not a constraint of the lists: the filter stays empty, and
    // matches the structures of the categories the mask lets through
    filter->mask = DMI_FILTER_MASK_DEFAULT;

    assert_true(dmi_filter_is_empty(filter));
    assert_true(dmi_filter_match(filter, system));
    assert_false(dmi_filter_match(filter, inactive));

    dmi_filter_destroy(filter);
}

static void test_filter_handle(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_entity_t *device = test_filter_entity(context, test_device_handle, DMI_TYPE(memory_device));
    const dmi_entity_t *system = test_filter_entity(context, test_system_handle, DMI_TYPE(system));

    dmi_filter_t *filter = dmi_filter_create(context);
    assert_non_null(filter);

    // Handle narrows the match down to its structure
    assert_true(dmi_filter_add_handle(filter, test_device_handle));
    assert_false(dmi_filter_is_empty(filter));
    assert_true(dmi_filter_match(filter, device));
    assert_false(dmi_filter_match(filter, system));
    assert_int_equal(test_filter_count(context, filter), 1);

    // Handles are alternatives, and the one no structure has matches nothing
    assert_true(dmi_filter_add_handle(filter, test_system_handle));
    assert_true(dmi_filter_add_handle(filter, 0x1234));
    assert_true(dmi_filter_match(filter, device));
    assert_true(dmi_filter_match(filter, system));
    assert_int_equal(test_filter_count(context, filter), 2);

    // Structure of the handle is still required to meet the mask
    filter->mask = DMI_FILTER_MASK_OEM;
    assert_false(dmi_filter_match(filter, device));

    dmi_filter_destroy(filter);
}

static void test_filter_type(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_entity_t *device = test_filter_entity(context, test_device_handle, DMI_TYPE(memory_device));
    const dmi_entity_t *system = test_filter_entity(context, test_system_handle, DMI_TYPE(system));
    const dmi_entity_t *vpro   = test_filter_entity(context, test_vpro_handle, DMI_TYPE(intel_vpro));
    const dmi_entity_t *tvt    = test_filter_entity(context, test_tvt_handle, DMI_TYPE(lenovo_tvt));

    dmi_filter_t *filter = dmi_filter_create(context);
    assert_non_null(filter);

    // Type number narrows the match down to the structures found at it
    assert_true(dmi_filter_add_type(filter, DMI_TYPE_ID(MEMORY_DEVICE)));
    assert_false(dmi_filter_is_empty(filter));
    assert_true(dmi_filter_match(filter, device));
    assert_false(dmi_filter_match(filter, system));
    assert_int_equal(test_filter_count(context, filter), test_device_count);

    // Type numbers are alternatives, and the structures found at one are
    // matched whatever specifications they are decoded by
    assert_true(dmi_filter_add_type(filter, DMI_TYPE_ID(INTEL_VPRO)));
    assert_true(dmi_filter_match(filter, vpro));
    assert_true(dmi_filter_match(filter, tvt));
    assert_int_equal(test_filter_count(context, filter), test_device_count + 2);

    dmi_filter_destroy(filter);
}

static void test_filter_module(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_entity_t *system = test_filter_entity(context, test_system_handle, DMI_TYPE(system));
    const dmi_entity_t *vpro   = test_filter_entity(context, test_vpro_handle, DMI_TYPE(intel_vpro));
    const dmi_entity_t *tvt    = test_filter_entity(context, test_tvt_handle, DMI_TYPE(lenovo_tvt));

    dmi_filter_t *filter = dmi_filter_create(context);
    assert_non_null(filter);

    // Module which is not enabled in the context decodes no structure, and
    // matches nothing
    assert_true(dmi_filter_add_module(filter, &dmi_hpe_module));
    assert_false(dmi_filter_is_empty(filter));
    assert_int_equal(test_filter_count(context, filter), 0);

    // Module narrows the match down to the structures decoded by its
    // specifications, which tells the ones sharing a type number apart
    assert_true(dmi_filter_add_module(filter, &dmi_intel_module));
    assert_true(dmi_filter_match(filter, vpro));
    assert_false(dmi_filter_match(filter, tvt));
    assert_false(dmi_filter_match(filter, system));

    // Modules are alternatives
    assert_true(dmi_filter_add_module(filter, &dmi_lenovo_module));
    assert_true(dmi_filter_match(filter, vpro));
    assert_true(dmi_filter_match(filter, tvt));
    assert_false(dmi_filter_match(filter, system));

    // ...and so are the lists: an entry of any of them is enough
    assert_true(dmi_filter_add_handle(filter, test_system_handle));
    assert_true(dmi_filter_match(filter, system));
    assert_true(dmi_filter_match(filter, tvt));

    dmi_filter_destroy(filter);
}

static void test_filter_arguments(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_entity_t *system = test_filter_entity(context, test_system_handle, DMI_TYPE(system));

    dmi_filter_t *filter = dmi_filter_create(context);
    assert_non_null(filter);

    // Nothing is added without a filter
    assert_false(dmi_filter_add_handle(nullptr, test_system_handle));
    assert_false(dmi_filter_add_type(nullptr, DMI_TYPE_ID(SYSTEM)));

    // Nothing is matched without a structure
    dmi_error_clear(context);
    assert_false(dmi_filter_match(filter, nullptr));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_ARGUMENT_NULL);

    // Missing filter is not an error: it has no entries, and matches nothing
    dmi_error_clear(context);
    assert_true(dmi_filter_is_empty(nullptr));
    assert_false(dmi_filter_match(nullptr, system));
    assert_null(dmi_error_peek_last(context));

    dmi_filter_destroy(filter);
}

//
// Get a structure of the dump by its handle, checking that it is of the type
// the tests take it for
//
static const dmi_entity_t *test_filter_entity(dmi_context_t *context, dmi_handle_t handle,
                                              const dmi_type_t *type)
{
    const dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(context), handle, type, false);
    assert_non_null(entity);

    return entity;
}

//
// Count the structures of the context an iterator yields with a filter
//
static size_t test_filter_count(dmi_context_t *context, dmi_filter_t *filter)
{
    dmi_registry_iter_t iter;
    size_t count = 0;

    assert_true(dmi_registry_iter_initialize(&iter, dmi_get_registry(context), filter));

    while (dmi_registry_iter_next(&iter) != nullptr)
        count++;

    return count;
}
