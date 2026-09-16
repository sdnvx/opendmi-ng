//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include "../../config.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <opendmi/utils/tty.h>
#include <opendmi/format/text/helpers.h>

void dmi_text_printf(
        dmi_text_session_t *session,
        dmi_tty_color_t color,
        const char *format,
        ...)
{
    va_list args;

    if (session->is_tty and (color != DMI_TTY_COLOR_NONE))
        dmi_tty_set_fg_color(color);

    va_start(args, format);
    vfprintf(session->stream, format, args);
    va_end(args);

    if (session->is_tty and (color != DMI_TTY_COLOR_NONE))
        dmi_tty_exit_attr_mode();
}

void dmi_text_hex_data(dmi_text_session_t *session, const void *data, size_t length)
{
    assert(session != nullptr);
    assert(data != nullptr);

    const unsigned char *ptr = dmi_cast(ptr, data);

    for (size_t i = 0; i < length; i++) {
        char sp = ' ';

        if (i % 0x10 == 0)
            fputs("\t\t", session->stream);
        if (i % 0x10 == 0x0f)
            sp = '\n';

        fprintf(session->stream, "%02X%c", (int)ptr[i], sp);
    }

    if (length % 0x10 != 0)
        fputc('\n', session->stream);
}
