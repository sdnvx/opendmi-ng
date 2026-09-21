//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-tpm-internal.h>

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

const char *dmi_intel_rsd_tpm_status_name(dmi_intel_rsd_tpm_status_t value)
{
    return dmi_name_lookup(&dmi_intel_rsd_tpm_status_names, (int)value);
}
