//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_TEXT_HELPERS_H
#define OPENDMI_FORMAT_TEXT_HELPERS_H

#pragma once

#include <opendmi/format/text/types.h>

__BEGIN_DECLS

void dmi_text_printf(
        dmi_text_session_t *session,
        dmi_tty_color_t color,
        const char *format,
        ...);

void dmi_text_hex_data(dmi_text_session_t *session, const void *data, size_t length);

/**
 * @brief Make a string, which comes from the data (e.g. firmware string),
 * safe to be written to a terminal.
 *
 * Control characters are written as visible escapes, as made by
 * `dmi_utf8_escape()`, so that the data cannot send commands to the terminal.
 *
 * @param[in]  session Text session.
 * @param[in]  str     NUL-terminated string.
 * @param[out] copy    Escaped copy of the string, which should be freed with
 *                     `dmi_free()`, or @c nullptr if no copy is needed.
 *
 * @return @p str itself if it needs no escapes, its escaped copy, or the
 *         placeholder of erroneous values if out of memory.
 */
const char *dmi_text_escape(dmi_text_session_t *session, const char *str, char **copy);

__END_DECLS

#endif // !OPENDMI_FORMAT_TEXT_HELPERS_H
