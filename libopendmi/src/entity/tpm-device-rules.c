//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/reader.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/tpm-device-internal.h>

void dmi_tpm_device_lint_version(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_tpm_device_t *info = dmi_entity_info(entity, DMI_TYPE(TPM_DEVICE));
    if (info == nullptr)
        return;

    unsigned major = dmi_version_major(info->spec_version);

    // The TCG has published the 1.2 and the 2.0 specifications, and the
    // format of the firmware version follows the major one
    if ((major == 1) or (major == 2))
        return;

    dmi_lint_issue(lint, entity, "specification-version", dmi_lint_entity_offset(lint, entity),
                   "device declares TPM %u.%u", major, dmi_version_minor(info->spec_version));
}

//
// Firmware of some vendors stores the identifier as a little-endian double
// word, so that it starts with the terminating zero, e.g. "\0XFI" for "IFX".
// The decoder puts it back in place, and the raw data still shows it.
//
void dmi_tpm_device_lint_vendor(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_reader_t reader;
    char id[4];

    if (not dmi_reader_initialize(&reader, entity))
        return;

    if (not dmi_reader_get_bytes_at(&reader, id, DMI_TPM_DEVICE_VENDOR_OFFSET, sizeof(id)))
        return;

    if ((id[0] != 0) or (id[3] == 0))
        return;

    dmi_lint_issue(lint, entity, "vendor-id", dmi_lint_entity_offset(lint, entity) +
                   DMI_TPM_DEVICE_VENDOR_OFFSET,
                   "identifier is stored as a little-endian double word");
}
