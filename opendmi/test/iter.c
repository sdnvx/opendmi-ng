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
#include <opendmi/utils/name.h>
#include <opendmi/entity/string-property.h>
#include <opendmi/format/iter.h>
#include <opendmi/test/helpers.h>

static void test_iter_array(void **pstate);
static void test_iter_array_empty(void **pstate);
static void test_iter_set(void **pstate);
static void test_iter_strings(void **pstate);
static void test_iter_properties(void **pstate);

typedef struct test_iter_info
{
    uint16_t  count;
    uint16_t *items;
    uint64_t  flags;
} test_iter_info_t;

static const dmi_name_set_t test_iter_flag_names =
{
    .code  = "test-flags",
    .names = (dmi_name_t[]){
        { 0,  "first",  "First"  },
        { 2,  "third",  "Third"  },
        { 63, "last",   "Last"   },
        DMI_NAME_NULL
    }
};

static const dmi_attribute_t test_iter_attrs[] = {
    DMI_ATTRIBUTE_ARRAY(test_iter_info_t, items, count, INTEGER, { .code = "items", .name = "Items" }),
    DMI_ATTRIBUTE(test_iter_info_t, flags, SET, {
        .code   = "flags",
        .name   = "Flags",
        .values = &test_iter_flag_names
    }),
    DMI_ATTRIBUTE_NULL
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_iter_array),
        cmocka_unit_test(test_iter_array_empty),
        cmocka_unit_test(test_iter_set),
        cmocka_unit_test(test_iter_strings),
        cmocka_unit_test(test_iter_properties)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_iter_array(void **pstate)
{
    dmi_unused(pstate);

    uint16_t items[] = { 10, 20, 30 };
    test_iter_info_t info = { .count = countof(items), .items = items };

    const dmi_attribute_t *attr = &test_iter_attrs[0];
    const void *value = dmi_member_ptr(&info, attr->value, dmi_data_t);

    dmi_format_array_iter_t iter;
    const dmi_data_t *ptr;
    size_t count = 0;

    dmi_format_array_iter_init(&iter, attr, dmi_data(&info), value);
    assert_int_equal(iter.count, countof(items));

    while ((ptr = dmi_format_array_iter_next(&iter)) != nullptr) {
        assert_int_equal(iter.index, count);
        assert_int_equal(dmi_deref(uint16_t, ptr), items[count]);
        count++;
    }

    assert_int_equal(count, countof(items));

    // Iterator stays at the end
    assert_null(dmi_format_array_iter_next(&iter));
}

static void test_iter_array_empty(void **pstate)
{
    dmi_unused(pstate);

    const dmi_attribute_t *attr = &test_iter_attrs[0];
    dmi_format_array_iter_t iter;

    // Empty array
    uint16_t items[1] = { 10 };
    test_iter_info_t info = { .count = 0, .items = items };

    dmi_format_array_iter_init(&iter, attr, dmi_data(&info), dmi_member_ptr(&info, attr->value, dmi_data_t));
    assert_null(dmi_format_array_iter_next(&iter));

    // Array is not allocated, but counter is set
    info = (test_iter_info_t){ .count = 5, .items = nullptr };

    dmi_format_array_iter_init(&iter, attr, dmi_data(&info), dmi_member_ptr(&info, attr->value, dmi_data_t));
    assert_int_equal(iter.count, 0);
    assert_null(dmi_format_array_iter_next(&iter));
}

static void test_iter_set(void **pstate)
{
    dmi_unused(pstate);

    // Unnamed bit 1 is set, but not returned
    test_iter_info_t info = { .flags = UINT64_C(0x8000000000000003) };

    const dmi_attribute_t *attr = &test_iter_attrs[1];
    const void *value = dmi_member_ptr(&info, attr->value, dmi_data_t);

    static const struct {
        size_t      id;
        const char *code;
        const char *name;
        bool        value;
    } expected[] = {
        { 0,  "first", "First", true  },
        { 2,  "third", "Third", false },
        { 63, "last",  "Last",  true  }
    };

    dmi_format_set_iter_t iter;
    const dmi_format_flag_t *flag;
    size_t count = 0;

    dmi_format_set_iter_init(&iter, attr, value);
    assert_int_equal(iter.mask, info.flags);

    while ((flag = dmi_format_set_iter_next(&iter)) != nullptr) {
        assert_true(count < countof(expected));
        assert_int_equal(flag->id, expected[count].id);
        assert_string_equal(flag->code, expected[count].code);
        assert_string_equal(flag->name, expected[count].name);
        assert_int_equal(flag->value, expected[count].value);
        count++;
    }

    assert_int_equal(count, countof(expected));
    assert_null(dmi_format_set_iter_next(&iter));
}

static void test_iter_strings(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);

    static const dmi_data_t data[] = {
        126, 5, 0x01, 0x00, 0x00,
        'A', 0, ' ', 'B', ' ', 0,
        0
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);

    dmi_format_string_iter_t iter;
    const char *str;
    size_t count = 0;

    dmi_format_string_iter_init(&iter, entity);

    // Raw strings are returned
    static const char *expected[] = { "A", " B " };

    while ((str = dmi_format_string_iter_next(&iter)) != nullptr) {
        assert_int_equal(iter.index, count + 1);
        assert_string_equal(str, expected[count]);
        count++;
    }

    assert_int_equal(count, countof(expected));
    assert_null(dmi_format_string_iter_next(&iter));

    dmi_entity_destroy(entity);
    dmi_destroy(context);
}

static void test_iter_properties(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);

    // Inactive structure without properties
    static const dmi_data_t parent_data[] = {
        126, 5, 0x01, 0x00, 0x00,
        0, 0
    };

    // String properties with named identifier, identifier within the vendor
    // range and without value
    static const dmi_data_t property_data[][13] = {
        { 46, 9, 0x00, 0x02, 0x01, 0x00, 0x01, 0x00, 0x01, 'P', 0, 0 },
        { 46, 9, 0x01, 0x02, 0x01, 0x80, 0x01, 0x00, 0x01, 'V', 0, 0 },
        { 46, 9, 0x02, 0x02, 0x01, 0xC0, 0x00, 0x00, 0x01, 0, 0 }
    };

    static const struct {
        dmi_property_t  ident;
        const char     *value;
    } expected[] = {
        { DMI_PROPERTY_ID_UEFI_DEVICE_PATH, "P"     },
        { 0x8001,                           "V"     },
        { 0xC001,                           nullptr }
    };

    dmi_entity_t *parent = dmi_entity_create(context, parent_data, sizeof(parent_data));
    assert_non_null(parent);

    dmi_format_property_iter_t iter;
    const dmi_string_property_t *property;

    // Entity without properties
    dmi_format_property_iter_init(&iter, parent);
    assert_null(dmi_format_property_iter_next(&iter));

    dmi_entity_t *properties[countof(property_data)];

    for (size_t i = 0; i < countof(property_data); i++) {
        properties[i] = dmi_entity_create(context, property_data[i], sizeof(property_data[i]));
        assert_non_null(properties[i]);
        assert_true(dmi_entity_decode(properties[i]));
        assert_true(dmi_entity_add_property(parent, properties[i]->info));
    }

    // Properties are returned in the order they were added
    size_t count = 0;

    dmi_format_property_iter_init(&iter, parent);

    while ((property = dmi_format_property_iter_next(&iter)) != nullptr) {
        assert_true(count < countof(expected));
        assert_int_equal(property->ident, expected[count].ident);

        if (expected[count].value != nullptr)
            assert_string_equal(property->value, expected[count].value);
        else
            assert_null(property->value);

        count++;
    }

    assert_int_equal(count, countof(expected));
    assert_null(dmi_format_property_iter_next(&iter));

    for (size_t i = 0; i < countof(properties); i++)
        dmi_entity_destroy(properties[i]);

    dmi_entity_destroy(parent);
    dmi_destroy(context);
}
