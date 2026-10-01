//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/acer.h>

#include <opendmi/entity/acer/hotkeys-internal.h>

static const dmi_attribute_t dmi_acer_hotkeys_attrs[] =
{
    DMI_ATTRIBUTE(dmi_acer_hotkeys_t, comm_functions, SET, {
        .code   = "comm-functions",
        .name   = "Communication button functions",
        .values = &dmi_acer_comm_function_names
    }),
    DMI_ATTRIBUTE(dmi_acer_hotkeys_t, app_functions, INTEGER, {
        .code  = "app-functions",
        .name  = "Application button functions",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_acer_hotkeys_t, media_functions, INTEGER, {
        .code  = "media-functions",
        .name  = "Media button functions",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_acer_hotkeys_t, display_functions, INTEGER, {
        .code  = "display-functions",
        .name  = "Display button functions",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_acer_hotkeys_t, other_functions, INTEGER, {
        .code  = "other-functions",
        .name  = "Other button functions",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_acer_hotkeys_t, comm_key, INTEGER, {
        .code  = "comm-key",
        .name  = "Communication function key number",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE_ARRAY(dmi_acer_hotkeys_t, hotkeys, hotkey_count, STRUCT, {
        .code  = "hotkeys",
        .name  = "Hotkeys",
        .attrs = DMI_ATTRIBUTES({
            DMI_ATTRIBUTE(dmi_acer_hotkey_entry_t, key, INTEGER, {
                .code  = "key",
                .name  = "Key",
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            DMI_ATTRIBUTE(dmi_acer_hotkey_entry_t, kind, INTEGER, {
                .code  = "kind",
                .name  = "Kind"
            }),
            DMI_ATTRIBUTE(dmi_acer_hotkey_entry_t, function, INTEGER, {
                .code  = "function",
                .name  = "Function",
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            {}
        })
    }),
    {}
};

const dmi_entity_spec_t dmi_acer_hotkeys_spec =
{
    .type        = DMI_TYPE(acer_hotkeys),
    .code        = "acer-hotkeys",
    .name        = "Acer hotkey functions",
    .description = (const char *[]){
        "Tells the functions of the hotkeys of a laptop, which the Acer WMI "
        "driver reads.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0F,
        .decoded_length = sizeof(dmi_acer_hotkeys_t)
    },

    // Hotkeys begin with the number of the key of the communication function,
    // which the Acer WMI driver reads alone, and run to the end of the
    // structure
    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_acer_hotkeys_t, comm_functions,    dmi_word_t),
        DMI_FIELD(dmi_acer_hotkeys_t, app_functions,     dmi_word_t),
        DMI_FIELD(dmi_acer_hotkeys_t, media_functions,   dmi_word_t),
        DMI_FIELD(dmi_acer_hotkeys_t, display_functions, dmi_word_t),
        DMI_FIELD(dmi_acer_hotkeys_t, other_functions,   dmi_word_t),
        DMI_FIELD_ARRAY(dmi_acer_hotkeys_t, hotkeys, hotkey_count,
            .stride = 4,
            .fields = DMI_FIELDS({
                DMI_FIELD(dmi_acer_hotkey_entry_t, key,      dmi_byte_t),
                DMI_FIELD(dmi_acer_hotkey_entry_t, kind,     dmi_byte_t),
                DMI_FIELD(dmi_acer_hotkey_entry_t, function, dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes = dmi_acer_hotkeys_attrs,

    .handlers = {
        .derive  = dmi_acer_hotkeys_derive
    }
};

//
// Structures of 0x0F bytes hold the functions and the number of the key of
// the communication function only, the way the Acer WMI driver reads them
//
const dmi_entity_spec_t dmi_acer_hotkeys_basic_spec =
{
    .type        = DMI_TYPE(acer_hotkeys),
    .code        = "acer-hotkeys-basic",
    .name        = "Acer hotkey functions",
    .description = (const char *[]){
        "Tells the functions of the hotkeys of a laptop, which the Acer WMI "
        "driver reads.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x0F,
        .decoded_length = sizeof(dmi_acer_hotkeys_t),
        .signature      = DMI_SIGNATURE({
            .length = 0x0F
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_acer_hotkeys_t, comm_functions,    dmi_word_t),
        DMI_FIELD(dmi_acer_hotkeys_t, app_functions,     dmi_word_t),
        DMI_FIELD(dmi_acer_hotkeys_t, media_functions,   dmi_word_t),
        DMI_FIELD(dmi_acer_hotkeys_t, display_functions, dmi_word_t),
        DMI_FIELD(dmi_acer_hotkeys_t, other_functions,   dmi_word_t),
        DMI_FIELD(dmi_acer_hotkeys_t, comm_key,          dmi_byte_t),
        {}
    }),

    .attributes = dmi_acer_hotkeys_attrs
};

const dmi_name_set_t dmi_acer_comm_function_names =
{
    .code  = "acer-comm-function",
    .names = DMI_NAMES({
        {
            .id   = DMI_ACER_COMM_FUNCTION_WIFI,
            .code = "wifi",
            .name = "WiFi"
        },
        {
            .id   = DMI_ACER_COMM_FUNCTION_3G,
            .code = "3g",
            .name = "3G"
        },
        {
            .id   = DMI_ACER_COMM_FUNCTION_WIMAX,
            .code = "wimax",
            .name = "WiMAX"
        },
        {
            .id   = DMI_ACER_COMM_FUNCTION_BLUETOOTH,
            .code = "bluetooth",
            .name = "Bluetooth"
        },
        {
            .id   = DMI_ACER_COMM_FUNCTION_RF_BUTTON,
            .code = "rf-button",
            .name = "Radio button"
        },
        {}
    })
};

bool dmi_acer_hotkeys_derive(dmi_entity_t *entity)
{
    dmi_acer_hotkeys_t *info = dmi_entity_info(entity, DMI_TYPE(acer_hotkeys));
    if (info == nullptr)
        return false;

    // Key of the communication function begins the first hotkey
    if (info->hotkey_count > 0)
        info->comm_key = info->hotkeys[0].key;

    return true;
}
