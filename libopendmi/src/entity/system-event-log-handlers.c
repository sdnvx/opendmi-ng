//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/stream.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/system-event-log-internal.h>

const dmi_attribute_t dmi_system_log_type_descriptor_attrs[] =
{
    DMI_ATTRIBUTE(dmi_system_log_type_descriptor_t, type, ENUM, {
        .code   = "type",
        .name   = "Type",
        .values = &dmi_event_log_type_names
    }),
    DMI_ATTRIBUTE(dmi_system_log_type_descriptor_t, data_format, ENUM, {
        .code   = "data-format",
        .name   = "Data format",
        .values = &dmi_event_log_data_format_names
    }),
    {}
};

const dmi_attribute_t dmi_system_log_io_ports_attrs[] =
{
    DMI_ATTRIBUTE(dmi_system_log_io_ports_t, index_port, INTEGER, {
        .code  = "index-port",
        .name  = "Index port",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_system_log_io_ports_t, data_port, INTEGER, {
        .code  = "data-port",
        .name  = "Data port",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    {}
};

//
// Access method address is interpreted according to the access method.
//
bool dmi_system_event_log_derive(dmi_entity_t *entity)
{
    dmi_system_event_log_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_EVENT_LOG));
    if (info == nullptr)
        return false;

    info->access_ports.index_port = (uint16_t)(info->access_address & 0xFFFFu);
    info->access_ports.data_port  = (uint16_t)(info->access_address >> 16);
    info->access_gpnv_handle      = (uint16_t)(info->access_address & 0xFFFFu);

    return true;
}

void dmi_system_event_log_cleanup(dmi_entity_t *entity)
{
    dmi_system_event_log_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_EVENT_LOG));
    if (info == nullptr)
        return;

    dmi_free(info->descriptors);
}
