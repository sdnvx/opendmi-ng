//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#include <wordexp.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <spawn.h>
#include <signal.h>
#include <errno.h>

#include <opendmi/context.h>
#include <opendmi/pager.h>
#include <opendmi/internal.h>
#include <opendmi/utils/file.h>

#ifndef environ
extern char **environ;
#endif

/**
 * @brief Pager, which is used if `PAGER` environment variable is not set.
 */
static const char *dmi_pager_default = "less";

/**
 * @brief Options for default pager: quit if output fits on one screen, show
 * colors and do not clear the screen. Used if `LESS` environment variable is
 * not set.
 */
static const char *dmi_pager_default_options = "FRX";

static pid_t dmi_pager_pid = -1;

/**
 * @brief Signals, on which the process waits for pager before exiting.
 */
static const int dmi_pager_signals[] = { SIGINT, SIGQUIT, SIGTERM, SIGHUP };

static void dmi_wait_pager(void)
{
    if (dmi_pager_pid > 0) {
        int ret;
        do {
            ret = waitpid(dmi_pager_pid, NULL, 0);
        } while (ret == -1 && errno == EINTR);
        dmi_pager_pid = -1;
    }
}

static void dmi_wait_pager_exit(void)
{
    fclose(stdout); // Ensure the pager process receives EOF
    dmi_wait_pager();
}

static void dmi_wait_pager_signal(int signo)
{
    //
    // Pager shares the terminal and usually ignores signals like SIGINT, so
    // the shell must not get the terminal back before the pager exits. Only
    // async-signal-safe functions are used here.
    //
    close(STDOUT_FILENO);
    dmi_wait_pager();

    signal(signo, SIG_DFL);
    raise(signo);
}

bool dmi_pager_start(dmi_context_t *context)
{
    if (dmi_pager_pid > 0)
        return true;

    bool success = false;
    wordexp_t we = {};
    int rv;
    int fds[2];

    // Empty value disables pager
    const char *pager = getenv("PAGER");
    bool is_default = (pager == nullptr);

    if (is_default)
        pager = dmi_pager_default;
    if (*pager == 0)
        return true;

    // Options are passed to the pager via environment, as git does
    if (setenv("LESS", dmi_pager_default_options, 0) < 0) {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to set pager options: %s", strerror(errno));
        return false;
    }

    rv = wordexp(pager, &we, WRDE_NOCMD);
    if (rv != 0) {
        switch (rv) {
        case WRDE_BADCHAR:
        case WRDE_BADVAL:
        case WRDE_SYNTAX:
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Invalid $PAGER value: '%s'", pager);
            break;

        case WRDE_CMDSUB:
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Command substitution is not allowed in $PAGER");
            break;

        case WRDE_NOSPACE:
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            break;
        }

        return false;
    }

    if (we.we_wordc == 0) {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Empty $PAGER value");
        wordfree(&we);
        return false;
    }

    do {
        if (pipe(fds) < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to create pipe: %s", strerror(errno));
            break;
        }

        posix_spawn_file_actions_t actions;
        if (posix_spawn_file_actions_init(&actions) != 0) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            dmi_file_close(fds[STDIN_FILENO]);
            dmi_file_close(fds[STDOUT_FILENO]);
            break;
        }

        int fa_rv = 0;
        fa_rv = posix_spawn_file_actions_adddup2(&actions, fds[STDIN_FILENO], STDIN_FILENO);
        if (fa_rv == 0)
            fa_rv = posix_spawn_file_actions_addclose(&actions, fds[STDIN_FILENO]);
        if (fa_rv == 0)
            fa_rv = posix_spawn_file_actions_addclose(&actions, fds[STDOUT_FILENO]);

        if (fa_rv != 0) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Failed to configure pager stdio: %s", strerror(fa_rv));
            posix_spawn_file_actions_destroy(&actions);
            dmi_file_close(fds[STDIN_FILENO]);
            dmi_file_close(fds[STDOUT_FILENO]);
            break;
        }

        int spawn_rv = posix_spawnp(&dmi_pager_pid, we.we_wordv[0], &actions, NULL, we.we_wordv, environ);
        posix_spawn_file_actions_destroy(&actions);

        if (spawn_rv != 0) {
            dmi_file_close(fds[STDIN_FILENO]);
            dmi_file_close(fds[STDOUT_FILENO]);
            dmi_pager_pid = -1;

            // Output is not paged if default pager is not installed
            if ((spawn_rv == ENOENT) and is_default) {
                success = true;
                break;
            }

            if (spawn_rv == ENOENT) {
                dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Pager executable not found: '%s'", we.we_wordv[0]);
            } else {
                dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to exec pager: %s", strerror(spawn_rv));
            }
            break;
        }

        if (dup2(fds[STDOUT_FILENO], STDOUT_FILENO) < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_DUP, "%s", strerror(errno));
            dmi_file_close(fds[STDIN_FILENO]);
            dmi_file_close(fds[STDOUT_FILENO]);
            kill(dmi_pager_pid, SIGKILL);
            waitpid(dmi_pager_pid, NULL, 0);
            dmi_pager_pid = -1;
            break;
        }

        dmi_file_close(fds[STDIN_FILENO]);
        dmi_file_close(fds[STDOUT_FILENO]);

        atexit(dmi_wait_pager_exit);

        for (size_t i = 0; i < countof(dmi_pager_signals); i++) {
            struct sigaction action = {};

            action.sa_handler = dmi_wait_pager_signal;
            sigemptyset(&action.sa_mask);
            sigaction(dmi_pager_signals[i], &action, nullptr);
        }

        success = true;
    } while (false);

    wordfree(&we);

    return success;
}
