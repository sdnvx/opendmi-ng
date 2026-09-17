//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
// DRAFT: contract of the C++ version type. Declarations only, no
// implementation.
//
#ifndef OPENDMI_UTILS_VERSION_HH
#define OPENDMI_UTILS_VERSION_HH

#pragma once

#include <compare>
#include <iosfwd>
#include <string>
#include <type_traits>

namespace dmi {
    namespace capi {
#       include <opendmi/utils/version.h>
    }

    /**
     * @brief Detail level of a formatted version string.
     */
    enum class version_level : std::underlying_type_t<capi::dmi_version_level_t> {
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
        capi::dmi_version_t m_value = DMI_VERSION_NONE;

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
        constexpr version(unsigned int major, unsigned int minor, unsigned int revision = 0) noexcept;

        /**
         * @brief Construct a version from a packed value of the C API.
         */
        constexpr explicit version(capi::dmi_version_t value) noexcept;

        /**
         * @brief Major component.
         */
        [[nodiscard]] constexpr unsigned int major() const noexcept;

        /**
         * @brief Minor component.
         */
        [[nodiscard]] constexpr unsigned int minor() const noexcept;

        /**
         * @brief Revision component.
         */
        [[nodiscard]] constexpr unsigned int revision() const noexcept;

        /**
         * @brief Check whether the version is known, i.e. not `0.0.0`.
         */
        [[nodiscard]] constexpr explicit operator bool() const noexcept;

        /**
         * @brief Compare two versions component-wise.
         */
        [[nodiscard]] constexpr auto operator<=>(const version &other) const noexcept = default;
        [[nodiscard]] constexpr bool operator==(const version &other) const noexcept = default;

        /**
         * @brief Format the version as a string.
         *
         * Trailing zero components are omitted at the revision level, so
         * `3.4.0` is formatted as "3.4".
         *
         * @param[in] level Detail level to include.
         * @return Formatted version string.
         * @throws std::bad_alloc on allocation failure.
         */
        [[nodiscard]] std::string str(version_level level = version_level::revision) const;

        /**
         * @brief Packed value, for use with the C API.
         */
        [[nodiscard]] constexpr capi::dmi_version_t native() const noexcept;
    };

    /**
     * @brief Write the version to an output stream, as str() formats it.
     */
    std::ostream &operator<<(std::ostream &stream, const version &version);

    //
    // The packed layout is defined by the C API and must not drift
    //
    static_assert(version(3, 4, 1).native() == DMI_VERSION(3, 4, 1));
    static_assert(version().native() == DMI_VERSION_NONE);
    static_assert(sizeof(version) == sizeof(capi::dmi_version_t));
}

#endif // !OPENDMI_UTILS_VERSION_HH
