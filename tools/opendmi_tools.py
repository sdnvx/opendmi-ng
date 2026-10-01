#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
"""Helpers shared by the maintenance scripts."""
import os
import subprocess
import sys
import yaml

from yaml.loader import SafeLoader

tools_dir = os.path.dirname(os.path.realpath(__file__))
base_dir  = os.path.abspath(os.path.join(tools_dir, '..'))

# Build directory, relative to the source tree unless absolute (as in coverage.sh)
build_dir = os.path.join(base_dir, os.environ.get("BUILD_DIR", "build"))

opendmi_path = os.path.join(build_dir, "opendmi", "bin", "opendmi")

def export_dump(dump_path: str):
    """
    Export an SMBIOS dump as YAML with the opendmi tool and parse it.

    Diagnostics of the tool are passed through to stderr. Returns the parsed
    data, or None if the tool fails or its output cannot be parsed.
    """
    env = os.environ.copy()
    env["LANG"] = "en_US.UTF-8"
    env["LC_ALL"] = "C"

    try:
        process = subprocess.run(
            [opendmi_path, "--file", dump_path, "export", "--all", "--format=yaml"],
            capture_output=True,
            env=env,
            check=True
        )
    except OSError as e:
        print(f"ERROR: Unable to run {opendmi_path}: {e.strerror}", file=sys.stderr)
        return None
    except subprocess.CalledProcessError as e:
        print(f"ERROR: Unable to read SMBIOS dump: {dump_path} (exit code {e.returncode})",
              file=sys.stderr)
        sys.stderr.buffer.write(e.stderr)
        sys.stderr.buffer.flush()
        return None

    sys.stderr.buffer.write(process.stderr)
    sys.stderr.buffer.flush()

    try:
        return yaml.load(process.stdout, Loader=SafeLoader)
    except yaml.YAMLError as e:
        print(f"ERROR: Unable to parse output for {dump_path}: {e}", file=sys.stderr)
        return None
