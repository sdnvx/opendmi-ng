//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <string.h>
#include <opendmi/reader.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-internal.h>

//
// Size of 0xFF means that the actual one is in the extended field, which was
// added in SMBIOS 3.1, so a structure of an earlier version carries no size
// at all.
//
void dmi_firmware_lint_rom_size(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_reader_t reader;
    dmi_byte_t value;

    if (not dmi_reader_initialize(&reader, entity))
        return;

    if (not dmi_reader_get_bytes_at(&reader, &value, DMI_FIRMWARE_ROM_SIZE_OFFSET, sizeof(value)))
        return;

    if (value != DMI_FIRMWARE_ROM_SIZE_EXTENDED)
        return;

    // Extended size is the last field of the structure of SMBIOS 3.1
    if (entity->level >= dmi_version(3, 1, 0))
        return;

    dmi_lint_issue(lint, entity, "rom-size", dmi_lint_entity_offset(lint, entity) +
                   DMI_FIRMWARE_ROM_SIZE_OFFSET,
                   "ROM size refers to the extended one, which the structure does not carry");
}

//
// Release date is a string, which the specification requires to be written as
// mm/dd/yyyy since SMBIOS 2.3, while the earlier two-digit year leaves the
// century to the reader.
//
void dmi_firmware_lint_release_date(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_reader_t reader;
    dmi_string_t index;

    if (not dmi_reader_initialize(&reader, entity))
        return;

    if (not dmi_reader_get_bytes_at(&reader, &index, DMI_FIRMWARE_DATE_OFFSET, sizeof(index)))
        return;

    const char *date = dmi_entity_string(entity, index);

    if ((date == nullptr) or (strlen(date) == strlen("mm/dd/yyyy")))
        return;

    dmi_lint_issue(lint, entity, "release-date", dmi_lint_string_offset(lint, entity, index),
                   "release date \"%s\" is not written as mm/dd/yyyy", date);
}
