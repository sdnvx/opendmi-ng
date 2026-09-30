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
#include <opendmi/registry.h>
#include <opendmi/test/logger.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/calling-iface.h>
#include <opendmi/entity/dell/revisions.h>
#include <opendmi/entity/dell/system-id.h>
#include <opendmi/entity/dell/token-refs.h>
#include <opendmi/entity/dell/video-rom.h>

static int test_setup(void **pstate);
static int test_teardown(void **pstate);
static const void *test_info(dmi_context_t *context, dmi_handle_t handle, const dmi_entity_spec_t *spec);

static void test_dell_video_rom(void **pstate);
static void test_dell_token_refs(void **pstate);
static void test_dell_system_id(void **pstate);

static const char *test_g15_path = OPENDMI_TEST_DATA "/dell/g15-5510.bin";
static const char *test_poweredge_path = OPENDMI_TEST_DATA "/dell/poweredge-1800.bin";
static const char *test_precision_path = OPENDMI_TEST_DATA "/dell/precision-490.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_dell_video_rom, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_token_refs, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_system_id, test_setup, test_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_dell_video_rom(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_g15_path));

    const dmi_dell_video_rom_t *info = test_info(context, 0xD800, &dmi_dell_video_rom_spec);
    assert_string_equal(info->vendor, "\"Intel Corp.\"");
    assert_string_equal(info->version, "\"2089\"");

    dmi_close(context);

    assert_true(dmi_load(context, test_poweredge_path));

    info = test_info(context, 0xD800, &dmi_dell_video_rom_spec);
    assert_string_equal(info->vendor, "ATI");
    assert_string_equal(info->version, "RADEON 7000 V6.11");
}

static void test_dell_token_refs(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_precision_path));

    const dmi_dell_token_refs_1_t *refs = test_info(context, 0xDC00, &dmi_dell_token_refs_1_spec);
    assert_int_equal(refs->tokens[0], 0xF420);
    assert_int_equal(refs->tokens[2], 0xF410);
    assert_int_equal(refs->tokens[5], 0xF430);
    assert_int_equal(refs->tokens[6], 0xF440);

    const dmi_dell_token_refs_2_t *pairs = test_info(context, 0xDD00, &dmi_dell_token_refs_2_spec);
    assert_int_equal(pairs->unknown_2, 1);
    assert_int_equal(pairs->tokens[1], 0xF510);

    // Tokens are the ones the calling interface defines
    dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(context), 0xDA02,
                                               DMI_TYPE(dell_calling_iface), false);
    assert_non_null(entity);

    const dmi_dell_calling_iface_t *iface = dmi_entity_info(entity, DMI_TYPE(dell_calling_iface));
    assert_non_null(iface);

    bool found = false;
    for (size_t i = 0; i < iface->token_count; i++) {
        if (iface->tokens[i].id == 0xF420)
            found = true;
    }
    assert_true(found);
}

static void test_dell_system_id(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_g15_path));

    // System ID is the one of the revisions and IDs
    const dmi_dell_system_id_t *info = test_info(context, 0x0000, &dmi_dell_system_id_spec);
    assert_string_equal(info->system_id, "0A64");
    assert_memory_equal(info->identifier, "_SID", 4);

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(dell_revisions), false);
    assert_non_null(entity);

    const dmi_dell_revisions_t *revisions = dmi_entity_info(entity, DMI_TYPE(dell_revisions));
    assert_non_null(revisions);
    assert_int_equal(revisions->system_id, strtol(info->system_id, nullptr, 16));
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

static const void *test_info(dmi_context_t *context, dmi_handle_t handle, const dmi_entity_spec_t *spec)
{
    dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(context), handle, DMI_TYPE_ANY, false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, spec);

    const void *info = dmi_entity_info(entity, spec->type);
    assert_non_null(info);

    return info;
}
