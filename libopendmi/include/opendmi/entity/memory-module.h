//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_MEMORY_MODULE_H
#define OPENDMI_ENTITY_MEMORY_MODULE_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/utils/name.h>

typedef struct dmi_memory_module       dmi_memory_module_t;
typedef struct dmi_memory_module_size  dmi_memory_module_size_t;
typedef union  dmi_memory_module_type  dmi_memory_module_type_t;
typedef union  dmi_memory_module_error dmi_memory_module_error_t;

/**
 * @brief Memory module type details.
 */
dmi_packed_union(dmi_memory_module_type)
{
    /**
     * @brief Raw value.
     */
    dmi_word_t __value;

    dmi_packed_struct()
    {
        dmi_word_t is_other      : 1; ///< Other
        dmi_word_t is_unknown    : 1; ///< Unknown
        dmi_word_t is_standard   : 1; ///< Standard
        dmi_word_t is_fast_page  : 1; ///< Fast page mode
        dmi_word_t is_edo        : 1; ///< EDO
        dmi_word_t has_parity    : 1; ///< Parity
        dmi_word_t has_ecc       : 1; ///< ECC
        dmi_word_t is_simm       : 1; ///< SIMM
        dmi_word_t is_dimm       : 1; ///< DIMM
        dmi_word_t is_burst_edo  : 1; ///< Burst EDO
        dmi_word_t is_sdram      : 1; ///< SDRAM

        dmi_word_t __reserved : 5;
    };
};

dmi_static_assert_value_union(dmi_memory_module_type);

/**
 * @brief Memory module error status.
 */
dmi_packed_union(dmi_memory_module_error)
{
    dmi_byte_t __value;

    dmi_packed_struct()
    {
        /**
         * @brief Uncorrectable errors received for the module, if set. All or
         * a portion of the module has been disabled. This bit is only reset on
         * power-on.
         */
        dmi_byte_t has_uncorrectable : 1;

        /**
         * @brief Correctable errors received for the module, if set. This bit
         * is reset only during a system reset.
         */
        dmi_byte_t has_correctable : 1;

        /**
         * @brief If set, the error status information should be obtained from
         * the event log; bits 1and 0 are reserved.
         */
        dmi_byte_t has_event_log : 1;

        /**
         * @brief Reserved, set to zero.
         */
        dmi_byte_t __reserved : 5;
    };
};

dmi_static_assert_value_union(dmi_memory_module_error);

typedef enum dmi_memory_module_size_status
{
    DMI_MEMORY_MODULE_SIZE_STATUS_PRESENT,          ///< Present
    DMI_MEMORY_MODULE_SIZE_STATUS_INVALID,          ///< Invalid
    DMI_MEMORY_MODULE_SIZE_STATUS_NOT_DETERMINABLE, ///< Not determinable
    DMI_MEMORY_MODULE_SIZE_STATUS_NOT_ENABLED,      ///< Not enabled
    DMI_MEMORY_MODULE_SIZE_STATUS_NOT_INSTALLED,    ///< Not installed
} dmi_memory_module_size_status_t;

/**
 * @brief Memory mode size.
 */
struct dmi_memory_module_size
{
    /**
     * @brief Size of module in bytes, if applicable.
     */
    dmi_size_t value;

    /**
     * @brief Number of bank connections per module.
     */
    unsigned bank_count;

    /**
     * @brief Status.
     */
    dmi_memory_module_size_status_t status;
};

/**
 * @brief Memory module information structure (type 6, obsolete).
 */
struct dmi_memory_module
{
    /**
     * @brief Memory controller entity.
     */
    dmi_entity_t *controller;

    /**
     * @brief Socket reference designator. Example: "202".
     */
    const char *socket;

    /**
     * @brief Each nibble indicates a bank (RAS#) connection. 0x0F means no
     * connection.
     *
     * Example: If banks 1 & 3 (RAS# 1 & 3) were connected to a SIMM socket
     * the values for that socket would be { 1, 3 }. If only bank 2 (RAS# 2)
     * were connected, the values for that socket would be { USHRT_MAX, 2 }.
     */
    unsigned short bank_connections[2];

    /**
     * @brief Speed of the memory module, in ns (for example, 70 for a 70ns
     * module). If the speed is unknown, the field is set to 0.
     */
    unsigned short current_speed;

    /**
     * @brief The physical characteristics of the memory modules that are
     * supported by (and currently installed in) the system.
     */
    dmi_memory_module_type_t current_type;

    /**
     * @brief The size of the memory module that is installed in the socket.
     */
    dmi_memory_module_size_t installed_size;

    /**
     * @brief The amount of memory currently enabled for the system’s use from
     * the module.
     */
    dmi_memory_module_size_t enabled_size;

    /**
     * @brief Error status.
     */
    dmi_memory_module_error_t error_status;
};

extern const dmi_name_set_t dmi_memory_module_type_names;

/**
 * @brief Memory module information entity specification.
 */
extern const dmi_entity_spec_t dmi_memory_module_spec;

__BEGIN_DECLS

const char *dmi_memory_module_size_status_name(dmi_memory_module_size_status_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_MEMORY_MODULE_H
