//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_UUID_HH
#define OPENDMI_UTILS_UUID_HH

#pragma once

#include <algorithm>
#include <array>
#include <compare>
#include <cstddef>
#include <format>
#include <iterator>
#include <ostream>
#include <span>
#include <string>

#include <opendmi/utils.hh>

namespace dmi {
    namespace capi {
#       include <opendmi/utils/uuid.h>
    }

    /**
     * @brief Universally unique identifier.
     *
     * A value type wrapping `dmi_uuid_t`, which holds 16 bytes in RFC 4122
     * byte order. SMBIOS stores the first three fields in little-endian byte
     * order instead, so raw SMBIOS data is converted with decode() and
     * encode().
     *
     * The default-constructed value is the nil UUID. SMBIOS gives special
     * meaning to two values: the nil UUID (all zeros) means that the ID is
     * not present, and the max UUID (all `FFh`) means that the ID is not
     * present but can be set. Both of them convert to `false`.
     *
     * UUIDs compare lexicographically by bytes in RFC 4122 order.
     *
     * @note Only the raw value of `dmi_uuid_t` is accessed here, so that
     *       the class is usable in constant expressions: reading another
     *       member of a union is not a constant expression.
     */
    class uuid
    {
    public:
        /**
         * @brief UUID bytes, either in RFC 4122 or in SMBIOS byte order.
         */
        using bytes_t = std::array<std::byte, 16>;

        /**
         * @brief Value type of the C API.
         */
        using native_t = capi::dmi_uuid_t;

    private:
        native_t m_value {};

    public:
        /**
         * @brief Construct the nil UUID.
         */
        constexpr uuid() noexcept = default;

        /**
         * @brief Construct a UUID from bytes in RFC 4122 order.
         */
        constexpr explicit uuid(const bytes_t &bytes) noexcept {
            for (std::size_t i = 0; i < bytes.size(); i++)
                m_value.__value[i] = std::to_integer<capi::dmi_byte_t>(bytes[i]);
        }

        /**
         * @brief Construct a UUID from a value of the C API.
         */
        constexpr explicit uuid(const native_t &value) noexcept
            : m_value(value) { }

        /**
         * @brief Decode a UUID as it is stored in SMBIOS structures.
         *
         * @param[in] data Raw 16 bytes in SMBIOS byte order.
         * @return Decoded UUID.
         */
        [[nodiscard]]
        static uuid decode(std::span<const std::byte, 16> data) noexcept {
            return uuid(capi::dmi_uuid_decode(reinterpret_cast<const capi::dmi_byte_t *>(data.data())));
        }

        /**
         * @brief Encode the UUID as it is stored in SMBIOS structures.
         *
         * @return Raw 16 bytes in SMBIOS byte order.
         */
        [[nodiscard]]
        bytes_t encode() const noexcept {
            bytes_t data;

            capi::dmi_uuid_encode(m_value, reinterpret_cast<uint8_t *>(data.data()));

            return data;
        }

        /**
         * @brief UUID bytes in RFC 4122 order.
         */
        [[nodiscard]]
        constexpr std::span<const capi::dmi_byte_t, 16> bytes() const noexcept {
            return m_value.__value;
        }

        /**
         * @brief Check whether this is the nil UUID, i.e. all zeros.
         *
         * In SMBIOS it means that the ID is not present.
         */
        [[nodiscard]]
        constexpr bool is_nil() const noexcept {
            return std::ranges::all_of(m_value.__value, [](capi::dmi_byte_t value) { return value == 0x00; });
        }

        /**
         * @brief Check whether this is the max UUID, i.e. all `FFh`.
         *
         * In SMBIOS it means that the ID is not present, but can be set.
         */
        [[nodiscard]]
        constexpr bool is_max() const noexcept {
            return std::ranges::all_of(m_value.__value, [](capi::dmi_byte_t value) { return value == 0xFF; });
        }

        /**
         * @brief Check whether the ID is present, i.e. neither the nil nor
         * the max UUID.
         */
        [[nodiscard]]
        constexpr explicit operator bool() const noexcept {
            return !is_nil() && !is_max();
        }

        /**
         * @brief Compare two UUIDs lexicographically by bytes.
         *
         * Comparisons are not defaulted, as unions have none.
         */
        [[nodiscard]]
        constexpr std::strong_ordering operator<=>(const uuid &other) const noexcept {
            return std::lexicographical_compare_three_way(
                std::begin(m_value.__value), std::end(m_value.__value),
                std::begin(other.m_value.__value), std::end(other.m_value.__value));
        }

        [[nodiscard]]
        constexpr bool operator==(const uuid &other) const noexcept {
            return std::ranges::equal(m_value.__value, other.m_value.__value);
        }

        /**
         * @brief Format the UUID as a string.
         *
         * Hexadecimal digits are uppercase, as the library formats UUID
         * attributes: `00112233-4455-6677-8899-AABBCCDDEEFF`.
         *
         * @return Formatted UUID string.
         * @throws std::bad_alloc on allocation failure.
         */
        [[nodiscard]]
        std::string str() const {
            std::string result;

            result.reserve(36);
            for (std::size_t i = 0; i < std::size(m_value.__value); i++) {
                if ((i == 4) || (i == 6) || (i == 8) || (i == 10))
                    result += '-';

                std::format_to(std::back_inserter(result), "{:02X}", m_value.__value[i]);
            }

            return result;
        }

        /**
         * @brief Value for use with the C API.
         */
        [[nodiscard]]
        constexpr native_t native() const noexcept {
            return m_value;
        }
    };

    /**
     * @brief Write the UUID to an output stream, as str() formats it.
     */
    inline std::ostream &operator<<(std::ostream &stream, const uuid &uuid)
    {
        return stream << uuid.str();
    }

    //
    // The class adds nothing to the C API value
    //
    static_assert(sizeof(uuid) == sizeof(uuid::native_t));
    static_assert(uuid().is_nil());
    static_assert(uuid(uuid().native()).is_nil());
}

/**
 * @brief Format a UUID with `std::format()`, as str() formats it.
 *
 * The standard string format options, e.g. width, fill and alignment, are
 * supported: `std::format("{:>40}", uuid)`.
 */
template <>
struct std::formatter<dmi::uuid, char> : std::formatter<std::string, char>
{
    template <class Context>
    auto format(const dmi::uuid &value, Context &context) const
    {
        return std::formatter<std::string, char>::format(value.str(), context);
    }
};

#endif // !OPENDMI_UTILS_UUID_HH
