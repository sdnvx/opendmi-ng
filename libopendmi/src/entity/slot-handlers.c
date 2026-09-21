//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

#include <opendmi/entity/common.h>
#include <opendmi/entity/slot-internal.h>

//
// PCI address holds the device and the function numbers in one byte, and
// leaves both unset when the bus number says they do not apply.
//
bool dmi_slot_decode_pci_addr(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    return dmi_pci_addr_decode(dmi_entity_stream(entity), value);
}

void dmi_slot_cleanup(dmi_entity_t *entity)
{
    dmi_slot_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_SLOTS));
    if (info == nullptr)
        return;

    dmi_free(info->peer_groups);
}
