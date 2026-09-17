//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include "../config.h"

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
#include <opendmi/utils/file.h>

#ifndef environ
extern char **environ;
#endif

static pid_t pid = -1;

/**
 * @brief Signals, on which the process waits for pager before exiting.
 */
static const int dmi_pager_signals[] = { SIGINT, SIGQUIT, SIGTERM, SIGHUP };

static void dmi_wait_pager(void)
{
    if (pid > 0) {
        int ret;
        do {
            ret = waitpid(pid, NULL, 0);
        } while (ret == -1 && errno == EINTR);
        pid = -1;
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
    if (pid > 0)
        return true;

    bool success = false;
    wordexp_t we = {};
    int rv;
    int fds[2];

    const char *pager = getenv("PAGER");
    if ((pager == nullptr) or (*pager == 0))
        return true;

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

        int spawn_rv = posix_spawnp(&pid, we.we_wordv[0], &actions, NULL, we.we_wordv, environ);
        posix_spawn_file_actions_destroy(&actions);

        if (spawn_rv != 0) {
            if (spawn_rv == ENOENT) {
                dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Pager executable not found: '%s'", we.we_wordv[0]);
            } else {
                dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to exec pager: %s", strerror(spawn_rv));
            }
            dmi_file_close(fds[STDIN_FILENO]);
            dmi_file_close(fds[STDOUT_FILENO]);
            break;
        }

        if (dup2(fds[STDOUT_FILENO], STDOUT_FILENO) < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_DUP, "%s", strerror(errno));
            dmi_file_close(fds[STDIN_FILENO]);
            dmi_file_close(fds[STDOUT_FILENO]);
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            pid = -1;
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
