#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
import os
import re
import hashlib
import sys
import logging
import subprocess
import yaml
import yaml.loader

# Resolve directory names
tool_dir = os.path.dirname(os.path.realpath(__file__))
base_dir = os.path.abspath(os.path.join(tool_dir, '..'))
data_dir = os.path.join(base_dir, "data")
build_dir = os.path.join(base_dir, "build/opendmi")

num_updated = 0
num_skipped = 0
num_orphans = 0
num_errors  = 0

# Dumps by the checksums of their data, which tell the duplicates
checksums = {}

yaml.SafeLoader.yaml_implicit_resolvers = {
    k: [r for r in v if r[0] != 'tag:yaml.org,2002:timestamp'] for
    k, v in yaml.SafeLoader.yaml_implicit_resolvers.items()
}

def vendor_dirs():
    for entry in os.scandir(data_dir):
        if not entry.is_dir():
            continue
        yield entry.path

def data_files(vendor_dir: str):
    for entry in os.scandir(vendor_dir):
        if not entry.is_file():
            continue

        _, extension = os.path.splitext(entry.path)
        if extension != '.bin':
            continue

        yield entry.path

def update_checksum(file_path: str):
    """Write the checksum of a dump next to it, in the format of sha256sum."""
    with open(file_path, 'rb') as dump_file:
        digest = hashlib.sha256(dump_file.read()).hexdigest()

    checksums.setdefault(digest, []).append(os.path.relpath(file_path, data_dir))

    base, _ = os.path.splitext(file_path)
    line = f"{digest}  {os.path.basename(file_path)}\n"

    try:
        with open(base + ".sha256") as checksum_file:
            if checksum_file.read() == line:
                return
    except FileNotFoundError:
        pass

    with open(base + ".sha256", 'w') as checksum_file:
        checksum_file.write(line)

def load_index(index_path: str):
    with open(index_path) as index_file:
        return yaml.load(index_file, Loader=yaml.loader.SafeLoader)

def get_data_entity(data: dict, type: int) -> dict:
    for entry in data['table']:
        if entry['type'] == type:
            return entry['attributes']

def get_firmware_info(data: dict) -> dict:
    return get_data_entity(data, 0)

def get_system_info(data: dict) -> dict:
    return get_data_entity(data, 1)

def get_type_map(data: dict) -> dict:
    type_map = {}
    for entry in data['table']:
        entry_handle = entry['handle']
        entry_type   = entry['type']
        entry_descr  = entry['description']

        if entry_type in type_map:
            type_map[entry_type]['count'] += 1
            type_map[entry_type]['handles'].append(entry_handle)
        else:
            type_map[entry_type] = {
                'descr': entry_descr,
                'count': 1,
                'handles': [entry_handle]
            }

    return type_map

def read_file_data(file_path: str) -> dict:
    try:
        process = subprocess.Popen(
            [
                f"{build_dir}/bin/opendmi", "--file", file_path,
                "export", "--all", "--format=yaml"
            ],
            stdout = subprocess.PIPE,
            stderr = subprocess.PIPE
        )
        stdout, stderr = process.communicate()

        sys.stdout.buffer.write(stderr)
        sys.stdout.buffer.flush()

        return yaml.load(stdout, Loader=yaml.loader.SafeLoader)
    except subprocess.CalledProcessError as e:
        logging.error("Unable to decode file")
        print(e.stderr)
        return None

def process_file(file_path: str, spec: dict, out_file) -> bool:
    file_rel = os.path.relpath(file_path, data_dir)

    data = read_file_data(file_path)
    if not data:
        logging.error("Update failed")
        return False

    firmware = get_firmware_info(data)
    system   = get_system_info(data)
    type_map = get_type_map(data)

    if 'product' in spec:
        title = spec['product']
    else:
        title = system['product']

    print(f"== {title} ({os.path.basename(file_path)})", file=out_file)
    print("", file=out_file)

    print("[horizontal]", file=out_file)
    print(f"File path:: `{file_rel}`", file=out_file)
    print(f"Product name:: {system.get('product', '-')}", file=out_file)
    print(f"Product version:: {system.get('version', '-')}", file=out_file)
    print(f"Product family:: {system.get('family', '-')}", file=out_file)
    print(f"Firmware vendor:: {firmware.get('vendor', '-')}", file=out_file)
    print(f"Firmware version:: {firmware.get('version', '-')}", file=out_file)
    print(f"Release date:: {firmware.get('release-date', '-')}", file=out_file)
    print(f"SMBIOS version:: {data['entry']['smbios-version']}", file=out_file)
    print("", file=out_file)

    print(f"=== Contents", file=out_file)
    print("", file=out_file)

    print(f"[cols=\"1,3,1,2\"]", file=out_file)
    print("|===", file=out_file)
    print("| Type | Description | Count | Handles", file=out_file)
    print("", file=out_file)

    for type in sorted(type_map):
        info = type_map[type]
        handles = ", ".join(list(map(lambda x: f"`0x{x:04X}`", info['handles'])))
        print(f"a| `{type}` | {info['descr']} | {info['count']}  a| {handles}", file=out_file)

    print("|===", file=out_file)

    logging.info("Update completed")
    return True

def process_vendor(vendor_dir: str):
    global num_updated, num_orphans, num_skipped

    vendor_rel = os.path.relpath(vendor_dir, base_dir)
    logging.info(f"=> {vendor_rel}")

    index_path = os.path.join(vendor_dir, '00-index.yml')
    if not os.path.isfile(index_path):
        logging.warning("No index file found, skipping")
        return

    index = load_index(index_path)
    files = data_files(vendor_dir)

    logging.info(f"Vendor: {index['vendor']}")
    logging.info("")

    out_base, _ = os.path.splitext(index_path)
    out_path = out_base + ".adoc"

    with open(out_path, 'w') as out_file:
        print(f"= {index['vendor']}", file=out_file)
        print(":toc:", file=out_file)
        print(":toclevels: 1", file=out_file)
        print("", file=out_file)

        for file_path in sorted(files):
            file_name = os.path.basename(file_path)

            logging.info(f"==> {vendor_rel}/{file_name}...")
            update_checksum(file_path)

            if not file_name in index['files']:
                logging.warning("No entry found, skipping")
                num_skipped += 1
                continue

            if process_file(file_path, index['files'][file_name], out_file):
                num_updated += 1
            else:
                num_errors += 1

            del index['files'][file_name]

        for file_name in index['files'].keys():
            logging.warning(f"Orphan entry: {file_name}")
            num_orphans += 1

    logging.info("")

def main(args: list[str]):
    global num_updated, num_orphans, num_skipped, num_errors

    logging.basicConfig(level=logging.DEBUG, handlers=[])

    console = logging.StreamHandler()
    console.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(message)s')
    console.setFormatter(formatter)

    logging.getLogger('').addHandler(console)

    for vendor_dir in vendor_dirs():
        process_vendor(vendor_dir)

    for paths in checksums.values():
        if len(paths) > 1:
            logging.warning(f"Duplicate dumps: {', '.join(sorted(paths))}")

    num_duplicates = sum(len(paths) - 1 for paths in checksums.values())

    logging.info(f"Updated: {num_updated}, skipped: {num_skipped}, orphans: {num_orphans}, errors: {num_errors}, "
                 f"duplicates: {num_duplicates}")
    logging.info("")

if __name__ == "__main__":
	main(sys.argv[1:])
