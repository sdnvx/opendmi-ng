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

#include "physical-attrs-internal.h"

const dmi_entity_spec_t dmi_hpe_physical_attrs_legacy_spec =
{
    .type        = DMI_TYPE(hpe_physical_attrs),
    .code        = "hpe-physical-attrs-legacy",
    .name        = "HP/HPE physical attribute information",
    .description = (const char *[]){
        "Keeps the physical product number and serial number of the server.",
        //
        nullptr
    },
    .params = {
        .generations    = { .maximum = DMI_HPE_GEN7 },
        .minimum_length = 0x15,
        .decoded_length = sizeof(dmi_hpe_physical_attrs_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BINARY(dmi_hpe_physical_attrs_t, identifier_raw, 16),
        DMI_FIELD_STRING(dmi_hpe_physical_attrs_t, serial_number),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_physical_attrs_t, identifier, STRING, {
            .code  = "identifier",
            .name  = "Product and serial number",
            .flags = DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        DMI_ATTRIBUTE(dmi_hpe_physical_attrs_t, serial_number, STRING, {
            .code  = "serial-number",
            .name  = "Serial number",
            .flags = DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_physical_attrs_derive
    }
};

const dmi_entity_spec_t dmi_hpe_physical_attrs_spec =
{
    .type        = DMI_TYPE(hpe_physical_attrs),
    .code        = "hpe-physical-attrs",
    .name        = "HP/HPE physical attribute information",
    .description = (const char *[]){
        "Keeps the physical serial number and UUID of the server, whose "
        "standard fields hold virtual ones, e.g. on Synergy, where a workload "
        "moves from one server to another along with its identity.",
        //
        nullptr
    },
    .params = {
        .generations    = { .minimum = DMI_HPE_GEN8 },
        .minimum_length = 0x15,
        .decoded_length = sizeof(dmi_hpe_physical_attrs_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_UUID(dmi_hpe_physical_attrs_t, uuid),
        DMI_FIELD_STRING(dmi_hpe_physical_attrs_t, serial_number),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_physical_attrs_t, uuid, UUID, {
            .code   = "uuid",
            .name   = "UUID",
            .unspec = &(const dmi_uuid_t){},
            .flags  = DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        DMI_ATTRIBUTE(dmi_hpe_physical_attrs_t, serial_number, STRING, {
            .code  = "serial-number",
            .name  = "Serial number",
            .flags = DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        {}
    })
};
