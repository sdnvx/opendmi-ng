//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_VERSION_HH
#define OPENDMI_UTILS_VERSION_HH

#pragma once

#include <compare>
#include <format>
#include <ostream>
#include <string>
#include <type_traits>

#include <opendmi/utils.hh>

namespace dmi {
    namespace capi {
#       include <opendmi/utils/version.h>
    }

    using version_t = capi::dmi_version_t;
    using version_level_t = capi::dmi_version_level_t;

    /**
     * @brief Detail level of a formatted version string.
     */
    enum class version_level : std::underlying_type_t<version_level_t> {
        major    = capi::DMI_VERSION_LEVEL_MAJOR,    ///< `major`
        minor    = capi::DMI_VERSION_LEVEL_MINOR,    ///< `major.minor`
        revision = capi::DMI_VERSION_LEVEL_REVISION  ///< `major.minor.revision`
    };

    /**
     * @brief SMBIOS version number.
     *
     * A value type wrapping `dmi_version_t`: three components packed into a
     * 32-bit integer. The default-constructed value is the unknown version,
     * which compares equal to `version(0, 0, 0)` and converts to `false`.
     *
     * Versions compare component-wise, so ordering can be used to test whether a
     * structure or a field is supported:
     *
     * ```cpp
     * if (context.smbios_version() >= dmi::version(3, 1)) { ... }
     * ```
     */
    class version
    {
    private:
        version_t m_value = DMI_VERSION_NONE;

    public:
        /**
         * @brief Construct the unknown version.
         */
        constexpr version() noexcept = default;

        /**
         * @brief Construct a version from its components.
         *
         * Components are truncated to eight bits each.
         */
        constexpr version(unsigned int major, unsigned int minor, unsigned int revision = 0) noexcept
            : m_value(DMI_VERSION(major, minor, revision)) { }

        /**
         * @brief Construct a version from a packed value of the C API.
         */
        constexpr explicit version(version_t value) noexcept
            : m_value(value) { }

        /**
         * @brief Major component.
         */
        [[nodiscard]]
        constexpr unsigned int major_number() const noexcept {
            return capi::dmi_version_major(m_value);
        }

        /**
         * @brief Minor component.
         */
        [[nodiscard]]
        constexpr unsigned int minor_number() const noexcept {
            return capi::dmi_version_minor(m_value);
        }

        /**
         * @brief Revision component.
         */
        [[nodiscard]]
        constexpr unsigned int revision_number() const noexcept {
            return capi::dmi_version_revision(m_value);
        }

        /**
         * @brief Check whether the version is known, i.e. not `0.0.0`.
         */
        [[nodiscard]]
        constexpr explicit operator bool() const noexcept {
            return m_value != DMI_VERSION_NONE;
        }

        /**
         * @brief Compare two versions component-wise.
         */
        [[nodiscard]]
        constexpr auto operator<=>(const version &other) const noexcept = default;
        [[nodiscard]]
        constexpr bool operator==(const version &other) const noexcept = default;

        /**
         * @brief Format the version as a string.
         *
         * The revision component is omitted when it is zero, so `3.4.0` is
         * formatted as "3.4".
         *
         * @return Formatted version string.
         * @throws std::bad_alloc on allocation failure.
         */
        [[nodiscard]]
        std::string str() const {
            return utils::adopt_string(capi::dmi_version_format(m_value));
        }

        /**
         * @brief Format the version as a string with explicit detail level.
         *
         * Unlike str(), every requested component is included, so `3.4.0` is
         * formatted as "3.4.0" at the revision level.
         *
         * @param[in] level Detail level to include.
         * @return Formatted version string.
         * @throws std::bad_alloc on allocation failure.
         */
        [[nodiscard]]
        std::string str(version_level level) const {
            return utils::adopt_string(capi::dmi_version_format_ex(m_value, static_cast<version_level_t>(level)));
        }

        /**
         * @brief Packed value, for use with the C API.
         */
        [[nodiscard]]
        constexpr version_t native() const noexcept {
            return m_value;
        }
    };

    /**
     * @brief Write the version to an output stream, as str() formats it.
     */
    inline std::ostream &operator<<(std::ostream &stream, const version &version)
    {
        return stream << version.str();
    }

    //
    // The packed layout is defined by the C API and must not drift
    //
    static_assert(version(3, 4, 1).major_number() == 3);
    static_assert(version(3, 4, 1).minor_number() == 4);
    static_assert(version(3, 4, 1).revision_number() == 1);
    static_assert(version(3, 4, 1).native() == DMI_VERSION(3, 4, 1));
    static_assert(version().native() == DMI_VERSION_NONE);
    static_assert(sizeof(version) == sizeof(version_t));
}

/**
 * @brief Format a version with `std::format()`, as str() formats it.
 *
 * The standard string format options, e.g. width, fill and alignment, are
 * supported: `std::format("{:>8}", version)`.
 */
template <>
struct std::formatter<dmi::version, char> : std::formatter<std::string, char>
{
    template <class Context>
    auto format(const dmi::version &value, Context &context) const
    {
        return std::formatter<std::string, char>::format(value.str(), context);
    }
};

#endif // !OPENDMI_UTILS_VERSION_HH
