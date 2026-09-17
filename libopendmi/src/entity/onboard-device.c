//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/onboard-device.h>

static bool dmi_onboard_device_decode(dmi_entity_t *entity);
static void dmi_onboard_device_cleanup(dmi_entity_t *entity);

const dmi_name_set_t dmi_onboard_device_type_names =
{
    .code  = "onboard-device-types",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_ONBOARD_DEVICE_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_ONBOARD_DEVICE_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_ONBOARD_DEVICE_TYPE_UNKNOWN),
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_VIDEO,
            .code = "video",
            .name = "Video"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_SCSI_CONTROLLER,
            .code = "scsi-controller",
            .name = "SCSI controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_ETHERNET,
            .code = "ethernet",
            .name = "Ethernet"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_TOKEN_RING,
            .code = "token-ring",
            .name = "Token ring"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_SOUND,
            .code = "sound",
            .name = "Sound"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_PATA_CONTROLLER,
            .code = "pata-controller",
            .name = "PATA controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_SATA_CONTROLLER,
            .code = "sata-controller",
            .name = "SATA controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_SAS_CONTROLLER,
            .code = "sas-controller",
            .name = "SAS controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_WIRELESS_LAN,
            .code = "wireless lan",
            .name = "Wireless LAN"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_BLUETOOTH,
            .code = "bluetooth",
            .name = "Bluetooth"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_WIRELESS_WAN,
            .code = "wireless-wan",
            .name = "Wireless WAN"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_EMM_CONTROLLER,
            .code = "emm-controller",
            .name = "eMM controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_NVME_CONTROLLER,
            .code = "nvme-controller",
            .name = "NVMe controller"
        },
        {
            .id   = DMI_ONBOARD_DEVICE_TYPE_UFS_CONTROLLER,
            .code = "ufs-controller",
            .name = "UFS controller"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_onboard_device_spec =
{
    .code            = "onboard-device",
    .name            = "Onboard devices information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of devices "
        "that are onboard (soldered onto) a system element, usually the "
        "baseboard. In general, an entry in this table implies that the "
        "firmware has some level of control over the enabling of the "
        "associated device for use by the system.",
        //
        "Note: This structure is obsolete starting with version 2.6 of SMBIOS "
        "specification; the Onboard Devices Extended Information (Type 41) "
        "structure should be used instead. Firmware providers can choose to "
        "implement both types to allow existing SMBIOS browsers to properly "
        "display the system’s onboard devices information.",
        nullptr
    },
    .type            = DMI_TYPE(ONBOARD_DEVICE),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x06,
    .decoded_length  = sizeof(dmi_onboard_device_t),
    .attributes      = (const dmi_attribute_t[]) {
        DMI_ATTRIBUTE(dmi_onboard_device_t, instance_count, INTEGER, {
            .code  = "instance-count",
            .name  = "Instance count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_onboard_device_t, instances, instance_count, STRUCT, {
            .code  = "instances",
            .name  = "Instances",
            .attrs = (const dmi_attribute_t[]){
                DMI_ATTRIBUTE(dmi_onboard_device_instance_t, type, ENUM, {
                    .code    = "type",
                    .name    = "Type",
                    .unspec  = dmi_value_ptr(DMI_ONBOARD_DEVICE_TYPE_UNSPEC),
                    .unknown = dmi_value_ptr(DMI_ONBOARD_DEVICE_TYPE_UNKNOWN),
                    .values  = &dmi_onboard_device_type_names
                }),
                DMI_ATTRIBUTE(dmi_onboard_device_instance_t, is_enabled, BOOL, {
                    .code    = "is-enabled",
                    .name    = "Enabled"
                }),
                DMI_ATTRIBUTE(dmi_onboard_device_instance_t, description, STRING, {
                    .code    = "description",
                    .name    = "Description"
                }),
                DMI_ATTRIBUTE_NULL
            }
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_onboard_device_decode,
        .cleanup = dmi_onboard_device_cleanup
    }
};

const char *dmi_onboard_device_type_name(dmi_onboard_device_type_t value)
{
    return dmi_name_lookup(&dmi_onboard_device_type_names, (int)value);
}

static bool dmi_onboard_device_decode(dmi_entity_t *entity)
{
    dmi_onboard_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(ONBOARD_DEVICE));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    // Number of devices is determined by the structure length, and each
    // device is described by two bytes
    size_t remaining = dmi_stream_remaining(stream);
    size_t instance_size = 2 * sizeof(dmi_byte_t);

    info->instance_count = remaining / instance_size;

    info->instances = dmi_alloc_array(entity->context, sizeof(dmi_onboard_device_instance_t), info->instance_count);
    if (info->instances == nullptr)
        return false;

    for (size_t i = 0; i < info->instance_count; i++) {
        dmi_onboard_device_instance_details_t details;
        dmi_onboard_device_instance_t *instance = &info->instances[i];

        bool status =
            dmi_stream_decode(stream, dmi_byte_t, &details.__value) and
            dmi_stream_decode_str(stream, &instance->description);
        if (not status)
            return false;

        instance->type        = details.type;
        instance->is_enabled  = details.is_enabled;
    }

    // Remaining byte does not form a complete device
    if (not dmi_stream_is_done(stream))
        return dmi_entity_incomplete(entity);

    return true;
}

static void dmi_onboard_device_cleanup(dmi_entity_t *entity)
{
    dmi_onboard_device_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(ONBOARD_DEVICE));
    if (info == nullptr)
        return;

    dmi_free(info->instances);
}
