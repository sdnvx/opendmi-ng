//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/internal.h>
#include <opendmi/utils/uuid.h>

typedef struct test_vector test_vector_t;

struct test_vector
{
    uint8_t    encoded[16];
    dmi_uuid_t decoded;
};

static void test_uuid_encode(void **pstate);
static void test_uuid_decode(void **pstate);

static const test_vector_t test_data[] =
{
    {
        .encoded = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        .decoded = {
            .time_low                  = 0x00000000U,
            .time_mid                  = 0x0000U,
            .time_hi_and_version       = 0x0000U,
            .clock_seq_hi_and_reserved = 0x00U,
            .clock_seq_low             = 0x00U,
            .node                      = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
        }
    },
    {
        .encoded = {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        },
        .decoded = {
            .time_low                  = 0xFFFFFFFFU,
            .time_mid                  = 0xFFFFU,
            .time_hi_and_version       = 0xFFFFU,
            .clock_seq_hi_and_reserved = 0xFFU,
            .clock_seq_low             = 0xFFU,
            .node                      = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
        }
    },
    {
        .encoded = {
            0x33, 0x22, 0x11, 0x00, 0x55, 0x44, 0x77, 0x66,
            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
        },
        .decoded = {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            .time_low                  = 0x33221100U,
            .time_mid                  = 0x5544U,
            .time_hi_and_version       = 0x7766U,
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            .time_low                  = 0x00112233U,
            .time_mid                  = 0x4455U,
            .time_hi_and_version       = 0x6677U,
#else
#   error "Unsupported endianness"
#endif
            .clock_seq_hi_and_reserved = 0x88U,
            .clock_seq_low             = 0x99U,
            .node                      = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }
        }
    }
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_uuid_encode),
        cmocka_unit_test(test_uuid_decode)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_uuid_encode(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < countof(test_data); i++) {
        uint8_t result[16];

        dmi_uuid_encode(test_data[i].decoded, result);

        assert_memory_equal(result, test_data[i].encoded, sizeof(result));
    }
}

static void test_uuid_decode(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < countof(test_data); i++) {
        dmi_uuid_t result = dmi_uuid_decode(test_data[i].encoded);

        assert_memory_equal(result.__value, test_data[i].decoded.__value, sizeof(result));
    }
}
