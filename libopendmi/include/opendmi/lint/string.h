//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LINT_STRING_H
#define OPENDMI_LINT_STRING_H

#pragma once

#include <opendmi/lint.h>

__BEGIN_DECLS

/**
 * @brief Strings are no longer than the specification allows.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_string_too_long_rule;

/**
 * @brief Strings hold printable text.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_string_non_printable_rule;

/**
 * @brief Strings are not blank.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_string_blank_rule;

/**
 * @brief Strings have no leading or trailing whitespace.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_string_padded_rule;

/**
 * @brief Every string is referenced by a field of its structure.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_string_unreferenced_rule;

__END_DECLS

#endif // !OPENDMI_LINT_STRING_H
