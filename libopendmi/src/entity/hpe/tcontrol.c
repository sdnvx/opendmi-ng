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

#include <opendmi/entity/hpe/tcontrol.h>

const dmi_entity_spec_t dmi_hpe_tcontrol_spec =
{
    .type        = DMI_TYPE(hpe_tcontrol),
    .code        = "hpe-tcontrol",
    .name        = "HP/HPE processor TControl information",
    .description = (const char *[]){
        "Gives the Tcontrol value of a processor, the temperature the fans "
        "begin to spin up at, which Intel programs into each processor.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x07,
        .decoded_length = sizeof(dmi_hpe_tcontrol_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_tcontrol_t, processor_handle, dmi_word_t),
        DMI_FIELD(dmi_hpe_tcontrol_t, tcontrol,         dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_tcontrol_t, processor_handle, HANDLE, {
            .code    = "processor-handle",
            .name    = "Processor handle",
            .targets = dmi_types(DMI_TYPE(processor))
        }),
        DMI_ATTRIBUTE(dmi_hpe_tcontrol_t, tcontrol, INTEGER, {
            .code   = "tcontrol",
            .name   = "TControl value",
            .unspec = dmi_value_ptr((uint8_t)0)
        }),
        {}
    })
};
