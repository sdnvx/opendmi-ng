//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/ipmi-device-internal.h>

//
// Revision is decoded into a version, which keeps no trace of the digits it
// was made of, so the raw data is read instead.
//
void dmi_ipmi_device_lint_revision(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_reader_t reader;
    dmi_byte_t value;

    if (not dmi_reader_initialize(&reader, dmi_entity_buffer(entity),
                                  dmi_entity_offset(entity), entity->body_length))
        return;

    if (not dmi_reader_get_bytes_at(&reader, &value, DMI_IPMI_DEVICE_REVISION_OFFSET,
                                    sizeof(value)))
        return;

    if (((value & 0x0F) <= 9) and (((value >> 4) & 0x0F) <= 9))
        return;

    dmi_lint_issue(lint, entity, "specification-version",
                   dmi_lint_entity_offset(lint, entity) + DMI_IPMI_DEVICE_REVISION_OFFSET,
                   "revision 0x%02X is not a binary-coded decimal", (unsigned)value);
}
