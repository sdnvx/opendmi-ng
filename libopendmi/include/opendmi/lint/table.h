//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LINT_TABLE_H
#define OPENDMI_LINT_TABLE_H

#pragma once

#include <opendmi/lint.h>

__BEGIN_DECLS

/**
 * @brief Table holds every structure completely.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_table_truncated_rule;

/**
 * @brief Table ends with an end-of-table structure.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_table_terminator_rule;

/**
 * @brief Table has no data past the end-of-table structure.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_table_trailing_data_rule;

/**
 * @brief Table has the structures required by the specification.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_table_required_rule;

/**
 * @brief Table has the structures recommended for the platform.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_table_recommended_rule;

/**
 * @brief Structures which have to be unique are not repeated.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_table_singleton_rule;

/**
 * @brief Handles are outside of the range reserved by the specification.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_table_reserved_handle_rule;

__END_DECLS

#endif // !OPENDMI_LINT_TABLE_H
