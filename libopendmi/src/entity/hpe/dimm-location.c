//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/field.h>
#include <opendmi/platform.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include <opendmi/entity/hpe/dimm-location-internal.h>

const dmi_entity_spec_t dmi_hpe_dimm_location_spec =
{
    .type        = DMI_TYPE(hpe_dimm_location),
    .code        = "hpe-dimm-location",
    .name        = "HP/HPE DIMM location record",
    .description = (const char *[]){
        "Tells where the DIMM socket of a memory device (type 17) is: the "
        "memory board, the processor and the channel it belongs to.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x08,
        .decoded_length = sizeof(dmi_hpe_dimm_location_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_dimm_location_t, device_handle, dmi_word_t),
        DMI_FIELD(dmi_hpe_dimm_location_t, board,         dmi_byte_t),
        DMI_FIELD(dmi_hpe_dimm_location_t, dimm,          dmi_byte_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_dimm_location_t, processor, dmi_byte_t,
                  .absent = dmi_value_ptr((uint8_t)UINT8_MAX)),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_dimm_location_t, logical_dimm, dmi_byte_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD_STRING(dmi_hpe_dimm_location_t, uefi_device_path),
        DMI_FIELD_STRING(dmi_hpe_dimm_location_t, uefi_device_name),
        DMI_FIELD_STRING(dmi_hpe_dimm_location_t, device_name),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_dimm_location_t, controller, dmi_byte_t),
        DMI_FIELD(dmi_hpe_dimm_location_t, channel,    dmi_byte_t),
        DMI_FIELD(dmi_hpe_dimm_location_t, ie_dimm,    dmi_byte_t,
                  .absent = dmi_value_ptr((uint8_t)UINT8_MAX)),
        DMI_FIELD(dmi_hpe_dimm_location_t, ie_pldm_id, dmi_byte_t,
                  .absent = dmi_value_ptr((uint8_t)UINT8_MAX)),
        DMI_FIELD(dmi_hpe_dimm_location_t, vendor_id,            dmi_word_t),
        DMI_FIELD(dmi_hpe_dimm_location_t, device_id,            dmi_word_t),
        DMI_FIELD(dmi_hpe_dimm_location_t, controller_vendor_id, dmi_word_t),
        DMI_FIELD(dmi_hpe_dimm_location_t, controller_device_id, dmi_word_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_dimm_location_t, interleave, dmi_byte_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD_STRING(dmi_hpe_dimm_location_t, part_number),

        DMI_FIELD_GROUP(.present = dmi_member(dmi_hpe_dimm_location_t, has_channel_index)),
        DMI_FIELD(dmi_hpe_dimm_location_t, channel_index, dmi_byte_t,
                  .absent = dmi_value_ptr((uint8_t)UINT8_MAX)),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, device_handle, HANDLE, {
            .code    = "device-handle",
            .name    = "Memory device handle",
            .targets = dmi_types(DMI_TYPE(memory_device))
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, is_system_board, BOOL, {
            .code = "is-system-board",
            .name = "System board"
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_dimm_location_t, is_system_board, {
            .code     = "board",
            .name     = "Board number",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(false, dmi_hpe_dimm_location_t, board, INTEGER, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, dimm, INTEGER, {
            .code = "dimm",
            .name = "DIMM number"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, processor, INTEGER, {
            .code   = "processor",
            .name   = "Processor number",
            .unspec = dmi_value_ptr((uint8_t)UINT8_MAX)
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, logical_dimm, INTEGER, {
            .code   = "logical-dimm",
            .name   = "Logical DIMM number",
            .unspec = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, uefi_device_path, STRING, {
            .code = "uefi-device-path",
            .name = "UEFI device path"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, uefi_device_name, STRING, {
            .code = "uefi-device-name",
            .name = "UEFI device name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, device_name, STRING, {
            .code = "device-name",
            .name = "Device name"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, controller, INTEGER, {
            .code   = "controller",
            .name   = "Memory controller number",
            .unspec = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, channel, INTEGER, {
            .code   = "channel",
            .name   = "Memory channel number",
            .unspec = dmi_value_ptr((uint8_t)0)
        }),
        // Fields of the Innovation Engine are reserved from Gen12 onwards
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_dimm_location_t, has_ie, {
            .code     = "ie-dimm",
            .name     = "IE DIMM number",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_dimm_location_t, ie_dimm, INTEGER, {
                    .code   = "ie-dimm",
                    .name   = "IE DIMM number",
                    .unspec = dmi_value_ptr((uint8_t)UINT8_MAX)
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_dimm_location_t, has_ie, {
            .code     = "ie-pldm-id",
            .name     = "IE PLDM ID",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_dimm_location_t, ie_pldm_id, INTEGER, {
                    .code   = "ie-pldm-id",
                    .name   = "IE PLDM ID",
                    .unspec = dmi_value_ptr((uint8_t)UINT8_MAX)
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, vendor_id, INTEGER, {
            .code   = "vendor-id",
            .name   = "Vendor ID",
            .unspec = dmi_value_ptr((uint16_t)0),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, device_id, INTEGER, {
            .code   = "device-id",
            .name   = "Device ID",
            .unspec = dmi_value_ptr((uint16_t)0),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, controller_vendor_id, INTEGER, {
            .code   = "controller-vendor-id",
            .name   = "Controller manufacturer ID",
            .unspec = dmi_value_ptr((uint16_t)0),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX | DMI_ATTRIBUTE_FLAG_JEP106
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, controller_device_id, INTEGER, {
            .code   = "controller-device-id",
            .name   = "Controller product ID",
            .unspec = dmi_value_ptr((uint16_t)0),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, interleave, INTEGER, {
            .code   = "interleave",
            .name   = "Best interleave",
            .unspec = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_location_t, part_number, STRING, {
            .code = "part-number",
            .name = "Part number"
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_dimm_location_t, has_channel_index, {
            .code     = "channel-index",
            .name     = "DIMM index",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_dimm_location_t, channel_index, INTEGER, {}),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_dimm_location_derive
    }
};

bool dmi_hpe_dimm_location_derive(dmi_entity_t *entity)
{
    dmi_hpe_dimm_location_t *info = dmi_entity_info(entity, DMI_TYPE(hpe_dimm_location));
    if (info == nullptr)
        return false;

    const dmi_platform_t *platform = dmi_get_platform(dmi_entity_context(entity));
    unsigned generation = (platform != nullptr) ? platform->generation : 0;

    info->is_system_board   = (info->board == UINT8_MAX);
    info->has_ie            = (generation < DMI_HPE_GEN12);

    return true;
}
