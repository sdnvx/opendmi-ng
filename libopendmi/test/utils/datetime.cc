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

#include <opendmi/utils/datetime.hh>

static void test_date_build(void **pstate);
static void test_date_components(void **pstate);
static void test_date_compare(void **pstate);
static void test_date_traits(void **pstate);
static void test_date_parse(void **pstate);
static void test_date_format(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_date_build),
        cmocka_unit_test(test_date_components),
        cmocka_unit_test(test_date_compare),
        cmocka_unit_test(test_date_traits),
        cmocka_unit_test(test_date_parse),
        cmocka_unit_test(test_date_format)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_date_build(void **pstate)
{
    (void)pstate;

    static const struct {
        unsigned int year;
        unsigned int month;
        unsigned int day;
        dmi::capi::dmi_date_t value;
    } test_data[] = {
        { 0x0000, 0x00, 0x00, 0x00000000u },
        { 0x0000, 0x00, 0x12, 0x00000012u },
        { 0x0000, 0x12, 0x00, 0x00001200u },
        { 0x1234, 0x00, 0x00, 0x12340000u },
        { 0x1234, 0x56, 0x78, 0x12345678u }
    };

    for (const auto &test : test_data) {
        const dmi::date date(test.year, test.month, test.day);

        assert_int_equal(date.native(), test.value);
        assert_int_equal(dmi::date(test.value).native(), test.value);
    }

    // The year is truncated to sixteen bits, the rest to eight bits each
    assert_int_equal(dmi::date(0x12345, 0x678, 0x9AB).native(), 0x234578ABu);
}

static void test_date_components(void **pstate)
{
    (void)pstate;

    const dmi::date date(2026, 9, 18);

    assert_int_equal(date.year(), 2026);
    assert_int_equal(date.month(), 9);
    assert_int_equal(date.day(), 18);

    // Unknown date is the only one which is false
    assert_true(static_cast<bool>(date));
    assert_false(static_cast<bool>(dmi::date()));
    assert_false(static_cast<bool>(dmi::date(0, 0, 0)));
    assert_true(static_cast<bool>(dmi::date(0, 0, 1)));
}

static void test_date_compare(void **pstate)
{
    (void)pstate;

    assert_true(dmi::date(2026, 9, 18) == dmi::date(2026, 9, 18));
    assert_true(dmi::date(2026, 9, 18) != dmi::date(2026, 9, 19));

    // Dates are ordered chronologically
    assert_true(dmi::date(2025, 12, 31) < dmi::date(2026, 1, 1));
    assert_true(dmi::date(2026, 9, 18) > dmi::date(2026, 9, 17));
    assert_true(dmi::date(2026, 9, 18) >= dmi::date(2026, 9, 18));
    assert_true(dmi::date() < dmi::date(1, 1, 1));
}

//
// Compile-time part of the contract. It is verified by building this test,
// and the assertion keeps the checks from being optimized away.
//
static void test_date_traits(void **pstate)
{
    (void)pstate;

    static_assert(std::is_trivially_copyable_v<dmi::date>);
    static_assert(sizeof(dmi::date) == sizeof(dmi::capi::dmi_date_t));

    // Every accessor is usable in a constant expression
    static_assert(dmi::date(2026, 9, 18).year() == 2026);
    static_assert(dmi::date(2026, 9, 18).month() == 9);
    static_assert(dmi::date(2026, 9, 18).day() == 18);
    static_assert(dmi::date(2026, 9, 18).native() == DMI_DATE(2026, 9, 18));
    static_assert(dmi::date(2026, 9, 17) < dmi::date(2026, 9, 18));
    static_assert(!static_cast<bool>(dmi::date()));

    // The packed value is not implicitly convertible to a date
    static_assert(!std::is_convertible_v<dmi::capi::dmi_date_t, dmi::date>);
    static_assert(std::is_constructible_v<dmi::date, dmi::capi::dmi_date_t>);

    // Dates are formattable, see the std::formatter specialization
    static_assert(std::is_default_constructible_v<std::formatter<dmi::date, char>>);

    assert_int_equal(dmi::date(2026, 9, 18).native(), DMI_DATE(2026, 9, 18));
}

static void test_date_parse(void **pstate)
{
    (void)pstate;

    // Firmware reports dates as MM/DD/YY or MM/DD/YYYY
    const auto full = dmi::date::parse("09/18/2026");
    assert_true(full.has_value());
    assert_int_equal(full->native(), DMI_DATE(2026, 9, 18));

    // A two-digit year is interpreted as 19YY
    const auto brief = dmi::date::parse("09/18/26");
    assert_true(brief.has_value());
    assert_int_equal(brief->native(), DMI_DATE(1926, 9, 18));

    // Malformed strings are rejected instead of yielding the unknown date
    static const char *const invalid[] = {
        "", "bogus", "09/18", "09/18/2026/12", "09-18-2026", "aa/18/2026", "+9/18/2026", "09/18/202611"
    };

    for (const auto *str : invalid)
        assert_false(dmi::date::parse(str).has_value());
}

static void test_date_format(void **pstate)
{
    (void)pstate;

    // Dates are formatted as ISO 8601
    assert_string_equal(dmi::date(2026, 9, 18).str().c_str(), "2026-09-18");
    assert_string_equal(dmi::date().str().c_str(), "0000-00-00");

    // Output stream and std::format() produce the same text as str()
    std::ostringstream stream;
    stream << dmi::date(2026, 9, 18);
    assert_string_equal(stream.str().c_str(), "2026-09-18");

    assert_string_equal(std::format("{}", dmi::date(2026, 9, 18)).c_str(), "2026-09-18");

    // Standard string format options are supported
    assert_string_equal(std::format("[{:>12}]", dmi::date(2026, 9, 18)).c_str(), "[  2026-09-18]");
}
