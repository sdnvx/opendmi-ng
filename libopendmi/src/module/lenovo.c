//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/lenovo.h>

#include <opendmi/entity/lenovo/oem.h>
#include <opendmi/entity/lenovo/records.h>
#include <opendmi/entity/lenovo/tvt.h>

// Structure types of the module
const dmi_type_t dmi_type_lenovo_tvt             = { .id = DMI_TYPE_ID(LENOVO_TVT)        };
const dmi_type_t dmi_type_lenovo_date            = { .id = DMI_TYPE_ID(LENOVO_DATE)       };
const dmi_type_t dmi_type_lenovo_tpm_info        = { .id = DMI_TYPE_ID(LENOVO_DATE)       };
const dmi_type_t dmi_type_lenovo_mobile_oem      = { .id = DMI_TYPE_ID(LENOVO_MOBILE_OEM) };
const dmi_type_t dmi_type_lenovo_device_presence = { .id = DMI_TYPE_ID(LENOVO_MOBILE_OEM) };
const dmi_type_t dmi_type_lenovo_bay_io          = { .id = DMI_TYPE_ID(LENOVO_MOBILE_OEM) };
const dmi_type_t dmi_type_lenovo_oem             = { .id = DMI_TYPE_ID(LENOVO_OEM)        };
const dmi_type_t dmi_type_lenovo_ecp             = { .id = DMI_TYPE_ID(LENOVO_OEM)        };
const dmi_type_t dmi_type_lenovo_mtm             = { .id = DMI_TYPE_ID(LENOVO_MTM)        };

/**
 * @brief Lenovo extension module.
 */
const dmi_module_t dmi_lenovo_module =
{
    .code      = "lenovo",
    .name      = "IBM/Lenovo extensions",
    .entities  = (const dmi_entity_spec_t *[]){
        &dmi_lenovo_tvt_spec,
        &dmi_lenovo_date_spec,
        &dmi_lenovo_tpm_info_spec,
        &dmi_lenovo_mtm_spec,
        // Specifications of the OEM structures of known layouts come first,
        // since the first signature a structure matches is the one it takes
        &dmi_lenovo_device_presence_spec,
        &dmi_lenovo_bay_io_spec,
        &dmi_lenovo_mobile_oem_spec,
        &dmi_lenovo_ecp_spec,
        &dmi_lenovo_oem_spec,
        nullptr
    },
    .platforms = DMI_PLATFORMS({
        { .firmware_vendor = DMI_VENDOR_IBM    },
        { .firmware_vendor = DMI_VENDOR_LENOVO },
        DMI_PLATFORM_NULL
    })
};
