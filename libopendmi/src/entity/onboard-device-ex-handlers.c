//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/onboard-device-ex-internal.h>

//
// PCI address holds the device and the function numbers in one byte, and
// leaves both unset when the bus number says they do not apply.
//
bool dmi_onboard_device_ex_decode_pci_addr(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    return dmi_pci_addr_decode(dmi_entity_stream(entity), value);
}
