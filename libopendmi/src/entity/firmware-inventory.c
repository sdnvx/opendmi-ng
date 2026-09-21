//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-inventory-internal.h>

const dmi_entity_spec_t dmi_firmware_inventory_spec =
{
    .code            = "firmware-inventory",
    .name            = "Firmware inventory information",
    .type            = DMI_TYPE(FIRMWARE_INVENTORY),
    .description     = (const char *[]){
        "The information in this structure defines an inventory of firmware "
        "components in the system. This can include firmware components such "
        "as platform firmware, BMC, as well as firmware for other devices in "
        "the system.",
        //
        "The information can be used by software to display the firmware "
        "inventory in a uniform manner. It can also be used by a management "
        "controller, such as a BMC, for remote system management. This "
        "structure is not intended to replace other standard programmatic "
        "interfaces for firmware updates.",
        //
        "One Type 45 structure is provided for each firmware component.",
        //
        nullptr
    },
    .params = {
        .minimum_version = DMI_VERSION(3, 5, 0),
        .minimum_length  = 0x17,
        .decoded_length  = sizeof(dmi_firmware_inventory_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_firmware_inventory_t, name),
        DMI_FIELD_STRING(dmi_firmware_inventory_t, version),
        DMI_FIELD(dmi_firmware_inventory_t, version_format, dmi_byte_t),
        DMI_FIELD_STRING(dmi_firmware_inventory_t, ident),
        DMI_FIELD(dmi_firmware_inventory_t, ident_format, dmi_byte_t),
        DMI_FIELD_STRING(dmi_firmware_inventory_t, release_date),
        DMI_FIELD_STRING(dmi_firmware_inventory_t, vendor),
        DMI_FIELD_STRING(dmi_firmware_inventory_t, lowest_version),
        DMI_FIELD(dmi_firmware_inventory_t, image_size, dmi_qword_t),
        DMI_FIELD(dmi_firmware_inventory_t, features,   dmi_word_t),
        DMI_FIELD(dmi_firmware_inventory_t, state,      dmi_byte_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD_ARRAY(dmi_firmware_inventory_t, components, component_count,
            .count_length = sizeof(dmi_byte_t),
            .fields       = DMI_FIELDS({
                DMI_FIELD(dmi_firmware_inventory_component_t, handle, dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes      = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, name, STRING, {
            .code    = "name",
            .name    = "Name"
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_firmware_inventory_t, parsed_version.format, {
            .code     = "version",
            .name     = "Version",
            .variants = dmi_firmware_version_variants(version, parsed_version)
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, version_format, ENUM, {
            .code    = "version-format",
            .name    = "Version format",
            .values  = &dmi_version_format_names
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_firmware_inventory_t, parsed_ident.format, {
            .code     = "ident",
            .name     = "Identifier",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_FIRMWARE_IDENT_FORMAT_GUID, dmi_firmware_inventory_t, parsed_ident.guid, UUID, {}),
                DMI_VARIANT_DEFAULT(dmi_firmware_inventory_t, ident, STRING, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, ident_format, ENUM, {
            .code    = "ident-format",
            .name    = "Identifier format",
            .values  = &dmi_firmware_ident_format_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, release_date, STRING, {
            .code    = "release-date",
            .name    = "Release date"
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Vendor"
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_firmware_inventory_t, parsed_lowest_version.format, {
            .code     = "lowest-version",
            .name     = "Lowest version",
            .variants = dmi_firmware_version_variants(lowest_version, parsed_lowest_version)
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, image_size, SIZE, {
            .code    = "image-size",
            .name    = "Image size",
            .unknown = dmi_value_ptr(DMI_SIZE_MAX)
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, features, SET, {
            .code    = "characteristics",
            .name    = "Characteristics",
            .values  = &dmi_firmware_inventory_feature_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, state, ENUM, {
            .code    = "state",
            .name    = "State",
            .unspec  = dmi_value_ptr(DMI_FIRMWARE_INVENTORY_STATE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_FIRMWARE_INVENTORY_STATE_UNSPEC),
            .values  = &dmi_firmware_inventory_state_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_inventory_t, component_count, INTEGER, {
            .code    = "component-count",
            .name    = "Associated components count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_firmware_inventory_t, components, component_count, STRUCT, {
            .code    = "components",
            .name    = "Associated components",
            .attrs   = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_firmware_inventory_component_t, handle, HANDLE, {
                    .code = "handle",
                    .name = "Handle",
                    .link = dmi_member(dmi_firmware_inventory_component_t, entity)
                }),
                {}
            })
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("firmware-inventory.version", dmi_firmware_inventory_lint_version, {
            .name              = "Version of the firmware is no older than the lowest supported one",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .derive  = dmi_firmware_inventory_derive,
        .cleanup = dmi_firmware_inventory_cleanup
    }
};
