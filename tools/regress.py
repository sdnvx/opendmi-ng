#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
import os
import getopt
import sys
import yaml

from yaml.loader import SafeLoader
from jsonschema import validate, ValidationError
from colorama import init, Fore, Style

from opendmi_tools import base_dir, export_dump

schema_dir = os.path.join(base_dir, "opendmi/share/opendmi")
data_dir   = os.path.join(base_dir, "data")

schema = None

init(autoreset=True)

def data_files():
    for root, _, files in os.walk(data_dir):
        for file_name in files:
            file_path = os.path.join(root, file_name)

            file_base, file_ext = os.path.splitext(file_path)
            if file_ext != ".bin":
                continue

            yield file_path, file_base + ".yaml"

def load_yaml(path: str):
    try:
        with open(path) as file:
            return yaml.load(file, Loader=SafeLoader)
    except FileNotFoundError:
        print(f"ERROR: File not found: {path}", file=sys.stderr)
        return None

def validate_data(dump_path: str, data) -> bool:
    try:
        validate(data, schema)
        return True
    except ValidationError as e:
        print(f"ERROR: Schema validation failed for {dump_path}: {e.message}", file=sys.stderr)
        return False

def generate_spec(dump_path: str, spec_path: str) -> bool:
    print(f"Generating: {dump_path.removeprefix(base_dir)} -> {spec_path.removeprefix(base_dir)}")

    data = export_dump(dump_path)
    if data is None or not validate_data(dump_path, data):
        return False

    try:
        with open(spec_path, "w+") as file:
            yaml.dump(data, file, sort_keys=False)
    except OSError as e:
        print(f"ERROR: Unable to write specification: {spec_path}: {e.strerror}", file=sys.stderr)
        return False

    return True

def generate_diff(old, new):
    if isinstance(old, dict) and isinstance(new, dict):
        diff = {}
        all_keys = list(old.keys()) + [k for k in new if k not in old]

        for k in all_keys:
            if k in old and k not in new:
                diff[k] = ('-', old[k])
            elif k not in old and k in new:
                diff[k] = ('+', new[k])
            else:
                res = generate_diff(old[k], new[k])
                if res: diff[k] = res

        return diff if diff else None

    elif isinstance(old, list) and isinstance(new, list):
        diff = []
        max_len = max(len(old), len(new))
        has_changes = False

        for i in range(max_len):
            v1 = old[i] if i < len(old) else None
            v2 = new[i] if i < len(new) else None
            if i >= len(new):
                diff.append(('-', v1))
                has_changes = True
            elif i >= len(old):
                diff.append(('+', v2))
                has_changes = True
            else:
                res = generate_diff(v1, v2)
                if res:
                    diff.append(res)
                    has_changes = True
                else:
                    diff.append(None)

        return diff if has_changes else None

    return ('changed', old, new) if old != new else None

def render_diff(diff, depth=0, key=None):
    indent = "  " * depth
    k_str = f"{key}: " if key is not None else ""

    if isinstance(diff, tuple) and len(diff) == 3 and diff[0] == 'changed':
        _, old_val, new_val = diff
        render_node(key, old_val, '-', depth)
        render_node(key, new_val, '+', depth)

    elif isinstance(diff, tuple) and diff[0] in ('-', '+'):
        sign, val = diff
        render_node(key, val, sign, depth)

    elif isinstance(diff, dict):
        if depth > 0:
            print(f"  {indent}{k_str}{{")
        for k, v in diff.items():
            render_diff(v, depth + (1 if depth > 0 else 0), k)
        if depth > 0:
            print(f"  {indent}}}")

    elif isinstance(diff, list):
        print(f"  {indent}{k_str}[")
        for item in diff:
            render_diff(item, depth + 1)
        print(f"  {indent}]")

def render_node(key, value, sign, depth):
    indent = "  " * depth
    k_prefix = f"{key}: " if key is not None else ""

    color = Fore.RED if sign == '-' else Fore.GREEN

    if isinstance(value, dict):
        print(f"{color}{sign} {indent}{k_prefix}{{")
        for k, v in value.items():
            render_node(k, v, sign, depth + 1)
        print(f"{color}{sign} {indent}}}")

    elif isinstance(value, list):
        print(f"{color}{sign} {indent}{k_prefix}[")
        for item in value:
            render_node(None, item, sign, depth + 1)
        print(f"{color}{sign} {indent}]")

    else:
        print(f"{color}{sign} {indent}{k_prefix}{value}")

def check_spec(dump_path: str, spec_path: str) -> bool:
    if not os.path.isfile(spec_path):
        print(f"Skipping: {dump_path} (no spec)")
        return True

    print(f"Checking: {dump_path.removeprefix(base_dir)} <- {spec_path.removeprefix(base_dir)}")
    spec = load_yaml(spec_path)
    data = export_dump(dump_path)
    if spec is None or data is None:
        return False

    diff = generate_diff(spec, data)
    if diff:
        render_diff(diff)
        return False

    # Baselines are validated when generated, so an unchanged output is valid
    return True

def show_usage():
    print(f"Usage: {os.path.basename(sys.argv[0])} [options]")
    print()
    print("Decode every SMBIOS dump under data/ and compare the result with its YAML")
    print("specification. Exits with a non-zero status if any check fails.")
    print()
    print("Options:")
    print("    -h, --help      Print this help and exit")
    print("    -b, --baseline  Regenerate the specifications instead of checking them")
    print()
    print("Environment:")
    print("    BUILD_DIR       Build directory (default: build)")

def main(argv) -> int:
    global schema

    mode = "test"

    try:
        opts, args = getopt.getopt(argv, "hb", ["help", "baseline"])
    except getopt.GetoptError as e:
        print(str(e))
        print('Use -h or --help for help')
        sys.exit(os.EX_USAGE)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            show_usage()
            sys.exit(0)
        elif opt in ("-b", "--baseline"):
            mode = "generate"

    yaml.SafeLoader.yaml_implicit_resolvers = {
        k: [r for r in v if r[0] != 'tag:yaml.org,2002:timestamp'] for
            k, v in yaml.SafeLoader.yaml_implicit_resolvers.items()
    }

    print("Loading schema...")
    schema = load_yaml(os.path.join(schema_dir, "opendmi.yml"))
    if schema == None:
        return os.EX_OSFILE

    failed = []
    for dump_path, spec_path in data_files():
        if mode == "generate":
            success = generate_spec(dump_path, spec_path)
        else:
            success = check_spec(dump_path, spec_path)

        if not success:
            failed.append(dump_path.removeprefix(base_dir))

    if failed:
        print(f"{Fore.RED}Failed: {len(failed)}", file=sys.stderr)
        for path in failed:
            print(f"    {path}", file=sys.stderr)
        return 1

    return os.EX_OK

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
