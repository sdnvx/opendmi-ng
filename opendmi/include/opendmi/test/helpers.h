//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_TEST_HELPERS_H
#define OPENDMI_TEST_HELPERS_H

#pragma once

#include <opendmi/option.h>

__BEGIN_DECLS

/**
 * @brief Count duplicate option names in option sets.
 *
 * Option sets are looked up in order, so an option name defined in several
 * sets is only accessible via the first one. Every duplicate short or long
 * name is reported to the standard error stream.
 *
 * @param[in] options Option sets, terminated by @c nullptr.
 *
 * @return Number of duplicate option names.
 */
size_t dmi_test_option_duplicates(const dmi_option_set_t **options);

__END_DECLS

#endif // !OPENDMI_TEST_HELPERS_H
