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
#include <io.h>
#include <errno.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/win32.h>

static HANDLE dmi_pager_process = nullptr;

static void dmi_wait_pager_exit(void)
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

    wchar_t *pager = _wgetenv(L"PAGER");
    if ((pager == nullptr) or (pager[0] == L'\0'))
        return true;

    wchar_t pipe_name[64];
    static unsigned pipe_counter = 0;
    swprintf(pipe_name, countof(pipe_name), L"\\\\.\\pipe\\OPENDMI-%u-%u",
             GetCurrentProcessId(), ++pipe_counter);

    HANDLE pipe_write = CreateNamedPipeW(
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
        return false;
    }

    HANDLE pipe_read = CreateFileW(
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
        CloseHandle(pipe_write);
        return false;
    }

    PROCESS_INFORMATION process_info = {};
    STARTUPINFOW startup_info = {
        .cb = sizeof(startup_info),
        .dwFlags = STARTF_USESTDHANDLES,
        .hStdInput = pipe_read,
        .hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE),
        .hStdError = GetStdHandle(STD_ERROR_HANDLE),
    };

    BOOL success = CreateProcessW(
        nullptr,
        pager,
        nullptr,
        nullptr,
        TRUE,
        0,
        nullptr,
        nullptr,
        &startup_info,
        &process_info
    );

    if (not success) {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "CreateProcessW failed: %s", dmi_win32err_to_string(GetLastError()));
        CloseHandle(pipe_write);
        CloseHandle(pipe_read);
        return false;
    }

    CloseHandle(process_info.hThread);
    CloseHandle(pipe_read);

    do {
        int fd = _open_osfhandle((intptr_t)pipe_write, 0);
        if (fd < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to open pipe handle: %s", strerror(errno));
            break;
        }

        // Pipe handle is owned by the file descriptor now
        pipe_write = INVALID_HANDLE_VALUE;

        if (_dup2(fd, STDOUT_FILENO) < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to dup pipe handle: %s", strerror(errno));
            _close(fd);
            break;
        }
        _close(fd);
        dmi_pager_process = process_info.hProcess;
        atexit(dmi_wait_pager_exit);

        return true;
    } while (false);

    if (pipe_write != INVALID_HANDLE_VALUE)
        CloseHandle(pipe_write);
    CloseHandle(process_info.hProcess);

    return false;
}
