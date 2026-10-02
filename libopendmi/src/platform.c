//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/platform.h>
#include <opendmi/utils.h>
#include <opendmi/utils/string.h>
#include <opendmi/internal.h>

/**
 * @internal
 * @brief Check whether a vendor satisfies a condition on it, which may be
 * `DMI_VENDOR_ANY`.
 */
static bool dmi_vendor_match(dmi_vendor_t condition, dmi_vendor_t vendor);

dmi_platform_t *dmi_platform_create(dmi_context_t *context)
{
    dmi_platform_t *platform = dmi_alloc(context, sizeof(*platform));

    if (platform == nullptr)
        return nullptr;

    platform->context          = context;
    platform->firmware_vendor  = DMI_VENDOR_OTHER;
    platform->system_vendor    = DMI_VENDOR_OTHER;
    platform->baseboard_vendor = DMI_VENDOR_OTHER;
    platform->processor_vendor = DMI_VENDOR_OTHER;

    return platform;
}

dmi_platform_t *dmi_platform_clone(const dmi_platform_t *platform)
{
    if (platform == nullptr)
        return dmi_trace_argument_null(nullptr, platform, nullptr);

    dmi_platform_t *copy = dmi_platform_create(platform->context);
    if (copy == nullptr)
        return nullptr;

    copy->firmware_vendor  = platform->firmware_vendor;
    copy->system_vendor    = platform->system_vendor;
    copy->baseboard_vendor = platform->baseboard_vendor;
    copy->processor_vendor = platform->processor_vendor;
    copy->generation       = platform->generation;

    if (not dmi_platform_set_product(copy, platform->product) or
        not dmi_platform_set_family(copy, platform->family))
    {
        dmi_platform_destroy(copy);
        return nullptr;
    }

    return copy;
}

bool dmi_platform_set_product(dmi_platform_t *platform, const char *product)
{
    if (platform == nullptr)
        return dmi_trace_argument_null(nullptr, platform);

    return dmi_string_set(platform->context, &platform->product, product);
}

bool dmi_platform_set_family(dmi_platform_t *platform, const char *family)
{
    if (platform == nullptr)
        return dmi_trace_argument_null(nullptr, platform);

    return dmi_string_set(platform->context, &platform->family, family);
}

void dmi_platform_destroy(dmi_platform_t *platform)
{
    if (platform == nullptr)
        return;

    dmi_free(platform->product);
    dmi_free(platform->family);
    dmi_free(platform);
}

bool dmi_platform_match(const dmi_platform_t *platform, const dmi_platform_match_t *match)
{
    if (platform == nullptr)
        return dmi_trace_argument_null(nullptr, platform);
    if (match == nullptr)
        return dmi_trace_argument_null(nullptr, match);

    if (not dmi_vendor_match(match->firmware_vendor, platform->firmware_vendor) or
        not dmi_vendor_match(match->system_vendor, platform->system_vendor) or
        not dmi_vendor_match(match->baseboard_vendor, platform->baseboard_vendor) or
        not dmi_vendor_match(match->processor_vendor, platform->processor_vendor))
    {
        return false;
    }

    if (match->family == nullptr)
        return true;

    return (platform->family != nullptr) and (strcmp(platform->family, match->family) == 0);
}

bool dmi_platform_in_generations(
        const dmi_platform_t    *platform,
        const dmi_generations_t *generations)
{
    if (generations == nullptr)
        return true;

    if ((generations->minimum == 0) and (generations->maximum == 0))
        return true;

    // Layouts of different generations are told apart by the range only, so
    // no bounded range is taken for a platform of unknown generation
    if ((platform == nullptr) or (platform->generation == 0))
        return false;

    if ((generations->minimum != 0) and (platform->generation < generations->minimum))
        return false;
    if ((generations->maximum != 0) and (platform->generation > generations->maximum))
        return false;

    return true;
}

static bool dmi_vendor_match(dmi_vendor_t condition, dmi_vendor_t vendor)
{
    return (condition == DMI_VENDOR_ANY) or (condition == vendor);
}
