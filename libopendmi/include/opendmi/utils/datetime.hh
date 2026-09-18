//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_DATETIME_HH
#define OPENDMI_UTILS_DATETIME_HH

#pragma once

#include <compare>
#include <format>
#include <optional>
#include <ostream>
#include <string>
#include <type_traits>

#include <opendmi/utils.hh>

namespace dmi {
    namespace capi {
#       include <opendmi/utils/datetime.h>
    }

    using date_t = capi::dmi_date_t;

    /**
     * @brief Date reported by firmware.
     *
     * A value type wrapping `dmi_date_t`: year, month and day packed into a
     * 32-bit integer. The default-constructed value is the unknown date,
     * which compares equal to `date(0, 0, 0)` and converts to `false`.
     *
     * Dates compare component-wise, so they are ordered chronologically.
     */
    class date
    {
    private:
        date_t m_value = DMI_DATE_NONE;

    public:
        /**
         * @brief Construct the unknown date.
         */
        constexpr date() noexcept = default;

        /**
         * @brief Construct a date from its components.
         *
         * The year is truncated to sixteen bits, the month and the day to
         * eight bits each. Components are not validated.
         */
        constexpr date(unsigned int year, unsigned int month, unsigned int day) noexcept
            : m_value(DMI_DATE(year, month, day)) { }

        /**
         * @brief Construct a date from a packed value of the C API.
         */
        constexpr explicit date(date_t value) noexcept
            : m_value(value) { }

        /**
         * @brief Calendar year, e.g. 2026.
         */
        [[nodiscard]]
        constexpr unsigned int year() const noexcept {
            return capi::dmi_date_year(m_value);
        }

        /**
         * @brief Month of the year, 1 to 12.
         */
        [[nodiscard]]
        constexpr unsigned int month() const noexcept {
            return capi::dmi_date_month(m_value);
        }

        /**
         * @brief Day of the month, 1 to 31.
         */
        [[nodiscard]]
        constexpr unsigned int day() const noexcept {
            return capi::dmi_date_day(m_value);
        }

        /**
         * @brief Check whether the date is known, i.e. not `0000-00-00`.
         */
        [[nodiscard]]
        constexpr explicit operator bool() const noexcept {
            return m_value != DMI_DATE_NONE;
        }

        /**
         * @brief Compare two dates component-wise, i.e. chronologically.
         */
        [[nodiscard]]
        constexpr auto operator<=>(const date &other) const noexcept = default;
        [[nodiscard]]
        constexpr bool operator==(const date &other) const noexcept = default;

        /**
         * @brief Parse a date as firmware reports it, `MM/DD/YY` or
         * `MM/DD/YYYY`.
         *
         * A two-digit year is interpreted as `19YY`, so `09/18/26` is
         * parsed as September 18, 1926.
         *
         * @param[in] str Date string to parse.
         * @return Parsed date, or an empty value if the string is malformed.
         */
        [[nodiscard]]
        static std::optional<date> parse(const std::string &str) {
            const date_t value = capi::dmi_date_parse(str.c_str());

            if (value == DMI_DATE_NONE)
                return std::nullopt;

            return date(value);
        }

        /**
         * @brief Format the date as an ISO 8601 string, `YYYY-MM-DD`.
         *
         * @return Formatted date string.
         * @throws std::bad_alloc on allocation failure.
         */
        [[nodiscard]]
        std::string str() const {
            return utils::adopt_string(capi::dmi_date_format(m_value));
        }

        /**
         * @brief Packed value, for use with the C API.
         */
        [[nodiscard]]
        constexpr date_t native() const noexcept {
            return m_value;
        }
    };

    /**
     * @brief Write the date to an output stream, as str() formats it.
     */
    inline std::ostream &operator<<(std::ostream &stream, const date &date)
    {
        return stream << date.str();
    }

    //
    // The packed layout is defined by the C API and must not drift
    //
    static_assert(date(2026, 9, 18).year() == 2026);
    static_assert(date(2026, 9, 18).month() == 9);
    static_assert(date(2026, 9, 18).day() == 18);
    static_assert(date(2026, 9, 18).native() == DMI_DATE(2026, 9, 18));
    static_assert(date().native() == DMI_DATE_NONE);
    static_assert(sizeof(date) == sizeof(date_t));
}

/**
 * @brief Format a date with `std::format()`, as str() formats it.
 *
 * The standard string format options, e.g. width, fill and alignment, are
 * supported: `std::format("{:>12}", date)`.
 */
template <>
struct std::formatter<dmi::date, char> : std::formatter<std::string, char>
{
    template <class Context>
    auto format(const dmi::date &value, Context &context) const
    {
        return std::formatter<std::string, char>::format(value.str(), context);
    }
};

#endif // !OPENDMI_UTILS_DATETIME_HH
