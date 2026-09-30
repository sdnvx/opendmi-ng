//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/common-internal.h>
#include <opendmi/entity/hpe/inventory-internal.h>

const dmi_entity_spec_t dmi_hpe_inventory_spec =
{
    .type        = DMI_TYPE(HPE_INVENTORY),
    .code        = "hpe-inventory",
    .name        = "HP/HPE firmware inventory record",
    .description = (const char *[]){
        "Tells the version of the firmware of a device which reports it "
        "through its UEFI driver.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x27,
        .decoded_length = sizeof(dmi_hpe_inventory_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_inventory_t, correlation_handle, dmi_word_t),
        DMI_FIELD(dmi_hpe_inventory_t, package_version,    dmi_dword_t),
        DMI_FIELD_STRING(dmi_hpe_inventory_t, version_string),
        DMI_FIELD(dmi_hpe_inventory_t, image_size,     dmi_qword_t),
        DMI_FIELD(dmi_hpe_inventory_t, attrs_defined,  dmi_qword_t),
        DMI_FIELD(dmi_hpe_inventory_t, attrs_set,      dmi_qword_t),
        DMI_FIELD(dmi_hpe_inventory_t, lowest_version, dmi_dword_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, correlation_handle, HANDLE, {
            .code    = "correlation-handle",
            .name    = "Device correlation handle",
            .targets = dmi_types(DMI_TYPE(HPE_DEVICE_CORRELATION))
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, package_version, INTEGER, {
            .code  = "package-version",
            .name  = "Package version",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, version_string, STRING, {
            .code = "version-string",
            .name = "Version string"
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, image_size, SIZE, {
            .code   = "image-size",
            .name   = "Image size",
            .unspec = dmi_value_ptr((uint64_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, attrs_defined, INTEGER, {
            .code  = "attrs-defined",
            .name  = "Attributes defined",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, attrs_set, INTEGER, {
            .code  = "attrs-set",
            .name  = "Attributes set",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, is_updatable, ENUM, {
            .code   = "is-updatable",
            .name   = "Updatable",
            .unspec = dmi_value_ptr(DMI_HPE_FLAG_UNSPEC),
            .values = &dmi_hpe_flag_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, is_reset_required, ENUM, {
            .code   = "is-reset-required",
            .name   = "Reset required",
            .unspec = dmi_value_ptr(DMI_HPE_FLAG_UNSPEC),
            .values = &dmi_hpe_flag_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, is_auth_required, ENUM, {
            .code   = "is-auth-required",
            .name   = "Authentication required",
            .unspec = dmi_value_ptr(DMI_HPE_FLAG_UNSPEC),
            .values = &dmi_hpe_flag_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, is_in_use, ENUM, {
            .code   = "is-in-use",
            .name   = "In use",
            .unspec = dmi_value_ptr(DMI_HPE_FLAG_UNSPEC),
            .values = &dmi_hpe_flag_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, is_uefi_image, ENUM, {
            .code   = "is-uefi-image",
            .name   = "UEFI image",
            .unspec = dmi_value_ptr(DMI_HPE_FLAG_UNSPEC),
            .values = &dmi_hpe_flag_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_inventory_t, lowest_version, INTEGER, {
            .code   = "lowest-version",
            .name   = "Lowest supported version",
            .unspec = dmi_value_ptr((uint32_t)0),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_inventory_derive
    }
};

/**
 * @internal
 * @brief Value of an attribute, which is undefined unless the mask of the
 * defined attributes has it.
 */
static dmi_hpe_flag_t dmi_hpe_inventory_flag(const dmi_hpe_inventory_t *info, dmi_hpe_inventory_attr_t attr)
{
    uint64_t bit = UINT64_C(1) << attr;

    if (not (info->attrs_defined & bit))
        return DMI_HPE_FLAG_UNSPEC;

    return (info->attrs_set & bit) ? DMI_HPE_FLAG_YES : DMI_HPE_FLAG_NO;
}

bool dmi_hpe_inventory_derive(dmi_entity_t *entity)
{
    dmi_hpe_inventory_t *info = dmi_entity_info(entity, DMI_TYPE(HPE_INVENTORY));
    if (info == nullptr)
        return false;

    info->is_updatable      = dmi_hpe_inventory_flag(info, DMI_HPE_INVENTORY_ATTR_UPDATABLE);
    info->is_reset_required = dmi_hpe_inventory_flag(info, DMI_HPE_INVENTORY_ATTR_RESET);
    info->is_auth_required  = dmi_hpe_inventory_flag(info, DMI_HPE_INVENTORY_ATTR_AUTHENTICATED);
    info->is_in_use         = dmi_hpe_inventory_flag(info, DMI_HPE_INVENTORY_ATTR_IN_USE);
    info->is_uefi_image     = dmi_hpe_inventory_flag(info, DMI_HPE_INVENTORY_ATTR_UEFI_IMAGE);

    return true;
}
