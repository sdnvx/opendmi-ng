//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/reader.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/oob-remote-access-internal.h>

const dmi_name_set_t dmi_oob_connection_names =
{
    .code  = "oob-connection",
    .names = DMI_NAMES({
        {
            .id   = 0,
            .code = "is-inbound-enabled",
            .name = "Inbound connections enabled"
        },
        {
            .id   = 1,
            .code = "is-outbound-enabled",
            .name = "Outbound connections enabled"
        },
        {}
    })
};
