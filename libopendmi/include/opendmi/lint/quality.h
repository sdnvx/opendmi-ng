//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LINT_QUALITY_H
#define OPENDMI_LINT_QUALITY_H

#pragma once

#include <opendmi/lint.h>

__BEGIN_DECLS

/**
 * @brief Strings hold data rather than the placeholders of the firmware
 * vendor.
 */
extern __dmi_api const dmi_lint_rule_t dmi_lint_quality_placeholder_rule;

__END_DECLS

#endif // !OPENDMI_LINT_QUALITY_H
