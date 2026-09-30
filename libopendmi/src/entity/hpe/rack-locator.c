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

#include <opendmi/entity/hpe/rack-locator.h>

const dmi_entity_spec_t dmi_hpe_rack_locator_spec =
{
    .type        = DMI_TYPE(HPE_RACK_LOCATOR),
    .code        = "hpe-rack-locator",
    .name        = "HP/HPE system/rack locator",
    .description = (const char *[]){
        "Tells where a blade server is: the rack, the enclosure and the bay.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0B,
        .decoded_length = sizeof(dmi_hpe_rack_locator_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_hpe_rack_locator_t, rack_name),
        DMI_FIELD_STRING(dmi_hpe_rack_locator_t, enclosure_name),
        DMI_FIELD_STRING(dmi_hpe_rack_locator_t, enclosure_model),
        DMI_FIELD_STRING(dmi_hpe_rack_locator_t, server_bay),
        DMI_FIELD(dmi_hpe_rack_locator_t, bay_count,        dmi_byte_t),
        DMI_FIELD(dmi_hpe_rack_locator_t, filled_bay_count, dmi_byte_t),
        DMI_FIELD_STRING(dmi_hpe_rack_locator_t, enclosure_serial),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_rack_locator_t, rack_name, STRING, {
            .code = "rack-name",
            .name = "Rack name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_rack_locator_t, enclosure_name, STRING, {
            .code = "enclosure-name",
            .name = "Enclosure name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_rack_locator_t, enclosure_model, STRING, {
            .code = "enclosure-model",
            .name = "Enclosure model"
        }),
        DMI_ATTRIBUTE(dmi_hpe_rack_locator_t, enclosure_serial, STRING, {
            .code = "enclosure-serial",
            .name = "Enclosure serial number"
        }),
        DMI_ATTRIBUTE(dmi_hpe_rack_locator_t, bay_count, INTEGER, {
            .code = "bay-count",
            .name = "Enclosure bays"
        }),
        DMI_ATTRIBUTE(dmi_hpe_rack_locator_t, server_bay, STRING, {
            .code = "server-bay",
            .name = "Server bay"
        }),
        DMI_ATTRIBUTE(dmi_hpe_rack_locator_t, filled_bay_count, INTEGER, {
            .code = "filled-bay-count",
            .name = "Bays filled"
        }),
        {}
    })
};
