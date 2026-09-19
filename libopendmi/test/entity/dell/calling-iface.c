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
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/module.h>
#include <opendmi/module/dell.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/dell/calling-iface.h>

static int test_dell_calling_iface_setup(void **pstate);
static int test_dell_calling_iface_teardown(void **pstate);

static void test_dell_calling_iface_decode(void **pstate);
static void test_dell_calling_iface_decode_truncated(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_dell_calling_iface_decode, test_dell_calling_iface_setup, test_dell_calling_iface_teardown),
        cmocka_unit_test_setup_teardown(test_dell_calling_iface_decode_truncated, test_dell_calling_iface_setup, test_dell_calling_iface_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_dell_calling_iface_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_add_extension(context, dmi_module_find("dell"))) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_dell_calling_iface_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_dell_calling_iface_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Tokens of Dell Precision 490, with an unused token added
    static const uint8_t data[] = {
        218, 0x23, 0x00, 0xDA,                  // Header
        0xB2, 0x00, 0x17,                       // Command I/O address and code
        0x0B, 0x0E, 0x38, 0x00,                 // Supported commands
        0x00, 0x80, 0x00, 0x80, 0x01, 0x00,     // Token
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Unused token
        0x02, 0x80, 0x02, 0x80, 0x01, 0x00,     // Token
        0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,     // End-of-table marker
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    const dmi_dell_calling_iface_t *info = dmi_entity_info(entity, DMI_TYPE(DELL_CALLING_IFACE));
    assert_non_null(info);

    assert_int_equal(info->cmd_io_address, 0xB2);
    assert_int_equal(info->cmd_io_code, 0x17);
    assert_int_equal(info->supported_cmds, 0x00380E0B);

    assert_int_equal(info->token_count, 2);
    assert_int_equal(info->tokens[0].id, 0x8000);
    assert_int_equal(info->tokens[0].location, 0x8000);
    assert_int_equal(info->tokens[0].value, 0x0001);
    assert_int_equal(info->tokens[1].id, 0x8002);

    dmi_entity_destroy(entity);
}

static void test_dell_calling_iface_decode_truncated(void **pstate)
{
    dmi_context_t *context = *pstate;

    // The last token is truncated
    static const uint8_t data[] = {
        218, 0x14, 0x00, 0xDA,
        0xB2, 0x00, 0x17,
        0x00, 0x0E, 0x20, 0x00,
        0x00, 0x80, 0x00, 0x80, 0x01, 0x00,
        0x02, 0x80, 0x02,
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    const dmi_dell_calling_iface_t *info = dmi_entity_info(entity, DMI_TYPE(DELL_CALLING_IFACE));
    assert_non_null(info);
    assert_int_equal(info->token_count, 1);

    dmi_entity_destroy(entity);
}
