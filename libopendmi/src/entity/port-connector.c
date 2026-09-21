//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/port-connector-internal.h>

const dmi_entity_spec_t dmi_port_connector_spec =
{
    .code            = "port-connector",
    .name            = "Port connector information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of a system "
        "port connector (for example, parallel, serial, keyboard, or mouse "
        "ports). The port’s type and connector information are provided. One "
        "structure is present for each port provided by the system.",
        //
        nullptr
    },
    .type            = DMI_TYPE(PORT_CONNECTOR),
    .params = {
        .minimum_version  = DMI_VERSION(2, 0, 0),
        .recommended_from = DMI_VERSION(2, 3, 0),
        .minimum_length   = 0x09,
        .decoded_length   = sizeof(dmi_port_connector_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_port_connector_t, internal_designator),
        DMI_FIELD(dmi_port_connector_t, internal_connector, dmi_byte_t),
        DMI_FIELD_STRING(dmi_port_connector_t, external_designator),
        DMI_FIELD(dmi_port_connector_t, external_connector, dmi_byte_t),
        DMI_FIELD(dmi_port_connector_t, port_type,          dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_port_connector_t, internal_designator, STRING, {
            .code   = "internal-designator",
            .name   = "Internal reference designator"
        }),
        DMI_ATTRIBUTE(dmi_port_connector_t, internal_connector, ENUM, {
            .code   = "internal-connector",
            .name   = "Internal connector type",
            .values = &dmi_connector_type_names
        }),
        DMI_ATTRIBUTE(dmi_port_connector_t, external_designator, STRING, {
            .code   = "external-designator",
            .name   = "External reference designator"
        }),
        DMI_ATTRIBUTE(dmi_port_connector_t, external_connector, ENUM, {
            .code   = "external-connector",
            .name   = "External connector type",
            .values = &dmi_connector_type_names
        }),
        DMI_ATTRIBUTE(dmi_port_connector_t, port_type, ENUM, {
            .code   = "port-type",
            .name   = "Port type",
            .values = &dmi_port_type_names
        }),
        {}
    }),

    .handlers   = {
    }
};
