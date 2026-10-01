//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_HPE_USB_DEVICE_INTERNAL_H
#define OPENDMI_ENTITY_HPE_USB_DEVICE_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/hpe/usb-device.h>

/**
 * @internal
 * @brief USB class of mass storage devices.
 */
#define DMI_HPE_USB_CLASS_STORAGE 0x08

/**
 * @internal
 * @brief USB class of hubs.
 */
#define DMI_HPE_USB_CLASS_HUB 0x09

/**
 * @internal
 * @brief Names of the USB subclasses of mass storage devices.
 */
extern const dmi_name_set_t dmi_hpe_usb_storage_subclass_names;

/**
 * @internal
 * @brief Names of the USB protocols of mass storage devices.
 */
extern const dmi_name_set_t dmi_hpe_usb_storage_proto_names;

/**
 * @internal
 * @brief Names of the USB protocols of hubs.
 */
extern const dmi_name_set_t dmi_hpe_usb_hub_proto_names;

/**
 * @internal
 * @brief Convert the capacity of the device from megabytes to bytes.
 *
 * @param[in,out] entity Structure being derived.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_hpe_usb_device_derive(dmi_entity_t *entity);

#endif // !OPENDMI_ENTITY_HPE_USB_DEVICE_INTERNAL_H
