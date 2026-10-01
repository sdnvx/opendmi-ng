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
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/win32.h>

#include <opendmi/backend/windows.h>

enum {
    SystemFirmwareTableInformation = 76,
};

/**
 * @internal
 * @brief Status of a query with a buffer too small for the data, which tells
 * the size needed.
 *
 * @details It comes from `<ntstatus.h>`, which clashes with `<windows.h>`.
 */
#ifndef STATUS_BUFFER_TOO_SMALL
#   define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
#endif

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

/**
 * @internal
 * @brief Query the SMBIOS table from the system and keep it as the session.
 *
 * @details The table is queried twice: first to learn its size, then to
 * read it into a buffer of that size. The lengths reported by the system and
 * by the firmware are validated against the data actually returned.
 *
 * @param[in] context Context being opened.
 * @param[in] path    Path to the data source, unused.
 *
 * @error DMI_ERROR_SYSTEM Table cannot be queried, or its length is invalid
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_windows_open(dmi_context_t *context, const char *path);

/**
 * @internal
 * @brief Copy the SMBIOS structure table queried on opening into @p buffer.
 *
 * @param[in]  context Context being opened.
 * @param[out] buffer  Buffer to read the table into.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_windows_read_table(dmi_context_t *context, dmi_buffer_t *buffer);

/**
 * @internal
 * @brief Release the SMBIOS table queried on opening.
 *
 * @param[in] context Context being closed.
 *
 * @return Always `true`.
 */
static bool dmi_windows_close(dmi_context_t *context);

dmi_backend_t dmi_windows_backend =
{
    .name       = "Windows SystemFirmwareTableInformation",
    .open       = dmi_windows_open,
    .read_entry = nullptr, // Windows does not provide entry point data
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
    if ((!NT_SUCCESS(status) && (status != STATUS_BUFFER_TOO_SMALL)) || (size <= sizeof(sfti))) {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to query SMBIOS table size: %s", dmi_ntstatus_to_string(status));
        return false;
    }

    SYSTEM_FIRMWARE_TABLE_INFORMATION *session = dmi_alloc(context, size);
    if (!session) {
        return false;
    }
    *session = sfti;

    const ULONG allocated = size;

    status = NtQuerySystemInformation(SystemFirmwareTableInformation, session, size, &size);
    if (!NT_SUCCESS(status))
    {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Unable to query SMBIOS table: %s", dmi_ntstatus_to_string(status));
        dmi_free(session);
        return false;
    }

    RAW_SMBIOS_DATA *data = dmi_cast(data, session->TableBuffer);

    // Lengths reported by the system and by firmware are checked against the
    // data actually returned, so that the table is not read beyond it
    const size_t header = offsetof(SYSTEM_FIRMWARE_TABLE_INFORMATION, TableBuffer);
    if ((size > allocated) || (size < header) ||
        (session->TableBufferLength > size - header) ||
        (session->TableBufferLength < sizeof(RAW_SMBIOS_DATA)) ||
        (data->Length > session->TableBufferLength - sizeof(RAW_SMBIOS_DATA)))
    {
        dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "Invalid SMBIOS table length (%lu bytes returned)",
                           (unsigned long)size);
        dmi_free(session);
        return false;
    }

    context->state.smbios_version = dmi_version(data->SMBIOSMajorVersion, data->SMBIOSMinorVersion, data->DmiRevision);
    context->state.session        = session;

    return true;
}

static bool dmi_windows_read_table(dmi_context_t *context, dmi_buffer_t *buffer)
{
    SYSTEM_FIRMWARE_TABLE_INFORMATION *session = dmi_cast(session, context->state.session);
    RAW_SMBIOS_DATA *data = dmi_cast(data, session->TableBuffer);

    return dmi_buffer_assign(buffer, data->SMBIOSTableData, data->Length);
}

static bool dmi_windows_close(dmi_context_t *context)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);

    dmi_free(context->state.session);

    return true;
}
