//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/attribute.h>
#include <opendmi/internal.h>
#include <opendmi/utils/uuid.h>
#include <opendmi/utils/version.h>

static int test_attribute_setup(void **pstate);
static int test_attribute_teardown(void **pstate);

static void test_attribute_format_address(void **pstate);
static void test_attribute_format_binary(void **pstate);
static void test_attribute_format_mac(void **pstate);
static void test_attribute_format_bool(void **pstate);
static void test_attribute_format_bool_ex(void **pstate);
static void test_attribute_format_decimal(void **pstate);
static void test_attribute_format_enum(void **pstate);
static void test_attribute_format_handle(void **pstate);
static void test_attribute_format_integer(void **pstate);
static void test_attribute_format_set(void **pstate);
static void test_attribute_format_size(void **pstate);
static void test_attribute_format_string(void **pstate);
static void test_attribute_format_uuid(void **pstate);
static void test_attribute_format_version(void **pstate);
static void test_attribute_get_count(void **pstate);
static int free_attribute_value(void **pstate);

static dmi_context_t *context = nullptr;

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_teardown(test_attribute_format_address, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_binary, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_mac, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_bool, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_bool_ex, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_decimal, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_enum, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_handle, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_integer, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_set, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_size, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_string, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_uuid, free_attribute_value),
        cmocka_unit_test_teardown(test_attribute_format_version, free_attribute_value),
        cmocka_unit_test(test_attribute_get_count)
    };

    return cmocka_run_group_tests(tests, test_attribute_setup, test_attribute_teardown);
}

static int test_attribute_setup(void **pstate)
{
    dmi_unused(pstate);

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    return 0;
}

static int test_attribute_teardown(void **pstate)
{
    dmi_unused(pstate);

    dmi_destroy(context);
    return 0;
}

static void test_attribute_format_address(void **pstate)
{
    dmi_unused(pstate);
    skip();
}

static void test_attribute_format_binary(void **pstate)
{
    static const dmi_data_t data[] = { 0x5F, 0x41, 0x0A, 0xFF };

    const struct {
        dmi_binary_t  value;
        bool          pretty;
        const char   *expected;
    } test_data[] = {
        { { data, 1 },             false, "5f"          },
        { { data, 1 },             true,  "5F"          },
        { { data, countof(data) }, false, "5f410aff"    },
        { { data, countof(data) }, true,  "5F 41 0A FF" },
        { { nullptr, 0 },          false, ""            },
        { { nullptr, 0 },          true,  ""            }
    };

    static const dmi_attribute_t attr = {
        .value   = {
            .size   = sizeof(dmi_binary_t),
            .offset = 0
        },
        .counter = DMI_MEMBER_NULL,
        .type    = DMI_ATTRIBUTE_TYPE_BINARY,
        .params  = {}
    };

    *pstate = nullptr;

    for (size_t i = 0; i < countof(test_data); i++) {
        char *result = dmi_attribute_format(context, &attr, &test_data[i].value, test_data[i].pretty);
        *pstate = result;

        assert_non_null(result);
        assert_string_equal(result, test_data[i].expected);

        free(result);
        *pstate = nullptr;
    }

    // Empty data is unspecified
    assert_true(dmi_attribute_is_unspecified(&attr, &test_data[4].value));
    assert_false(dmi_attribute_is_unspecified(&attr, &test_data[0].value));
}

static void test_attribute_format_mac(void **pstate)
{
    // MAC-48 address in a longer field, EUI-64 address, and a short value
    static const dmi_data_t mac48[] = { 0x00, 0x1B, 0x21, 0x3C, 0x4D, 0x5E, 0x00, 0x00, 0x00, 0x00 };
    static const dmi_data_t eui64[] = { 0x00, 0x1B, 0x21, 0xFF, 0xFE, 0x3C, 0x4D, 0x5E, 0x00, 0x00 };
    static const dmi_data_t zeros[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const dmi_data_t short_mac[] = { 0x0A, 0x00 };

    const struct {
        dmi_binary_t  value;
        bool          pretty;
        const char   *expected;
    } test_data[] = {
        { { mac48, countof(mac48) },         true,  "00:1B:21:3C:4D:5E"       },
        { { mac48, countof(mac48) },         false, "00:1b:21:3c:4d:5e"       },
        { { eui64, countof(eui64) },         true,  "00:1B:21:FF:FE:3C:4D:5E" },
        { { zeros, countof(zeros) },         true,  "00:00:00:00:00:00"       },
        { { short_mac, countof(short_mac) }, true,  "0A:00"                   }
    };

    static const dmi_attribute_t attr = {
        .value   = {
            .size   = sizeof(dmi_binary_t),
            .offset = 0
        },
        .counter = DMI_MEMBER_NULL,
        .type    = DMI_ATTRIBUTE_TYPE_BINARY,
        .params  = {
            .flags = DMI_ATTRIBUTE_FLAG_MAC
        }
    };

    *pstate = nullptr;

    for (size_t i = 0; i < countof(test_data); i++) {
        char *result = dmi_attribute_format(context, &attr, &test_data[i].value, test_data[i].pretty);
        *pstate = result;

        assert_non_null(result);
        assert_string_equal(result, test_data[i].expected);

        free(result);
        *pstate = nullptr;
    }
}

static void test_attribute_format_bool(void **pstate)
{
    dmi_unused(pstate);

    static const dmi_attribute_t attr = {
        .value   = {
            .size   = sizeof(dmi_handle_t),
            .offset = 0
        },
        .counter = DMI_MEMBER_NULL,
        .type    = DMI_ATTRIBUTE_TYPE_BOOL,
        .params  = {}
    };

    const struct {
        const void *value;
        bool pretty;
        const char *expected;
    } test_data[] = {
        { dmi_value_ptr((bool)0),  true,  "no"    },
        { dmi_value_ptr((bool)0),  false, "false" },
        { dmi_value_ptr((bool)1),  true,  "yes"   },
        { dmi_value_ptr((bool)1),  false, "true"  },
        { dmi_value_ptr((bool)-1), true,  "yes"   },
        { dmi_value_ptr((bool)-1), false, "true"  },
        { dmi_value_ptr((bool)2),  true,  "yes"   },
        { dmi_value_ptr((bool)2),  false, "true"  }
    };

    *pstate = nullptr;

    for (size_t i = 0; i < countof(test_data); i++) {
        const void *value    = test_data[i].value;
        bool        pretty   = test_data[i].pretty;
        const char *expected = test_data[i].expected;
        char       *result   = nullptr;

        result = dmi_attribute_format(context, &attr, value, pretty);
        *pstate = result;

        assert_non_null(result);
        assert_string_equal(result, expected);

        free(result);
        *pstate = nullptr;
    }
}

static void test_attribute_format_bool_ex(void **pstate)
{
    dmi_unused(pstate);

    const dmi_name_set_t test_values = {
        .names = (const dmi_name_t[]) {
            { .id = false, .code = "off", .name = "OFF" },
            { .id = true,  .code = "on",  .name = "ON"  }
        }
    };

    const dmi_attribute_t attr = {
        .value   = {
            .size   = sizeof(dmi_handle_t),
            .offset = 0
        },
        .counter = DMI_MEMBER_NULL,
        .type    = DMI_ATTRIBUTE_TYPE_BOOL,
        .params  = {
            .values = &test_values
        }
    };

    const struct {
        const void *value;
        bool pretty;
        const char *expected;
    } test_data[] = {
        { dmi_value_ptr((bool)0),  true,  "OFF" },
        { dmi_value_ptr((bool)0),  false, "off" },
        { dmi_value_ptr((bool)1),  true,  "ON"  },
        { dmi_value_ptr((bool)1),  false, "on"  },
        { dmi_value_ptr((bool)-1), true,  "ON"  },
        { dmi_value_ptr((bool)-1), false, "on"  },
        { dmi_value_ptr((bool)2),  true,  "ON"  },
        { dmi_value_ptr((bool)2),  false, "on"  }
    };

    *pstate = nullptr;

    for (size_t i = 0; i < countof(test_data); i++) {
        const void *value    = test_data[i].value;
        bool        pretty   = test_data[i].pretty;
        const char *expected = test_data[i].expected;
        char       *result   = nullptr;

        result = dmi_attribute_format(context, &attr, value, pretty);
        *pstate = result;

        assert_non_null(result);
        assert_string_equal(result, expected);

        free(result);
        *pstate = nullptr;
    }
}

static void test_attribute_format_decimal(void **pstate)
{
    dmi_unused(pstate);

    const struct {
        const void *value;
        size_t size;
        unsigned int scale;
        unsigned int flags;
        const char *expected;
    } test_data[] = {
        // Unsigned values
        { dmi_value_ptr((uint16_t)1234u),       sizeof(uint16_t), 0, 0, "1234"      },
        { dmi_value_ptr((uint16_t)1234u),       sizeof(uint16_t), 1, 0, "123.4"     },
        { dmi_value_ptr((uint16_t)1234u),       sizeof(uint16_t), 3, 0, "1.234"     },
        { dmi_value_ptr((uint16_t)1200u),       sizeof(uint16_t), 3, 0, "1.2"       },
        { dmi_value_ptr((uint16_t)1000u),       sizeof(uint16_t), 3, 0, "1.0"       },
        { dmi_value_ptr((uint16_t)5u),          sizeof(uint16_t), 2, 0, "0.05"      },
        { dmi_value_ptr((uint16_t)0u),          sizeof(uint16_t), 2, 0, "0.0"       },
        { dmi_value_ptr((uint16_t)0x9000u),     sizeof(uint16_t), 3, 0, "36.864"    },
        { dmi_value_ptr((uint16_t)0xFFFFu),     sizeof(uint16_t), 1, 0, "6553.5"    },
        { dmi_value_ptr((uint8_t)0xFFu),        sizeof(uint8_t),  1, 0, "25.5"      },
        { dmi_value_ptr((uint32_t)0xFFFFFFFFu), sizeof(uint32_t), 2, 0, "42949672.95" },
        { dmi_value_ptr((uint64_t)UINT64_MAX),  sizeof(uint64_t), 1, 0, "1844674407370955161.5" },
        // Signed values
        { dmi_value_ptr((int16_t)255),       sizeof(int16_t), 1, DMI_ATTRIBUTE_FLAG_SIGNED, "25.5"   },
        { dmi_value_ptr((int16_t)-255),      sizeof(int16_t), 1, DMI_ATTRIBUTE_FLAG_SIGNED, "-25.5"  },
        { dmi_value_ptr((int16_t)-5),        sizeof(int16_t), 1, DMI_ATTRIBUTE_FLAG_SIGNED, "-0.5"   },
        { dmi_value_ptr((int16_t)-5),        sizeof(int16_t), 3, DMI_ATTRIBUTE_FLAG_SIGNED, "-0.005" },
        { dmi_value_ptr((int16_t)-1000),     sizeof(int16_t), 3, DMI_ATTRIBUTE_FLAG_SIGNED, "-1.0"   },
        { dmi_value_ptr((int16_t)0),         sizeof(int16_t), 1, DMI_ATTRIBUTE_FLAG_SIGNED, "0.0"    },
        { dmi_value_ptr((int8_t)-128),       sizeof(int8_t),  1, DMI_ATTRIBUTE_FLAG_SIGNED, "-12.8"  },
        { dmi_value_ptr((int64_t)INT64_MIN), sizeof(int64_t), 1, DMI_ATTRIBUTE_FLAG_SIGNED, "-922337203685477580.8" }
    };

    *pstate = nullptr;

    for (size_t i = 0; i < countof(test_data); i++) {
        const void *value    = test_data[i].value;
        const char *expected = test_data[i].expected;
        char       *result   = nullptr;

        const dmi_attribute_t attr = {
            .value   = {
                .size   = test_data[i].size,
                .offset = 0
            },
            .counter = DMI_MEMBER_NULL,
            .type    = DMI_ATTRIBUTE_TYPE_DECIMAL,
            .params  = {
                .scale = test_data[i].scale,
                .flags = test_data[i].flags
            }
        };

        for (int pretty = 0; pretty <= 1; pretty++) {
            result = dmi_attribute_format(context, &attr, value, pretty);
            *pstate = result;

            assert_non_null(result);
            assert_string_equal(result, expected);

            free(result);
            *pstate = nullptr;
        }
    }
}

static void test_attribute_format_enum(void **pstate)
{
    dmi_unused(pstate);
    skip();
}

static void test_attribute_format_handle(void **pstate)
{
    dmi_unused(pstate);

    const struct {
        const void *value;
        bool pretty;
        const char *expected;
    } test_data[] = {
        { dmi_value_ptr((dmi_handle_t)0x0000u), false, "0x0000" },
        { dmi_value_ptr((dmi_handle_t)0x0000u), true,  "0x0000" },
        { dmi_value_ptr((dmi_handle_t)0x1234u), false, "0x1234" },
        { dmi_value_ptr((dmi_handle_t)0x1234u), true,  "0x1234" },
        { dmi_value_ptr((dmi_handle_t)0xABCDu), false, "0xABCD" },
        { dmi_value_ptr((dmi_handle_t)0xABCDu), true,  "0xABCD" },
        { dmi_value_ptr((dmi_handle_t)0xFFFFu), false, "0xFFFF" },
        { dmi_value_ptr((dmi_handle_t)0xFFFFu), true , "0xFFFF" }
    };

    static const dmi_attribute_t attr = {
        .value   = {
            .size   = sizeof(dmi_handle_t),
            .offset = 0
        },
        .counter = DMI_MEMBER_NULL,
        .type    = DMI_ATTRIBUTE_TYPE_HANDLE,
        .params  = {}
    };

    *pstate = nullptr;

    for (size_t i = 0; i < countof(test_data); i++) {
        const void *value    = test_data[i].value;
        bool        pretty   = test_data[i].pretty;
        const char *expected = test_data[i].expected;
        char       *result   = nullptr;

        result = dmi_attribute_format(context, &attr, value, pretty);
        *pstate = result;

        assert_non_null(result);
        assert_string_equal(result, expected);

        free(result);
        *pstate = nullptr;
    }

}

static void test_attribute_format_integer(void **pstate)
{
    dmi_unused(pstate);
    skip();
}

static void test_attribute_format_set(void **pstate)
{
    dmi_unused(pstate);
    skip();
}

static void test_attribute_format_size(void **pstate)
{
    dmi_unused(pstate);
    skip();
}

static void test_attribute_format_string(void **pstate)
{
    dmi_unused(pstate);
    skip();
}

static void test_attribute_format_uuid(void **pstate)
{
    dmi_unused(pstate);

    const struct {
        uint8_t value[16];
        bool pretty;
        const char *expected;
    } test_data[] = {
        {
            .value = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            },
            .pretty   = false,
            .expected = "00000000-0000-0000-0000-000000000000"
        },
        {
            .value = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            },
            .pretty   = true,
            .expected = "00000000-0000-0000-0000-000000000000"
        },
        {
            .value = {
                0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
            },
            .pretty   = false,
            .expected = "00112233-4455-6677-8899-AABBCCDDEEFF"
        },
        {
            .value = {
                0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
            },
            .pretty   = true,
            .expected = "00112233-4455-6677-8899-AABBCCDDEEFF"
        },
        {
            .value = {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
            },
            .pretty   = false,
            .expected = "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF"
        },
        {
            .value = {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
            },
            .pretty   = true,
            .expected = "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF"
        }
    };

    static const dmi_attribute_t attr = {
        .value   = {
            .size   = sizeof(dmi_uuid_t),
            .offset = 0
        },
        .counter = DMI_MEMBER_NULL,
        .type    = DMI_ATTRIBUTE_TYPE_UUID,
        .params  = {}
    };

    *pstate = nullptr;

    for (size_t i = 0; i < countof(test_data); i++) {
        const void *value    = test_data[i].value;
        bool        pretty   = test_data[i].pretty;
        const char *expected = test_data[i].expected;
        char       *result   = nullptr;

        result = dmi_attribute_format(context, &attr, value, pretty);
        *pstate = result;

        assert_non_null(result);
        assert_string_equal(result, expected);

        free(result);
        *pstate = nullptr;
    }
}

static void test_attribute_format_version(void **pstate)
{
    dmi_unused(pstate);

    const struct {
        const void *value;
        int scale;
        bool pretty;
        const char *expected;
    } test_data[] = {
        { dmi_value_ptr(dmi_version(1, 2, 3)), 0, false, "1.2.3" },
        { dmi_value_ptr(dmi_version(1, 2, 3)), 0, true,  "1.2.3" },
        { dmi_value_ptr(dmi_version(1, 2, 3)), 1, false, "1"     },
        { dmi_value_ptr(dmi_version(1, 2, 3)), 1, true,  "1"     },
        { dmi_value_ptr(dmi_version(1, 2, 3)), 2, false, "1.2"   },
        { dmi_value_ptr(dmi_version(1, 2, 3)), 2, true,  "1.2"   },
        { dmi_value_ptr(dmi_version(1, 2, 3)), 3, false, "1.2.3" },
        { dmi_value_ptr(dmi_version(1, 2, 3)), 3, true,  "1.2.3" },
        { dmi_value_ptr(dmi_version(1, 2, 3)), 4, false, "1.2.3" },
        { dmi_value_ptr(dmi_version(1, 2, 3)), 4, true,  "1.2.3" }
    };

    *pstate = nullptr;

    for (size_t i = 0; i < countof(test_data); i++) {
        const void *value    = test_data[i].value;
        int         scale    = test_data[i].scale;
        bool        pretty   = test_data[i].pretty;
        const char *expected = test_data[i].expected;
        char       *result   = nullptr;

        const dmi_attribute_t attr = {
            .value   = {
                .size   = sizeof(dmi_version_t),
                .offset = 0
            },
            .counter = DMI_MEMBER_NULL,
            .type    = DMI_ATTRIBUTE_TYPE_VERSION,
            .params  = {
                .scale = scale
            }
        };

        result = dmi_attribute_format(context, &attr, value, pretty);
        *pstate = result;

        assert_non_null(result);
        assert_string_equal(result, expected);

        free(result);
        *pstate = nullptr;
    }
}

static int free_attribute_value(void **pstate)
{
    if (*pstate == nullptr)
        return 0;

    free(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_attribute_get_count(void **pstate)
{
    dmi_unused(pstate);

    // Counters are followed by non-zero bytes, which must not be read
    typedef struct test_counters
    {
        uint8_t  count_8;
        uint8_t  guard_8;
        uint16_t count_16;
        uint16_t guard_16;
        unsigned count_32;
        unsigned guard_32;
        size_t   count_size;
        size_t   guard_size;
        char   **items;
    } test_counters_t;

    const test_counters_t info = {
        .count_8    = 3,
        .guard_8    = 0xFF,
        .count_16   = 300,
        .guard_16   = 0xFFFF,
        .count_32   = 70000,
        .guard_32   = UINT_MAX,
        .count_size = 5,
        .guard_size = SIZE_MAX
    };

    const dmi_attribute_t attrs[] = {
        DMI_ATTRIBUTE_ARRAY(test_counters_t, items, count_8, STRING, { .code = "a", .name = "A" }),
        DMI_ATTRIBUTE_ARRAY(test_counters_t, items, count_16, STRING, { .code = "b", .name = "B" }),
        DMI_ATTRIBUTE_ARRAY(test_counters_t, items, count_32, STRING, { .code = "c", .name = "C" }),
        DMI_ATTRIBUTE_ARRAY(test_counters_t, items, count_size, STRING, { .code = "d", .name = "D" }),
        DMI_ATTRIBUTE(test_counters_t, items, STRING, { .code = "e", .name = "E" })
    };

    assert_int_equal(dmi_attribute_get_count(&attrs[0], &info), 3);
    assert_int_equal(dmi_attribute_get_count(&attrs[1], &info), 300);
    assert_int_equal(dmi_attribute_get_count(&attrs[2], &info), 70000);
    assert_int_equal(dmi_attribute_get_count(&attrs[3], &info), 5);

    // Attribute without counter
    assert_int_equal(dmi_attribute_get_count(&attrs[4], &info), 0);
}
