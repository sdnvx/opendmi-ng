//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include "config.h"

#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <libgen.h>
#include <getopt.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/command.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils/file.h>
#include <opendmi/utils/tty.h>

#ifdef _WIN32
static inline struct tm *localtime_r(const time_t *timep, struct tm *result)
{
    errno_t err = localtime_s(result, timep);
    if (err != 0)
        return nullptr;
    return result;
}
#endif

static void dmi_show_version(void);
static void dmi_show_usage(void);

static bool dmi_log_init(dmi_context_t *context);
static void dmi_log_close(void);

static void dmi_log_handler(
        dmi_log_t       *target,
        dmi_log_level_t  level,
        const char      *format,
        va_list          args);

static void dmi_log_tty_handler(
        dmi_log_level_t  level,
        const char      *format,
        va_list          args);
static void dmi_log_file_handler(
        dmi_log_level_t  level,
        const char      *format,
        va_list          args);

static off_t dmi_log_file_lock(void);
static void dmi_log_file_unlock(off_t start);

static FILE     *log_file   = nullptr;
static bool      log_lock   = true;
static dmi_log_t log_target = { dmi_log_handler };

int main(int argc, char *argv[])
{
    int rv;
    dmi_context_t *context;
    const dmi_option_set_t *options[] = { &dmi_global_options, nullptr };
    const dmi_command_t *command;

    // Initialize command environment
    dmi_command_init(basename(argv[0]));
    argc--, argv++;

    // Create DMI context
    context = dmi_create(DMI_CONTEXT_FLAG_LINK | DMI_CONTEXT_FLAG_RELAXED);
    if (context == nullptr) {
        fprintf(stderr, "Unable to create DMI context\n");
        return EXIT_FAILURE;
    }

    rv = EXIT_FAILURE;
    do {
        int nopts;

        // Parse global options
        nopts = dmi_option_parse(context, options, argc, argv);
        if (nopts < 0) {
            rv = EXIT_USAGE;
            break;
        }
        argc -= nopts, argv += nopts;

        if (dmi_global_config.show_version) {
            rv = EXIT_SUCCESS;
            dmi_show_version();
            break;
        }
        if (dmi_global_config.show_usage) {
            rv = EXIT_SUCCESS;
            dmi_show_usage();
            break;
        }

        if (argc == 0) {
            rv = EXIT_USAGE;
            dmi_command_message("Missing command");
            break;
        }

        // Find command
        command = dmi_command_find(argv[0]);
        if (command == nullptr) {
            rv = EXIT_USAGE;
            dmi_command_message("Invalid command: %s", argv[0]);
            break;
        }
        argc--, argv++;

        // Initialize logging
        if (not dmi_log_init(context))
            break;

        // Execute command
        rv = dmi_command_run(command, context, argc, argv);
    } while (false);

    // Close DMI context
    dmi_destroy(context);

    return rv;
}

static void dmi_show_version(void)
{
    dmi_command_banner();
}

static void dmi_show_usage(void)
{
    dmi_command_usage(nullptr);
}

static bool dmi_log_init(dmi_context_t *context)
{
    assert(context != nullptr);

    if (not dmi_global_config.log_enable)
        return true;

    // Open log file before enabling the logger, so that nothing is logged to
    // the terminal instead of the file on errors
    if (dmi_global_config.log_path != nullptr) {
        log_file = fopen(dmi_global_config.log_path, "a");
        if (log_file == nullptr) {
            dmi_command_message("Unable to open log file: %s: %s",
                                dmi_global_config.log_path, strerror(errno));
            return false;
        }

        atexit(dmi_log_close);
    }

    dmi_set_logger(context, &log_target);
    dmi_set_log_level(context, dmi_global_config.log_level);

    return true;
}

static void dmi_log_close(void)
{
    if (log_file != nullptr)
        fclose(log_file);
}

static void dmi_log_handler(
        dmi_log_t       *target,
        dmi_log_level_t  level,
        const char      *format,
        va_list          args)
{
    dmi_unused(target);
    assert(level >= 0);
    assert(format != nullptr);

    if (log_file != nullptr)
        dmi_log_file_handler(level, format, args);
    else
        dmi_log_tty_handler(level, format, args);
}

static void dmi_log_tty_handler(
        dmi_log_level_t  level,
        const char      *format,
        va_list          args)
{
    FILE *out = stderr;

    assert(level >= 0);
    assert(format != nullptr);

    if (dmi_has_tty())
        out = stdout;

    switch (level) {
    case DMI_LOG_DEBUG:
        dmi_tty_set_fg_color(8);
        break;

    case DMI_LOG_INFO:
        dmi_tty_set_fg_color(DMI_TTY_COLOR_GREEN);
        break;

    case DMI_LOG_NOTICE:
        dmi_tty_set_fg_color(DMI_TTY_COLOR_TEAL);
        break;

    case DMI_LOG_WARNING:
        dmi_tty_set_fg_color(DMI_TTY_COLOR_YELLOW);
        break;

    case DMI_LOG_ERROR:
        dmi_tty_set_fg_color(DMI_TTY_COLOR_RED);
        break;

    default:
        // fallthrough
    }

    fprintf(out, "%s: ", dmi_log_level_name(level));
    dmi_tty_exit_attr_mode();

    vfprintf(out, format, args);
    fprintf(out, "\n");
}

static void dmi_log_file_handler(
        dmi_log_level_t  level,
        const char      *format,
        va_list          args)
{
    time_t now;
    struct tm now_tm;
    off_t pos;

    assert(level >= 0);
    assert(format != nullptr);

    time(&now);
    localtime_r(&now, &now_tm);

    pos = dmi_log_file_lock();

    fprintf(log_file, "[%04d-%02d-%02d %02d:%02d:%02d] %s: ",
            now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday,
            now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec,
            dmi_log_level_name(level));
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");

    fflush(log_file);
    dmi_log_file_unlock(pos);
}

static off_t dmi_log_file_lock(void)
{
    int fd = fileno(log_file);
    off_t start = -1;

    if (not log_lock)
        return -1;

    log_lock = false;

    do {
        if (not dmi_file_lock(fd, 0)) {
            dmi_command_message("Unable to lock log file: %s", strerror(errno));
            break;
        }

        start = dmi_file_tell(fd);
        if (start < 0) {
            dmi_command_message("Unable to get log position: %s", strerror(errno));
            break;
        }

        log_lock = true;
    } while (false);

    return start;
}

static void dmi_log_file_unlock(off_t start)
{
    if (not(log_lock) or (start < 0))
        return;

    log_lock = false;

    do {
        int fd = fileno(log_file);

        off_t end = dmi_file_tell(fd);
        if (end < 0) {
            dmi_command_message("Unable to get log position: %s", strerror(errno));
            break;
        }

        if (not dmi_file_unlock(fd, start - end)) {
            dmi_command_message("Unable to unlock log file: %s", strerror(errno));
            break;
        }

        log_lock = true;
    } while (false);
}
