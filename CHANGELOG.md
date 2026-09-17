# OpenDMI: Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Add `DMI_SIZE_MAX` constant for unknown `dmi_size_t` values
- Add structure type attribute to chassis contained elements
- Add decoding of processor voltage and supported voltages

### Fixed

- Represent reserved minimum and maximum counts of chassis elements as unknown values
- Show unknown firmware image size in firmware inventory information as unknown
- Fix handle attribute code of group associations items
- Keep table area size specified in the entry point instead of overwriting it with the size of data provided by backend

## [0.4.0] - September 17, 2026

### Added

- Add Intel RSD FPGA information decoder #88
- Add tests for module handling functions
- Add tests for context handling functions
- Add tests for entry point handling functions
- Add tests for registry handling functions
- Add tests for stream handling functions
- Add tests for entity handling functions
- Add tests for management controller information decoder
- Add tests for IPMI device information decoder
- Add tests for firmware inventory information decoder
- Add tests for Intel RSD FPGA information decoder
- Add entity type conflicts check to `dmi_add_extension()`
- Add `dmi_has_extension()` function
- Add tests for command line options parser
- Add tests for command line commands, including check for duplicate option names
- Add support for filtering entities by module
- Add `-M`/`--all-modules` option to `types` command and entity filter
- Add test coverage support for CLI
- Add CI workflows for FreeBSD and NetBSD
- Add tests for output formats
- Add tests for processor information decoder
- Add tests for firmware information decoder
- Add check for duplicate attribute codes to module tests
- Add `dmi_processor_status_name()` function
- Add `dmi_attribute_get_count()` function
- Add UTF-8 validation to JSON output
- Add `DMI_ERROR_INVALID_DUMP` error code
- Add tests for file utilities
- Add tests for loading of corrupted dumps
- Add tests for output format iterators
- Add module tests for static library
- Add `dmi_module_next()` function for iteration over built-in and registered modules
- Add `dmi_stream_has()` function
- Add `dmi_entity_stop()` and `dmi_entity_incomplete()` functions for decoders of extended structures
- Add `DMI_ENTITY_STATE_INCOMPLETE` entity state for structures, which end in the middle of a set of fields
- Add `DMI_ENTITY_STATE_PARTIAL` entity state for structures of older specification versions
- Add compile-time size checks for value unions
- Add tests for portable battery information decoder
- Add `-V`/`--verbose` option to `show` command to show structure versions and states, overriding `-q`/`--quiet` and vice versa
- Add structure states to XML, JSON and YAML output
- Add `dmi_entity_state_names` name set
- Use `less` as default pager on POSIX systems if `PAGER` environment variable is not set
- Use pager for `show`, `list`, `explain`, `entry` and `modules` commands

### Changed

- Replace `--log=<file>` option with `--log-file=<file>`, `-l`/`--log` option now takes no argument
- Rename short form of `--dump` option in `show` and `export` commands from `-u` to `-D`
- Rename `-a`/`--all` option of `lint` command to `-A`/`--all-checks`
- Make argument of `-m`/`--module` option mandatory in `types` command and entity filter
- Accept option arguments starting with dash, as `getopt()` does
- Reject unexpected arguments of commands
- Migrate management controller information decoder to stream API
- Fetch CMocka from release tarball instead of Git repository
- Copy library DLL next to CLI executable on Windows
- Split CI workflow for Linux and macOS into separate workflows
- Change type of string index argument of `dmi_entity_string_ex()` to `size_t`
- Replace nested `status` structure in `dmi_processor_t` with `is_populated` and `status` fields
- Write YAML strings, dates, versions and enumeration values as quoted scalars without tags
- Report files, which are not SMBIOS dumps, with a descriptive error message
- Share iteration over arrays, flags and strings between output formats
- Replace `dmi_modules` list with static `dmi_builtin_modules` array, built-in modules are no longer registered by constructors
- Make `dmi_module_register()` return `bool` and reject modules with duplicate codes
- Write flags of value sets in XML output as `flag` elements with `name` attribute
- Migrate cache, firmware, portable battery, system boot, system event log, chassis, baseboard, processor and memory device information decoders to stream API
- Decode only completely present fields of incomplete sets of fields, and mark such structures as incomplete
- Use common handling of structures extended in newer specification versions in all decoders
- Use the type of raw value for all bit fields of value unions
- Pass output options (mode, raw data dump and pretty output) to output format handlers and entity printing functions
- Remove raw SMBIOS data structures of decoders migrated to stream API
- Use `nullptr` instead of `NULL` in documentation

### Fixed

- Fix SMBIOS 3.x revision number decoding
- Fix SMBIOS 2.1 maximum entity size decoding
- Fix headers fileset in `libopendmi`
- Fix `dmi_registry_iter_init()` return value
- Fix signed numbers handling in `dmi_attribute_format_decimal()`
- Fix potential use-after-free in `dmi_close()`
- Fix handling of CR/LF in dump files on Windows
- Fix entity length check in `dmi_entity_create()`
- Fix memory leak in `dmi_entity_decode_strings()`
- Fix `dmi_registry_get()` behavior on invalid handles
- Fix error queue overflow handling in `dmi_error_slot_get()`
- Fix legacy entry point SMBIOS version decoding
- Fix buffer overrun errors in chassis information decoder
- Fix buffer overrun errors in baseboard information decoder
- Fix cache installed size decoding
- Fix memory device extended size and rank decoding
- Fix memory array extended capacity decoding
- Fix temperature probe accuracy decoding
- Fix voltage probe min/max/nominal values, tolerance, resolution and accuracy decoding
- Fix current probe min/max/nominal values, tolerance, resolution and accuracy decoding
- Fix IPMI device address type decoding
- Fix firmware inventory ID format decoding
- Fix Intel RSD processor CPUID entity specification
- Fix format-truncation warning on GCC 16 #112
- Fix UUID decoding test on big-endian architectures
- Fix command line arguments counting in `dmi_option_parse()`
- Fix handling of optional arguments with short options
- Fix `dmi_option_find_long()` behavior on long-only options
- Fix segmentation fault on `-m`/`-S` command line options
- Fix conflict of `-u` option with entity filter in `show` and `export` commands
- Fix conflict of `-a`/`--all` options with entity filter in `lint` command
- Fix unterminated long option names in `lint` command
- Fix missing error message on unknown type in `-t`/`--type` option
- Fix log file opening error handling
- Fix exit codes of command line tool on output errors
- Fix incomplete writes handling in `dmi_dump_save()` and `export` command
- Fix redundant zero-length calls in `dmi_file_read()` and `dmi_file_write()`
- Fix crash of `entry` command on missing entry point data
- Fix raw output of `types` and `modules` commands when pager is used
- Fix terminal state after interrupting pager with Ctrl-C
- Fix double handle close on pager start errors on Windows
- Fix termination on unknown terminal type
- Fix out-of-bounds read on empty SMBIOS table area
- Fix stack overflow on long date strings in `dmi_date_parse()`
- Fix address type and logging target in FreeBSD and NetBSD backends
- Fix session memory leaks in JSON, YAML and XML output formats
- Fix whole SMBIOS table rejection on a single malformed structure in non-strict mode
- Fix hang and crashes in output formats on structures with more than 255 strings
- Fix unclosed tags in XML output on string output errors
- Fix missing socket status and version in processor information
- Fix duplicate `error-correction` attribute code in memory controller information
- Fix terminal color detection, which never enabled colored log messages and disabled colored output
- Fix reading of array counters narrower than `size_t` in output formats
- Fix false flags in output of sets with 32 or more bits
- Fix invalid UTF-8 in JSON output, invalid bytes are now replaced with U+FFFD #94
- Fix missing firmware features in SMBIOS 2.0 firmware information
- Fix missing extended firmware features in SMBIOS 2.1 to 2.3 firmware information
- Fix invalid UTF-8 and control characters in XML output #94
- Fix YAML values being read as numbers, booleans or dates (e.g. serial number `00000000` or version `2.10`)
- Fix reading of empty files on platforms, where zero-sized allocation fails
- Fix whole SMBIOS table rejection on a structure with invalid length in non-strict mode
- Fix structure count of truncated SMBIOS tables
- Fix missing extension modules when linking with static library
- Fix invalid element names in XML output for flags with codes starting with a digit (e.g. `5v`)
- Fix name of Intel RSD cabled PCIe port information structure
- Fix wrong bit field layout of value unions with MinGW, which made all flags reported as set on Windows
- Fix minimum length of portable battery information
- Fix level of firmware information with two characteristics extension bytes
- Fix cache handles of SMBIOS 2.0 processor information
- Fix bogus rack type and height in chassis information with incomplete SMBIOS 3.9 fields
- Fix `dmi_stream_skip()` not updating remaining size
- Fix unchecked decoding results of system information and Dell revisions structures
- Fix decoding failures of structures with incomplete optional fields, completely present fields are decoded now
- Fix ignored `-q`/`--quiet` option of `show` command, meta-data and handle references are now hidden
- Fix raw color escape sequences shown by `less` pager, `LESS` environment variable now defaults to `FRX`
- Fix missing colors in pager output of `show` command

## [0.3.2] - April 19, 2026

### Added

- Add `dmi_stream_skip()` and `dmi_stream_remaining()` functions
- Add memory module size checks on controller linking

### Changed

- Enable linking in `show` command
- Relax linking of memory controller information
- Normalize firmware language information structure layout
- Migrate firmware language information decoder to stream API
- Migrate memory channel information decoder to stream API
- Migrate memory controller information decoder to stream API
- Migrate memory module information decoder to stream API
- Migrate onboard device information decoder to stream API
- Migrate string property decoder to stream API

### Fixed

- Fix build and CI on Windows #110
- Fix memory module installed/enabled size decoding
- Fix memory module bank count decoding

## [0.3.1] - April 17, 2026

### Added

- Add additional information structure decoding #76
- Add firmware inventory components linking #70
- Add `--dump` option to `show` and `export` commands
- Add regression test script #93

### Changed

- Migrate firmware inventory information decoder to stream API
- Refactor logging engine

### Fixed

- Fix firmware inventory version decoding
- Fix issues found by PVS-Studio
- Fix linking of ICU4C artifacts #79

## [0.3.0] - April 11, 2026

### Added

- Add Windows backend #99
- Add NetBSD backend #97
- Add pager support on Windows
- Add `explain` command implementation
- Add Intel RSD network card information decoder #81
- Add Intel RSD PCIe information decoder #82
- Add Intel RSD storage device information decoder #84
- Add Intel RSD TPM information decoder #85
- Add Intel RSD TXT information decoder #86
- Add Intel RSD memory device information decoder #87
- Add Intel RSD cabled PCIe port information decoder #89
- Add registry status flags support
- Add `dmi_pci_class_t`, `dmi_pci_slot_t`, `dmi_pci_vendor_id_t`, `dmi_pci_device_id_t` types for PCI identifiers
- Add `--compiler` option to `build.sh`
- Add new manual pages:
  - `dmi_base64_decode()`, `dmi_base64_encode()`
  - `dmi_entity_create()`, `dmi_entity_destroy()`
  - `dmi_entity_decode()`, `dmi_entity_link()`
  - `dmi_entity_data()`, `dmi_entity_info()`
  - `dmi_entity_handle()`, `dmi_entity_type()`, `dmi_entity_name()`
  - `dmi_entity_string()`, `dmi_entity_string_ex()`
  - `dmi_error_message()`
  - `dmi_error_get_first()`, `dmi_error_get_last()`
  - `dmi_error_peek_first()`, `dmi_error_peek_last()`
  - `dmi_error_raise()`, `dmi_error_raise_ex()`
  - `dmi_error_clear()`
  - `dmi_file_lock()`, `dmi_file_unlock()`
  - `dmi_file_seek()`, `dmi_file_tell()`, `dmi_file_stat()`
  - `dmi_file_read()`, `dmi_file_write()`
  - `dmi_file_close()`
  - `dmi_stream_initialize()`
  - `dmi_stream_decode()`, `dmi_stream_decode_at()`
  - `dmi_stream_decode_bcd()`, `dmi_stream_decode_bcd_at()`
  - `dmi_stream_decode_str()`, `dmi_stream_decode_str_at()`
  - `dmi_stream_decode_uuid()`, `dmi_stream_decode_uuid_at()`
  - `dmi_stream_is_done()`
  - `dmi_stream_read()`, `dmi_stream_read_at()`
  - `dmi_stream_read_data()`, `dmi_stream_read_data_at()`
  - `dmi_stream_seek()`
  - `dmi_stream_reset()`
  - `dmi_uuid_decode()`, `dmi_uuid_encode()`
  - `dmi_version_format()`, `dmi_version_format_ex()`
- Compress manual pages after creation

### Changed

- Disable `--no-sysfs` option on non-Linux systems
- Disable `--path` option on Windows systems

### Fixed

- Fix crash on truncated SMBIOS data #103
- Fix context creation on truncated SMBIOS data #103
- Fix minor CppCheck warnings
- Fix minor AsciiDoctor warnings
- Fix manual page name sections formatting

## [0.2.3] - March 25, 2026

### Added

- Add memory channel device linking #73
- Add colorization of boolean values
- Add YAML schema validation on regression tests

### Changed

- Migrate out-of-band remote access decoder to stream API
- Migrate system configuration options decoder to stream API
- Migrate system reset information decoder to stream API
- Migrate system information decoder to stream API
- Migrate temperature/voltage/current probe decoders to stream API
- Migrate TPM device characteristics decoder to stream API

### Fixed

- Fix binary strings output in YAML export
- Fix unknown entity level output in YAML export
- Fix address attributes display format #66
- Fix `entry` command exit code
- Fix table area address display format

## [0.2.2] - March 20, 2026

### Added

- Add `entry` command implementation

### Fixed

- Update outdated SMBIOS data indices
- Fix handling of incorrect UTF-8 sequences in YAML export
- Fix missing tag for binary data in YAML export

## [0.2.1] - March 17, 2026

### Added

- Add YAML emitter error handling
- Add type tags output in YAML export
- Add version number to entry point specification
- Store entry point specification in context
- Add context address size detection
- Add binary data output in JSON export
- Add binary data output in YAML export
- Add raw strings output in JSON export
- Add raw strings output in YAML export
- Add Base64 encoding/decoding functions to utils
- Add Lenovo ThinkCentre M720q 10T8 data

### Changed

- Use literal style for binary data in YAML export

### Fixed

- Fix `opendmi` invocation in reindex script
- Fix error counting in reindex script
- Fix build without YAML/JSON/XML libraries
- Fix CHANGELOG format

## [0.2.0] - March 16, 2026

### Added

- Add all-new command line interface
- Add AMI, Intel and Sun extension skeletons
- Add FreeBSD backend
- Add JSON output support
- Add SMBIOS data reindexing tool
- Add logging to files
- Add structure levels output in YAML dumps
- Add autoloading of vendor-specific extensions
- Add new structure decoders:
  - AMI type 221 (reverse-engineered, partial)
  - Dell revisions and IDs (type 208)
  - Dell parallel port (type 209)
  - Dell serial port (type 210)
  - Dell infrared port (type 211)
- Add specifications for OEM-specific structures:
  - Dell revisions and IDs (type 208)
  - Dell parallel port (type 209)
  - Dell serial port (type 210)
  - Dell infrared port (type 211)
- Complete system slots decoder
- Add memory controller structure linking
- Add system boot status decoding
- Add support for enumeration range names
- Add reference manual skeleton
- Add automated manual pages generation via AsciiDoctor #7
- Add new manual pages:
  - `dmi_memory_array_addr`
  - `dmi_pointing_device`
  - `dmi_cooling_device`
- Add `--prefix` option to `build.sh` #77
- Add `install` command to `build.sh` #78
- Add test coverage build

### Changed

- Migrate to CMocka for testing
- Rename `dmi_pointing_device_interface_t` to `dmi_pointing_device_iface_t`
- Move `<opendmi/name.h>` to `<opendmi/utils/name.h>`
- Refactor entity decoders API

### Fixed

- Fix build on FreeBSD platform
- Fix firmware inventory components list decoding
- Fix portable battery SBDS manufacture date decoding
- Fix display of end-of-table structure
- Fix Centronics connector naming
- Fix management protocol naming
- Fix zero handle linking behavior
- Fix group association items list decoding

## [0.1.4] - January 13, 2026

### Fixed

- Fix memory controller module size decoding
- Fix hardware security settings decoding
- Fix power supply structure minimum length
- Fix cooling device structure minimum length
- Fix current probe structure minimum length
- Fix temperature probe structure minimum length
- Fix voltage probe structure minimum length
- Fix baseboard structure minimum length

## [0.1.3] - January 11, 2026

### Added

- Add automatic string trimming
- Add baseboard structure linking
- Add physical memory array structure linking
- Add power supply structure linking

### Fixed

- Fix invalid memory access in `dmi_entity_string_ex()`
- Fix memory leak in memory channel decoder
- Fix memory leak in context error queue

## [0.1.2] - January 09, 2026

### Added

- Add `--type` option for filtering by type
- Add tests for `dmi_date_parse()`

### Changed

- Downgrade CMake to version 3.25 to improve compatibility

### Fixed

- Fix `dmi_file_read()` behavior on SysFS
- Fix `dmi_file_map()` error handling
- Fix firmware ROM size decoding
- Fix platform firmware version decoding
- Fix embedded controller firmware version decoding
- Fix processor socket type decoding
- Fix cache extended maximum and installed sizes decoding
- Fix memory array extended capacity decoding
- Fix memory array mapping extended addresses decoding
- Fix memory device mapping extended addresses decoding
- Fix cooling device description decoding
- Fix portable battery SBDS version decoding
- Fix system event log structure version decoding

## [0.1.1] - January 07, 2026

### Added

- Add colors for unknown and unspecified values
- Add display of structure types in handle arrays
- Add new manual pages:
  - `dmi_bswap16()`
  - `dmi_bswap32()`
  - `dmi_bswap64()`
- Mark unspecified values for IPMI device interrupt trigger mode and polarity

### Changed

- Unify date handling via `dmi_date_t` type:
  - Use `dmi_date_t` for firmware release date
  - Use `dmi_date_t` for portable battery manufacture date
  - Remove field `sbds_manufacture_date` from portable battery descriptor

### Fixed

- Fix baseboard features set decoding
- Fix battery SBDS manufacture date decoding
- Fix IPMI device non-volatile storage address decoding
- Fix incorrect decoding of incomplete structures:
  - system information (`dmi_system_t`)
  - baseboard (`dmi_baseboard_t`)
  - processor (`dmi_processor_t`)
  - memory device (`dmi_memory_device_t`)
  - system enclosure or chassis (`dmi_chassis_t`)
- Fix incorrect address display formats:
  - firmware BIOS segment address
  - memory array starting and ending addresses
  - memory device starting and ending addresses
  - memory error address
  - management device address
- Fix formatting of signed integers smaller than `int`

## [0.1] - January 02, 2026

- First public release
- Full support for SMBIOS specification up to version 3.9
- Basic implementation of `libopendmi` library with Linux, macOS and dump file backends
- Basic implementation of `opendmi` command line tool with XML and YAML output formats and color output support
