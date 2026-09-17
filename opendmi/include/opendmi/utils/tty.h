//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_TTY_H
#define OPENDMI_UTILS_TTY_H

#pragma once

#include <opendmi/defs.h>

/**
 * @brief Xterm system colors.
 */
typedef enum dmi_tty_color
{
    DMI_TTY_COLOR_NONE    = -1,
    DMI_TTY_COLOR_BLACK   = 0,
    DMI_TTY_COLOR_MAROON  = 1,
    DMI_TTY_COLOR_GREEN   = 2,
    DMI_TTY_COLOR_OLIVE   = 3,
    DMI_TTY_COLOR_NAVY    = 4,
    DMI_TTY_COLOR_PURPLE  = 5,
    DMI_TTY_COLOR_TEAL    = 6,
    DMI_TTY_COLOR_SILVER  = 7,
    DMI_TTY_COLOR_GREY    = 8,
    DMI_TTY_COLOR_RED     = 9,
    DMI_TTY_COLOR_LIME    = 10,
    DMI_TTY_COLOR_YELLOW  = 11,
    DMI_TTY_COLOR_BLUE    = 12,
    DMI_TTY_COLOR_FUCHSIA = 13,
    DMI_TTY_COLOR_AQUA    = 14,
    DMI_TTY_COLOR_WHITE   = 15
} dmi_tty_color_t;

typedef enum dmi_tty_attr
{
    DMI_TTY_ATTR_BOLD      = 1 << 0,
    DMI_TTY_ATTR_UNDERLINE = 1 << 1,
    DMI_TTY_ATTR_BLINK     = 1 << 2
} dmi_tty_attr_t;

__BEGIN_DECLS

void dmi_tty_init(void);
bool dmi_has_tty(void);

/**
 * @brief Check whether standard output was a terminal on initialization, i.e.
 * before it was possibly redirected to pager.
 */
bool dmi_tty_is_stdout(void);

void dmi_tty_attr_on(int attrs);
void dmi_tty_attr_off(int attrs);
void dmi_tty_set_fg_color(dmi_tty_color_t color);
void dmi_tty_set_bg_color(dmi_tty_color_t color);

void dmi_tty_cprintf(int color, const char *format, ...);
void dmi_tty_vcprintf(int color, const char *format, va_list args);

void dmi_tty_exit_attr_mode(void);

void dmi_tty_header(const char *format, ...);

__END_DECLS

#endif // !OPENDMI_UTILS_TTY_H
