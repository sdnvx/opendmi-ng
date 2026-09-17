//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#if __has_include(<sys/mman.h>)
#   include <sys/mman.h>
#endif

#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#if __has_include(<share.h>)
#   include <share.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/file.h>

#ifndef _WIN32
static void dmi_memory_get_data(dmi_data_t *dst, const dmi_data_t *src, size_t length);
#endif

void *dmi_alloc(dmi_context_t *context, size_t size)
{
    void *ptr;

    ptr = calloc(1, size);
    if ((ptr == nullptr) and (context != nullptr))
        dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);

    return ptr;
}

void dmi_free(void *ptr)
{
    if (ptr == nullptr)
        return;

    free(ptr);
}

bool dmi_checksum_test(const void *data, size_t length)
{
    if (data == nullptr) {
        errno = EINVAL;
        return false;
    }

    return dmi_checksum_calc(data, length) == 0;
}

uint8_t dmi_checksum_calc(const void *data, size_t length)
{
    if (data == nullptr) {
        errno = EINVAL;
        return 0;
    }

    uint8_t sum   = 0;
    size_t  index = 0;

    while (index < length) {
        sum += ((const uint8_t *)data)[index++];
    }

    return (uint8_t)-sum;
}

uint32_t dmi_ipow32(uint32_t value, unsigned int factor)
{
    uint32_t result = 1;

    if (factor == 0)
        return result;

    while (factor > 1) {
        if (factor % 2)
            result *= value, factor--;

        value *= value, factor /= 2;
    }

    return value * result;
}

uint64_t dmi_ipow64(uint64_t value, unsigned int factor)
{
    uint64_t result = 1;

    if (factor == 0)
        return result;

    while (factor > 1) {
        if (factor % 2)
            result *= value, factor--;

        value *= value, factor /= 2;
    }

    return value * result;
}

dmi_data_t *dmi_file_get(
        dmi_context_t *context,
        const char    *path,
        off_t          offset,
        size_t        *plength)
{
    if (context == nullptr)
        return nullptr;

    if (path == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "path");
        return nullptr;
    }
    if (plength == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "plength");
        return nullptr;
    }

    int         fd     = -1;
    dmi_data_t *data   = nullptr;
    size_t      length = *plength;
    ssize_t     nread  = 0;

    bool success = false;
    do {
        dmi_file_stat_t st;

        // Open file
#if defined(_WIN32)
        if (_sopen_s(&fd, path, _O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD) != 0) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(errno));
            break;
        }
#else
        if ((fd = open(path, O_RDONLY)) < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(errno));
            break;
        }
#endif

        // Determine actual number of bytes to read
        if (length == 0) {
            if (dmi_file_stat(fd, &st) < 0) {
                dmi_error_raise_ex(context, DMI_ERROR_FILE_STAT, "%s: %s", path, strerror(errno));
                break;
            }

            length = st.st_size;
        }

        // Allocate output buffer. Zero-sized allocation may fail on some
        // platforms, so at least one byte is allocated for empty files.
        if ((data = dmi_alloc(context, length > 0 ? length : 1)) == nullptr)
            break;

        // Read data into buffer
        nread = dmi_file_read(fd, data, offset, length);
        if (nread < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_READ, "%s: %s", path, strerror(errno));
            break;
        }

        success = true;
    } while (false);

    // Cleanup
    if (fd >= 0)
        dmi_file_close(fd);

    // Handle errors
    if (not success) {
        dmi_free(data);
        return nullptr;
    }

    *plength = (size_t)nread;

    return data;
}

#if !defined(_WIN32)
dmi_data_t *dmi_memory_get(dmi_context_t *context, const char *path, size_t base, size_t length)
{
    if (context == nullptr)
        return nullptr;

    if (path == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "path");
        return nullptr;
    }
    if (length == 0) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "length");
        return nullptr;
    }

    bool   success   = false;
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    size_t offset    = base % page_size;
    int    fd        = -1;

    // cppcheck-suppress constVariablePointer
    dmi_data_t *ptr  = MAP_FAILED;
    dmi_data_t *data = nullptr;

    do {
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(errno));
            break;
        }

        dmi_file_stat_t st;
        if (dmi_file_stat(fd, &st) < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_STAT, "%s: %s", path, strerror(errno));
            break;
        }

        if ((geteuid() == 0) and (not S_ISCHR(st.st_mode))) {
            dmi_error_raise_ex(context, DMI_ERROR_SYSTEM, "%s: not a character device", path);
            break;
        }
        if (S_ISREG(st.st_mode) and (base + length > (size_t)st.st_size)) {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "%s: unable to map beyond the end of file", path);
            break;
        }

        data = dmi_alloc(context, length);
        if (data == nullptr)
            break;

        ptr = mmap(nullptr, offset + length, PROT_READ, MAP_SHARED, fd, base - offset);
        if (ptr == MAP_FAILED) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_MAP, "%s: %s", path, strerror(errno));
            break;
        }

        // cppcheck-suppress nullPointerArithmeticOutOfMemory
        dmi_memory_get_data(data, ptr + offset, length);

        success = true;
    } while (false);

    if (ptr != MAP_FAILED) {
        // cppcheck-suppress nullPointerOutOfMemory
        munmap(ptr, offset + length);
    }
    if (fd >= 0)
        dmi_file_close(fd);

    if (not success) {
        dmi_free(data);
        return nullptr;
    }

    return data;
}

static void dmi_memory_get_data(dmi_data_t *dst, const dmi_data_t *src, size_t length)
{
#   if defined(__aarch64__)
        // Avoid unaligned memory access on memory device
        for (size_t i = 0; i < length; i++) {
            *(dst + i) = *(src + i);
        }
#   else
        memcpy(dst, src, length);
#   endif
}
#endif // !defined(_WIN32)
