//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/reader.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/tpm-device-internal.h>

const dmi_name_set_t dmi_tpm_device_feature_names =
{
    .code  = "tpm-device-feature",
    .names = DMI_NAMES({
        {
            .id   = 2,
            .code = "is-unsupported",
            .name = "Characteristics are not supported"
        },
        {
            .id   = 3,
            .code = "is-update-configurable",
            .name = "Configurable via firmware update"
        },
        {
            .id   = 4,
            .code = "is-software-configurable",
            .name = "Configurable via platform software support"
        },
        {
            .id = 5,
            .code = "is-proprietary-configurable",
            .name = "Configurable via OEM proprietary mechanism"
        },
        {}
    })
};
