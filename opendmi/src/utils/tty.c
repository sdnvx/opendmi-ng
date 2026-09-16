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

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#   define NOMINMAX
#   include <windows.h>
#endif

#include <stdio.h>
#include <stdarg.h>

#include <opendmi/utils/tty.h>

static bool dmi_tty = false;
static int  dmi_tty_bg_color = DMI_TTY_COLOR_BLACK;
static int  dmi_tty_fg_color = DMI_TTY_COLOR_WHITE;
static int  dmi_tty_attrs = 0;

static int dmi_tty_ansi_fg_color(dmi_tty_color_t color)
{
    switch (color) {
        case DMI_TTY_COLOR_BLACK: return 30;
        case DMI_TTY_COLOR_MAROON: return 31;
        case DMI_TTY_COLOR_GREEN: return 32;
        case DMI_TTY_COLOR_OLIVE: return 33;
        case DMI_TTY_COLOR_NAVY: return 34;
        case DMI_TTY_COLOR_PURPLE: return 35;
        case DMI_TTY_COLOR_TEAL: return 36;
        case DMI_TTY_COLOR_SILVER: return 37;
        case DMI_TTY_COLOR_GREY: return 90;
        case DMI_TTY_COLOR_RED: return 91;
        case DMI_TTY_COLOR_LIME: return 92;
        case DMI_TTY_COLOR_YELLOW: return 93;
        case DMI_TTY_COLOR_BLUE: return 94;
        case DMI_TTY_COLOR_FUCHSIA: return 95;
        case DMI_TTY_COLOR_AQUA: return 96;
        case DMI_TTY_COLOR_WHITE: return 97;
        case DMI_TTY_COLOR_NONE:
        default: return 39;
    }
}

static int dmi_tty_ansi_bg_color(dmi_tty_color_t color)
{
    switch (color) {
        case DMI_TTY_COLOR_BLACK: return 40;
        case DMI_TTY_COLOR_MAROON: return 41;
        case DMI_TTY_COLOR_GREEN: return 42;
        case DMI_TTY_COLOR_OLIVE: return 43;
        case DMI_TTY_COLOR_NAVY: return 44;
        case DMI_TTY_COLOR_PURPLE: return 45;
        case DMI_TTY_COLOR_TEAL: return 46;
        case DMI_TTY_COLOR_SILVER: return 47;
        case DMI_TTY_COLOR_GREY: return 100;
        case DMI_TTY_COLOR_RED: return 101;
        case DMI_TTY_COLOR_LIME: return 102;
        case DMI_TTY_COLOR_YELLOW: return 103;
        case DMI_TTY_COLOR_BLUE: return 104;
        case DMI_TTY_COLOR_FUCHSIA: return 105;
        case DMI_TTY_COLOR_AQUA: return 106;
        case DMI_TTY_COLOR_WHITE: return 107;
        case DMI_TTY_COLOR_NONE:
        default: return 49;
    }
}

void dmi_tty_init(void)
{
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;

    if (handle != INVALID_HANDLE_VALUE and GetConsoleMode(handle, &mode)) {
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        dmi_tty = SetConsoleMode(handle, mode) != 0;
    }
#else
    dmi_tty = isatty(STDOUT_FILENO) != 0;
#endif
}

bool dmi_has_tty(void)
{
    return dmi_tty;
}

void dmi_tty_attr_on(int attrs)
{
    if (not dmi_tty)
        return;

    dmi_tty_attrs |= attrs;

    if (attrs & DMI_TTY_ATTR_BOLD)
        fputs("\x1b[1m", stdout);
    if (attrs & DMI_TTY_ATTR_UNDERLINE)
        fputs("\x1b[4m", stdout);
    if (attrs & DMI_TTY_ATTR_BLINK)
        fputs("\x1b[5m", stdout);
}

void dmi_tty_attr_off(int attrs)
{
    if (not dmi_tty)
        return;

    dmi_tty_attrs &= ~attrs;

    dmi_tty_exit_attr_mode();

    dmi_tty_attr_on(dmi_tty_attrs);
    dmi_tty_set_bg_color((dmi_tty_color_t)dmi_tty_bg_color);
    dmi_tty_set_fg_color((dmi_tty_color_t)dmi_tty_fg_color);
}

void dmi_tty_set_fg_color(dmi_tty_color_t color)
{
    if (not dmi_tty)
        return;

    dmi_tty_fg_color = (int)color;

    printf("\x1b[%dm", dmi_tty_ansi_fg_color(color));
}

void dmi_tty_set_bg_color(dmi_tty_color_t color)
{
    if (not dmi_tty)
        return;

    dmi_tty_bg_color = (int)color;

    printf("\x1b[%dm", dmi_tty_ansi_bg_color(color));
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
    if (dmi_tty)
        fputs("\x1b[m", stdout);
}

void dmi_tty_header(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    dmi_tty_attr_on(DMI_TTY_ATTR_BOLD);
    dmi_tty_vcprintf(DMI_TTY_COLOR_WHITE, format, args);
    dmi_tty_exit_attr_mode();
    va_end(args);

    fputs("\n\n", stdout);
}
