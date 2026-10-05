//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_COMMON_INTERNAL_H
#define OPENDMI_ENTITY_COMMON_INTERNAL_H

#pragma once

/**
 * @file
 * @internal
 * @brief Helpers the structures of several types share, see common-handlers.c.
 */

#include <opendmi/entity/common.h>

__BEGIN_DECLS

/**
 * @internal
 * @brief Decode a PCI address, which a field carries as a double word of the
 * segment group, the bus, and the device and function packed into one byte.
 *
 * Addresses of no bus carry no device and function either.
 */
bool dmi_pci_addr_decode(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode a PCI address, which undoes `dmi_pci_addr_decode()`.
 */
bool dmi_pci_addr_encode(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

__END_DECLS

#endif // !OPENDMI_ENTITY_COMMON_INTERNAL_H
