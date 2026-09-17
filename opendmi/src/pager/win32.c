//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include "../config.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <errno.h>

#include <opendmi/context.h>
#include <opendmi/utils.h>
#include <opendmi/utils/win32.h>

static HANDLE child_process_handle = NULL;

static void dmi_wait_pager_exit(void)
{
    fclose(stdout); // Ensure the pager process receives EOF

    if (child_process_handle) {
        WaitForSingleObject(child_process_handle, INFINITE);
        CloseHandle(child_process_handle);
        child_process_handle = NULL;
    }
}

bool dmi_pager_start(dmi_context_t *context)
{
    if (child_process_handle != NULL)
        return true;

    wchar_t *pager = _wgetenv(L"PAGER");
    if (pager == nullptr || pager[0] == L'\0')
        return true;

    wchar_t pipeName[64];
    static unsigned pidCounter = 0;
    swprintf(pipeName, sizeof(pipeName) / sizeof(*pipeName), L"\\\\.\\pipe\\OPENDMI-%u-%u", GetCurrentProcessId(), ++pidCounter);

    HANDLE hChildPipeWrite = CreateNamedPipeW(
        pipeName,
        PIPE_ACCESS_OUTBOUND | FILE_FLAG_FIRST_PIPE_INSTANCE,
        0,
        1,
        8192,
        8192,
        0,
        NULL
    );
    if (hChildPipeWrite == INVALID_HANDLE_VALUE)
    {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "CreateNamedPipeW failed: %s", dmi_win32err_to_string(GetLastError()));
        return false;
    }

    HANDLE hChildPipeRead = CreateFileW(
        pipeName,
        GENERIC_READ,
        0,
        &(SECURITY_ATTRIBUTES){
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = NULL,
            .bInheritHandle = TRUE,
        },
        OPEN_EXISTING,
        0,
        NULL
    );
    if (hChildPipeRead == INVALID_HANDLE_VALUE)
    {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "CreateFileW failed: %s", dmi_win32err_to_string(GetLastError()));
        CloseHandle(hChildPipeWrite);
        return false;
    }

    PROCESS_INFORMATION piProcInfo = {};
    STARTUPINFOW siStartInfo = {
        .cb = sizeof(siStartInfo),
        .dwFlags = STARTF_USESTDHANDLES,
        .hStdInput = hChildPipeRead,
        .hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE),
        .hStdError = GetStdHandle(STD_ERROR_HANDLE),
    };

    BOOL success = CreateProcessW(
        NULL,
        pager,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &siStartInfo,
        &piProcInfo
    );

    if (!success)
    {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "CreateProcessW failed: %s", dmi_win32err_to_string(GetLastError()));
        CloseHandle(hChildPipeWrite);
        CloseHandle(hChildPipeRead);
        return false;
    }

    CloseHandle(piProcInfo.hThread);
    CloseHandle(hChildPipeRead);

    do {
        int newStdout = _open_osfhandle((intptr_t)hChildPipeWrite, 0);
        if (newStdout < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to open pipe handle: %s", strerror(errno));
            break;
        }

        // Pipe handle is owned by the file descriptor now
        hChildPipeWrite = INVALID_HANDLE_VALUE;

        if (_dup2(newStdout, STDOUT_FILENO) < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to dup pipe handle: %s", strerror(errno));
            _close(newStdout);
            break;
        }
        _close(newStdout);
        child_process_handle = piProcInfo.hProcess;
        atexit(dmi_wait_pager_exit);

        return true;
    } while (false);

    if (hChildPipeWrite != INVALID_HANDLE_VALUE)
        CloseHandle(hChildPipeWrite);
    CloseHandle(piProcInfo.hProcess);

    return false;
}
