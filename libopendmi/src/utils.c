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
#include <errno.h>
#include <assert.h>

#if defined(__KERNEL__)
#   include <linux/io.h>
#else
#   include <fcntl.h>
#   include <inttypes.h>
#endif

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

// Files are not accessible from the kernel
#if !defined(__KERNEL__)
#   include <opendmi/utils/file.h>
#endif

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

#if defined(__KERNEL__)
bool dmi_file_load(
        dmi_buffer_t *buffer,
        const char   *path,
        off_t         offset,
        size_t        length)
{
    dmi_unused(offset);
    dmi_unused(length);

    dmi_error_raise_ex(dmi_buffer_context(buffer), DMI_ERROR_SERVICE_UNAVAILABLE,
                       "%s: files are not accessible from the kernel", path);

    return false;
}
#else
bool dmi_file_load(
        dmi_buffer_t *buffer,
        const char   *path,
        off_t         offset,
        size_t        length)
{
    dmi_context_t *context = dmi_buffer_context(buffer);

    if (buffer == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "buffer");
        return false;
    }
    if (path == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "path");
        return false;
    }

    int     fd    = -1;
    ssize_t nread = 0;

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

        // Data is read into the buffer itself, which is made long enough for
        // the whole of it and cut down to what has been read
        if (not dmi_buffer_resize(buffer, length))
            break;

        // An empty file is read by reading nothing at all, and the buffer
        // holds no data to read into
        if (length == 0) {
            success = true;
            break;
        }

        nread = dmi_file_read(fd, buffer->data, offset, length);
        if (nread < 0) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_READ, "%s: %s", path, strerror(errno));
            break;
        }

        success = dmi_buffer_resize(buffer, (size_t)nread);
    } while (false);

    // Cleanup
    if (fd >= 0)
        dmi_file_close(fd);

    // Handle errors
    if (not success) {
        dmi_buffer_clear(buffer);
        return false;
    }

    return true;
}
#endif // !defined(__KERNEL__)

#if defined(__KERNEL__)
bool dmi_memory_load(dmi_buffer_t *buffer, const char *path, uint64_t base, size_t length)
{
    dmi_context_t *context = dmi_buffer_context(buffer);

    // Physical memory is mapped directly, there is no device to go through
    dmi_unused(path);

    if (buffer == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "buffer");
        return false;
    }
    if (length == 0) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "length");
        return false;
    }

    // Length comes from firmware, which is not trusted to map whatever it
    // likes
    if (length > DMI_TABLE_MAX_SIZE) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_MAP, "Region of %zu bytes exceeds the limit of %zu bytes",
                           length, (size_t)DMI_TABLE_MAX_SIZE);
        return false;
    }

    // Physical address of a 32-bit kernel without PAE is 32-bit, and the one
    // of SMBIOS 3.0 table is 64-bit
    if ((resource_size_t)base != base) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_MAP, "Address 0x%llx is out of range",
                           (unsigned long long)base);
        return false;
    }

    if (not dmi_buffer_resize(buffer, length))
        return false;

    // Firmware tables are mapped the same way the kernel maps them itself,
    // see dmi_remap() of the architectures
    const dmi_data_t *ptr = memremap(base, length, MEMREMAP_WB);
    if (ptr == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_MAP, "Unable to map 0x%llx-0x%llx",
                           (unsigned long long)base, (unsigned long long)(base + length - 1));
        dmi_buffer_clear(buffer);
        return false;
    }

    dmi_memory_get_data(buffer->data, ptr, length);
    memunmap((void *)ptr);

    return true;
}
#elif !defined(_WIN32)
bool dmi_memory_load(dmi_buffer_t *buffer, const char *path, uint64_t base, size_t length)
{
    dmi_context_t *context = dmi_buffer_context(buffer);

    if (buffer == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "buffer");
        return false;
    }
    if (path == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "path");
        return false;
    }
    if (length == 0) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "length");
        return false;
    }

    // Length comes from firmware, which is not trusted to map whatever it
    // likes
    if (length > DMI_TABLE_MAX_SIZE) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_MAP, "%s: region of %zu bytes exceeds the limit of %zu bytes",
                           path, length, (size_t)DMI_TABLE_MAX_SIZE);
        return false;
    }

    // Offset of the device is signed, and is 32-bit on some 32-bit systems,
    // while the address of SMBIOS 3.0 table is 64-bit
    const uint64_t offset_max = (sizeof(off_t) < sizeof(int64_t)) ? INT32_MAX : INT64_MAX;
    if (base > offset_max) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_MAP, "%s: address 0x%" PRIx64 " is out of range",
                           path, base);
        return false;
    }

    bool   success   = false;
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    size_t offset    = (size_t)(base % page_size);
    int    fd        = -1;

    // cppcheck-suppress constVariablePointer
    dmi_data_t *ptr  = MAP_FAILED;

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
        // Region is checked without computing its end, which can overflow
        if (S_ISREG(st.st_mode) and
            ((length > (uint64_t)st.st_size) or (base > (uint64_t)st.st_size - length)))
        {
            dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "%s: unable to map beyond the end of file", path);
            break;
        }

        // Data is copied into the buffer, so that the mapping is of no
        // interest once the region has been read
        if (not dmi_buffer_resize(buffer, length))
            break;

        ptr = mmap(nullptr, offset + length, PROT_READ, MAP_SHARED, fd, (off_t)(base - offset));
        if (ptr == MAP_FAILED) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_MAP, "%s: %s", path, strerror(errno));
            break;
        }

        // cppcheck-suppress nullPointerArithmeticOutOfMemory
        dmi_memory_get_data(buffer->data, ptr + offset, length);

        success = true;
    } while (false);

    if (ptr != MAP_FAILED) {
        // cppcheck-suppress nullPointerOutOfMemory
        munmap(ptr, offset + length);
    }
    if (fd >= 0)
        dmi_file_close(fd);

    if (not success) {
        dmi_buffer_clear(buffer);
        return false;
    }

    return true;
}
#endif

#if !defined(_WIN32)
static void dmi_memory_get_data(dmi_data_t *dst, const dmi_data_t *src, size_t length)
{
#   if defined(__aarch64__)
        // Avoid unaligned memory access on memory device. Bytes are read
        // through a volatile pointer, so that the compiler cannot turn the
        // loop back into memcpy(), which uses wide unaligned accesses.
        const volatile dmi_data_t *vsrc = src;

        for (size_t i = 0; i < length; i++) {
            *(dst + i) = *(vsrc + i);
        }
#   else
        memcpy(dst, src, length);
#   endif
}
#endif // !defined(_WIN32)
