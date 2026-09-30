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

#include <opendmi/anonymize.h>
#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/internal.h>
#include <opendmi/module.h>
#include <opendmi/module/ami.h>
#include <opendmi/registry.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/ami/firewire-guid.h>

static int test_setup(void **pstate);
static int test_teardown(void **pstate);

static void test_firewire_guid_decode(void **pstate);
static void test_firewire_guid_private(void **pstate);
static void test_firewire_guid_signature(void **pstate);

static dmi_entity_t *test_entity(dmi_context_t *context);
static const dmi_attribute_t *test_attribute(const dmi_entity_t *entity, const char *code);

static const char *test_asus_path = OPENDMI_TEST_DATA "/asus/m5a97-pro.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_firewire_guid_decode, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_firewire_guid_private, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_firewire_guid_signature, test_setup, test_teardown)
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

static void test_firewire_guid_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_asus_path));
    assert_true(dmi_has_extension(context, &dmi_ami_module));

    dmi_entity_t *entity = test_entity(context);
    const dmi_ami_firewire_guid_t *info = dmi_entity_info(entity, DMI_TYPE(ami_firewire_guid));
    assert_non_null(info);

    assert_int_equal(info->signature.length, 8);
    assert_memory_equal(info->signature.data, "\xFE\xDC\xBA\x98\x76\x54\x32\x10", 8);
    assert_string_equal(info->name, "V1394GUID");
    assert_int_equal(info->unknown, 0);

    // Data begins like the bus information block of IEEE 1394: the lengths
    // of the block and of its CRC, which are no identifiers
    assert_int_equal(info->data.length, 40);
    assert_memory_equal(info->data.data, "\x04\x04", 2);
}

//
// Data holds the GUID of the controller, which identifies the board, and is
// replaced in the anonymized copy of the table
//
static void test_firewire_guid_private(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_asus_path));

    dmi_entity_t *entity = test_entity(context);
    const dmi_ami_firewire_guid_t *info = dmi_entity_info(entity, DMI_TYPE(ami_firewire_guid));
    assert_non_null(info);

    assert_true(test_attribute(entity, "data")->params.flags & DMI_ATTRIBUTE_FLAG_PRIVATE);

    dmi_buffer_t *table = dmi_buffer_create(context);
    assert_non_null(table);
    assert_true(dmi_anonymize(context, table));

    // Copy keeps the signature, and holds other data
    size_t offset = (size_t)(info->data.data - context->state.table->data);
    assert_true(offset + info->data.length <= table->length);
    assert_memory_equal(table->data + offset - 8, info->signature.data, 8);
    assert_memory_not_equal(table->data + offset, info->data.data, info->data.length);

    dmi_buffer_destroy(table);
}

static void test_firewire_guid_signature(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_ami_module));

    // Structure of type 139 without the signature is not decoded
    uint8_t data[0x38] = {
        139, 0x36, 0x34, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_null(entity->spec);
    dmi_entity_destroy(entity);

    // ...while the one with it is
    memcpy(data + 0x04, "\xFE\xDC\xBA\x98\x76\x54\x32\x10", 8);

    entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_ptr_equal(entity->spec, &dmi_ami_firewire_guid_spec);
    dmi_entity_destroy(entity);

    dmi_buffer_destroy(buffer);
}

static dmi_entity_t *test_entity(dmi_context_t *context)
{
    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(ami_firewire_guid), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_ami_firewire_guid_spec);

    return entity;
}

static const dmi_attribute_t *test_attribute(const dmi_entity_t *entity, const char *code)
{
    for (const dmi_attribute_t *attr = entity->spec->attributes; attr->params.name != nullptr; attr++) {
        if (strcmp(attr->params.code, code) == 0)
            return attr;
    }

    fail_msg("No attribute %s", code);
}
