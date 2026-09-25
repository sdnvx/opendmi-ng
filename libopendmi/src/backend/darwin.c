//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef __APPLE__
#   error "Unsupported OS type"
#endif // !__APPLE__

#include <errno.h>
#include <assert.h>

#include <IOKit/IOKitLib.h>

#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include <opendmi/backend/darwin.h>

typedef struct dmi_darwin_session dmi_darwin_session_t;

struct dmi_darwin_session
{
    io_service_t service;
};

static bool dmi_darwin_open(dmi_context_t *context, const char *path);
static bool dmi_darwin_read_entry(dmi_context_t *context, dmi_buffer_t *buffer);
static bool dmi_darwin_read_table(dmi_context_t *context, dmi_buffer_t *buffer);
static bool dmi_darwin_close(dmi_context_t *context);

static bool dmi_darwin_read_data(dmi_context_t *context, CFStringRef key, dmi_buffer_t *buffer);

dmi_backend_t dmi_darwin_backend =
{
    .name       = "Apple SMBIOS service",
    .open       = dmi_darwin_open,
    .read_entry = dmi_darwin_read_entry,
    .read_table = dmi_darwin_read_table,
    .close      = dmi_darwin_close
};

static bool dmi_darwin_open(dmi_context_t *context, const char *path)
{
    dmi_darwin_session_t *session = nullptr;

    assert(context != nullptr);
    assert(context->state.session == nullptr);

    dmi_unused(path);

    // Allocate backend descriptor
    session = dmi_alloc(context, sizeof(dmi_darwin_session_t));
    if (session == nullptr)
        return false;

    // Establish SMBIOS service connection
    bool success = false;
    do {
        // Connect to SMBIOS service
        session->service = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("AppleSMBIOS"));
        if (session->service == MACH_PORT_NULL) {
            dmi_error_raise_ex(context, DMI_ERROR_SERVICE_UNAVAILABLE, "AppleSMBIOS");
            break;
        }

        success = true;
    } while (false);

    if (not success) {
        dmi_free(session);
        return false;
    }

    context->state.session = session;

    return true;
}

static bool dmi_darwin_read_entry(dmi_context_t *context, dmi_buffer_t *buffer)
{
    return dmi_darwin_read_data(context, CFSTR("SMBIOS-EPS"), buffer);
}

static bool dmi_darwin_read_table(dmi_context_t *context, dmi_buffer_t *buffer)
{
    return dmi_darwin_read_data(context, CFSTR("SMBIOS"), buffer);
}

static bool dmi_darwin_close(dmi_context_t *context)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);

    dmi_darwin_session_t *session = dmi_cast(session, context->state.session);

    if (session->service != MACH_PORT_NULL)
        IOObjectRelease(session->service);

    dmi_free(session);

    return true;
}

static bool dmi_darwin_read_data(dmi_context_t *context, CFStringRef key, dmi_buffer_t *buffer)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(buffer != nullptr);

    dmi_darwin_session_t *session = dmi_cast(session, context->state.session);

    bool      success = false;
    CFDataRef ref     = nullptr;

    do {
        ref = (CFDataRef)IORegistryEntryCreateCFProperty(session->service, key,
                                                         kCFAllocatorDefault, kNilOptions);
        if (ref == NULL) {
            dmi_error_raise(context, DMI_ERROR_INTERNAL);
            return false;
        }

        size_t length = CFDataGetLength(ref);

        // Data is copied into the buffer of the context, which owns it, so
        // that the service is of no interest once it has been read
        if (not dmi_buffer_resize(buffer, length))
            break;

        CFDataGetBytes(ref, CFRangeMake(0, (CFIndex)length), (UInt8 *)buffer->data);

        success = true;
    } while (false);

    if (ref != nullptr)
        CFRelease(ref);

    return success;
}
