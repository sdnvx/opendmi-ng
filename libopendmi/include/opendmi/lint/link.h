//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LINT_LINK_H
#define OPENDMI_LINT_LINK_H

#pragma once

#include <opendmi/lint.h>

__BEGIN_DECLS

/**
 * @brief References point to the structures of the table.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_link_dangling_rule;

/**
 * @brief References point to the structures of the types they expect.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_link_wrong_type_rule;

/**
 * @brief Structures do not reference themselves.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_link_self_rule;

/**
 * @brief Structures are referenced by the rest of the table.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_link_orphan_rule;

__END_DECLS

#endif // !OPENDMI_LINT_LINK_H
