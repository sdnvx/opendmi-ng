//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LINT_OVERLAY_H
#define OPENDMI_LINT_OVERLAY_H

#pragma once

#include <opendmi/lint.h>

__BEGIN_DECLS

/**
 * @brief Additional information entries refer to the structures of the table.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_overlay_dangling_rule;

/**
 * @brief Additional information entries refer to the fields of the structures.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_overlay_out_of_bounds_rule;

/**
 * @brief Additional information entries carry values.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_overlay_empty_rule;

__END_DECLS

#endif // !OPENDMI_LINT_OVERLAY_H
