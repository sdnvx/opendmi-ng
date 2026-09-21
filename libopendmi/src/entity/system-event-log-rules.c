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

void dmi_system_event_log_lint_area(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_system_event_log_t *info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_EVENT_LOG));

    if ((info == nullptr) or (info->area_length == 0))
        return;

    size_t offset = dmi_lint_entity_offset(lint, entity);

    // Both the header and the data live in the area of the log, which is
    // what its length covers
    if (info->header_offset >= info->area_length) {
        dmi_lint_issue(lint, entity, "header-offset", offset,
                       "header starts at 0x%X, while the area is 0x%X bytes long",
                       info->header_offset, info->area_length);
    }

    if (info->data_offset >= info->area_length) {
        dmi_lint_issue(lint, entity, "data-offset", offset,
                       "data starts at 0x%X, while the area is 0x%X bytes long",
                       info->data_offset, info->area_length);
    }
}

void dmi_system_event_log_lint_descriptors(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_system_event_log_t *info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_EVENT_LOG));

    if ((info == nullptr) or (info->descriptor_count == 0))
        return;

    // Length of the descriptors is read from the structure itself, since the
    // decoder keeps the descriptors rather than their layout
    dmi_stream_t stream;
    dmi_byte_t length;

    if (not dmi_stream_initialize(&stream, entity))
        return;

    if (not dmi_stream_read_data_at(&stream, &length, DMI_SYSTEM_EVENT_LOG_DESCRIPTOR_OFFSET,
                                    sizeof(length)))
        return;

    if (length == DMI_SYSTEM_EVENT_LOG_DESCRIPTOR_LENGTH)
        return;

    dmi_lint_issue(lint, entity, "descriptors", dmi_lint_entity_offset(lint, entity) +
                   DMI_SYSTEM_EVENT_LOG_DESCRIPTOR_OFFSET,
                   "descriptors are %u bytes long, expected %d",
                   (unsigned)length, DMI_SYSTEM_EVENT_LOG_DESCRIPTOR_LENGTH);
}
