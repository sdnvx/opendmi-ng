//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#ifdef ENABLE_CURSES
#   if __has_include(<ncurses/ncurses.h>)
#       ifdef _WIN32
#           define NCURSES_STATIC 1 // Required to avoid linking errors on Windows
#       endif
#       include <ncurses/ncurses.h>
#   elif __has_include(<ncurses/curses.h>)
#       include <ncurses/curses.h>
#   elif __has_include(<ncurses.h>)
#       include <ncurses.h>
#   elif __has_include(<curses.h>)
#       include <curses.h>
#   endif
#   if __has_include(<ncurses/term.h>)
#       include <ncurses/term.h>
#   elif __has_include(<term.h>)
#       include <term.h>
#   endif
#endif // ENABLE_CURSES

#include <opendmi/internal.h>
#include <opendmi/utils/tty.h>

#include <opendmi/format/text/helpers.h>

void dmi_text_printf(
        dmi_text_session_t *session,
        dmi_tty_color_t color,
        const char *format,
        ...)
{
    va_list args;

#ifdef ENABLE_CURSES
    if (session->is_tty and (color != DMI_TTY_COLOR_NONE))
        tputs(tparm(tigetstr("setaf"), color), 1, putchar);
#else
    dmi_unused(color);
#endif // ENABLE_CURSES

    va_start(args, format);
    vfprintf(session->stream, format, args);
    va_end(args);

#ifdef ENABLE_CURSES
    if (session->is_tty and (color != DMI_TTY_COLOR_NONE))
        tputs(tparm(tigetstr("sgr0")), 1, putchar);
#endif // ENABLE_CURSES
}

void dmi_text_hex_data(dmi_text_session_t *session, const void *data, size_t length)
{
    assert(session != nullptr);
    assert(data != nullptr);

    const unsigned char *ptr = dmi_cast(ptr, data);

    // Bytes are separated by spaces, lines have no trailing ones
    for (size_t i = 0; i < length; i++) {
        if (i % 0x10 == 0)
            fprintf(session->stream, "\t\t");
        else
            fputc(' ', session->stream);

        fprintf(session->stream, "%02X", (int)ptr[i]);

        if ((i % 0x10 == 0x0f) or (i + 1 == length))
            fputc('\n', session->stream);
    }
}
