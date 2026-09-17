//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/stream.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system.h>

static bool dmi_system_decode(dmi_entity_t *entity);

const dmi_name_set_t dmi_system_wakeup_type_names =
{
    .code  = "system-wakeup-types",
    .names = (dmi_name_t[]){
        DMI_NAME_RESERVED(DMI_SYSTEM_WAKEUP_TYPE_RESERVED),
        DMI_NAME_OTHER(DMI_SYSTEM_WAKEUP_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_SYSTEM_WAKEUP_TYPE_UNKNOWN),
        {
            .id   = DMI_SYSTEM_WAKEUP_TYPE_APM_TIMER,
            .code = "apm-timer",
            .name = "APM Timer"
        },
        {
            .id   = DMI_SYSTEM_WAKEUP_TYPE_MODEM_RING,
            .code = "modem-ring",
            .name = "Modem Ring"
        },
        {
            .id   = DMI_SYSTEM_WAKEUP_TYPE_LAN_REMOTE,
            .code = "lan-remote",
            .name = "LAN Remote"
        },
        {
            .id = DMI_SYSTEM_WAKEUP_TYPE_POWER_SWITCH,
            .code = "power-switch",
            .name = "Power Switch"
        },
        {
            .id   = DMI_SYSTEM_WAKEUP_TYPE_PCI_PME,
            .code = "pci-pme",
            .name = "PCI PME#"
        },
        {
            .id   = DMI_SYSTEM_WAKEUP_TYPE_POWER_RESTORE,
            .code = "power-restore",
            .name = "AC Power Restored"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_system_spec =
{
    .code            = "system",
    .name            = "System information",
    .description     = (const char *[]){
        "The information in this structure defines attributes of the overall "
        "system and is intended to be associated with the Component ID group "
        "of the system\'s MIF. An SMBIOS implementation is associated with a "
        "single system instance and contains one and only one System "
        "Information (Type 1) structure.",
        //
        nullptr
    },
    .type            = DMI_TYPE(SYSTEM),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .required_from   = DMI_VERSION(2, 3, 0),
    .required_till   = DMI_VERSION_NONE,
    .unique          = true,
    .minimum_length  = 0x8,
    .decoded_length  = sizeof(dmi_system_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_system_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Manufacturer"
        }),
        DMI_ATTRIBUTE(dmi_system_t, product, STRING, {
            .code    = "product",
            .name    = "Product"
        }),
        DMI_ATTRIBUTE(dmi_system_t, version, STRING, {
            .code    = "version",
            .name    = "Version"
        }),
        DMI_ATTRIBUTE(dmi_system_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number"
        }),
        DMI_ATTRIBUTE(dmi_system_t, uuid, UUID, {
            .code    = "uuid",
            .name    = "UUID",
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_system_t, wakeup_type, ENUM, {
            .code    = "wakeup-type",
            .name    = "Wakeup type",
            .values  = &dmi_system_wakeup_type_names,
            .unknown = dmi_value_ptr(DMI_SYSTEM_WAKEUP_TYPE_UNKNOWN),
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_system_t, sku_number, STRING, {
            .code    = "sku-number",
            .name    = "SKU number",
            .level   = DMI_VERSION(2, 4, 0)
        }),
        DMI_ATTRIBUTE(dmi_system_t, family, STRING, {
            .code    = "family",
            .name    = "Family",
            .level   = DMI_VERSION(2, 4, 0)
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers      = {
        .decode = dmi_system_decode
    }
};

const char *dmi_system_wakeup_type_name(dmi_system_wakeup_type_t value)
{
    return dmi_name_lookup(&dmi_system_wakeup_type_names, (int)value);
}

static bool dmi_system_decode(dmi_entity_t *entity)
{
    dmi_system_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    bool status =
        dmi_stream_decode_str(stream, &info->vendor) and
        dmi_stream_decode_str(stream, &info->product) and
        dmi_stream_decode_str(stream, &info->version) and
        dmi_stream_decode_str(stream, &info->serial_number);
    if (not status)
        return false;

    // SMBIOS 2.1 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 1, 0);

    status =
        dmi_stream_decode_uuid(stream, &info->uuid) and
        dmi_stream_decode(stream, dmi_byte_t, &info->wakeup_type);
    if (not status)
        return dmi_entity_incomplete(entity);

    // SMBIOS 2.4 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 4, 0);

    status =
        dmi_stream_decode_str(stream, &info->sku_number) and
        dmi_stream_decode_str(stream, &info->family);
    if (not status)
        return dmi_entity_incomplete(entity);

    return true;
}
