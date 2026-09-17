//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_CONTEXT_HH
#define OPENDMI_CONTEXT_HH

#pragma once

#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>

#include <opendmi/types.hh>
#include <opendmi/vendor.hh>

namespace dmi {
    class entity_range;
    class filter;
    class logger;
    class module;
    class version;

    namespace capi {
#       include <opendmi/context.h>
    }

    enum class context_flags : std::underlying_type_t<capi::dmi_context_flags_t> {
        relaxed = capi::DMI_CONTEXT_FLAG_RELAXED,
        strict  = capi::DMI_CONTEXT_FLAG_STRICT,
        link    = capi::DMI_CONTEXT_FLAG_LINK
    };

    /**
     * @brief DMI context.
     *
     * The context owns the connection to a DMI data source, the decoded structure
     * registry and the error queue. It is the entry point of the API: every other
     * object is either created from a context or borrowed from it.
     *
     * A context is created closed. Opening it reads the entry point and the
     * structure table, decodes the structures and makes them available through
     * entities(). A context may be opened and closed repeatedly, and opening it
     * again discards the previously decoded structures.
     *
     * Entities, attributes and strings obtained from a context are non-owning
     * views into its memory. They are invalidated by close(), by opening the
     * context again and by its destruction.
     *
     * @note A context is not thread-safe, and neither are the views into it.
     * @note Unless stated otherwise, member functions throw dmi::error on
     *       failure, using the error queue of the context as the message source.
     * @note A moved-from context is empty: its native_handle() is `nullptr`, and
     *       only destruction and assignment are allowed on it.
     */
    class context
    {
    public:
        /**
         * @brief Create a closed context.
         *
         * @param[in] flags Context flags.
         * @throws dmi::error if the context cannot be created.
         */
        explicit context(context_flags flags = context_flags::relaxed);

        /**
         * @brief Close the context and release its resources.
         */
        ~context();

        context(context &&other) noexcept;
        context &operator=(context &&other) noexcept;

        context(const context &) = delete;
        context &operator=(const context &) = delete;

        /**
         * @brief Open the platform DMI data source.
         *
         * @param[in] device Path to the memory device, or an empty view to use
         *                   the platform default.
         * @throws dmi::error if the source cannot be opened or decoded.
         */
        void open(std::string_view device = {});

        /**
         * @brief Load a binary dump file.
         *
         * The dump format is the one used by `dmidecode --dump-bin`.
         *
         * @param[in] path Path to the dump file.
         * @throws dmi::error if the dump cannot be loaded or decoded.
         */
        void load_dump(const std::filesystem::path &path);

        /**
         * @brief Save the DMI data of an open context to a dump file.
         *
         * @param[in] path      Path to the dump file.
         * @param[in] overwrite Overwrite an existing file.
         * @throws dmi::error if the context is closed or the file cannot be
         *         written.
         */
        void save_dump(const std::filesystem::path &path, bool overwrite = false);

        /**
         * @brief Close the context, invalidating all views into it.
         */
        void close() noexcept;

        /**
         * @brief Check whether the context is open.
         */
        [[nodiscard]] bool is_open() const noexcept;

        /**
         * @brief All decoded structures, in table order.
         *
         * @throws dmi::error if the context is closed.
         */
        [[nodiscard]] entity_range entities() const;

        /**
         * @brief Decoded structures matching the filter, in table order.
         *
         * The filter is borrowed and must outlive the returned range.
         *
         * @throws dmi::error if the context is closed.
         */
        [[nodiscard]] entity_range entities(const filter &filter) const;

        /**
         * @brief SMBIOS version reported by the entry point.
         */
        [[nodiscard]] dmi::version smbios_version() const noexcept;

        /**
         * @brief Detected firmware vendor.
         */
        [[nodiscard]] dmi::vendor vendor() const noexcept;

        /**
         * @brief Firmware vendor name as reported by the firmware.
         */
        [[nodiscard]] std::string_view vendor_name() const noexcept;

        /**
         * @brief Number of structures reported by the entry point.
         *
         * @note Some vendors report zero, so this is not a reliable count of the
         *       structures available through entities().
         */
        [[nodiscard]] std::size_t entity_count() const noexcept;

        /**
         * @brief Raw structure table data.
         */
        [[nodiscard]] std::span<const std::byte> table_data() const noexcept;

        /**
         * @brief Raw entry point data, or an empty view if the backend provides
         * none.
         */
        [[nodiscard]] std::span<const std::byte> entry_data() const noexcept;

        /**
         * @brief Context flags.
         */
        [[nodiscard]] context_flags flags() const noexcept;

        /**
         * @brief Set context flags. Takes effect the next time the context is
         * opened.
         */
        void set_flags(context_flags flags) noexcept;

        /**
         * @brief Enable a vendor extension module.
         *
         * The module is borrowed and must outlive the context.
         *
         * @throws dmi::error if the module is already enabled or its structure
         *         types conflict with the enabled ones.
         */
        void add_extension(const module &module);

        /**
         * @brief Check whether a vendor extension module is enabled.
         */
        [[nodiscard]] bool has_extension(const module &module) const noexcept;

        /**
         * @brief Set the logger.
         *
         * The logger is borrowed and must outlive the context.
         */
        void set_logger(logger &logger) noexcept;

        /**
         * @brief Find a structure type by its code, e.g. "memory-device".
         *
         * @return Structure type, or an empty value if the code is unknown.
         */
        [[nodiscard]] std::optional<dmi::type> find_type(std::string_view code) const noexcept;

        /**
         * @brief Human-readable name of a structure type.
         *
         * Types without a specification are named "OEM-specific" or "Unknown".
         */
        [[nodiscard]] std::string_view type_name(dmi::type type) const noexcept;

        /**
         * @brief Underlying C context, for use with the C API.
         *
         * The context keeps its ownership.
         */
        [[nodiscard]] capi::dmi_context_t *native_handle() const noexcept;

    private:
        capi::dmi_context_t *m_context = nullptr;
    };
}

#endif // !OPENDMI_CONTEXT_HH
