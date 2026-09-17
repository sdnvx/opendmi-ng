//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MGMT_CONTROLLER_H
#define OPENDMI_ENTITY_MGMT_CONTROLLER_H

#pragma once

#include <opendmi/entity.h>

typedef struct dmi_mgmt_controller   dmi_mgmt_controller_t;
typedef struct dmi_mgmt_proto_record dmi_mgmt_proto_record_t;

/**
 * @brief Management interface types.
 */
typedef enum dmi_mgmt_if_type {
    DMI_MGMT_IF_TYPE_MCTP_KCS        = 0x02, ///< Keyboard Controller Style
    DMI_MGMT_IF_TYPE_MCTP_8250_UART  = 0x03, ///< 8250 UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_16450_UART = 0x04, ///< 16450 UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_16550_UART = 0x05, ///< 16550/16550A UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_16650_UART = 0x06, ///< 16650/16650A UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_16750_UART = 0x07, ///< 16750/16750A UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_16850_UART = 0x08, ///< 16850/16850A UART Register Compatible
    DMI_MGMT_IF_TYPE_MCTP_I2C_SMBUS  = 0x09, ///< I2C/SMBUS
    DMI_MGMT_IF_TYPE_MCTP_I3C        = 0x0A, ///< I3C
    DMI_MGMT_IF_TYPE_MCTP_PCIE_VDM   = 0x0B, ///< PCIe VDM
    DMI_MGMT_IF_TYPE_MCTP_MMBI       = 0x0C, ///< MMBI
    DMI_MGMT_IF_TYPE_MCTP_PCC        = 0x0D, ///< PCC
    DMI_MGMT_IF_TYPE_MCTP_UCIE       = 0x0E, ///< UCIe
    DMI_MGMT_IF_TYPE_MCTP_USB        = 0x0F, ///< USB
    DMI_MGMT_IF_TYPE_NETWORK_HOST_IF = 0x40, ///< Network Host Interface (DSP0270)
    DMI_MGMT_IF_TYPE_OEM             = 0xF0, ///< OEM-defined
} dmi_mgmt_if_type_t;

/**
 * @brief Management protocol types.
 */
typedef enum dmi_mgmt_proto {
    DMI_MGMT_PROTO_IPMI            = 0x02, ///< IPMI: Intelligent Platform Management Interface
    DMI_MGMT_PROTO_MCTP            = 0x03, ///< MCTP: Management Component Transport Protoco
    DMI_MGMT_PROTO_REDFISH_OVER_IP = 0x04, ///< Redfish over IP
    DMI_MGMT_PROTO_OEM             = 0xF0  ///< OEM-defined
} dmi_mgmt_proto_t;

/**
 * @brief Management Controller Host Interface (Type 42) structure.
 */
struct dmi_mgmt_controller
{
    dmi_mgmt_if_type_t if_type;

    size_t if_data_length;

    dmi_byte_t *if_data;

    size_t proto_records_count;

    dmi_mgmt_proto_record_t **proto_records;
};

/**
 * @brief Decoded Protocol Record Data.
 */
struct dmi_mgmt_proto_record
{
    dmi_mgmt_proto_t type;

    size_t length;

    dmi_byte_t data[];
};

/**
 * @brief Management controller host interface entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_mgmt_controller_host_if_spec;

#endif // !OPENDMI_ENTITY_MGMT_CONTROLLER_H
