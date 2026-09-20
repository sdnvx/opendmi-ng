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
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/string-property.h>

//
// SMBIOS table built by the test
//
typedef struct test_table
{
    dmi_data_t data[512];
    size_t     size;
} test_table_t;

static void test_property_linked(void **pstate);
static void test_property_duplicate(void **pstate);
static void test_property_invalid_parent(void **pstate);
static void test_property_unlinked(void **pstate);
static void test_property_arguments(void **pstate);
static void test_property_names(void **pstate);

static void test_table_build(test_table_t *table);
static void test_table_put(test_table_t *table, const void *data, size_t size);
static void test_table_put_word(test_table_t *table, uint16_t value);
static void test_table_put_oem_strings(test_table_t *table, dmi_handle_t handle);
static void test_table_put_property(test_table_t *table, dmi_handle_t handle, uint16_t ident,
                                    const char *value, dmi_handle_t parent);

static dmi_context_t *test_create(unsigned int flags);
static dmi_context_t *test_open(unsigned int flags);
static dmi_entity_t *test_get(dmi_context_t *context, dmi_handle_t handle);
static void test_check_invalid(dmi_context_t *context);

static dmi_log_t test_logger = { dmi_test_log_handler };

static test_table_t test_table;

// Parent structure with string properties
static const dmi_handle_t test_parent = 0x0100;

// Structure without string properties
static const dmi_handle_t test_other = 0x0101;

static const char *test_device_path = "PciRoot(0x0)/Pci(0x1C,0x0)";

int main(void)
{
    test_table_build(&test_table);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_property_linked),
        cmocka_unit_test(test_property_duplicate),
        cmocka_unit_test(test_property_invalid_parent),
        cmocka_unit_test(test_property_unlinked),
        cmocka_unit_test(test_property_arguments),
        cmocka_unit_test(test_property_names)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_property_linked(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = test_open(DMI_CONTEXT_FLAG_LINK);
    assert_non_null(context);

    const dmi_entity_t *parent = test_get(context, test_parent);

    // Properties are found by identifier, regardless of its range
    assert_string_equal(dmi_entity_property(parent, DMI_PROPERTY_ID_UEFI_DEVICE_PATH), test_device_path);
    assert_string_equal(dmi_entity_property(parent, 0x8001), "vendor");

    // Property without a value, and a missing property
    assert_null(dmi_entity_property(parent, 0xC001));
    assert_null(dmi_entity_property(parent, 0x0005));

    // Only valid properties of the parent are attached, in the table order
    assert_int_equal(parent->properties.length, 4);

    // Other structures have no properties
    const dmi_entity_t *other = test_get(context, test_other);
    assert_int_equal(other->properties.length, 0);
    assert_null(dmi_entity_property(other, DMI_PROPERTY_ID_UEFI_DEVICE_PATH));

    // String property refers to its parent
    const dmi_string_property_t *info = dmi_entity_info(test_get(context, 0x0200), DMI_TYPE(STRING_PROPERTY));
    assert_non_null(info);
    assert_ptr_equal(info->parent, parent);
    assert_int_equal(info->ident, DMI_PROPERTY_ID_UEFI_DEVICE_PATH);

    dmi_destroy(context);
}

static void test_property_duplicate(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = test_open(DMI_CONTEXT_FLAG_LINK);
    assert_non_null(context);

    // The first property in the table order is used
    const char *value = dmi_entity_property(test_get(context, test_parent), DMI_PROPERTY_ID_UEFI_DEVICE_PATH);
    assert_string_equal(value, test_device_path);

    dmi_destroy(context);
}

static void test_property_invalid_parent(void **pstate)
{
    dmi_unused(pstate);

    // Invalid string properties do not fail linking in relaxed mode
    dmi_context_t *context = test_create(DMI_CONTEXT_FLAG_LINK);
    assert_non_null(context);

    dmi_registry_t *registry = dmi_get_registry(context);

    assert_true(dmi_registry_scan(registry));
    assert_true(dmi_registry_decode(registry));
    assert_true(dmi_registry_link(registry));
    assert_true(registry->status & DMI_REGISTRY_STATUS_LINKED);

    test_check_invalid(context);
    dmi_destroy(context);

    // The same properties are reported in strict mode, but fail linking
    context = test_create(DMI_CONTEXT_FLAG_LINK | DMI_CONTEXT_FLAG_STRICT);
    assert_non_null(context);

    registry = dmi_get_registry(context);

    assert_true(dmi_registry_scan(registry));
    assert_true(dmi_registry_decode(registry));
    assert_false(dmi_registry_link(registry));
    assert_false(registry->status & DMI_REGISTRY_STATUS_LINKED);

    test_check_invalid(context);
    dmi_destroy(context);
}

static void test_property_unlinked(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = test_open(0);
    assert_non_null(context);

    // Properties are attached only while linking structures
    const dmi_entity_t *parent = test_get(context, test_parent);
    assert_int_equal(parent->properties.length, 0);
    assert_null(dmi_entity_property(parent, DMI_PROPERTY_ID_UEFI_DEVICE_PATH));

    // String properties themselves are decoded anyway
    const dmi_string_property_t *info = dmi_entity_info(test_get(context, 0x0200), DMI_TYPE(STRING_PROPERTY));
    assert_non_null(info);
    assert_int_equal(info->parent_handle, test_parent);
    assert_string_equal(info->value, test_device_path);
    assert_null(info->parent);

    dmi_destroy(context);
}

static void test_property_arguments(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_entity_property(nullptr, DMI_PROPERTY_ID_UEFI_DEVICE_PATH));
    assert_false(dmi_entity_add_property(nullptr, nullptr));

    dmi_context_t *context = test_open(DMI_CONTEXT_FLAG_LINK);
    assert_non_null(context);

    dmi_entity_t *parent = test_get(context, test_parent);

    dmi_error_clear(context);
    assert_false(dmi_entity_add_property(parent, nullptr));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_NULL_ARGUMENT);
    assert_int_equal(parent->properties.length, 4);

    dmi_destroy(context);
}

static void test_property_names(void **pstate)
{
    dmi_unused(pstate);

    assert_string_equal(dmi_name_lookup(&dmi_property_names, DMI_PROPERTY_ID_UEFI_DEVICE_PATH), "UEFI device path");
    assert_string_equal(dmi_name_lookup(&dmi_property_names, DMI_PROPERTY_ID_RESERVED), "Reserved");

    // Ranges reserved for DMTF, firmware vendors and OEMs
    assert_string_equal(dmi_name_lookup(&dmi_property_names, 0x0002), "Reserved");
    assert_string_equal(dmi_name_lookup(&dmi_property_names, 0x7FFF), "Reserved");
    assert_string_equal(dmi_name_lookup(&dmi_property_names, 0x8000), "Firmware vendor specific");
    assert_string_equal(dmi_name_lookup(&dmi_property_names, 0xBFFF), "Firmware vendor specific");
    assert_string_equal(dmi_name_lookup(&dmi_property_names, 0xC000), "OEM specific");
    assert_string_equal(dmi_name_lookup(&dmi_property_names, 0xFFFF), "OEM specific");

    // Property value attribute has its own name
    const dmi_attribute_t *attr = dmi_string_property_spec.attributes;
    while ((attr->params.name != nullptr) and (strcmp(attr->params.code, "value") != 0))
        attr++;

    assert_non_null(attr->params.name);
    assert_string_equal(attr->params.name, "Value");
}

static void test_table_build(test_table_t *table)
{
    static const dmi_data_t end_of_table[] = { 127, 0x04, 0xFF, 0x00, 0x00, 0x00 };

    table->size = 0;

    test_table_put_oem_strings(table, test_parent);
    test_table_put_oem_strings(table, test_other);

    test_table_put_property(table, 0x0200, DMI_PROPERTY_ID_UEFI_DEVICE_PATH, test_device_path, test_parent);
    test_table_put_property(table, 0x0201, 0x8001, "vendor", test_parent);
    test_table_put_property(table, 0x0202, 0xC001, nullptr, test_parent);

    // Parent does not exist
    test_table_put_property(table, 0x0203, DMI_PROPERTY_ID_UEFI_DEVICE_PATH, "orphan", 0x0999);

    // Parent is a string property
    test_table_put_property(table, 0x0204, DMI_PROPERTY_ID_UEFI_DEVICE_PATH, "nested", 0x0200);

    // The same property of the same parent, specified again
    test_table_put_property(table, 0x0205, DMI_PROPERTY_ID_UEFI_DEVICE_PATH, "duplicate", test_parent);

    // Parent is not specified
    test_table_put_property(table, 0x0206, DMI_PROPERTY_ID_UEFI_DEVICE_PATH, "unspecified", DMI_HANDLE_INVALID);

    test_table_put(table, end_of_table, sizeof(end_of_table));
}

static void test_table_put(test_table_t *table, const void *data, size_t size)
{
    assert_true(table->size + size <= sizeof(table->data));

    memcpy(table->data + table->size, data, size);
    table->size += size;
}

static void test_table_put_word(test_table_t *table, uint16_t value)
{
    const dmi_data_t data[] = { value & 0xFF, value >> 8 };

    test_table_put(table, data, sizeof(data));
}

static void test_table_put_oem_strings(test_table_t *table, dmi_handle_t handle)
{
    static const dmi_data_t header[] = { 11, 0x05 };
    static const dmi_data_t body[] = { 0x01, 'O', 'E', 'M', 0x00, 0x00 };

    test_table_put(table, header, sizeof(header));
    test_table_put_word(table, handle);
    test_table_put(table, body, sizeof(body));
}

static void test_table_put_property(test_table_t *table, dmi_handle_t handle, uint16_t ident,
                                    const char *value, dmi_handle_t parent)
{
    static const dmi_data_t header[] = { 46, 0x09 };
    static const dmi_data_t no_strings[] = { 0x00, 0x00 };

    const dmi_data_t string_number = (value != nullptr) ? 1 : 0;

    test_table_put(table, header, sizeof(header));
    test_table_put_word(table, handle);
    test_table_put_word(table, ident);
    test_table_put(table, &string_number, sizeof(string_number));
    test_table_put_word(table, parent);

    if (value != nullptr) {
        test_table_put(table, value, strlen(value) + 1);
        test_table_put(table, no_strings, 1);
    } else {
        test_table_put(table, no_strings, sizeof(no_strings));
    }
}

//
// Create context over the test table, with an empty registry
//
static dmi_context_t *test_create(unsigned int flags)
{
    dmi_context_t *context = dmi_create(flags);
    if (context == nullptr)
        return nullptr;

    dmi_set_logger(context, &test_logger);
    dmi_set_log_level(context, DMI_LOG_ERROR);

    // String property structures were added in SMBIOS 3.5
    context->state.smbios_version = DMI_VERSION(3, 5, 0);
    context->state.table_data     = test_table.data;
    context->state.table_size     = test_table.size;

    context->state.registry = dmi_registry_create(context, 0);
    if (dmi_get_registry(context) == nullptr) {
        dmi_destroy(context);
        return nullptr;
    }

    return context;
}

//
// Open context over the test table, as dmi_open() does. Structures are linked
// only if requested by flags.
//
static dmi_context_t *test_open(unsigned int flags)
{
    dmi_context_t *context = test_create(flags);
    if (context == nullptr)
        return nullptr;

    dmi_registry_t *registry = dmi_get_registry(context);

    bool success =
        dmi_registry_scan(registry) and
        dmi_registry_decode(registry);

    if (success and (flags & DMI_CONTEXT_FLAG_LINK))
        success = dmi_registry_link(registry);

    if (not success) {
        dmi_destroy(context);
        return nullptr;
    }

    return context;
}

static dmi_entity_t *test_get(dmi_context_t *context, dmi_handle_t handle)
{
    dmi_registry_t *registry = dmi_get_registry(context);
    dmi_entity_t   *entity   = dmi_registry_lookup(registry, handle, DMI_TYPE_ANY, false);

    assert_non_null(entity);

    return entity;
}

//
// Check that every invalid string property is reported and left unattached,
// and that linking went on after them
//
static void test_check_invalid(dmi_context_t *context)
{
    static const struct {
        dmi_handle_t      handle;
        const char       *text;
        dmi_error_code_t  reason;
    } expected[] = {
        { 0x0203, "0x0203: parent 0x0999 not found",        DMI_ERROR_ENTITY_NOT_FOUND    },
        { 0x0204, "0x0204: parent 0x0200 is a string",      DMI_ERROR_INVALID_ENTITY_TYPE },
        { 0x0206, "0x0206: parent is not specified",        DMI_ERROR_ENTITY_NOT_FOUND    }
    };

    for (size_t i = 0; i < countof(expected); i++) {
        const dmi_string_property_t *info = dmi_entity_info(test_get(context, expected[i].handle),
                                                            DMI_TYPE(STRING_PROPERTY));
        assert_non_null(info);
        assert_null(info->parent);
    }

    bool reported[countof(expected)] = {};
    size_t link_errors = 0;

    const dmi_error_t *error;
    while ((error = dmi_error_get_first(context)) != nullptr) {
        if (error->reason == DMI_ERROR_ENTITY_LINK) {
            link_errors++;
            continue;
        }

        for (size_t i = 0; i < countof(expected); i++) {
            if ((error->reason == expected[i].reason) and (strstr(error->message, expected[i].text) != nullptr))
                reported[i] = true;
        }
    }

    for (size_t i = 0; i < countof(expected); i++)
        assert_true(reported[i]);

    assert_int_equal(link_errors, countof(expected));

    // String property cannot be a parent, and valid properties are attached
    // despite the invalid ones
    assert_int_equal(test_get(context, 0x0200)->properties.length, 0);
    assert_int_equal(test_get(context, test_parent)->properties.length, 4);
    assert_string_equal(dmi_entity_property(test_get(context, test_parent), DMI_PROPERTY_ID_UEFI_DEVICE_PATH),
                        test_device_path);
}
