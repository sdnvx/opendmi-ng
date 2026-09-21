//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LINT_VALUE_H
#define OPENDMI_LINT_VALUE_H

#pragma once

#include <opendmi/lint.h>

__BEGIN_DECLS

/**
 * @brief Values of enumerated fields are defined by the specification.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_value_invalid_enum_rule;

/**
 * @brief Fields hold no values reserved by the specification.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_value_reserved_rule;

/**
 * @brief Bit fields have no bits reserved by the specification set.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_value_reserved_bits_rule;

/**
 * @brief Fields encoded as binary-coded decimals hold decimal digits.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_value_bcd_rule;

/**
 * @brief UUIDs are set.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_value_uuid_rule;

__END_DECLS

#endif // !OPENDMI_LINT_VALUE_H
