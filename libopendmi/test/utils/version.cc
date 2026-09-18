//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <cstdlib>
#include <format>
#include <sstream>
#include <string>
#include <type_traits>

#include <cmocka.h>

#include <opendmi/utils/version.hh>

static void test_version_build(void **pstate);
static void test_version_components(void **pstate);
static void test_version_compare(void **pstate);
static void test_version_traits(void **pstate);
static void test_version_format(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_version_build),
        cmocka_unit_test(test_version_components),
        cmocka_unit_test(test_version_compare),
        cmocka_unit_test(test_version_traits),
        cmocka_unit_test(test_version_format)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_version_build(void **pstate)
{
    (void)pstate;

    static const struct {
        unsigned int major;
        unsigned int minor;
        unsigned int revision;
        dmi::capi::dmi_version_t value;
    } test_data[] = {
        { 0x00, 0x00, 0x00, 0x00000000u },
        { 0x00, 0x00, 0x12, 0x00000012u },
        { 0x00, 0x12, 0x00, 0x00001200u },
        { 0x12, 0x00, 0x00, 0x00120000u },
        { 0x12, 0x34, 0x56, 0x00123456u }
    };

    for (const auto &test : test_data) {
        const dmi::version version(test.major, test.minor, test.revision);

        assert_int_equal(version.native(), test.value);
        assert_int_equal(dmi::version(test.value).native(), test.value);
    }

    // Components are truncated to eight bits each
    assert_int_equal(dmi::version(0x1234, 0x5678, 0x9ABC).native(), 0x003478BCu);
}

static void test_version_components(void **pstate)
{
    (void)pstate;

    const dmi::version version(3, 4, 1);

    assert_int_equal(version.major(), 3);
    assert_int_equal(version.minor(), 4);
    assert_int_equal(version.revision(), 1);

    // Unknown version is the only one which is false
    assert_true(static_cast<bool>(version));
    assert_false(static_cast<bool>(dmi::version()));
    assert_false(static_cast<bool>(dmi::version(0, 0, 0)));
    assert_true(static_cast<bool>(dmi::version(0, 0, 1)));
}

static void test_version_compare(void **pstate)
{
    (void)pstate;

    assert_true(dmi::version(3, 4) == dmi::version(3, 4, 0));
    assert_true(dmi::version(3, 4) != dmi::version(3, 4, 1));

    // Versions are ordered component-wise
    assert_true(dmi::version(2, 8) < dmi::version(3, 0));
    assert_true(dmi::version(3, 4, 1) > dmi::version(3, 4));
    assert_true(dmi::version(3, 4) >= dmi::version(3, 4));
    assert_true(dmi::version() < dmi::version(0, 0, 1));
}

//
// Compile-time part of the contract. It is verified by building this test,
// and the assertions keep the checks from being optimized away.
//
static void test_version_traits(void **pstate)
{
    (void)pstate;

    static_assert(std::is_trivially_copyable_v<dmi::version>);
    static_assert(sizeof(dmi::version) == sizeof(dmi::capi::dmi_version_t));

    // Every accessor is usable in a constant expression
    static_assert(dmi::version(3, 4, 1).major() == 3);
    static_assert(dmi::version(3, 4, 1).minor() == 4);
    static_assert(dmi::version(3, 4, 1).revision() == 1);
    static_assert(dmi::version(3, 4, 1).native() == DMI_VERSION(3, 4, 1));
    static_assert(dmi::version(3, 4) < dmi::version(3, 4, 1));
    static_assert(!static_cast<bool>(dmi::version()));

    // The packed value is not implicitly convertible to a version
    static_assert(!std::is_convertible_v<dmi::capi::dmi_version_t, dmi::version>);
    static_assert(std::is_constructible_v<dmi::version, dmi::capi::dmi_version_t>);

    // Versions are formattable, see the std::formatter specialization
    static_assert(std::is_default_constructible_v<std::formatter<dmi::version, char>>);

    assert_int_equal(dmi::version(3, 4, 1).native(), DMI_VERSION(3, 4, 1));
}

static void test_version_format(void **pstate)
{
    (void)pstate;

    // Revision is omitted when it is zero
    assert_string_equal(dmi::version(3, 4, 1).str().c_str(), "3.4.1");
    assert_string_equal(dmi::version(3, 4).str().c_str(), "3.4");
    assert_string_equal(dmi::version().str().c_str(), "0.0");

    // Every requested component is included at the explicit detail level
    assert_string_equal(dmi::version(3, 4, 1).str(dmi::version_level::major).c_str(), "3");
    assert_string_equal(dmi::version(3, 4, 1).str(dmi::version_level::minor).c_str(), "3.4");
    assert_string_equal(dmi::version(3, 4, 0).str(dmi::version_level::revision).c_str(), "3.4.0");

    // Output stream and std::format() produce the same text as str()
    std::ostringstream stream;
    stream << dmi::version(3, 4, 1);
    assert_string_equal(stream.str().c_str(), "3.4.1");

    assert_string_equal(std::format("{}", dmi::version(3, 4, 1)).c_str(), "3.4.1");

    // Standard string format options are supported
    assert_string_equal(std::format("[{:>8}]", dmi::version(3, 4)).c_str(), "[     3.4]");
    assert_string_equal(std::format("[{:*<6}]", dmi::version(3, 4)).c_str(), "[3.4***]");
}
