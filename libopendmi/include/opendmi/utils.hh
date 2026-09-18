//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_HH
#define OPENDMI_UTILS_HH

#pragma once

#include <memory>
#include <new>
#include <string>

namespace dmi {
    namespace capi {
#       include <opendmi/utils.h>
    }

    namespace utils {
        /**
         * @internal
         * @brief Take ownership of a string returned by the C API.
         *
         * @param[in] text String allocated by the library, or @c nullptr.
         * @return The string as `std::string`.
         * @throws std::bad_alloc if the string has not been formatted.
         */
        inline std::string adopt_string(char *text)
        {
            const std::unique_ptr<char, decltype(&capi::dmi_free)> owned(text, capi::dmi_free);

            if (owned == nullptr)
                throw std::bad_alloc();

            return owned.get();
        }
    }
}

#endif // !OPENDMI_UTILS_HH
