//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#if __has_include(<unistd.h>)
#   include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>

// Files are not accessible from the kernel
#if !defined(__KERNEL__)
#   include <fcntl.h>
#endif

// Dumps are moved into place by the system, see dmi_save_commit()
#if defined(_WIN32)
#   define WIN32_LEAN_AND_MEAN
#   define NOMINMAX
#   include <windows.h>
#   include <process.h>
#endif

#include <opendmi/anonymize.h>
#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>

#if !defined(__KERNEL__)
#   include <opendmi/utils/file.h>
#   include <opendmi/utils/string.h>
#endif

#if defined(_WIN32)
#   include <opendmi/utils/win32.h>
#endif

#if !defined(__KERNEL__)
/**
 * @internal
 * @brief Number of names tried for the temporary file of a dump.
 */
#define DMI_SAVE_TEMP_ATTEMPTS 100

/**
 * @internal
 * @brief File a dump is being written to.
 */
typedef struct dmi_save_target
{
    // Descriptor the dump is written through
    int fd;

    // Temporary file which is moved over the target once the dump is
    // complete, or nullptr if the target is written directly
    char *temp;

    // Whether the target exists, and its status if it does
    bool            exists;
    dmi_file_stat_t st;
} dmi_save_target_t;

/**
 * @internal
 * @brief Check that the data of a context can be saved.
 */
static bool dmi_save_check(dmi_context_t *context, const char *path);

/**
 * @internal
 * @brief Get the table a dump is written with: the table of the context, or
 * the anonymized copy of it, which is stored into @p panonymized to be freed
 * by the caller.
 *
 * @return Table, or @c nullptr if the copy cannot be made.
 */
static const dmi_buffer_t *dmi_save_table(dmi_context_t *context, unsigned flags, dmi_buffer_t **panonymized);

/**
 * @internal
 * @brief Open the file a dump is written to.
 *
 * @details
 * Regular file is never written in place: the dump goes to a temporary file,
 * which replaces the target only once it is complete, so that a failure
 * neither destroys the file being overwritten nor leaves an incomplete dump
 * behind, and the replaced file keeps its permissions. Devices, pipes and
 * symbolic links, e.g. `/dev/stdout`, cannot be replaced that way, and are
 * written directly.
 *
 * Devices and pipes are streams rather than files, which a dump is written to
 * whether it is overwriting or not, and so are the symbolic links to them,
 * e.g. `/dev/stdout`, while a symbolic link to a file is not followed to
 * replace the file unless the dump is overwriting.
 */
static bool dmi_save_open(dmi_context_t *context, const char *path, bool overwrite, dmi_save_target_t *target);

/**
 * @internal
 * @brief Tell whether the existing target of a dump which is not a regular
 * file is a stream: a device or a pipe, or a symbolic link to one.
 */
static bool dmi_save_is_stream(const char *path, const dmi_file_stat_t *st);

/**
 * @internal
 * @brief Close the file a dump has been written to, and move the temporary
 * file into place if the dump is complete, or remove it otherwise.
 *
 * @return `true` if the dump has been written and saved, `false` otherwise.
 */
static bool dmi_save_close(
        dmi_context_t     *context,
        const char        *path,
        bool               overwrite,
        dmi_save_target_t *target,
        bool               success);

/**
 * @internal
 * @brief Tell the status of the target of a dump, without following it if it
 * is a symbolic link.
 */
static int dmi_save_stat(const char *path, dmi_file_stat_t *st);

/**
 * @internal
 * @brief Create the temporary file a dump is written to.
 *
 * @details
 * The file is created next to the target, so that it can be renamed over it,
 * and is never one which has existed before.
 *
 * @return Path to the file, which is to be freed by the caller, or
 *         @c nullptr on failure.
 */
static char *dmi_save_temp_open(dmi_context_t *context, const char *path, int *pfd);

/**
 * @internal
 * @brief Move a completely written dump into place.
 *
 * @details
 * Existing file is replaced atomically if @p overwrite is set, and is never
 * replaced otherwise, even if it has been created while the dump was being
 * written.
 */
static bool dmi_save_commit(dmi_context_t *context, const char *temp, const char *path, bool overwrite);

/**
 * @internal
 * @brief Write dump data completely, raising an error on failures.
 */
static bool dmi_dump_write(
        dmi_context_t    *context,
        int               fd,
        const char       *path,
        const dmi_data_t *data,
        size_t            size);

/**
 * @internal
 * @brief Build entry point structure for a dump file.
 *
 * @details
 * Dump file layout is compatible with dmidecode: the entry point structure
 * is padded with zeroes to #DMI_ENTRY_MAX_SIZE bytes and followed by the
 * structure table. The table address in the entry point is replaced with the
 * table offset in the file, and the checksum is adjusted accordingly. If the
 * backend provides no entry point data, a 64-bit entry point is generated.
 */
static bool dmi_dump_entry_build(dmi_context_t *context, dmi_byte_t *entry);

/**
 * @internal
 * @brief Generate 64-bit entry point structure for a dump file.
 */
static bool dmi_dump_entry_generate(dmi_context_t *context, dmi_byte_t *entry);

#endif // !defined(__KERNEL__)

/**
 * @internal
 * @brief Set field of entry point structure placed in a writable buffer.
 *
 * @details
 * Entry point structure fields are read-only, so the value is copied to the
 * field address. Copying also avoids unaligned access to packed fields.
 */
#define dmi_entry_set(field, value) \
    memcpy((void *)&(field), &(const __typeof__(field)){ value }, sizeof(field))

/**
 * @internal
 * @brief Update checksum of `length` bytes of entry point structure placed
 * in a writable buffer.
 */
#define dmi_entry_set_checksum(eps, length)                                  \
    do {                                                                     \
        dmi_entry_set((eps)->checksum, 0);                                   \
        dmi_entry_set((eps)->checksum, dmi_checksum_calc((eps), (length))); \
    } while (false)

#if defined(__KERNEL__)
bool dmi_save(dmi_context_t *context, const char *path, unsigned flags)
{
    dmi_unused(flags);

    if (context == nullptr)
        return false;

    dmi_error_raise_ex(context, DMI_ERROR_SERVICE_UNAVAILABLE,
                       "%s: files are not accessible from the kernel", path);

    return false;
}
#else
bool dmi_save(dmi_context_t *context, const char *path, unsigned flags)
{
    if (context == nullptr)
        return false;

    if (not dmi_save_check(context, path))
        return false;

    dmi_byte_t entry[DMI_ENTRY_MAX_SIZE];
    if (not dmi_dump_entry_build(context, entry))
        return false;

    // Anonymized copy has the length of the table, so the entry point built
    // for the table holds for it too
    dmi_buffer_t *anonymized = nullptr;

    const dmi_buffer_t *table = dmi_save_table(context, flags, &anonymized);
    if (table == nullptr)
        return false;

    bool overwrite = (flags & DMI_SAVE_FLAG_OVERWRITE) != 0;

    dmi_save_target_t target = {
        .fd = -1
    };

    bool success = false;

    if (dmi_save_open(context, path, overwrite, &target)) {
        success = dmi_dump_write(context, target.fd, path, entry, sizeof(entry)) and
                  dmi_dump_write(context, target.fd, path, table->data, table->length);

        success = dmi_save_close(context, path, overwrite, &target, success);
    }

    dmi_buffer_destroy(anonymized);

    return success;
}

static bool dmi_save_check(dmi_context_t *context, const char *path)
{
    if (path == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_NULL_ARGUMENT, "path");
        return false;
    }
    if (context->state.table == nullptr) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_STATE, "Context is not open");
        return false;
    }

    // Backends which have no access to the entry point leave the context
    // without one, and its data is generated on saving
    if ((context->state.entry != nullptr) and
        (context->state.entry->length > DMI_ENTRY_MAX_SIZE))
    {
        dmi_error_raise(context, DMI_ERROR_INVALID_EPS_LENGTH);
        return false;
    }

    return true;
}

static const dmi_buffer_t *dmi_save_table(dmi_context_t *context, unsigned flags, dmi_buffer_t **panonymized)
{
    *panonymized = nullptr;

    if ((flags & DMI_SAVE_FLAG_ANONYMIZE) == 0)
        return context->state.table;

    dmi_buffer_t *anonymized = dmi_buffer_create(context);
    if (anonymized == nullptr)
        return nullptr;

    if (not dmi_anonymize(context, anonymized)) {
        dmi_buffer_destroy(anonymized);
        return nullptr;
    }

    *panonymized = anonymized;

    return anonymized;
}

static bool dmi_save_open(dmi_context_t *context, const char *path, bool overwrite, dmi_save_target_t *target)
{
    target->exists = (dmi_save_stat(path, &target->st) == 0);

    bool is_direct = target->exists and not S_ISREG(target->st.st_mode);
    bool is_stream = is_direct and dmi_save_is_stream(path, &target->st);

    if (target->exists and not is_direct and not overwrite) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(EEXIST));
        return false;
    }

    if (is_direct) {
        int mode = O_CREAT | O_WRONLY | O_TRUNC;
#if defined(O_BINARY)
        mode |= O_BINARY;
#endif
        if (not overwrite and not is_stream)
            mode |= O_EXCL;

        target->fd = open(path, mode, 0666);
        if (target->fd < 0)
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(errno));
    } else {
        target->temp = dmi_save_temp_open(context, path, &target->fd);
    }

    if (target->fd < 0)
        return false;

#if !defined(_WIN32)
    // Replaced file keeps its permissions
    if (target->exists and (target->temp != nullptr))
        (void)fchmod(target->fd, target->st.st_mode & 0777);
#endif

    return true;
}

static bool dmi_save_is_stream(const char *path, const dmi_file_stat_t *st)
{
    if (not S_ISLNK(st->st_mode))
        return not S_ISREG(st->st_mode);

    // Link is followed to tell what it points to, and one which points to
    // nothing is not a stream
    dmi_file_stat_t target;

#if defined(_WIN32)
    if (_stat(path, &target) != 0)
#else
    if (stat(path, &target) != 0)
#endif
        return false;

    return not S_ISREG(target.st_mode);
}

static bool dmi_save_close(
        dmi_context_t     *context,
        const char        *path,
        bool               overwrite,
        dmi_save_target_t *target,
        bool               success)
{
    if (dmi_file_close(target->fd) < 0) {
        if (success)
            dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s: %s", path, strerror(errno));
        success = false;
    }

    if (target->temp != nullptr) {
        if (success)
            success = dmi_save_commit(context, target->temp, path, overwrite);

        // Do not leave incomplete dump behind
        if (not success)
            remove(target->temp);

        dmi_free(target->temp);
        target->temp = nullptr;
    }

    return success;
}

static int dmi_save_stat(const char *path, dmi_file_stat_t *st)
{
#if defined(_WIN32)
    return _stat(path, st);
#else
    return lstat(path, st);
#endif
}

static char *dmi_save_temp_open(dmi_context_t *context, const char *path, int *pfd)
{
    int mode = O_CREAT | O_EXCL | O_WRONLY;
#if defined(O_BINARY)
    mode |= O_BINARY;
#endif

#if defined(_WIN32)
    long pid = (long)_getpid();
#else
    long pid = (long)getpid();
#endif

    *pfd = -1;

    // Name is tried again if it is taken, e.g. by another dump of the same
    // process being saved at the same time
    for (unsigned attempt = 0; attempt < DMI_SAVE_TEMP_ATTEMPTS; attempt++) {
        char *temp = nullptr;

        if (dmi_asprintf(&temp, "%s.%ld-%u.tmp", path, pid, attempt) < 0) {
            dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);
            return nullptr;
        }

        int fd = open(temp, mode, 0666);
        if (fd >= 0) {
            *pfd = fd;
            return temp;
        }

        int error = errno;
        dmi_free(temp);

        if (error != EEXIST) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(error));
            return nullptr;
        }
    }

    dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: Unable to create temporary file", path);

    return nullptr;
}

static bool dmi_save_commit(dmi_context_t *context, const char *temp, const char *path, bool overwrite)
{
#if defined(_WIN32)
    // Unlike rename(), the system call replaces an existing file on request,
    // and fails if there is one otherwise
    if (MoveFileExA(temp, path, overwrite ? MOVEFILE_REPLACE_EXISTING : 0))
        return true;

    DWORD error = GetLastError();
    bool  is_existing = (error == ERROR_ALREADY_EXISTS) or (error == ERROR_FILE_EXISTS);

    dmi_error_raise_ex(context, is_existing ? DMI_ERROR_FILE_OPEN : DMI_ERROR_FILE_WRITE,
                       "%s: %s", path, dmi_win32err_to_string(error));

    return false;
#else
    if (not overwrite) {
        // Hard link is made only if there is no such file yet, unlike a
        // rename, which would replace a file created in the meantime
        if (link(temp, path) == 0) {
            remove(temp);
            return true;
        }

        if (errno == EEXIST) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(errno));
            return false;
        }

        // File systems with no hard links, e.g. FAT, leave nothing but a
        // rename, so the target is checked to be missing once again
        dmi_file_stat_t st;
        if (dmi_save_stat(path, &st) == 0) {
            dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "%s: %s", path, strerror(EEXIST));
            return false;
        }
    }

    if (rename(temp, path) < 0) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s: %s", path, strerror(errno));
        return false;
    }

    return true;
#endif
}

static bool dmi_dump_write(
        dmi_context_t    *context,
        int               fd,
        const char       *path,
        const dmi_data_t *data,
        size_t            size)
{
    ssize_t nwritten = dmi_file_write(fd, data, size);

    if (nwritten < 0) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s: %s", path, strerror(errno));
        return false;
    }
    if ((size_t)nwritten < size) {
        dmi_error_raise_ex(context, DMI_ERROR_FILE_WRITE, "%s: Incomplete write", path);
        return false;
    }

    return true;
}

static bool dmi_dump_entry_build(dmi_context_t *context, dmi_byte_t *entry)
{
    const dmi_entry_spec_t *spec = context->state.entry_spec;
    size_t length;

    memset(entry, 0, DMI_ENTRY_MAX_SIZE);

    // Entry point data is optional, Windows backend does not provide it
    if ((context->state.entry == nullptr) or (spec == nullptr))
        return dmi_dump_entry_generate(context, entry);

    memcpy(entry, context->state.entry->data, context->state.entry->length);

    if (spec->version >= DMI_VERSION(3, 0, 0)) {
        dmi_entry_v30_t *eps = dmi_cast(eps, entry);

        length = dmi_decode(eps->length);

        dmi_entry_set(eps->table_area_addr, dmi_encode_qword(DMI_ENTRY_MAX_SIZE));
        dmi_entry_set_checksum(eps, length);
    } else if (spec->version >= DMI_VERSION(2, 1, 0)) {
        dmi_entry_v21_t *eps = dmi_cast(eps, entry);
        dmi_entry_legacy_t *ieps = dmi_cast(ieps, &eps->ieps);

        // Unlike dmidecode, the intermediate entry point is always written
        // completely, even if the entry point length is 0x1E. Otherwise, its
        // checksum would become invalid.
        length = dmi_decode(eps->length);
        if (length < sizeof(dmi_entry_v21_t))
            length = sizeof(dmi_entry_v21_t);

        // Intermediate entry point bytes sum to zero before and after the
        // relocation, so the entry point checksum needs no adjustment.
        dmi_entry_set(ieps->table_area_addr, dmi_encode_dword(DMI_ENTRY_MAX_SIZE));
        dmi_entry_set_checksum(ieps, sizeof(dmi_entry_legacy_t));
    } else {
        dmi_entry_legacy_t *eps = dmi_cast(eps, entry);

        length = sizeof(dmi_entry_legacy_t);

        dmi_entry_set(eps->table_area_addr, dmi_encode_dword(DMI_ENTRY_MAX_SIZE));
        dmi_entry_set_checksum(eps, length);
    }

    // Only the entry point itself is written, the rest is zero-filled
    assert(length <= DMI_ENTRY_MAX_SIZE);
    memset(entry + length, 0, DMI_ENTRY_MAX_SIZE - length);

    return true;
}

static bool dmi_dump_entry_generate(dmi_context_t *context, dmi_byte_t *entry)
{
    dmi_version_t version = context->state.smbios_version;

    if (context->state.table->length > UINT32_MAX) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_STATE,
                           "SMBIOS table is too large: %zu bytes", context->state.table->length);
        return false;
    }

    dmi_entry_v30_t *eps = dmi_cast(eps, entry);

    const dmi_entry_v30_t data = {
        .length              = dmi_encode_byte(sizeof(dmi_entry_v30_t)),
        .version_major       = dmi_encode_byte((uint8_t)dmi_version_major(version)),
        .version_minor       = dmi_encode_byte((uint8_t)dmi_version_minor(version)),
        .version_rev         = dmi_encode_byte((uint8_t)dmi_version_revision(version)),
        .revision            = dmi_encode_byte(0x01), // SMBIOS 3.0 entry point
        .table_area_max_size = dmi_encode_dword((uint32_t)context->state.table->length),
        .table_area_addr     = dmi_encode_qword(DMI_ENTRY_MAX_SIZE)
    };

    memcpy(eps, &data, sizeof(data));

    // Anchor is not null-terminated, so it is not initialized from string
    memcpy(eps, DMI_ANCHOR_V30, strlen(DMI_ANCHOR_V30));

    dmi_entry_set_checksum(eps, sizeof(dmi_entry_v30_t));

    return true;
}

#endif // !defined(__KERNEL__)
