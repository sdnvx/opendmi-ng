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
#include <opendmi/utils/endian.h>

typedef struct test_vector_cvt_16 test_vector_cvt_16_t;
typedef struct test_vector_cvt_32 test_vector_cvt_32_t;
typedef struct test_vector_cvt_64 test_vector_cvt_64_t;

struct test_vector_cvt_16
{
    uint16_t host;
    union {
        uint8_t data[2];
        uint16_t value;
    } net;
};

struct test_vector_cvt_32
{
    uint32_t host;
    union {
        uint8_t  data[4];
        uint32_t value;
    } net;
};

struct test_vector_cvt_64
{
    uint64_t host;

    union {
        uint8_t data[8];
        uint64_t value;
    } net;
};

static void test_bswap16(void **pstate);
static void test_bswap32(void **pstate);
static void test_bswap64(void **pstate);

static void test_hton16(void **pstate);
static void test_hton32(void **pstate);
static void test_hton64(void **pstate);

static void test_ntoh16(void **pstate);
static void test_ntoh32(void **pstate);
static void test_ntoh64(void **pstate);

static const test_vector_cvt_16_t test_data_cvt_16[] =
{
    {
        .host = 0x0000u,
        .net  = { .data = { 0x00, 0x00 } }
    },
    {
        .host = 0x0123u,
        .net  = { .data = { 0x01, 0x23 } }
    },
    {
        .host = 0xFFFFu,
        .net  = { .data = { 0xFF, 0xFF } }
    }
};

static const test_vector_cvt_32_t test_data_cvt_32[] =
{
    {
        .host = 0x00000000Lu,
        .net = { .data = { 0x00, 0x00, 0x00, 0x00 } }
    },
    {
        .host = 0x01234567Lu,
        .net = { .data = { 0x01, 0x23, 0x45, 0x67 } }
    },
    {
        .host = 0xFFFFFFFFLu,
        .net = { .data = { 0xFF, 0xFF, 0xFF, 0xFF } }
    }
};

static const test_vector_cvt_64_t test_data_cvt_64[] =
{
    {
        .host = 0x0000000000000000LLu,
        .net  = { .data = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }
    },
    {
        .host = 0x0123456789ABCDEFLLu,
        .net  = { .data = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF } }
    },
    {
        .host = 0xFFFFFFFFFFFFFFFFLLu,
        .net  = { .data = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } }
    }
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_bswap16),
        cmocka_unit_test(test_bswap32),
        cmocka_unit_test(test_bswap64),
        cmocka_unit_test(test_hton16),
        cmocka_unit_test(test_hton32),
        cmocka_unit_test(test_hton64),
        cmocka_unit_test(test_ntoh16),
        cmocka_unit_test(test_ntoh32),
        cmocka_unit_test(test_ntoh64)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_bswap16(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        uint16_t value;
        uint16_t result;
    } test_data[] = {
        { 0x0000U, 0x0000U },
        { 0xFFFFU, 0xFFFFU },
        { 0x0123U, 0x2301U }
    };

    for (size_t i = 0; i < countof(test_data); i++) {
        uint16_t value  = test_data[i].value;
        uint16_t result = test_data[i].result;

        assert_uint_equal(__dmi_bswap16_compat(value), result);
        assert_uint_equal(dmi_bswap16(value), result);
        assert_uint_equal(dmi_bswap(value), result);
    }
}

static void test_bswap32(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        uint32_t value;
        uint32_t result;
    } test_data[] = {
        { 0x00000000U, 0x00000000U },
        { 0xFFFFFFFFU, 0xFFFFFFFFU },
        { 0x01234567U, 0x67452301U }
    };

    for (size_t i = 0; i < countof(test_data); i++) {
        uint32_t value  = test_data[i].value;
        uint32_t result = test_data[i].result;

        assert_uint_equal(__dmi_bswap32_compat(value), result);
        assert_uint_equal(dmi_bswap32(value), result);
        assert_uint_equal(dmi_bswap(value), result);
    }
}

static void test_bswap64(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        uint64_t value;
        uint64_t result;
    } test_data[] = {
        { 0x0000000000000000UL, 0x0000000000000000UL },
        { 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL },
        { 0x0123456789ABCDEFUL, 0xEFCDAB8967452301UL }
    };

    for (size_t i = 0; i < countof(test_data); i++) {
        uint64_t value  = test_data[i].value;
        uint64_t result = test_data[i].result;

        assert_uint_equal(__dmi_bswap64_compat(value), result);
        assert_uint_equal(dmi_bswap64(value), result);
        assert_uint_equal(dmi_bswap(value), result);
    }
}

static void test_hton16(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < countof(test_data_cvt_16); i++) {
        uint16_t host = test_data_cvt_16[i].host;
        uint16_t net  = test_data_cvt_16[i].net.value;

        assert_uint_equal(dmi_hton16(host), net);
        assert_uint_equal(dmi_hton(host), net);
    }
}

static void test_hton32(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < countof(test_data_cvt_32); i++) {
        uint32_t host = test_data_cvt_32[i].host;
        uint32_t net  = test_data_cvt_32[i].net.value;

        assert_uint_equal(dmi_hton32(host), net);
        assert_uint_equal(dmi_hton(host), net);
    }
}

static void test_hton64(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < countof(test_data_cvt_64); i++) {
        uint64_t host = test_data_cvt_64[i].host;
        uint64_t net  = test_data_cvt_64[i].net.value;

        assert_uint_equal(dmi_hton64(host), net);
        assert_uint_equal(dmi_hton(host), net);
    }
}

static void test_ntoh16(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < countof(test_data_cvt_16); i++) {
        uint16_t net  = test_data_cvt_16[i].net.value;
        uint16_t host = test_data_cvt_16[i].host;

        assert_uint_equal(dmi_ntoh16(net), host);
        assert_uint_equal(dmi_ntoh(net), host);
    }
}

static void test_ntoh32(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < countof(test_data_cvt_32); i++) {
        uint32_t net  = test_data_cvt_32[i].net.value;
        uint32_t host = test_data_cvt_32[i].host;

        assert_uint_equal(dmi_ntoh32(net), host);
        assert_uint_equal(dmi_ntoh(net), host);
    }
}

static void test_ntoh64(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < countof(test_data_cvt_64); i++) {
        uint64_t net  = test_data_cvt_64[i].net.value;
        uint64_t host = test_data_cvt_64[i].host;

        assert_uint_equal(dmi_ntoh64(net), host);
        assert_uint_equal(dmi_ntoh(net), host);
    }
}
