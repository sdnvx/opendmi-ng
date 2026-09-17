//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_FILE_H
#define OPENDMI_UTILS_FILE_H

#pragma once

#include <sys/stat.h>
#include <opendmi/types.h>

#if defined(_WIN32)
typedef struct _stat dmi_file_stat_t;
#else
typedef struct stat dmi_file_stat_t;
#endif

// Not all platforms (e.g. MSVC) provide file type test macros
#if !defined(S_ISREG)
#   define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
#endif

__BEGIN_DECLS

/**
 * @brief Retrieves status information for an open file descriptor.
 *
 * A cross-platform wrapper around `fstat()` that retries automatically
 * if interrupted by a signal (`EINTR`).
 *
 * @param[in]  fd  Open file descriptor to query.
 * @param[out] st  Pointer to a structure that receives the file status.
 *
 * @return `0` on success, or `-1` on error with `errno` set accordingly.
 */
int dmi_file_stat(int fd, dmi_file_stat_t *st);

/**
 * @brief Returns the current file offset for an open file descriptor.
 *
 * Equivalent to calling `dmi_file_seek(fd, 0, SEEK_CUR)`.
 *
 * @param[in] fd  Open file descriptor.
 *
 * @return The current byte offset from the beginning of the file,
 *         or `-1` on error with `errno` set accordingly.
 */
off_t dmi_file_tell(int fd);

/**
 * @brief Repositions the file offset of an open file descriptor.
 *
 * A wrapper around `lseek()` that retries automatically if interrupted
 * by a signal (`EINTR`).
 *
 * @param[in] fd      Open file descriptor.
 * @param[in] offset  Byte offset to apply, relative to @p whence.
 * @param[in] whence  Offset directive: `SEEK_SET`, `SEEK_CUR`, or `SEEK_END`.
 *
 * @return The resulting byte offset from the beginning of the file,
 *         or `-1` on error with `errno` set accordingly.
 */
off_t dmi_file_seek(int fd, off_t offset, int whence);

/**
 * @brief Acquires an advisory lock on a region of a file.
 *
 * Locks @p size bytes of the file starting at the current offset using
 * `lockf(F_LOCK)`. Blocks until the lock is acquired. Retries automatically
 * if interrupted by a signal (`EINTR`). Passing @p size of 0 locks from
 * the current offset to the end of the file.
 *
 * @param[in] fd    Open file descriptor.
 * @param[in] size  Number of bytes to lock, or 0 to lock to end of file.
 *
 * @return `true` if the lock was acquired, `false` on error.
 */
bool dmi_file_lock(int fd, off_t size);

/**
 * @brief Releases an advisory lock on a region of a file.
 *
 * Unlocks @p size bytes of the file starting at the current offset using
 * `lockf(F_ULOCK)`. Retries automatically if interrupted by a signal
 * (`EINTR`). Passing @p size of 0 unlocks from the current offset to the
 * end of the file.
 *
 * @param[in] fd    Open file descriptor.
 * @param[in] size  Number of bytes to unlock, or 0 to unlock to end of file.
 *
 * @return `true` if the lock was released, `false` on error.
 */
bool dmi_file_unlock(int fd, off_t size);

/**
 * @brief Reads exactly @p size bytes from an open file descriptor.
 *
 * Performs a full read by looping until all requested bytes are read or
 * end-of-file is reached. Retries automatically if interrupted by a signal
 * (`EINTR`). Returns fewer than @p size bytes only if EOF is encountered
 * before the read is complete.
 *
 * @param[in]  fd      Open file descriptor to read from.
 * @param[out] data    Buffer that receives the data; must be at least
 *                     @p size bytes.
 * @param[in]  offset  Starting offset. Should be set to `-1` for reading from
 *                     current position.
 * @param[in]  size    Number of bytes to read.
 *
 * @return The total number of bytes read (may be less than @p size at EOF),
 *         or `-1` on error with `errno` set accordingly.
 */
ssize_t dmi_file_read(int fd, dmi_data_t *data, off_t offset, size_t size);

/**
 * @brief Writes exactly @p size bytes to an open file descriptor.
 *
 * Performs a full write by looping until all requested bytes are written or
 * a zero-length write is returned. Retries automatically if interrupted by a
 * signal (`EINTR`). Returns fewer than @p size bytes only if the underlying
 * `write()` returns zero before all bytes are written.
 *
 * @param[in] fd    Open file descriptor to write to.
 * @param[in] data  Buffer containing the data to write; must be at least
 *                  @p size bytes.
 * @param[in] size  Number of bytes to write.
 *
 * @return The total number of bytes written (may be less than @p size if a
 *         zero-length write occurs), or `-1` on error with `errno` set
 *         accordingly.
 */
ssize_t dmi_file_write(int fd, const dmi_data_t *data, size_t size);

/**
 * @brief Closes an open file descriptor.
 *
 * A cross-platform wrapper around `close()`.
 *
 * @param[in] fd  Open file descriptor to close.
 *
 * @return `0` on success, or `-1` on error with `errno` set accordingly.
 */
int dmi_file_close(int fd);

__END_DECLS

#endif // !OPENDMI_UTILS_FILE_H
