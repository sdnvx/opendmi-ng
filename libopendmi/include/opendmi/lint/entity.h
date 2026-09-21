//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LINT_ENTITY_H
#define OPENDMI_LINT_ENTITY_H

#pragma once

#include <opendmi/lint.h>

__BEGIN_DECLS

/**
 * @brief Structure is long enough for its type.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entity_below_minimum_rule;

/**
 * @brief Length of the structure matches a version of its specification.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entity_unknown_length_rule;

/**
 * @brief Structure has been decoded.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entity_undecoded_rule;

/**
 * @brief Structure has no fields newer than the version of the entry point.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entity_newer_fields_rule;

/**
 * @brief Type of the structure is defined by the version of the entry point.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entity_newer_type_rule;

/**
 * @brief Type of the structure is known.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entity_unknown_type_rule;

/**
 * @brief Structure is not of a type obsoleted by the specification.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_entity_obsolete_rule;

__END_DECLS

#endif // !OPENDMI_LINT_ENTITY_H
