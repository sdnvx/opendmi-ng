# OpenDMI

[![Linux](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-linux.yml) [![FreeBSD](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-freebsd.yml/badge.svg)](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-freebsd.yml) [![NetBSD](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-netbsd.yml/badge.svg)](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-netbsd.yml) [![macOS](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-macos.yml/badge.svg)](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-macos.yml) [![Windows](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/sdnvx/opendmi-ng/actions/workflows/cmake-windows.yml)

OpenDMI is a cross-platform, commercial-grade DMI/SMBIOS framework, focused on functionality and ease of use. It provides direct access to all of the DMI/SMBIOS data, command line tools, bindings for major programming languages and even more.

The project is under active development, see [ROADMAP](ROADMAP.md) and [CHANGELOG](CHANGELOG.md) for details.

## Highlights

* Platform-agnostic access to DMI/SMBIOS structures.
* Full SMBIOS support up to version 3.9.
* Bindings for Python, Go and Rust languages.
* Modular extensions for handling OEM-specific structures.
* JSON, XML and YAML output support for automation purposes.
* Works on Linux, MacOS, Free/Net BSD and Windows platforms.
* Small footprint, no external dependencies.

## Components

* `opendmi` - Command line tool to query DMI/SMBIOS data
* `opendmi-dbus` - D-bus service providing access to DMI/SMBIOS data
* `libopendmi` - C library providing direct interface to DMI/SMBIOS
* `libopendmi-go` - Go bindings for `libopendmi`
* `libopendmi-python` - Python bindings for `libopendmi`
* `libopendmi-rust` - Rust bindings for `libopendmi`

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
| `modules` | List available modules |

Use `opendmi <command> --help` for detailed information on a specific command.

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

Show structures provided by the Dell extension module only:

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
