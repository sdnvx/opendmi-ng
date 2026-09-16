//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_TEXT_HELPERS_H
#define OPENDMI_FORMAT_TEXT_HELPERS_H

#pragma once

#include <opendmi/utils/tty.h>
#include <opendmi/format/text/types.h>

__BEGIN_DECLS

__attribute__((format(printf, 3, 4)))
void dmi_text_printf(
        dmi_text_session_t *session,
        dmi_tty_color_t color,
        const char *format,
        ...);

void dmi_text_hex_data(dmi_text_session_t *session, const void *data, size_t length);

__END_DECLS

#endif // !OPENDMI_FORMAT_TEXT_HELPERS_H
