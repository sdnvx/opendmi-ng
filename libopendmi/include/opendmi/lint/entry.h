//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LINT_ENTRY_H
#define OPENDMI_LINT_ENTRY_H

#pragma once

#include <opendmi/lint.h>

__BEGIN_DECLS

/**
 * @brief Checksum of the entry point matches its data.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entry_checksum_rule;

/**
 * @brief Length of the entry point matches its format.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entry_length_rule;

/**
 * @brief Entry point points to the table.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entry_table_address_rule;

/**
 * @brief Size of the table matches the one declared by the entry point.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entry_table_size_rule;

/**
 * @brief Number of the structures matches the one declared by the entry
 * point.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entry_entity_count_rule;

/**
 * @brief Structures fit the maximum size declared by the entry point.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entry_entity_max_size_rule;

__END_DECLS

#endif // !OPENDMI_LINT_ENTRY_H
