//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <format>
#include <sstream>
#include <string>
#include <type_traits>

#include <cmocka.h>

#include <opendmi/utils/uuid.hh>

static void test_uuid_decode(void **pstate);
static void test_uuid_encode(void **pstate);
static void test_uuid_special(void **pstate);
static void test_uuid_compare(void **pstate);
static void test_uuid_native(void **pstate);
static void test_uuid_traits(void **pstate);
static void test_uuid_format(void **pstate);

//
// Build UUID bytes from a list of integers
//
template <class... T>
static constexpr dmi::uuid::bytes_t bytes(T... values)
{
    return { static_cast<std::byte>(values)... };
}

// UUID as it is stored in SMBIOS, the first three fields are little-endian
static constexpr dmi::uuid::bytes_t test_smbios = bytes(
    0x33, 0x22, 0x11, 0x00, 0x55, 0x44, 0x77, 0x66,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
);

// The same UUID in RFC 4122 byte order
static constexpr dmi::uuid::bytes_t test_rfc = bytes(
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
);

static constexpr const char *test_string = "00112233-4455-6677-8899-AABBCCDDEEFF";

//
// Compare UUID bytes with expected ones, usable in constant expressions
//
static constexpr bool same_bytes(const dmi::uuid &uuid, const dmi::uuid::bytes_t &expected)
{
    return std::ranges::equal(uuid.bytes(), expected, {}, {}, [](std::byte value) {
        return std::to_integer<dmi::capi::dmi_byte_t>(value);
    });
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_uuid_decode),
        cmocka_unit_test(test_uuid_encode),
        cmocka_unit_test(test_uuid_special),
        cmocka_unit_test(test_uuid_compare),
        cmocka_unit_test(test_uuid_native),
        cmocka_unit_test(test_uuid_traits),
        cmocka_unit_test(test_uuid_format)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_uuid_decode(void **pstate)
{
    (void)pstate;

    // The first three fields are byte-swapped, the rest are copied as is
    const auto uuid = dmi::uuid::decode(test_smbios);
    assert_true(same_bytes(uuid, test_rfc));

    // Nil and max UUIDs are not affected by byte order
    const auto nil = bytes(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    assert_true(dmi::uuid::decode(nil).is_nil());

    const auto max = bytes(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                           0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    assert_true(dmi::uuid::decode(max).is_max());
}

static void test_uuid_encode(void **pstate)
{
    (void)pstate;

    // Encoding is the inverse of decoding
    const auto encoded = dmi::uuid(test_rfc).encode();
    assert_memory_equal(encoded.data(), test_smbios.data(), test_smbios.size());

    const auto roundtrip = dmi::uuid::decode(test_smbios).encode();
    assert_memory_equal(roundtrip.data(), test_smbios.data(), test_smbios.size());
}

static void test_uuid_special(void **pstate)
{
    (void)pstate;

    // The default-constructed value is the nil UUID
    assert_true(dmi::uuid().is_nil());
    assert_false(dmi::uuid().is_max());
    assert_false(static_cast<bool>(dmi::uuid()));

    // The max UUID means that the ID is not present, but can be set
    dmi::uuid::bytes_t max;
    max.fill(std::byte { 0xFF });

    assert_true(dmi::uuid(max).is_max());
    assert_false(dmi::uuid(max).is_nil());
    assert_false(static_cast<bool>(dmi::uuid(max)));

    // Any other value is present, even if it has a single non-zero byte
    assert_true(static_cast<bool>(dmi::uuid(test_rfc)));
    assert_true(static_cast<bool>(dmi::uuid(bytes(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1))));
}

static void test_uuid_compare(void **pstate)
{
    (void)pstate;

    assert_true(dmi::uuid(test_rfc) == dmi::uuid::decode(test_smbios));
    assert_true(dmi::uuid(test_rfc) != dmi::uuid());

    // UUIDs are ordered lexicographically by bytes in RFC 4122 order
    const auto low  = bytes(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    const auto high = bytes(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    assert_true(dmi::uuid() < dmi::uuid(low));
    assert_true(dmi::uuid(low) < dmi::uuid(high));
    assert_true(dmi::uuid(high) >= dmi::uuid(high));
}

static void test_uuid_native(void **pstate)
{
    (void)pstate;

    // The C API value has the same memory layout as the UUID bytes
    const auto native = dmi::capi::dmi_uuid_decode(reinterpret_cast<const dmi::capi::dmi_byte_t *>(test_smbios.data()));

    assert_true(dmi::uuid(native) == dmi::uuid::decode(test_smbios));
    assert_int_equal(std::memcmp(&native, test_rfc.data(), sizeof(native)), 0);

    const auto back = dmi::uuid(test_rfc).native();
    assert_int_equal(std::memcmp(&back, &native, sizeof(native)), 0);
}

//
// Compile-time part of the contract. It is verified by building this test,
// and the assertion keeps the checks from being optimized away.
//
static void test_uuid_traits(void **pstate)
{
    (void)pstate;

    static_assert(std::is_trivially_copyable_v<dmi::uuid>);
    static_assert(sizeof(dmi::uuid) == sizeof(dmi::uuid::native_t));

    // Construction from bytes and checks of special values are constant
    // expressions
    static_assert(dmi::uuid().is_nil());
    static_assert(!static_cast<bool>(dmi::uuid()));
    static_assert(same_bytes(dmi::uuid(test_rfc), test_rfc));
    static_assert(dmi::uuid() < dmi::uuid(test_rfc));

    // Conversion to and from the C API value is a constant expression too
    static_assert(dmi::uuid(dmi::uuid(test_rfc).native()) == dmi::uuid(test_rfc));

    // Neither the C API value nor raw bytes are implicitly convertible to
    // a UUID
    static_assert(!std::is_convertible_v<dmi::uuid::native_t, dmi::uuid>);
    static_assert(!std::is_convertible_v<dmi::uuid::bytes_t, dmi::uuid>);

    // UUIDs are formattable, see the std::formatter specialization
    static_assert(std::is_default_constructible_v<std::formatter<dmi::uuid, char>>);

    assert_true(dmi::uuid().is_nil());
}

static void test_uuid_format(void **pstate)
{
    (void)pstate;

    // Hexadecimal digits are uppercase, as in UUID attributes
    assert_string_equal(dmi::uuid(test_rfc).str().c_str(), test_string);
    assert_string_equal(dmi::uuid().str().c_str(), "00000000-0000-0000-0000-000000000000");

    // Output stream and std::format() produce the same text as str()
    std::ostringstream stream;
    stream << dmi::uuid(test_rfc);
    assert_string_equal(stream.str().c_str(), test_string);

    assert_string_equal(std::format("{}", dmi::uuid(test_rfc)).c_str(), test_string);

    // Standard string format options are supported
    assert_string_equal(std::format("[{:>38}]", dmi::uuid(test_rfc)).c_str(),
                        "[  00112233-4455-6677-8899-AABBCCDDEEFF]");
}
