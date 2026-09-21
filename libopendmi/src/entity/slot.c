//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/value.h>

#include <opendmi/entity/common.h>
#include <opendmi/entity/slot-internal.h>

const dmi_entity_spec_t dmi_slot_spec =
{
    .code        = "slot",
    .name        = "System slots",
    .description = (const char *[]){
        "The information in this structure defines the attributes of a system "
        "slot. One structure is provided for each slot in the system.",
        //
        nullptr
    },

    .type            = DMI_TYPE(SYSTEM_SLOTS),
    .params = {
        .minimum_version  = DMI_VERSION(2, 0, 0),
        .recommended_from = DMI_VERSION(2, 3, 0),
        .minimum_length   = 0x0C,
        .decoded_length   = sizeof(dmi_slot_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_slot_t, designator),
        DMI_FIELD(dmi_slot_t, type,      dmi_byte_t),
        DMI_FIELD(dmi_slot_t, bus_width, dmi_byte_t),
        DMI_FIELD(dmi_slot_t, usage,     dmi_byte_t),
        DMI_FIELD(dmi_slot_t, length,    dmi_byte_t),
        DMI_FIELD(dmi_slot_t, ident,     dmi_word_t),
        DMI_FIELD(dmi_slot_t, features,  dmi_byte_t),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 1, 0)),
        DMI_FIELD(dmi_slot_t, features_ex, dmi_byte_t),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 6, 0)),
        DMI_FIELD(dmi_slot_t, base_address, dmi_dword_t,
                  .decode = dmi_pci_addr_decode,
                  .encode = dmi_pci_addr_encode),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 2, 0)),
        DMI_FIELD(dmi_slot_t, base_bus_width, dmi_byte_t),

        // Peer grouping: a PCI address and the width of the bus of the peer
        // device, which the slot shares its bandwidth with
        DMI_FIELD_ARRAY(dmi_slot_t, peer_groups, peer_group_count,
            .count_length = sizeof(dmi_byte_t),
            .fields       = DMI_FIELDS({
                DMI_FIELD(dmi_slot_peer_group_t, address, dmi_dword_t,
                          .decode = dmi_pci_addr_decode,
                          .encode = dmi_pci_addr_encode),
                DMI_FIELD(dmi_slot_peer_group_t, bus_width, dmi_byte_t),
                {}
            })),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 4, 0)),
        DMI_FIELD(dmi_slot_t, information,    dmi_byte_t),
        DMI_FIELD(dmi_slot_t, physical_width, dmi_byte_t),
        DMI_FIELD(dmi_slot_t, pitch,          dmi_word_t),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 5, 0)),
        DMI_FIELD(dmi_slot_t, height, dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_slot_t, designator, STRING, {
            .code = "designator",
            .name = "Designator"
        }),
        DMI_ATTRIBUTE(dmi_slot_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_SLOT_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_TYPE_UNKNOWN),
            .values  = &dmi_slot_type_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, bus_width, ENUM, {
            .code    = "bus-width",
            .name    = "Data bus width",
            .unspec  = dmi_value_ptr(DMI_SLOT_WIDTH_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_WIDTH_UNKNOWN),
            .values  = &dmi_slot_width_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, usage, ENUM, {
            .code    = "usage",
            .name    = "Current usage",
            .unspec  = dmi_value_ptr(DMI_SLOT_USAGE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_USAGE_UNKNOWN),
            .values  = &dmi_slot_usage_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, length, ENUM, {
            .code    = "length",
            .name    = "Slot length",
            .unspec  = dmi_value_ptr(DMI_SLOT_LENGTH_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_LENGTH_UNKNOWN),
            .values  = &dmi_slot_length_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, ident, INTEGER, {
            .code    = "ident",
            .name    = "Identifier",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_slot_t, features, SET, {
            .code    = "features",
            .name    = "Characteristics",
            .values  = &dmi_slot_feature_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, features_ex, SET, {
            .code    = "features-ex",
            .name    = "Extended characteristics",
            .values  = &dmi_slot_feature_ex_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, base_address, STRUCT, {
            .code    = "base-address",
            .name    = "Base device address",
            .attrs   = dmi_pci_addr_attrs
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_slot_t, peer_groups, peer_group_count, STRUCT, {
            .code = "peer-groups",
            .name = "Peer groups",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_slot_peer_group_t, address, STRUCT, {
                    .code  = "address",
                    .name  = "Peer address",
                    .attrs = dmi_pci_addr_attrs
                }),
                DMI_ATTRIBUTE(dmi_slot_peer_group_t, bus_width, ENUM, {
                    .code    = "bus-width",
                    .name    = "Data bus width",
                    .unspec  = dmi_value_ptr(DMI_SLOT_WIDTH_UNSPEC),
                    .unknown = dmi_value_ptr(DMI_SLOT_WIDTH_UNKNOWN),
                    .values  = &dmi_slot_width_names
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_slot_t, information, INTEGER, {
            .code    = "information",
            .name    = "Information",
            .unspec  = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_slot_t, physical_width, ENUM, {
            .code    = "physical-width",
            .name    = "Physical width",
            .unspec  = dmi_value_ptr(DMI_SLOT_WIDTH_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_WIDTH_UNKNOWN),
            .values  = &dmi_slot_width_names
        }),
        DMI_ATTRIBUTE(dmi_slot_t, pitch, DECIMAL, {
            .code    = "pitch",
            .name    = "Pitch",
            .scale   = 2,
            .unit    = DMI_UNIT_MILLIMETER,
            .unspec  = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_slot_t, height, ENUM, {
            .code    = "height",
            .name    = "Height",
            .unspec  = dmi_value_ptr(DMI_SLOT_HEIGHT_UNSPEC),
            .unknown = dmi_value_ptr(DMI_SLOT_HEIGHT_UNKNOWN),
            .values  = &dmi_slot_height_names
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("slot.width", dmi_slot_lint_width, {
            .name              = "Physical width of the slot covers the width of its bus",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_slot_cleanup
    }
};
