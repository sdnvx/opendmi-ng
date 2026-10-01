//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_REVISIONS_INTERNAL_H
#define OPENDMI_ENTITY_REVISIONS_INTERNAL_H

#pragma once

#include <opendmi/field.h>
#include <opendmi/lint.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/dell/revisions.h>

// Operation handlers, see revisions-handlers.c

/**
 * @internal
 * @brief Decode the implementation version.
 *
 * @details Implementation version is carried as the major and the minor
 * number, one byte each, which the version number puts in the order it counts
 * them.
 *
 * @param[in]  field Field being decoded.
 * @param[in]  data  Data the field carries.
 * @param[out] value Variable to store the version in.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_dell_revisions_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value);

/**
 * @internal
 * @brief Encode the implementation version, which undoes
 * `dmi_dell_revisions_decode_version()`.
 *
 * @param[in]  field Field being encoded.
 * @param[in]  value Variable holding the version.
 * @param[out] data  Data the field is to carry.
 *
 * @return Always `true`.
 */
bool dmi_dell_revisions_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data);

#endif // !OPENDMI_ENTITY_REVISIONS_INTERNAL_H
