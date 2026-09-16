//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef _WIN32
#   error "Unsupported OS type"
#endif // !_WIN32

#include <winternl.h>
#include <windows.h>
#include <assert.h>
#include <stdio.h>

#include <opendmi/context.h>
#include <opendmi/utils.h>
#include <opendmi/utils/win32.h>

#include <opendmi/backend/windows.h>

enum {
    SystemFirmwareTableInformation = 76,
};

typedef struct _RAW_SMBIOS_DATA
{
    uint8_t Used20CallingMethod;
    uint8_t SMBIOSMajorVersion;
    uint8_t SMBIOSMinorVersion;
    uint8_t DmiRevision;
    uint32_t Length;
    uint8_t SMBIOSTableData[];
} RAW_SMBIOS_DATA;

typedef enum _SYSTEM_FIRMWARE_TABLE_ACTION
{
    SystemFirmwareTableEnumerate,
    SystemFirmwareTableGet,
    SystemFirmwareTableMax
} SYSTEM_FIRMWARE_TABLE_ACTION;

typedef struct _SYSTEM_FIRMWARE_TABLE_INFORMATION
{
    ULONG ProviderSignature; // (same as the GetSystemFirmwareTable function)
    SYSTEM_FIRMWARE_TABLE_ACTION Action;
    ULONG TableID;
    ULONG TableBufferLength;
    _Field_size_bytes_(TableBufferLength) UCHAR TableBuffer[];
} SYSTEM_FIRMWARE_TABLE_INFORMATION, *PSYSTEM_FIRMWARE_TABLE_INFORMATION;

static bool dmi_windows_open(dmi_context_t *context, const char *path);
static dmi_data_t *dmi_windows_read_entry(dmi_context_t *context, size_t *plength);
static dmi_data_t *dmi_windows_read_table(dmi_context_t *context, size_t *plength);
static bool dmi_windows_close(dmi_context_t *context);

dmi_backend_t dmi_windows_backend =
{
    .name       = "Windows SystemFirmwareTableInformation",
    .open       = dmi_windows_open,
    .read_entry = dmi_windows_read_entry,
    .read_table = dmi_windows_read_table,
    .close      = dmi_windows_close
};

static bool dmi_windows_open(dmi_context_t *context, const char *path)
{
    dmi_unused(path);

    SYSTEM_FIRMWARE_TABLE_INFORMATION sfti = {
        .ProviderSignature = 'RSMB',
        .Action = SystemFirmwareTableGet,
    };
    ULONG size = 0;
    NTSTATUS status = NtQuerySystemInformation(SystemFirmwareTableInformation, &sfti, sizeof(sfti), &size);
    if (size == 0) {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to query SMBIOS table size: %s", dmi_ntstatus_to_string(status));
        return false;
    }

    SYSTEM_FIRMWARE_TABLE_INFORMATION *session = dmi_alloc(context, size);
    if (!session) {
        return false;
    }
    *session = sfti;

    status = NtQuerySystemInformation(SystemFirmwareTableInformation, session, size, &size);
    if (!NT_SUCCESS(status))
    {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to query SMBIOS table: %s", dmi_ntstatus_to_string(status));
        dmi_free(session);
        return false;
    }

    context->state.session = session;

    return true;
}

static dmi_data_t *dmi_windows_read_entry(dmi_context_t *context, size_t *plength)
{
    SYSTEM_FIRMWARE_TABLE_INFORMATION *session = dmi_cast(session, context->state.session);
    RAW_SMBIOS_DATA *data = dmi_cast(data, session->TableBuffer);

    context->state.smbios_version = dmi_version(data->SMBIOSMajorVersion, data->SMBIOSMinorVersion, data->DmiRevision);
    *plength = 0; // Windows does not provide individual entry access

    return nullptr;
}

static dmi_data_t *dmi_windows_read_table(dmi_context_t *context, size_t *plength)
{
    SYSTEM_FIRMWARE_TABLE_INFORMATION *session = dmi_cast(session, context->state.session);
    RAW_SMBIOS_DATA *data = dmi_cast(data, session->TableBuffer);

    *plength = data->Length;
    return data->SMBIOSTableData;
}

static bool dmi_windows_close(dmi_context_t *context)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);

    dmi_free(context->state.session);

    return true;
}
