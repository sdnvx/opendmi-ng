//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/win32.h>
#include <opendmi/pager.h>

static HANDLE dmi_pager_process = nullptr;

static void dmi_pager_wait_exit(void)
{
    fclose(stdout); // Ensure the pager process receives EOF

    if (dmi_pager_process != nullptr) {
        WaitForSingleObject(dmi_pager_process, INFINITE);
        CloseHandle(dmi_pager_process);
        dmi_pager_process = nullptr;
    }
}

bool dmi_pager_start(dmi_context_t *context)
{
    if (dmi_pager_process != nullptr)
        return true;

    const wchar_t *value = _wgetenv(L"PAGER");
    if ((value == nullptr) or (value[0] == L'\0'))
        return true;

    wchar_t *pager = nullptr;
    HANDLE pipe_write = INVALID_HANDLE_VALUE;
    HANDLE pipe_read  = INVALID_HANDLE_VALUE;
    PROCESS_INFORMATION process_info = {};
    int fd = -1;
    bool success = false;

    do {
        // Command line may be modified by CreateProcessW(), so the environment
        // value is copied rather than passed as it is
        pager = _wcsdup(value);
        if (pager == nullptr) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            break;
        }

        wchar_t pipe_name[64];
        static unsigned pipe_counter = 0;
        swprintf(pipe_name, countof(pipe_name), L"\\\\.\\pipe\\OPENDMI-%u-%u",
                 GetCurrentProcessId(), ++pipe_counter);

        pipe_write = CreateNamedPipeW(
            pipe_name,
            PIPE_ACCESS_OUTBOUND | FILE_FLAG_FIRST_PIPE_INSTANCE,
            0,
            1,
            8192,
            8192,
            0,
            nullptr
        );
        if (pipe_write == INVALID_HANDLE_VALUE) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "CreateNamedPipeW failed: %s", dmi_win32err_to_string(GetLastError()));
            break;
        }

        pipe_read = CreateFileW(
            pipe_name,
            GENERIC_READ,
            0,
            &(SECURITY_ATTRIBUTES){
                .nLength = sizeof(SECURITY_ATTRIBUTES),
                .lpSecurityDescriptor = nullptr,
                .bInheritHandle = TRUE,
            },
            OPEN_EXISTING,
            0,
            nullptr
        );
        if (pipe_read == INVALID_HANDLE_VALUE) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "CreateFileW failed: %s", dmi_win32err_to_string(GetLastError()));
            break;
        }

        STARTUPINFOW startup_info = {
            .cb = sizeof(startup_info),
            .dwFlags = STARTF_USESTDHANDLES,
            .hStdInput = pipe_read,
            .hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE),
            .hStdError = GetStdHandle(STD_ERROR_HANDLE),
        };

        if (not CreateProcessW(nullptr, pager, nullptr, nullptr, TRUE, 0, nullptr, nullptr,
                               &startup_info, &process_info)) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "CreateProcessW failed: %s", dmi_win32err_to_string(GetLastError()));
            break;
        }

        fd = _open_osfhandle((intptr_t)pipe_write, 0);
        if (fd < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to open pipe handle: %s", strerror(errno));
            break;
        }

        // Pipe handle is owned by the file descriptor now
        pipe_write = INVALID_HANDLE_VALUE;

        if (_dup2(fd, STDOUT_FILENO) < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to dup pipe handle: %s", strerror(errno));
            break;
        }

        // Process handle is kept to wait for the pager on exit
        dmi_pager_process     = process_info.hProcess;
        process_info.hProcess = nullptr;

        atexit(dmi_pager_wait_exit);

        success = true;
    } while (false);

    // Standard output holds a duplicate of the pipe, and the pager holds the
    // inherited read end, so the handles of their own are closed in any case.
    // If the pager has been started but cannot be fed, closing the pipe gives
    // it the end of input, so that it exits.
    if (fd >= 0)
        _close(fd);
    if (pipe_write != INVALID_HANDLE_VALUE)
        CloseHandle(pipe_write);
    if (pipe_read != INVALID_HANDLE_VALUE)
        CloseHandle(pipe_read);
    if (process_info.hThread != nullptr)
        CloseHandle(process_info.hThread);
    if (process_info.hProcess != nullptr)
        CloseHandle(process_info.hProcess);

    free(pager);

    return success;
}

bool dmi_pager_has_quit(const FILE *stream, int error)
{
    assert(stream != nullptr);

    // Writes to the pipe, which is closed by the pager, fail with EPIPE
    return (dmi_pager_process != nullptr) and (stream == stdout) and (error == EPIPE);
}
