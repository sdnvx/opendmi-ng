//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <sys/stat.h>

#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#if __has_include(<io.h>)
#   include <io.h>
#endif

#include <errno.h>
#include <assert.h>

#include <opendmi/utils/file.h>

#if defined(_WIN32)
#   include <sys/locking.h>
#   include <limits.h>
#   define fstat(fd, buf)          _fstat(fd, buf)
#   define read(fd, buf, size)     _read(fd, buf, size)
#   define close(fd)               _close(fd)
#   define lockf(fd, cmd, size)    _locking(fd, cmd, size == 0 ? LONG_MAX : size)
#   define F_LOCK                  _LK_LOCK
#   define F_ULOCK                 _LK_UNLCK
#endif

int dmi_file_stat(int fd, dmi_file_stat_t *st)
{
    int rv;

    assert(fd >= 0);
    assert(st != nullptr);

retry:
    rv = fstat(fd, st);
    if ((rv < 0) and (errno == EINTR))
        goto retry;

    return rv;
}

off_t dmi_file_tell(int fd)
{
    assert(fd >= 0);

    return dmi_file_seek(fd, 0, SEEK_CUR);
}

off_t dmi_file_seek(int fd, off_t offset, int whence)
{
    off_t rv;

    assert(fd >= 0);

retry:
    rv = lseek(fd, offset, whence);
    if ((rv < 0) and (errno == EINTR))
        goto retry;

    return rv;
}

bool dmi_file_lock(int fd, off_t size)
{
    assert(fd >= 0);

retry:
    if (lockf(fd, F_LOCK, size) < 0) {
        if (errno == EINTR)
            goto retry;

        return false;
    }

    return true;
}

bool dmi_file_unlock(int fd, off_t size)
{
    assert(fd >= 0);

retry:
    if (lockf(fd, F_ULOCK, size) < 0) {
        if (errno == EINTR)
            goto retry;

        return false;
    }

    return true;
}

ssize_t dmi_file_read(int fd, dmi_data_t *data, off_t offset, size_t size)
{
    size_t nread = 0;

    assert(fd >= 0);
    assert(data != nullptr);

    if (offset >= 0) {
        if (dmi_file_seek(fd, offset, SEEK_SET) < 0)
            return -1;
    }

    while (nread < size) {
        ssize_t rv = read(fd, data + nread, size - nread);

        if (rv < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }

        if (rv == 0)
            break;

        nread += rv;
    }

    return nread;
}

ssize_t dmi_file_write(int fd, const dmi_data_t *data, size_t size)
{
    size_t nwritten = 0;

    assert(fd >= 0);
    assert(data != nullptr);

    while (nwritten < size) {
        ssize_t rv = write(fd, data + nwritten, size - nwritten);

        if (rv < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }

        if (rv == 0)
            break;

        nwritten += rv;
    }

    return nwritten;
}

int dmi_file_close(int fd)
{
    assert(fd >= 0);

    return close(fd);
}
