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

#include <opendmi/entity/hpe/backplane.h>

const dmi_entity_spec_t dmi_hpe_backplane_spec =
{
    .type        = DMI_TYPE(HPE_BACKPLANE),
    .code        = "hpe-backplane",
    .name        = "HP/HPE HDD backplane FRU information",
    .description = (const char *[]){
        "Describes a drive backplane: where its FRU is, and the SAS expander "
        "and the drive bays it has.",
        //
        nullptr
    },
    .params = {
        .generations    = { .maximum = DMI_HPE_GEN10_PLUS },
        .minimum_length = 0x09,
        .decoded_length = sizeof(dmi_hpe_backplane_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_backplane_t, i2c_address, dmi_byte_t),
        DMI_FIELD(dmi_hpe_backplane_t, box_number,  dmi_word_t),
        DMI_FIELD(dmi_hpe_backplane_t, nvram_id,    dmi_word_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_backplane_t, wwid, dmi_qword_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_backplane_t, bay_count, dmi_byte_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_backplane_t, a0_bay_count, dmi_byte_t),
        DMI_FIELD(dmi_hpe_backplane_t, a2_bay_count, dmi_byte_t),
        DMI_FIELD_STRING(dmi_hpe_backplane_t, name),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_backplane_t, i2c_address, INTEGER, {
            .code  = "i2c-address",
            .name  = "FRU I2C address",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_backplane_t, box_number, INTEGER, {
            .code = "box-number",
            .name = "Box number"
        }),
        DMI_ATTRIBUTE(dmi_hpe_backplane_t, nvram_id, INTEGER, {
            .code  = "nvram-id",
            .name  = "NVRAM ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_backplane_t, wwid, INTEGER, {
            .code   = "wwid",
            .name   = "SAS expander WWID",
            .unspec = dmi_value_ptr((uint64_t)0),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_backplane_t, bay_count, INTEGER, {
            .code = "bay-count",
            .name = "Total SAS bays"
        }),
        DMI_ATTRIBUTE(dmi_hpe_backplane_t, a0_bay_count, INTEGER, {
            .code = "a0-bay-count",
            .name = "A0 bay count"
        }),
        DMI_ATTRIBUTE(dmi_hpe_backplane_t, a2_bay_count, INTEGER, {
            .code = "a2-bay-count",
            .name = "A2 bay count"
        }),
        DMI_ATTRIBUTE(dmi_hpe_backplane_t, name, STRING, {
            .code = "name",
            .name = "Backplane name"
        }),
        {}
    })
};
