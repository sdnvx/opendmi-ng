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
#include <assert.h>

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

//
// Process identifier of the pager, which is read and reset by the signal
// handlers, so it is kept in an atomic type for them.
//
static_assert(sizeof(pid_t) <= sizeof(sig_atomic_t), "pid_t does not fit into sig_atomic_t");

static volatile sig_atomic_t dmi_pager_pid = -1;

/**
 * @brief Signals, on which the process waits for pager before exiting.
 */
static const int dmi_pager_signals[] = { SIGINT, SIGQUIT, SIGTERM, SIGHUP };

/**
 * @internal
 * @brief Waits for the pager to exit, if it is running.
 *
 * @details Only async-signal-safe functions are used, so that it is called
 * from signal handlers as well.
 */
static void dmi_pager_wait(void);

/**
 * @internal
 * @brief Closes the standard output and waits for the pager at exit.
 */
static void dmi_pager_wait_exit(void);

/**
 * @internal
 * @brief Waits for the pager on a terminating signal, then raises the signal
 * again with its default action.
 *
 * @param[in] signo Number of the signal caught.
 */
static void dmi_pager_wait_signal(int signo);

/**
 * @internal
 * @brief Splits the pager command into words, the way the shell does,
 * without command substitution.
 *
 * @details Words are freed on failure. On success, they are to be freed with
 * `wordfree`(3).
 *
 * @param[in]  context Context to raise errors on.
 * @param[in]  pager   Pager command.
 * @param[out] we      Variable to store the words in.
 *
 * @error DMI_ERROR_SYSTEM Command is invalid, empty, or uses command substitution
 * @error DMI_ERROR_OUT_OF_MEMORY Words cannot be allocated
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_pager_expand(dmi_context_t *context, const char *pager, wordexp_t *we);

/**
 * @internal
 * @brief Prepares file actions, which connect the read end of the pipe to
 * the standard input of the pager and close both ends in the pager.
 *
 * @details Actions are destroyed on failure.
 *
 * @param[in]  context Context to raise errors on.
 * @param[out] actions Variable to initialize.
 * @param[in]  fds     Descriptors of the pipe.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Actions cannot be initialized
 * @error DMI_ERROR_SYSTEM Actions cannot be added
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_pager_actions_initialize(
        dmi_context_t              *context,
        posix_spawn_file_actions_t *actions,
        const int                   fds[2]);

/**
 * @internal
 * @brief Spawns the pager reading from a new pipe.
 *
 * @details Output is not paged if the default pager is not installed, which
 * is not an error: @p ppid is set to `-1` then.
 *
 * @param[in]  context    Context to raise errors on.
 * @param[in]  argv       Pager command split into words.
 * @param[in]  is_default Whether the command is the default pager.
 * @param[out] ppid       Variable to store the process identifier in.
 * @param[out] pfd        Variable to store the write end of the pipe in.
 *
 * @error DMI_ERROR_SYSTEM Pipe cannot be created, or the pager cannot be spawned
 * @error DMI_ERROR_OUT_OF_MEMORY File actions cannot be initialized
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_pager_spawn(
        dmi_context_t *context,
        char         **argv,
        bool           is_default,
        pid_t         *ppid,
        int           *pfd);

/**
 * @internal
 * @brief Redirects the standard output to the pipe of the pager.
 *
 * @details The descriptor is closed in any case. The pager is killed and
 * waited for on failure.
 *
 * @param[in] context Context to raise errors on.
 * @param[in] pid     Process identifier of the pager.
 * @param[in] fd      Write end of the pipe.
 *
 * @error DMI_ERROR_FILE_DUP_FAILED Standard output cannot be redirected
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_pager_attach(dmi_context_t *context, pid_t pid, int fd);

/**
 * @internal
 * @brief Installs handlers of the signals, on which the process waits for
 * the pager, and ignores `SIGPIPE`.
 */
static void dmi_pager_handle_signals(void);

bool dmi_pager_start(dmi_context_t *context)
{
    if (dmi_pager_pid > 0)
        return true;

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

    wordexp_t we = {};
    if (not dmi_pager_expand(context, pager, &we))
        return false;

    pid_t pid = -1;
    int   fd  = -1;
    bool  success = dmi_pager_spawn(context, we.we_wordv, is_default, &pid, &fd);

    wordfree(&we);

    // Default pager is not installed
    if ((not success) or (pid < 0))
        return success;

    if (not dmi_pager_attach(context, pid, fd))
        return false;

    dmi_pager_pid = pid;
    atexit(dmi_pager_wait_exit);

    dmi_pager_handle_signals();

    return true;
}

bool dmi_pager_has_quit(const FILE *stream, int error)
{
    assert(stream != nullptr);

    return (dmi_pager_pid > 0) and (stream == stdout) and (error == EPIPE);
}

static void dmi_pager_wait(void)
{
    pid_t pid = (pid_t)dmi_pager_pid;

    if (pid > 0) {
        int ret;
        do {
            ret = waitpid(pid, NULL, 0);
        } while (ret == -1 && errno == EINTR);
        dmi_pager_pid = -1;
    }
}

static void dmi_pager_wait_exit(void)
{
    fclose(stdout); // Ensure the pager process receives EOF
    dmi_pager_wait();
}

static void dmi_pager_wait_signal(int signo)
{
    //
    // Pager shares the terminal and usually ignores signals like SIGINT, so
    // the shell must not get the terminal back before the pager exits. Only
    // async-signal-safe functions are used here.
    //
    close(STDOUT_FILENO);
    dmi_pager_wait();

    signal(signo, SIG_DFL);
    raise(signo);
}

static bool dmi_pager_expand(dmi_context_t *context, const char *pager, wordexp_t *we)
{
    int rv = wordexp(pager, we, WRDE_NOCMD);

    switch (rv) {
    case 0:
        break;

    case WRDE_BADCHAR:
    case WRDE_BADVAL:
    case WRDE_SYNTAX:
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Invalid $PAGER value: '%s'", pager);
        return false;

    case WRDE_CMDSUB:
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Command substitution is not allowed in $PAGER");
        return false;

    case WRDE_NOSPACE:
        // Words may be partially expanded, so they are to be freed
        wordfree(we);
        return dmi_trace_out_of_memory(context);

    default:
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to expand $PAGER value: '%s'", pager);
        return false;
    }

    if (we->we_wordc == 0) {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Empty $PAGER value");
        wordfree(we);
        return false;
    }

    return true;
}

static bool dmi_pager_actions_initialize(
        dmi_context_t              *context,
        posix_spawn_file_actions_t *actions,
        const int                   fds[2])
{
    if (posix_spawn_file_actions_init(actions) != 0)
        return dmi_trace_out_of_memory(context);

    int rv = posix_spawn_file_actions_adddup2(actions, fds[STDIN_FILENO], STDIN_FILENO);
    if (rv == 0)
        rv = posix_spawn_file_actions_addclose(actions, fds[STDIN_FILENO]);
    if (rv == 0)
        rv = posix_spawn_file_actions_addclose(actions, fds[STDOUT_FILENO]);

    if (rv != 0) {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Failed to configure pager stdio: %s", strerror(rv));
        posix_spawn_file_actions_destroy(actions);
        return false;
    }

    return true;
}

static bool dmi_pager_spawn(
        dmi_context_t *context,
        char         **argv,
        bool           is_default,
        pid_t         *ppid,
        int           *pfd)
{
    posix_spawn_file_actions_t actions;
    int fds[2];

    if (pipe(fds) < 0) {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to create pipe: %s", strerror(errno));
        return false;
    }

    if (not dmi_pager_actions_initialize(context, &actions, fds)) {
        dmi_file_close(fds[STDIN_FILENO]);
        dmi_file_close(fds[STDOUT_FILENO]);
        return false;
    }

    pid_t pid = -1;
    int   rv  = posix_spawnp(&pid, argv[0], &actions, NULL, argv, environ);

    posix_spawn_file_actions_destroy(&actions);

    // Read end is of no use in this process, whatever the outcome
    dmi_file_close(fds[STDIN_FILENO]);

    if (rv != 0) {
        dmi_file_close(fds[STDOUT_FILENO]);

        // Output is not paged if default pager is not installed
        if ((rv == ENOENT) and is_default) {
            *ppid = -1;
            return true;
        }

        if (rv == ENOENT) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Pager executable not found: '%s'", argv[0]);
        } else {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to exec pager: %s", strerror(rv));
        }
        return false;
    }

    *ppid = pid;
    *pfd  = fds[STDOUT_FILENO];

    return true;
}

static bool dmi_pager_attach(dmi_context_t *context, pid_t pid, int fd)
{
    if (dup2(fd, STDOUT_FILENO) < 0) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_DUP_FAILED, "%s", strerror(errno));
        dmi_file_close(fd);
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        return false;
    }

    dmi_file_close(fd);

    return true;
}

static void dmi_pager_handle_signals(void)
{
    for (size_t i = 0; i < countof(dmi_pager_signals); i++) {
        struct sigaction action = {};

        // Signals ignored by the parent process (e.g. SIGHUP by nohup)
        // stay ignored
        if ((sigaction(dmi_pager_signals[i], nullptr, &action) == 0) and
            (action.sa_handler == SIG_IGN))
            continue;

        action = (struct sigaction){};
        action.sa_handler = dmi_pager_wait_signal;
        sigemptyset(&action.sa_mask);
        sigaction(dmi_pager_signals[i], &action, nullptr);
    }

    // Output fails with EPIPE rather than terminates the process if the
    // pager is quit before reading the whole output, so that it is told
    // apart from other failures and the process exits normally
    struct sigaction action = {};

    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    sigaction(SIGPIPE, &action, nullptr);
}
