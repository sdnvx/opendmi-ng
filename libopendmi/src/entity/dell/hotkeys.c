//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/hotkeys-internal.h>

const dmi_entity_spec_t dmi_dell_hotkeys_spec =
{
    .type        = DMI_TYPE(dell_hotkeys),
    .code        = "dell-hotkeys",
    .name        = "Dell hotkeys",
    .description = (const char *[]){
        "Maps the scan codes of the hotkeys of a laptop to their functions, "
        "which the Dell WMI driver reads.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x04,
        .decoded_length = sizeof(dmi_dell_hotkeys_t)
    },

    // Hotkeys run to the end of the structure, which carries no number of
    // them of its own
    .fields = DMI_FIELDS({
        DMI_FIELD_ARRAY(dmi_dell_hotkeys_t, hotkeys, hotkey_count,
            .stride = 4,
            .fields = DMI_FIELDS({
                DMI_FIELD(dmi_dell_hotkey_entry_t, scancode, dmi_word_t),
                DMI_FIELD(dmi_dell_hotkey_entry_t, keycode,  dmi_word_t),
                {}
            })),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE_ARRAY(dmi_dell_hotkeys_t, hotkeys, hotkey_count, STRUCT, {
            .code  = "hotkeys",
            .name  = "Hotkey mappings",
            .attrs = DMI_ATTRIBUTES({
                DMI_ATTRIBUTE(dmi_dell_hotkey_entry_t, scancode, INTEGER, {
                    .code  = "scancode",
                    .name  = "Scan code",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_ATTRIBUTE(dmi_dell_hotkey_entry_t, keycode, INTEGER, {
                    .code  = "keycode",
                    .name  = "Key code",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .cleanup = dmi_dell_hotkeys_cleanup
    }
};

void dmi_dell_hotkeys_cleanup(dmi_entity_t *entity)
{
    dmi_dell_hotkeys_t *info = dmi_entity_info(entity, DMI_TYPE(dell_hotkeys));
    if (info == nullptr)
        return;

    dmi_free(info->hotkeys);
}
