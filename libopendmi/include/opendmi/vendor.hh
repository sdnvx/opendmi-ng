//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_VENDOR_HH
#define OPENDMI_VENDOR_HH

#pragma once

#include <type_traits>

namespace dmi {
    namespace capi {
#       include <opendmi/vendor.h>
    }

    enum class vendor : std::underlying_type_t<capi::dmi_vendor_t> {
        invalid = capi::DMI_VENDOR_INVALID, ///< Invalid
        other   = capi::DMI_VENDOR_OTHER,   ///< Other
        acer    = capi::DMI_VENDOR_ACER,    ///< Acer
        ami     = capi::DMI_VENDOR_AMI,     ///< AMI
        apple   = capi::DMI_VENDOR_APPLE,   ///< Apple
        dell    = capi::DMI_VENDOR_DELL,    ///< Dell
        hp      = capi::DMI_VENDOR_HP,      ///< HP
        hpe     = capi::DMI_VENDOR_HPE,     ///< HPE
        ibm     = capi::DMI_VENDOR_IBM,     ///< IBM
        intel   = capi::DMI_VENDOR_INTEL,   ///< Intel
        lenovo  = capi::DMI_VENDOR_LENOVO,  ///< Lenovo
    };
}

#endif // !OPENDMI_VENDOR_HH
