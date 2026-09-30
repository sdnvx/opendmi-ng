//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <strings.h>
#include <string.h>

#include <opendmi/vendor.h>
#include <opendmi/module/hpe.h>
#include <opendmi/utils/name.h>

static const dmi_name_set_t dmi_vendor_names =
{
    .code = "vendor",
    .names = DMI_NAMES({
        DMI_NAME_OTHER(DMI_VENDOR_OTHER),
        {
            .id   = DMI_VENDOR_ACER,
            .code = "acer",
            .name = "Acer"
        },
        {
            .id   = DMI_VENDOR_AMD,
            .code = "amd",
            .name = "AMD"
        },
        {
            .id   = DMI_VENDOR_AMI,
            .code = "ami",
            .name = "AMI"
        },
        {
            .id   = DMI_VENDOR_APPLE,
            .code = "apple",
            .name = "Apple"
        },
        {
            .id   = DMI_VENDOR_CISCO,
            .code = "cisco",
            .name = "Cisco"
        },
        {
            .id   = DMI_VENDOR_DELL,
            .code = "dell",
            .name = "Dell"
        },
        {
            .id   = DMI_VENDOR_HONOR,
            .code = "honor",
            .name = "Honor"
        },
        {
            .id   = DMI_VENDOR_HP,
            .code = "hp",
            .name = "HP"
        },
        {
            .id   = DMI_VENDOR_HPE,
            .code = "hpe",
            .name = "HPE"
        },
        {
            .id   = DMI_VENDOR_HUAWEI,
            .code = "huawei",
            .name = "Huawei"
        },
        {
            .id   = DMI_VENDOR_IBM,
            .code = "ibm",
            .name = "IBM"
        },
        {
            .id   = DMI_VENDOR_INTEL,
            .code = "intel",
            .name = "Intel"
        },
        {
            .id   = DMI_VENDOR_LENOVO,
            .code = "lenovo",
            .name = "Lenovo"
        },
        {
            .id   = DMI_VENDOR_UNISYS,
            .code = "unisys",
            .name = "Unisys"
        },
        {}
    })
};

static const dmi_vendor_spec_t dmi_vendor_specs[] =
{
    {
        .id    = DMI_VENDOR_ACER,
        .code  = "acer",
        .names = (const char *[]){
            "Acer",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_AMD,
        .code  = "amd",
        .names = (const char *[]){
            "AMD",
            "AMD processor",
            "Advanced Micro Devices, Inc.",
            "AuthenticAMD",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_AMI,
        .code  = "ami",
        .names = (const char *[]){
            "American Megatrends Inc.",
            "American Megatrends International, LLC.",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_APPLE,
        .code  = "apple",
        .names = (const char *[]){
            "Apple",
            "Apple Inc.",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_CISCO,
        .code  = "cisco",
        .names = (const char *[]){
            "Cisco Systems",
            "Cisco Systems Inc",
            "Cisco Systems, Inc.",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_DELL,
        .code  = "dell",
        .names = (const char *[]){
            "Dell Inc.",
            "Dell Computer Corporation",
            "Dell Computer Corp.",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_HONOR,
        .code  = "honor",
        .names = (const char *[]){
            "HONOR",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_HP,
        .code  = "hp",
        .names = (const char *[]){
            "HP",
            "Hewlett-Packard",
            nullptr
        },
        .detect = dmi_hpe_platform_detect
    },
    {
        .id    = DMI_VENDOR_HPE,
        .code  = "hpe",
        .names = (const char *[]){
            "HPE",
            "Hewlett-Packard Enterprise",
            "Hewlett Packard Enterprise",
            nullptr
        },
        .detect = dmi_hpe_platform_detect
    },
    {
        .id    = DMI_VENDOR_HUAWEI,
        .code  = "huawei",
        .names = (const char *[]){
            "HUAWEI",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_IBM,
        .code  = "ibm",
        .names = (const char *[]){
            "IBM",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_INTEL,
        .code  = "intel",
        .names = (const char *[]){
            "Intel",
            "Intel Corp.",
            "Intel Corporation",
            "Intel(R) Corporation",
            "GenuineIntel",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_LENOVO,
        .code  = "lenovo",
        .names = (const char *[]){
            "LENOVO",
            nullptr
        }
    },
    {
        .id    = DMI_VENDOR_UNISYS,
        .code  = "unisys",
        .names = (const char *[]){
            "UNISYS Corp.",
            "Unisys System",
            nullptr
        }
    },
    DMI_VENDOR_NULL
};

const char *dmi_vendor_name(dmi_vendor_t vendor)
{
    return dmi_name_lookup(&dmi_vendor_names, (int)vendor);
}

const dmi_vendor_spec_t *dmi_vendor_detect(const char *name)
{
    const dmi_vendor_spec_t *vendor;
    const char **vendor_name;

    if (name == nullptr)
        return nullptr;

    for (vendor = dmi_vendor_specs; vendor->id != DMI_VENDOR_INVALID; vendor++) {
        for (vendor_name = vendor->names; *vendor_name != nullptr; vendor_name++) {
            if (strcasecmp(name, *vendor_name) == 0)
                return vendor;
        }
    }

    return nullptr;
}
