# OpenDMI

[![Linux](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-linux.yml) [![FreeBSD](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-freebsd.yml/badge.svg)](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-freebsd.yml) [![NetBSD](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-netbsd.yml/badge.svg)](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-netbsd.yml) [![macOS](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-macos.yml/badge.svg)](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-macos.yml) [![Windows](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-windows.yml)

OpenDMI reads DMI/SMBIOS data and gives it back as structured data instead of
text to parse: a C library, command line tools, machine-readable output and
bindings for Go, Python and Rust. Every field has a code, a type and a unit, so
the same data reads the same way in the terminal, in JSON and through the API.

The project is under active development, see [ROADMAP](ROADMAP.md) and
[CHANGELOG](CHANGELOG.md) for details.

## At a glance

Structures are decoded field by field, including the ones other tools leave as
raw bytes:

```console
$ opendmi show -t processor
Processor information
	Socket Designation: U3E1
	Type: Central processor
	Family: Intel Core i5
	Vendor: Intel(R) Corporation
	ID: A4 06 09 00 FF FB EB BF
	Signature:
		Type: 0
		Family: 6
		Model: 154
		Stepping: 4
	Flags: 0xBFEBFBFF
		FPU (floating-point unit on-chip): yes
		VME (virtual mode extension): yes
		...
```

The same structure in JSON, with stable codes instead of human-readable labels:

```console
$ opendmi export -f json --pretty -t processor
```
```json
{
    "handle": "0x001a",
    "type": 4,
    "description": "Processor information",
    "attributes": {
        "socket-designation": "U3E1",
        "type": "central",
        "family": "intel-core-i5",
        "vendor": "Intel(R) Corporation",
        "id": "a4060900fffbebbf",
        "signature": {
            "type": "0",
            "family": "6",
            "model": "154",
            "stepping": "4"
        },
        "flags": {
            "fpu": true,
            "vme": true
        }
    }
}
```

The same data from C, with decoded structures mapped to plain C types:

```c
#include <opendmi/context.h>
#include <opendmi/entity/system.h>

dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_LINK);

if (dmi_open(context, nullptr)) {
    dmi_registry_t *registry = context->state.registry;
    dmi_entity_t   *entity   = dmi_registry_get_first(registry, DMI_TYPE(SYSTEM), false);
    dmi_system_t   *system   = dmi_entity_info(entity, DMI_TYPE(SYSTEM));

    if (system != nullptr)
        printf("%s %s\n", system->vendor, system->product);
}

dmi_destroy(context);
```

## Why OpenDMI

* **Data, not text.** Tools built on `dmidecode` parse its human-readable
  output. OpenDMI provides a C library, bindings and JSON, XML and YAML output,
  where every field is addressed by a stable code.
* **Structures are linked together.** Handles are resolved into references,
  string properties (type 46) are attached to the structures they describe, and
  additional information (type 40) is applied as an overlay. Other decoders
  print raw handles and leave the rest to the reader.
* **Broken firmware does not stop decoding.** Malformed structures are reported
  and skipped instead of ending the table, each structure carries its state
  (`decoded`, `incomplete`, `partial`), errors are collected in a queue, and a
  strict mode is available when correctness matters more than completeness.
  Known firmware quirks, such as swapped processor ID words or reversed TPM
  vendor identifiers, are handled.
* **OEM structures are first-class.** Vendor-specific structures are decoded by
  extension modules for Acer, AMI, Apple, Dell, HP/HPE, Intel, IBM/Lenovo and
  Sun, and their layouts are described in the reference manual.
* **Cross-platform by design.** Linux, FreeBSD, NetBSD, macOS and Windows are
  supported by separate backends, along with dump files and a generic fallback.
* **Compatible with existing dumps.** Dump files are interchangeable with
  `dmidecode --dump-bin` and `--from-dump`, in both directions.
* **Checked against real data.** Decoders are tested against a collection of
  dumps taken from real machines, with continuous integration on five platforms
  and static analysis on every change.

## Features

* All standard structures of SMBIOS 3.9 are decoded field by field.
* Values are decoded to their meaning: enumerations, bit sets, units and
  versions, with formats selected by the fields that define them.
* Text, JSON, XML and YAML output, with filtering by handle, type, module and
  structure state.
* Modular extensions for OEM-specific structures.
* C and C++ API, bindings for Go, Python and Rust, a D-Bus service and a Linux
  kernel module exposing SMBIOS data via SysFS — see
  [Components](#components) for what is ready today.
* Small footprint, no external dependencies in the library itself.

## Components

| Component | Description | Status |
|---|---|---|
| `libopendmi` | C/C++ library providing direct interface to DMI/SMBIOS | Usable, API not frozen until 1.0 |
| `opendmi` | Command line tool to query DMI/SMBIOS data | Usable, `lint` and `import` commands are on the way |
| `libopendmi-python` | Python bindings for `libopendmi` | In progress |
| `opendmi-dbus` | D-Bus service providing access to DMI/SMBIOS data | In progress |
| `opendmi-sysfs` | Linux kernel module providing DMI/SMBIOS data via SysFS | Planned |
| `libopendmi-go` | Go bindings for `libopendmi` | Planned |
| `libopendmi-rust` | Rust bindings for `libopendmi` | Planned |

Everything is scheduled for 1.0, see [ROADMAP](ROADMAP.md) for the order.

## Getting started

Build and install OpenDMI from sources:

```sh
$ ./build.sh configure --prefix /usr/local
$ ./build.sh build
$ ./build.sh install
```

Then read the data of the local system:

```sh
$ opendmi show
```

See [Building from sources](#building-from-sources) for configuration options,
optional components and packaging.

## Usage

The `opendmi` command line tool provides access to DMI/SMBIOS data. The general
usage pattern is:

```sh
$ opendmi [global options] <command> [command options]
```

### Global options

| Option | Description |
|---|---|
| `-v`, `--version` | Print version information and exit |
| `-h`, `--help` | Print help and exit |
| `-i <path>`, `--file=<path>` | Read DMI data from a binary file instead of the system |
| `-d <path>`, `--device=<path>` | Set path to memory device (default: `/dev/mem`) |
| `-m <module>`, `--module=<module>` | Enable the specified module |
| `-O`, `--overlay` | Apply additional information entries (type 40) to structures |
| `-l`, `--log` | Enable logging to the terminal |
| `--log-file=<path>` | Enable logging to a file |
| `-L <level>`, `--log-level=<level>` | Set logging level |

Values of options are specified either as a separate argument (`-i <path>`,
`--file <path>`) or attached to the option (`-i<path>`, `--file=<path>`).

### Commands

| Command | Description |
|---|---|
| `show` | Show SMBIOS structures data |
| `list` | List SMBIOS structures |
| `entry` | Show SMBIOS entry point data |
| `types` | List SMBIOS structure types |
| `explain` | Explain an SMBIOS structure type |
| `export` | Export SMBIOS data to external format (JSON, XML, YAML) |
| `dump` | Dump the entire SMBIOS table to a binary file |
| `import` | Import SMBIOS data from an external format * |
| `lint` | Check SMBIOS structures for errors * |
| `modules` | List available modules |

Use `opendmi <command> --help` for detailed information on a specific command.
Commands marked with an asterisk are on the way.

#### `show`

Show SMBIOS structures data in human-readable form. Structures are selected with
[filter options](#filter-options).

| Option | Description |
|---|---|
| `-q`, `--quiet` | Hide meta-data and handle references |
| `-V`, `--verbose` | Show structure versions and states |
| `-D`, `--dump` | Show raw structure data instead of decoded fields |

#### `list`

List SMBIOS structures with their handles and types. Structures are selected
with [filter options](#filter-options).

| Option | Description |
|---|---|
| `-r`, `--raw` | Raw output (default if standard output is a pipe) |

#### `entry`

Show SMBIOS entry point data. The command has no options.

#### `types`

List SMBIOS structure types. Only standard types are listed by default.

| Option | Description |
|---|---|
| `-m <module>`, `--module=<module>` | List types provided by the specified module |
| `-M`, `--all-modules` | List types provided by all modules |
| `-a`, `--all` | List all available types |
| `-r`, `--raw` | Raw output (default if standard output is a pipe) |

#### `explain`

Explain an SMBIOS structure type, given by its number or code:
`opendmi explain [--] <type>`. The command has no options.

#### `export`

Export SMBIOS data to external format. Structures are selected with
[filter options](#filter-options).

| Option | Description |
|---|---|
| `-o <path>`, `--output=<path>` | Set output file path (default: standard output) |
| `-f <format>`, `--format=<format>` | Set output format: `text`, `json`, `xml` or `yaml` (default: `yaml`) |
| `-D`, `--dump` | Export raw structure data instead of decoded fields |
| `-p`, `--pretty` | Enable pretty output |
| `-F`, `--force` | Overwrite existing files |

#### `dump`

Dump the entire SMBIOS table to a binary file, compatible with
`dmidecode --from-dump`.

| Option | Description |
|---|---|
| `-o <path>`, `--output=<path>` | Set output file path (default: `smbios.bin`) |
| `-F`, `--force` | Overwrite existing files |

#### `modules`

List available modules.

| Option | Description |
|---|---|
| `-r`, `--raw` | Raw output (default if standard output is a pipe) |

#### Filter options

The `show`, `list` and `export` commands show standard and OEM-specific
structures by default, excluding inactive and unknown ones. Filter options
select structures to show.

| Option | Description |
|---|---|
| `-H <handle>`, `--handle=<handle>` | Only show structures with the given handle(s) |
| `-t <type>`, `--type=<type>` | Only show structures of the given type(s), by number or code |
| `-s`, `--standard` | Show standard structures |
| `-S`, `--no-standard` | Don't show standard structures |
| `-e`, `--oem` | Show OEM-specific structures |
| `-E`, `--no-oem` | Don't show OEM-specific structures |
| `-i`, `--inactive` | Show inactive structures |
| `-I`, `--no-inactive` | Don't show inactive structures |
| `-u`, `--unknown` | Show unknown structures |
| `-U`, `--no-unknown` | Don't show unknown structures |
| `-m <module>`, `--module=<module>` | Show structures provided by the specified enabled module |
| `-M`, `--all-modules` | Show structures provided by all enabled modules |
| `-a`, `--all` | Show all structures |

Output of text commands (e.g. `show` or `types`) is shown through the pager set by
the `PAGER` environment variable, or `less` if it is not set. Unless the `LESS`
environment variable is set, `less` prints output that fits on one screen
directly. Set `PAGER` to an empty value to disable the pager.

### Examples

Show all SMBIOS structures in human-readable format:

```sh
$ opendmi show
```

Show structures of a specific type only:

```sh
$ opendmi show -t processor
```

Show structure versions and decoding states (for example, `incomplete` for
structures, which do not match any specification version):

```sh
$ opendmi show -V
```

Enable the Dell extension module, so that its structures are decoded:

```sh
$ opendmi -m dell show
```

Show structures provided by the enabled module only, hiding the rest:

```sh
$ opendmi -m dell show -m dell
```

List all structures with their handles and types:

```sh
$ opendmi list
```

Show SMBIOS entry point information:

```sh
$ opendmi entry
```

Explain a specific structure type:

```sh
$ opendmi explain processor
```

Export SMBIOS data to YAML (default format):

```sh
$ opendmi export -o smbios.yaml
```

Export SMBIOS data to JSON with pretty-printing:

```sh
$ opendmi export -f json --pretty -o smbios.json
```

Dump the raw SMBIOS table to a binary file:

```sh
$ opendmi dump -o smbios.bin
```

Read SMBIOS data from a previously saved dump:

```sh
$ opendmi -i smbios.bin show
```

## Building from sources

### Prerequisites

* GCC, CLang or MSVC compiler
* CMake 3.25 or newer
* AsciiDoctor (used to generate documentation and man pages)
* CppCheck (optional)

### Dependencies

#### opendmi

* `libxml2` — optional, only when XML support is enabled
* `libyaml` — optional, only when YAML support is enabled
* `libyajl` — optional, only when JSON support is enabled

#### libopendmi

* None

### Configuring

Use the following command to configure OpenDMI build:

```sh
$ ./build.sh configure
```

This produces a release build by default. Pass `--debug` to configure a debug
build (optimization disabled, debug assertions included):

```sh
$ ./build.sh configure --debug
```

To set the installation prefix:

```sh
$ ./build.sh configure --prefix /usr/local
```

To enable optional components:

```sh
$ ./build.sh configure --enable-dbus --enable-python
```

To enable all optional components at once:

```sh
$ ./build.sh configure --enable-all
```

To enable optional format support:

```sh
$ ./build.sh configure --with-json --with-xml --with-yaml
```

For all configuration options, see usage:

```sh
$ ./build.sh --help
```

To override defaults persistently without modifying `build.conf.dist`, create a
`build.conf` file in the project root — settings there take precedence.

### Building

Use the following command to build OpenDMI:

```sh
$ ./build.sh build
```

To build using a specific number of parallel jobs:

```sh
$ ./build.sh -j 4 build
```

To use a custom build directory:

```sh
$ ./build.sh -b /tmp/opendmi-build build
```

### Testing

OpenDMI uses the CTest framework for testing. You can simply run it to ensure
that build was successful:

```sh
$ ./build.sh test
```

To re-run only the failed tests:

```sh
$ ./build.sh test --failed
```

### Installing

Use the following command to install OpenDMI to the configured prefix:

```sh
$ ./build.sh install
```

### Using the library

The installed library can be found with CMake:

```cmake
find_package(OpenDMI 0.5 REQUIRED)
target_link_libraries(example PRIVATE OpenDMI::opendmi)
```

`OpenDMI::opendmi` and `OpenDMI::shared` refer to the shared library, and
`OpenDMI::static` refers to the static one. Binary compatibility is not
guaranteed between minor versions before 1.0, so the package is only
compatible with the same minor version.

The library can also be found with `pkg-config`:

```sh
$ cc example.c $(pkg-config --cflags --libs opendmi)
```

### Packaging

Use the following command to build distributable packages using CPack:

```sh
$ ./build.sh package
```

### Cleaning

To remove build artifacts without removing the configuration:

```sh
$ ./build.sh clean
```

To remove the entire build directory including the configuration:

```sh
$ ./build.sh distclean
```

## Contributing

The project is open to contributions. Please feel free to test it and create bug
reports, feature requests or pull requests on GitHub. You can also help the
project by sending SMBIOS dumps to the authors. To get the dump you can use the
following command:

```sh
$ dmidecode --dump-bin <filename>
```

Or, using OpenDMI itself:

```sh
$ opendmi dump -o <filename>
```

See [CONTRIBUTING](CONTRIBUTING.md) for more details. A full list of contributors
who helped the project so far can be found in the [CONTRIBUTORS](CONTRIBUTORS.md).

## Licensing

OpenDMI is licensed under BSD 3-clause license. See [LICENSE](LICENSE.md) for details. It
uses parts of some third-party libraries that are distributed under their own
terms (see [LICENSE-3RD-PARTY](LICENSE-3RD-PARTY.md)).

## SAST Tools

[PVS-Studio](https://pvs-studio.com/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C, C++, C#, and Java code.
