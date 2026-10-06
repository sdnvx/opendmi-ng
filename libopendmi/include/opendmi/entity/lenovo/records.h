//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_LENOVO_RECORDS_H
#define OPENDMI_ENTITY_LENOVO_RECORDS_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/utils/datetime.h>

#ifndef DMI_LENOVO_DATE_T
#   define DMI_LENOVO_DATE_T
    typedef struct dmi_lenovo_date dmi_lenovo_date_t;
#endif // !DMI_LENOVO_DATE_T

#ifndef DMI_LENOVO_MTM_T
#   define DMI_LENOVO_MTM_T
    typedef struct dmi_lenovo_mtm dmi_lenovo_mtm_t;
#endif // !DMI_LENOVO_MTM_T

#ifndef DMI_LENOVO_TPM_INFO_T
#   define DMI_LENOVO_TPM_INFO_T
    typedef struct dmi_lenovo_tpm_info dmi_lenovo_tpm_info_t;
#endif // !DMI_LENOVO_TPM_INFO_T

/**
 * @brief Lenovo date record structure (type 134).
 *
 * Holds a date of a ThinkPad, which falls within the years the model has been
 * made in, and so is probably its manufacture date. Reverse engineered from
 * the data corpus.
 */
struct dmi_lenovo_date
{
    /**
     * @brief Date, as the structure holds it: day, month and year in
     * binary-coded decimal, the year low byte first.
     */
    uint32_t raw_date;

    /**
     * @brief Date.
     */
    dmi_date_t date;
};

/**
 * @brief Lenovo TPM information structure (type 134).
 *
 * Tells the vendor of the TPM of a Lenovo laptop, by its TCG vendor ID, e.g.
 * `ATML` for Atmel or `STM ` for STMicroelectronics. The structures are told
 * from the date records of the same type by their first string, `TPM INFO`.
 * Reverse engineered from the data corpus.
 */
struct dmi_lenovo_tpm_info
{
    /**
     * @brief Value whose meaning is not established, `0` in all known data.
     */
    uint8_t unknown_1;

    /**
     * @brief TCG vendor ID of the TPM, as the structure holds it.
     */
    dmi_binary_t vendor_raw;

    /**
     * @brief TCG vendor ID of the TPM, e.g. `ATML`, or @c nullptr if it is
     * not printable.
     *
     * The string belongs to the structure, and is freed along with it.
     */
    char *vendor;

    /**
     * @brief Values whose meaning is not established, `1` and `1` in all
     * known data.
     */
    uint8_t unknown_2[2];

    /**
     * @brief Value whose meaning is not established, `0` in all known data.
     */
    uint16_t unknown_3;

    /**
     * @brief Value whose meaning is not established, `0`, `2` or `3` in the
     * known data.
     */
    uint8_t unknown_4;

    /**
     * @brief Signature, `TPM INFO`.
     */
    const char *signature;

    /**
     * @brief Description of the rest, `System Reserved`.
     */
    const char *description;
};

/**
 * @brief Lenovo machine type model structure (type 200).
 *
 * Tells the brand of a consumer or small business laptop of Lenovo, e.g. an
 * IdeaPad, a Yoga, a Legion or a ThinkBook, and its full machine type
 * model, e.g. `20VE00U9RU`, of which the system information (type 1) holds
 * only the machine type, e.g. `20VE`. Reverse engineered from the data
 * corpus.
 */
struct dmi_lenovo_mtm
{
    /**
     * @brief Brand, e.g. `IdeaPad`, which ThinkBooks hold too.
     */
    const char *brand;

    /**
     * @brief Machine type model.
     */
    const char *mtm;

    /**
     * @brief Bytes past the strings, whose meaning is not established.
     */
    dmi_binary_t data;
};

/**
 * @brief Lenovo date record entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_lenovo_date_spec;

/**
 * @brief Lenovo TPM information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_lenovo_tpm_info_spec;

/**
 * @brief Lenovo machine type model entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_lenovo_mtm_spec;

#endif // !OPENDMI_ENTITY_LENOVO_RECORDS_H
