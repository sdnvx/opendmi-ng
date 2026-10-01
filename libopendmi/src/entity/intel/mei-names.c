//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include "mei-internal.h"

const dmi_name_set_t dmi_intel_me_state_names =
{
    .code  = "intel-me-state",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_INTEL_ME_STATE_UNSPEC),
        {
            .id   = DMI_INTEL_ME_STATE_RESET,
            .code = "reset",
            .name = "Reset"
        },
        {
            .id   = DMI_INTEL_ME_STATE_INIT,
            .code = "init",
            .name = "Initializing"
        },
        {
            .id   = DMI_INTEL_ME_STATE_RECOVERY,
            .code = "recovery",
            .name = "Recovery"
        },
        {
            .id   = DMI_INTEL_ME_STATE_TEST,
            .code = "test",
            .name = "Test"
        },
        {
            .id   = DMI_INTEL_ME_STATE_M3_NO_UMA,
            .code = "m3-no-uma",
            .name = "M3 without UMA"
        },
        {
            .id   = DMI_INTEL_ME_STATE_NORMAL,
            .code = "normal",
            .name = "Normal"
        },
        {
            .id   = DMI_INTEL_ME_STATE_WAIT,
            .code = "wait",
            .name = "Waiting"
        },
        {
            .id   = DMI_INTEL_ME_STATE_TRANSITION,
            .code = "transition",
            .name = "Transition"
        },
        {
            .id   = DMI_INTEL_ME_STATE_INVALID_CPU,
            .code = "invalid-cpu",
            .name = "Invalid CPU plugged in"
        },
        {}
    })
};

const dmi_name_set_t dmi_intel_me_mode_names =
{
    .code  = "intel-me-mode",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_INTEL_ME_MODE_UNSPEC),
        {
            .id   = DMI_INTEL_ME_MODE_NORMAL,
            .code = "normal",
            .name = "Normal"
        },
        {
            .id   = DMI_INTEL_ME_MODE_DEBUG,
            .code = "debug",
            .name = "Debug"
        },
        {
            .id   = DMI_INTEL_ME_MODE_DISABLED,
            .code = "disabled",
            .name = "Temporarily disabled"
        },
        {
            .id   = DMI_INTEL_ME_MODE_OVERRIDE_JUMPER,
            .code = "override-jumper",
            .name = "Security override by jumper"
        },
        {
            .id   = DMI_INTEL_ME_MODE_OVERRIDE_MESSAGE,
            .code = "override-message",
            .name = "Security override by message"
        },
        {}
    })
};

// Error codes, named as coreboot names them
const dmi_name_set_t dmi_intel_me_error_names =
{
    .code  = "intel-me-error",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_INTEL_ME_ERROR_UNSPEC),
        {
            .id   = DMI_INTEL_ME_ERROR_NONE,
            .code = "none",
            .name = "No error"
        },
        {
            .id   = DMI_INTEL_ME_ERROR_UNCATEGORIZED,
            .code = "uncategorized",
            .name = "Uncategorized failure"
        },
        {
            .id   = DMI_INTEL_ME_ERROR_IMAGE,
            .code = "image",
            .name = "Image failure"
        },
        {
            .id   = DMI_INTEL_ME_ERROR_DEBUG,
            .code = "debug",
            .name = "Debug failure"
        },
        {}
    })
};

const dmi_name_set_t dmi_intel_me_sku_names =
{
    .code  = "intel-me-sku",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_INTEL_ME_SKU_UNSPEC),
        {
            .id   = DMI_INTEL_ME_SKU_CONSUMER,
            .code = "consumer",
            .name = "Consumer"
        },
        {
            .id   = DMI_INTEL_ME_SKU_CORPORATE,
            .code = "corporate",
            .name = "Corporate"
        },
        {
            .id   = DMI_INTEL_ME_SKU_LITE,
            .code = "lite",
            .name = "Lite"
        },
        {}
    })
};

const char *dmi_intel_me_state_name(dmi_intel_me_state_t value)
{
    return dmi_name_lookup(&dmi_intel_me_state_names, (int)value);
}

const char *dmi_intel_me_error_name(dmi_intel_me_error_t value)
{
    return dmi_name_lookup(&dmi_intel_me_error_names, (int)value);
}

const char *dmi_intel_me_mode_name(dmi_intel_me_mode_t value)
{
    return dmi_name_lookup(&dmi_intel_me_mode_names, (int)value);
}

const char *dmi_intel_me_sku_name(dmi_intel_me_sku_t value)
{
    return dmi_name_lookup(&dmi_intel_me_sku_names, (int)value);
}
