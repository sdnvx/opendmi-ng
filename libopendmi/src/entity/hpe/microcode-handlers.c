//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/platform.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "microcode-internal.h"

/**
 * @internal
 * @brief Read a binary-coded decimal part of the date of a patch.
 *
 * @details Date is 0xMMDDYYYY, each part in binary-coded decimal, the way the
 * header of the microcode update writes it.
 *
 * @param[in] value  Part of the date, aligned to its lowest digit.
 * @param[in] digits Number of the digits of the part.
 *
 * @return Value of the part.
 */
static unsigned dmi_hpe_microcode_bcd(uint32_t value, unsigned digits);

bool dmi_hpe_microcode_derive(dmi_entity_t *entity)
{
    dmi_hpe_microcode_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_microcode));
    if (info == nullptr)
        return false;

    const dmi_platform_t *platform = dmi_get_platform(dmi_entity_context(entity));
    bool is_amd = (platform != nullptr) and (platform->processor_vendor == DMI_VENDOR_AMD);

    for (size_t i = 0; i < info->patch_count; i++) {
        dmi_hpe_microcode_patch_t *patch = &info->patches[i];
        uint32_t raw = patch->raw_date;

        // AMD platforms leave the base family out, whose bits the rest of
        // the signature takes the place of, up to the extended family
        uint32_t cpuid = patch->cpuid;
        patch->signature = is_amd
            ? (cpuid & 0xFFu) | (0x0Fu << 8) | (((cpuid >> 8) & 0xFFu) << 16) | (((cpuid >> 16) & 0x0Fu) << 24)
            : cpuid;

        patch->date = dmi_date(dmi_hpe_microcode_bcd(raw & 0xFFFF, 4),
                               dmi_hpe_microcode_bcd(raw >> 24, 2),
                               dmi_hpe_microcode_bcd((raw >> 16) & 0xFF, 2));
    }

    return true;
}

static unsigned dmi_hpe_microcode_bcd(uint32_t value, unsigned digits)
{
    unsigned result = 0;

    for (unsigned i = digits; i > 0; i--)
        result = result * 10 + ((value >> ((i - 1) * 4)) & 0x0F);

    return result;
}
