#!/usr/bin/env python3
#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
"""Rebuild a binary SMBIOS dump out of the text output of `dmidecode -u`.

`dmidecode -u` (`--dump`) prints the bytes of every structure along with its
decoded form: the formatted area as "Header and Data", and each string as its
bytes, the terminating zero included, followed by its text. This is enough to
rebuild the table byte for byte. The entry point is not printed, so it is
made anew, of the kind and the version the "SMBIOS x.y[.z] present" line
tells: a 64-bit one for three parts of the version, a 32-bit one otherwise.

The dump is written in the layout of `dmidecode --dump-bin`, which OpenDMI
reads: the entry point padded to 32 bytes, followed by the table.

Plain `dmidecode` output, which prints the bytes of the structures it cannot
decode only, and their strings as text, is refused, since the table cannot be
rebuilt out of it exactly.
"""

import argparse
import os
import re
import struct
import sys

DUMP_HEADER_SIZE = 0x20

RE_VERSION   = re.compile(r'^SMBIOS (\d+)\.(\d+)(?:\.(\d+))? present\.?$')
RE_HANDLE    = re.compile(r'^Handle 0x([0-9A-Fa-f]{4}), DMI type (\d+), (\d+) bytes?$')
RE_HEX_ROW   = re.compile(r'^[0-9A-Fa-f]{2}(?: [0-9A-Fa-f]{2}){0,15}$')
RE_DMIDECODE = re.compile(r'^# dmidecode ')


class DumpError(Exception):
    pass


class Structure:
    def __init__(self, handle: int, type: int, length: int, line: int):
        self.handle  = handle
        self.type    = type
        self.length  = length
        self.line    = line
        self.data      = None   # Formatted area, header included
        self.strings   = None   # Strings, each with its terminating zero
        self.truncated = False  # Cut short by the length of the table
        self.text_only = False  # Strings given as text, with no bytes

    def encode(self) -> bytes:
        strings = b''.join(self.strings) if self.strings else b'\0'
        return bytes(self.data) + strings + b'\0'


class Table:
    def __init__(self, line: int):
        self.line       = line
        self.version    = None
        self.structures = []


def parse_hex_row(text: str) -> bytes | None:
    if not RE_HEX_ROW.match(text):
        return None
    return bytes(int(x, 16) for x in text.split())


def parse(lines: list[str]) -> list[Table]:
    """Split the output into tables, and the tables into structures."""
    tables = []
    table = None
    current = None
    section = None      # "data", "strings" or None
    string = None       # Bytes of the string being read, or None between strings
    text_due = False    # Text of the string which has just been read is due

    def new_table(number):
        nonlocal table, current, section
        table = Table(number)
        tables.append(table)
        current = None
        section = None

    for number, raw in enumerate(lines, start=1):
        line = raw.strip()

        # Several runs of dmidecode may be concatenated
        if RE_DMIDECODE.match(line):
            new_table(number)
            continue

        match = RE_VERSION.match(line)
        if match:
            if (table is None) or table.structures or (table.version is not None):
                new_table(number)
            parts = tuple(int(x) for x in match.groups() if x is not None)
            table.version = parts
            continue

        match = RE_HANDLE.match(line)
        if match:
            if table is None:
                new_table(number)
            current = Structure(int(match.group(1), 16), int(match.group(2)),
                                int(match.group(3)), number)
            table.structures.append(current)
            section = None
            continue

        if current is None:
            continue

        # Structure past the end of the table the entry point declares
        if line == '<TRUNCATED>':
            current.truncated = True
            continue

        if line == 'Header and Data:':
            section = 'data'
            current.data = bytearray()
            continue

        if line == 'Strings:':
            section = 'strings'
            current.strings = []
            string = None
            text_due = False
            continue

        if section == 'data':
            row = parse_hex_row(line)
            if row is not None:
                current.data += row
                continue
            section = None

        elif section == 'strings':
            # Text of a string follows its bytes, and is skipped
            if text_due:
                text_due = False
                continue

            row = parse_hex_row(line)
            if row is None:
                # Strings given as text only are the ones of plain output
                if not current.strings and string is None:
                    current.text_only = True
                section = None
                continue

            string = (string or b'') + row
            if 0 in string[:-1]:
                raise DumpError(f'line {number}: string of handle 0x{current.handle:04X} '
                                'holds a zero before its end')
            if string.endswith(b'\0'):
                current.strings.append(string)
                string = None
                text_due = True

    return [t for t in tables if t.structures]


def fixed_up(s: Structure) -> bool:
    """Tell a management device (type 34) whose length dmidecode has fixed up.

    Some firmware gives the structure a length of 0x10 instead of 0x0B, and
    dmidecode takes the bytes past 0x0B, if printable, for the beginning of
    the first string, so it prints 0x0B bytes of data and the rest along with
    the strings. The bytes of the structure are the same, since they follow
    each other, so the structure is rebuilt by joining them again.
    """
    return ((s.type == 34) and (s.length == 0x10) and (len(s.data) == 0x0B) and
            (s.data[1] == 0x10) and (len(b''.join(s.strings or [])) > 0x10 - 0x0B))


def check(table: Table, partial: bool) -> list[str]:
    """Check the structures of a table, returning the warnings."""
    warnings = []
    plain = [s for s in table.structures if s.text_only]
    if plain or all(s.data is None for s in table.structures if s.type in (0, 1)):
        raise DumpError('input is the output of plain dmidecode, which prints the bytes of '
                        'the structures it cannot decode only, and their strings as text; '
                        'run dmidecode -u')

    missing = [s for s in table.structures if s.data is None]
    truncated = [s for s in missing if s.truncated]

    if truncated and len(truncated) == len(missing):
        handles = ', '.join(f'0x{s.handle:04X}' for s in truncated)
        message = (f'structures {handles} run past the end of the table the entry point '
                   'declares, and dmidecode prints no bytes of them')
        if not partial:
            raise DumpError(message)
        warnings.append(message + '; they are left out')
        table.structures = [s for s in table.structures if s.data is not None]
        missing = []

    if missing:
        handles = ', '.join(f'0x{s.handle:04X}' for s in missing[:8])
        more = f' and {len(missing) - 8} more' if len(missing) > 8 else ''
        message = (f'{len(missing)} structures carry no bytes ({handles}{more}), '
                   'the output is not the one of dmidecode -u')
        if not partial:
            raise DumpError(message)
        warnings.append(message + '; they are left out')
        table.structures = [s for s in table.structures if s.data is not None]

    for s in table.structures:
        where = f'line {s.line}: handle 0x{s.handle:04X}'

        if len(s.data) != s.length and not fixed_up(s):
            raise DumpError(f'{where}: {len(s.data)} bytes of data, while the header line says {s.length}')
        if s.length < 4:
            raise DumpError(f'{where}: structure of {s.length} bytes is shorter than its header')

        header_type, header_length, header_handle = s.data[0], s.data[1], struct.unpack_from('<H', s.data, 2)[0]
        if (header_type, header_length, header_handle) != (s.type, s.length, s.handle):
            raise DumpError(f'{where}: header bytes say type {header_type}, {header_length} bytes, '
                            f'handle 0x{header_handle:04X}')

    types = {s.type for s in table.structures}
    if 127 not in types:
        warnings.append('table has no end-of-table structure (type 127), it may be cut short '
                        'or filtered by type')
    if not {0, 1} <= types:
        warnings.append('table lacks the firmware (type 0) or the system (type 1) information, '
                        'it may be filtered by type')

    return warnings


def entry_point(version: tuple, table: bytes, structures: list[Structure]) -> bytes:
    """Make an entry point pointing to the table right past the dump header."""
    if len(version) == 3:
        ep = bytearray(0x18)
        ep[0:5] = b'_SM3_'
        ep[6] = 0x18
        ep[7], ep[8], ep[9] = version
        ep[0x0A] = 1                                        # Entry point revision
        struct.pack_into('<I', ep, 0x0C, len(table))
        struct.pack_into('<Q', ep, 0x10, DUMP_HEADER_SIZE)
        ep[5] = (-sum(ep)) & 0xFF
        return bytes(ep)

    if len(table) > 0xFFFF:
        raise DumpError(f'table of {len(table)} bytes does not fit a 32-bit entry point')

    major, minor = version
    largest = max(len(s.encode()) for s in structures)

    ep = bytearray(0x1F)
    ep[0:4] = b'_SM_'
    ep[5] = 0x1F
    ep[6], ep[7] = major, minor
    struct.pack_into('<H', ep, 0x08, largest)
    ep[0x10:0x15] = b'_DMI_'
    struct.pack_into('<H', ep, 0x16, len(table))
    struct.pack_into('<I', ep, 0x18, DUMP_HEADER_SIZE)
    struct.pack_into('<H', ep, 0x1C, len(structures))
    ep[0x1E] = ((major & 0x0F) << 4) | (minor & 0x0F)
    ep[0x15] = (-sum(ep[0x10:0x1F])) & 0xFF
    ep[0x04] = (-sum(ep)) & 0xFF
    return bytes(ep)


def build(table: Table, version: tuple | None) -> bytes:
    version = version or table.version
    if version is None:
        raise DumpError('no "SMBIOS x.y present" line tells the version, give it with --smbios-version')

    data = b''.join(s.encode() for s in table.structures)
    ep = entry_point(version, data, table.structures)

    return ep.ljust(DUMP_HEADER_SIZE, b'\0') + data


def parse_version(text: str) -> tuple:
    match = re.fullmatch(r'(\d+)\.(\d+)(?:\.(\d+))?', text)
    if not match:
        raise argparse.ArgumentTypeError(f'invalid version: {text}')
    return tuple(int(x) for x in match.groups() if x is not None)


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(
        description='Rebuild a binary SMBIOS dump out of the output of dmidecode -u.')
    parser.add_argument('input', help='text output of dmidecode -u, or - for the standard input')
    parser.add_argument('-o', '--output', help='binary dump to write (default: input with .bin)')
    parser.add_argument('-t', '--table', type=int, default=None,
                        help='number of the table to take, from 1, when the input holds several')
    parser.add_argument('-V', '--smbios-version', type=parse_version,
                        help='SMBIOS version, e.g. 3.2.0 or 2.7, when the input tells none, or to '
                             'override it; three parts make a 64-bit entry point')
    parser.add_argument('-p', '--partial', action='store_true',
                        help='leave out the structures which carry no bytes instead of failing')
    parser.add_argument('-f', '--force', action='store_true', help='overwrite the output file')
    args = parser.parse_args(argv)

    try:
        if args.input == '-':
            text = sys.stdin.read()
        else:
            with open(args.input, encoding='utf-8', errors='replace') as file:
                text = file.read()

        tables = parse(text.splitlines())
        if not tables:
            raise DumpError('no structures found')

        if args.table is None:
            if len(tables) > 1:
                raise DumpError(f'input holds {len(tables)} tables, choose one with --table')
            table = tables[0]
        elif 1 <= args.table <= len(tables):
            table = tables[args.table - 1]
        else:
            raise DumpError(f'no table {args.table}, the input holds {len(tables)}')

        for warning in check(table, args.partial):
            print(f'warning: {warning}', file=sys.stderr)

        dump = build(table, args.smbios_version)

        output = args.output
        if output is None:
            if args.input == '-':
                raise DumpError('give the output file with --output')
            output = os.path.splitext(args.input)[0] + '.bin'

        if os.path.exists(output) and not args.force:
            raise DumpError(f'{output} exists, use --force to overwrite it')

        with open(output, 'wb') as file:
            file.write(dump)

    except (DumpError, OSError) as e:
        print(f'error: {e}', file=sys.stderr)
        return 1

    version = '.'.join(str(x) for x in (args.smbios_version or table.version))
    print(f'{output}: SMBIOS {version}, {len(table.structures)} structures, '
          f'{len(dump) - DUMP_HEADER_SIZE} bytes')

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
