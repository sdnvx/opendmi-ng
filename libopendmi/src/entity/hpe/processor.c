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

#include "processor-internal.h"

const dmi_entity_spec_t dmi_hpe_processor_spec =
{
    .type        = DMI_TYPE(hpe_processor),
    .code        = "hpe-processor",
    .name        = "HP/HPE processor specific information",
    .description = (const char *[]){
        "Supplements the processor information (type 4) of each processor "
        "socket or slot with the numbers the server tells its processors by.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0A,
        .decoded_length = sizeof(dmi_hpe_processor_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_processor_t, processor_handle, dmi_word_t),
        DMI_FIELD(dmi_hpe_processor_t, apic_id,          dmi_byte_t),

        DMI_FIELD_BITS(dmi_hpe_processor_t, is_bsp,               1),
        DMI_FIELD_BITS(dmi_hpe_processor_t, is_x2apic,            1),
        DMI_FIELD_BITS(dmi_hpe_processor_t, is_thermal_margining, 1),
        DMI_FIELD_PAD(dmi_byte_t),

        DMI_FIELD(dmi_hpe_processor_t, slot,   dmi_byte_t),
        DMI_FIELD(dmi_hpe_processor_t, socket, dmi_byte_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_processor_t, maximum_power, dmi_word_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_processor_t, x2apic_id, dmi_dword_t,
                  .absent = dmi_value_ptr((uint32_t)UINT32_MAX)),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_processor_t, uuid, dmi_qword_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_processor_t, interconnect_speed, dmi_word_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD_VECTOR(dmi_hpe_processor_t, qdf_raw,
            .fields = DMI_FIELDS({
                DMI_FIELD_ELEMENT(dmi_hpe_processor_t, qdf_raw, dmi_byte_t),
                {}
            })),

        // Reserved from Gen11 onwards
        DMI_FIELD_GROUP(),
        DMI_FIELD_SKIP(4),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_processor_t, processor_handle, HANDLE, {
            .code    = "processor-handle",
            .name    = "Processor handle",
            .targets = dmi_types(DMI_TYPE(processor))
        }),
        DMI_ATTRIBUTE(dmi_hpe_processor_t, apic_id, INTEGER, {
            .code = "apic-id",
            .name = "APIC ID"
        }),
        DMI_ATTRIBUTE(dmi_hpe_processor_t, is_bsp, BOOL, {
            .code = "is-bsp",
            .name = "Bootstrap processor"
        }),
        DMI_ATTRIBUTE(dmi_hpe_processor_t, is_x2apic, BOOL, {
            .code = "is-x2apic",
            .name = "x2APIC"
        }),
        DMI_ATTRIBUTE(dmi_hpe_processor_t, is_thermal_margining, BOOL, {
            .code = "is-thermal-margining",
            .name = "Advanced thermal margining"
        }),
        DMI_ATTRIBUTE(dmi_hpe_processor_t, slot, INTEGER, {
            .code   = "slot",
            .name   = "Physical slot",
            .unspec = dmi_value_ptr((uint8_t)UINT8_MAX)
        }),
        DMI_ATTRIBUTE(dmi_hpe_processor_t, socket, INTEGER, {
            .code   = "socket",
            .name   = "Physical socket",
            .unspec = dmi_value_ptr((uint8_t)UINT8_MAX)
        }),
        DMI_ATTRIBUTE(dmi_hpe_processor_t, maximum_power, INTEGER, {
            .code   = "maximum-power",
            .name   = "Maximum power",
            .unit   = DMI_UNIT_WATT,
            .unspec = dmi_value_ptr((uint16_t)0)
        }),
        // x2APIC ID means something in the x2APIC mode only
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_processor_t, is_x2apic, {
            .code     = "x2apic-id",
            .name     = "x2APIC ID",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_processor_t, x2apic_id, INTEGER, {
                    .code   = "x2apic-id",
                    .name   = "x2APIC ID",
                    .unspec = dmi_value_ptr((uint32_t)UINT32_MAX),
                    .flags  = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_hpe_processor_t, uuid, INTEGER, {
            .code   = "uuid",
            .name   = "Unique identifier",
            .unspec = dmi_value_ptr((uint64_t)0),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX | DMI_ATTRIBUTE_FLAG_PRIVATE
        }),
        DMI_ATTRIBUTE(dmi_hpe_processor_t, interconnect_speed, INTEGER, {
            .code   = "interconnect-speed",
            .name   = "Interconnect speed",
            .unit   = DMI_UNIT_MEGAXA_SECOND,
            .unspec = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_processor_t, qdf, STRING, {
            .code = "qdf",
            .name = "QDF/S-Spec"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_processor_derive
    }
};

//
// Number is taken only when it is printable, with the spaces padding it
// left out.
//
bool dmi_hpe_processor_derive(dmi_entity_t *entity)
{
    dmi_hpe_processor_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_processor));
    if (info == nullptr)
        return false;

    info->qdf = dmi_text_from_bytes(info->qdf_raw, sizeof(info->qdf_raw), info->qdf_buffer, true);

    return true;
}
