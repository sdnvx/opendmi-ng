//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/intel-rsd.h>

#include "tpm-internal.h"

const dmi_name_set_t dmi_intel_rsd_tpm_status_names =
{
    .code  = "intel-rsd-tpm-status",
    .names = DMI_NAMES({
        {
            .id   = DMI_INTEL_RSD_TPM_STATUS_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {
            .id   = DMI_INTEL_RSD_TPM_STATUS_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {}
    })
};

DMI_NAME_FUNCTION(dmi_intel_rsd_tpm_status)
