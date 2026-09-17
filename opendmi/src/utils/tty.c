//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include "../config.h"

#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#include <stdio.h>
#include <stdarg.h>

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

#include <opendmi/utils/tty.h>

static bool dmi_tty = false;
static bool dmi_tty_stdout = false;
static int  dmi_tty_bg_color = DMI_TTY_COLOR_BLACK;
static int  dmi_tty_fg_color = DMI_TTY_COLOR_WHITE;
static int  dmi_tty_attrs = 0;

void dmi_tty_init(void)
{
    // Standard output may be redirected to pager later
    dmi_tty_stdout = isatty(STDOUT_FILENO);

#ifdef ENABLE_CURSES
    // Initialize terminal. Unknown terminal type is not an error, colors are
    // just not used in this case.
    if (dmi_tty_stdout) {
        int error;

        if (setupterm(nullptr, STDOUT_FILENO, &error) != ERR)
            dmi_tty = has_colors() and (start_color() != ERR);
    }
#endif // ENABLE_CURSES
}

bool dmi_has_tty(void)
{
    return dmi_tty;
}

bool dmi_tty_is_stdout(void)
{
    return dmi_tty_stdout;
}

void dmi_tty_attr_on(int attrs)
{
    if (not dmi_tty)
        return;

    dmi_tty_attrs |= attrs;

#   ifdef ENABLE_CURSES
        if (attrs & DMI_TTY_ATTR_BOLD)
            tputs(tparm(tigetstr("bold")), 1, putchar);
        if (attrs & DMI_TTY_ATTR_UNDERLINE)
            tputs(tparm(tigetstr("smul")), 1, putchar);
        if (attrs & DMI_TTY_ATTR_BLINK)
            tputs(tparm(tigetstr("blink")), 1, putchar);
#   endif
}

void dmi_tty_attr_off(int attrs)
{
    if (not dmi_tty)
        return;

    dmi_tty_attrs &= ~attrs;

#   ifdef ENABLE_CURSES
        dmi_tty_exit_attr_mode();

        dmi_tty_attr_on(dmi_tty_attrs);
        dmi_tty_set_bg_color((dmi_tty_color_t)dmi_tty_bg_color);
        dmi_tty_set_fg_color((dmi_tty_color_t)dmi_tty_fg_color);
#   endif
}

void dmi_tty_set_fg_color(dmi_tty_color_t color)
{
    if (not dmi_tty)
        return;

    dmi_tty_fg_color = (int)color;

#   ifdef ENABLE_CURSES
        tputs(tparm(tigetstr("setaf"), color), 1, putchar);
#   endif
}

void dmi_tty_set_bg_color(dmi_tty_color_t color)
{
    if (not dmi_tty)
        return;

    dmi_tty_bg_color = (int)color;

#   ifdef ENABLE_CURSES
        tputs(tparm(tigetstr("setab"), color), 1, putchar);
#   endif
}

void dmi_tty_cprintf(int color, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    dmi_tty_vcprintf(color, format, args);
    va_end(args);
}

void dmi_tty_vcprintf(int color, const char *format, va_list args)
{
    dmi_tty_set_fg_color((dmi_tty_color_t)color);
    vprintf(format, args);
    dmi_tty_exit_attr_mode();
}

void dmi_tty_exit_attr_mode(void)
{
#ifdef ENABLE_CURSES
    if (dmi_tty)
        tputs(tparm(tigetstr("sgr0")), 1, putchar);
#endif // ENABLE_CURSES
}

void dmi_tty_header(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    dmi_tty_attr_on(DMI_TTY_ATTR_BOLD);
    dmi_tty_vcprintf(DMI_TTY_COLOR_WHITE, format, args);
    dmi_tty_exit_attr_mode();
    va_end(args);

    printf("\n\n");
}
