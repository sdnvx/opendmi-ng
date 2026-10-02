//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include "battery-internal.h"

const dmi_name_set_t dmi_battery_chemistry_names =
{
    .code  = "battery-chemistry",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_BATTERY_CHEMISTRY_UNSPEC),
        DMI_NAME_OTHER(DMI_BATTERY_CHEMISTRY_OTHER),
        DMI_NAME_UNKNOWN(DMI_BATTERY_CHEMISTRY_UNKNOWN),
        {
            .id   = DMI_BATTERY_CHEMISTRY_LEAD_ACID,
            .code = "lead-acid",
            .name = "Lead acid"
        },
        {
            .id   = DMI_BATTERY_CHEMISTRY_NI_CD,
            .code = "ni-cd",
            .name = "Nickel cadmium"
        },
        {
            .id   = DMI_BATTERY_CHEMISTRY_NI_MH,
            .code = "ni-mh",
            .name = "Nickel metal hydride"
        },
        {
            .id   = DMI_BATTERY_CHEMISTRY_LI_ION,
            .code = "li-ion",
            .name = "Lithium-ion"
        },
        {
            .id   = DMI_BATTERY_CHEMISTRY_ZN_AIR,
            .code = "zn-air",
            .name = "Zinc air"
        },
        {
            .id   = DMI_BATTERY_CHEMISTRY_LI_PO,
            .code = "li-po",
            .name = "Lithium polymer"
        },
        {}
    })
};

DMI_NAME_FUNCTION(dmi_battery_chemistry)
