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

#include <opendmi/backend/darwin.h>

typedef struct dmi_darwin_session dmi_darwin_session_t;

struct dmi_darwin_session
{
    io_service_t service;
};

static bool dmi_darwin_open(dmi_context_t *context, const char *path);
static dmi_data_t *dmi_darwin_read_entry(dmi_context_t *context, size_t *plength);
static dmi_data_t *dmi_darwin_read_table(dmi_context_t *context, size_t *plength);
static bool dmi_darwin_close(dmi_context_t *context);

static dmi_data_t *dmi_darwin_read_data(dmi_context_t *context, CFStringRef key, size_t *plength);

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

static dmi_data_t *dmi_darwin_read_entry(dmi_context_t *context, size_t *plength)
{
    return dmi_darwin_read_data(context, CFSTR("SMBIOS-EPS"), plength);
}

static dmi_data_t *dmi_darwin_read_table(dmi_context_t *context, size_t *plength)
{
    return dmi_darwin_read_data(context, CFSTR("SMBIOS"), plength);
}

static bool dmi_darwin_close(dmi_context_t *context)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);

    dmi_darwin_session_t *session = dmi_cast(session, context->state.session);

    if (session->service != MACH_PORT_NULL)
        IOObjectRelease(session->service);

    dmi_free(context->state.table_data);
    dmi_free(context->state.entry_data);
    dmi_free(session);

    return true;
}

static dmi_data_t *dmi_darwin_read_data(dmi_context_t *context, CFStringRef key, size_t *plength)
{
    assert(context != nullptr);
    assert(context->state.session != nullptr);
    assert(plength != nullptr);

    dmi_darwin_session_t *session = dmi_cast(session, context->state.session);

    bool        success = false;
    CFDataRef   ref     = nullptr;
    dmi_data_t *data    = nullptr;
    size_t      length  = 0;

    do {
        ref = (CFDataRef)IORegistryEntryCreateCFProperty(session->service, key,
                                                         kCFAllocatorDefault, kNilOptions);
        if (ref == NULL) {
            dmi_error_raise(context, DMI_ERROR_INTERNAL);
            return nullptr;
        }

        length = CFDataGetLength(ref);

        data = dmi_alloc(context, length);
		if (data == nullptr)
			break;

        CFDataGetBytes(ref, CFRangeMake(0, length), (UInt8 *)data);

        success = true;
    } while (false);

    if (ref != nullptr)
        CFRelease(ref);

    if (not success) {
        dmi_free(data);
        return nullptr;
    }

    *plength = length;

    return data;
}
