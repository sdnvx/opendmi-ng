//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/mgmt-device-threshold.h>

static bool dmi_mgmt_device_threshold_decode(dmi_entity_t *entity);

const dmi_entity_spec_t dmi_mgmt_device_threshold_spec =
{
    .code            = "mgmt-device-threshold",
    .name            = "Management device threshold data",
    .description     = (const char *[]){
        "The information in this structure defines threshold information for "
        "a component (probe or cooling-unit) contained within a Management "
        "Device.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MGMT_DEVICE_THRESHOLD),
    .minimum_version = DMI_VERSION(2, 3, 0),
    .minimum_length  = 0x10,
    .decoded_length  = sizeof(dmi_mgmt_device_threshold_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_mgmt_device_threshold_t, lower_non_critical, INTEGER, {
            .code    = "lower-non-critical",
            .name    = "Lower non-critical",
            .unknown = dmi_value_ptr((short)SHRT_MIN)
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_threshold_t, upper_non_critical, INTEGER, {
            .code    = "upper-non-critical",
            .name    = "Upper non-critical",
            .unknown = dmi_value_ptr((short)SHRT_MIN)
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_threshold_t, lower_critical, INTEGER, {
            .code    = "lower-critical",
            .name    = "Lower critical",
            .unknown = dmi_value_ptr((short)SHRT_MIN)
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_threshold_t, upper_critical, INTEGER, {
            .code    = "upper-critical",
            .name    = "Upper critical",
            .unknown = dmi_value_ptr((short)SHRT_MIN)
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_threshold_t, lower_non_recoverable, INTEGER, {
            .code    = "lower-non-recoverable",
            .name    = "Lower non-recoverable",
            .unknown = dmi_value_ptr((short)SHRT_MIN)
        }),
        DMI_ATTRIBUTE(dmi_mgmt_device_threshold_t, upper_non_recoverable, INTEGER, {
            .code    = "upper-non-recoverable",
            .name    = "Upper non-recoverable",
            .unknown = dmi_value_ptr((short)SHRT_MIN)
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_mgmt_device_threshold_decode
    }
};

static bool dmi_mgmt_device_threshold_decode(dmi_entity_t *entity)
{
    dmi_mgmt_device_threshold_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_THRESHOLD));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    return
        dmi_stream_decode(stream, dmi_word_t, &info->lower_non_critical) and
        dmi_stream_decode(stream, dmi_word_t, &info->upper_non_critical) and
        dmi_stream_decode(stream, dmi_word_t, &info->lower_critical) and
        dmi_stream_decode(stream, dmi_word_t, &info->upper_critical) and
        dmi_stream_decode(stream, dmi_word_t, &info->lower_non_recoverable) and
        dmi_stream_decode(stream, dmi_word_t, &info->upper_non_recoverable);
}
