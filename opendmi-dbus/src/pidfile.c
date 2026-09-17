//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#include <opendmi/log.h>
#include <opendmi/pidfile.h>
#include <opendmi/internal.h>

#include <opendmi/utils.h>
#include <opendmi/utils/file.h>
#include <opendmi/utils/string.h>


static void dmi_pidfile_destroy(dmi_pidfile_t *pidfile);

pid_t dmi_pidfile_read(const char *path)
{
    dmi_unused(path);

    return -1;
}

dmi_pidfile_t *dmi_pidfile_acquire(const char *path)
{
    dmi_pidfile_t *pidfile;

    pidfile = dmi_alloc(nullptr, sizeof(*pidfile));
    if (pidfile == nullptr)
    return nullptr;

    pidfile->fd = -1;

    bool success = false;
    char *pid = nullptr;
    do {
        int length;

        length = dmi_asprintf(&pid, "%d", getpid());
        if (length < 0)
            break;

        pidfile->path = strdup(path);
        if (pidfile->path == nullptr)
            break;

        pidfile->fd = open(pidfile->path, O_WRONLY | O_CREAT, 0644);
        if (pidfile->fd < 0)
            break;

        if (not dmi_file_lock(pidfile->fd, 0))
            break;

        if (ftruncate(pidfile->fd, 0) < 0)
            break;
        if (dmi_file_write(pidfile->fd, dmi_data(pid), length) < 0)
            break;

        success = true;
    } while (false);

    dmi_free(pid);

    if (not success) {
        dmi_pidfile_destroy(pidfile);
        return nullptr;
    }

    return pidfile;
}

void dmi_pidfile_release(dmi_pidfile_t *pidfile)
{
    assert(pidfile != nullptr);

    unlink(pidfile->path);
    dmi_pidfile_destroy(pidfile);
}

static void dmi_pidfile_destroy(dmi_pidfile_t *pidfile)
{
    if (pidfile->fd >= 0)
        dmi_file_close(pidfile->fd);

    dmi_free(pidfile->path);
    dmi_free(pidfile);
}
