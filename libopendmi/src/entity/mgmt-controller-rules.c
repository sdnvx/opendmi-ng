//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/reader.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/utils/endian.h>

#include <opendmi/entity/mgmt-controller-internal.h>

/**
 * @internal
 * @brief Offset of the length of the interface-specific data, which the
 * protocol records follow.
 */
#define DMI_MGMT_CONTROLLER_IF_LENGTH_OFFSET 0x05

// Records follow each other, each one carrying its own length, so they all
// fit the structure only if the lengths agree with it.
//
void dmi_mgmt_controller_lint_records(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_reader_t reader;
    dmi_byte_t length;

    if (not dmi_reader_initialize(&reader, dmi_entity_buffer(entity),
                                  dmi_entity_offset(entity), entity->body_length))
        return;

    if (not dmi_reader_get_bytes_at(&reader, &length, DMI_MGMT_CONTROLLER_IF_LENGTH_OFFSET,
                                    sizeof(length)))
        return;

    // Number of the records follows the interface-specific data
    size_t offset = DMI_MGMT_CONTROLLER_IF_LENGTH_OFFSET + sizeof(length) + length;
    dmi_byte_t count;

    if (not dmi_reader_get_bytes_at(&reader, &count, offset, sizeof(count))) {
        dmi_lint_issue(lint, entity, "if-data", dmi_lint_entity_offset(lint, entity) +
                       DMI_MGMT_CONTROLLER_IF_LENGTH_OFFSET,
                       "interface data of %u bytes leaves no room for the protocol records",
                       (unsigned)length);
        return;
    }

    offset += sizeof(count);

    for (unsigned i = 0; i < count; i++) {
        dmi_byte_t record[2];

        // Every record starts with its type and the length of its data
        if (dmi_reader_get_bytes_at(&reader, record, offset, sizeof(record))) {
            offset += sizeof(record) + record[1];

            if (offset <= entity->body_length)
                continue;
        }

        dmi_lint_issue(lint, entity, "protocol-records", dmi_lint_entity_offset(lint, entity),
                       "record %u of %u does not fit the structure of %zu bytes",
                       i + 1, (unsigned)count, entity->body_length);
        return;
    }
}
