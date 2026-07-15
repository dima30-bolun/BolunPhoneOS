#!/usr/bin/env python3
"""Create a deterministic Bolun MainOS development image.

The image format is intentionally simple for host-side smoke tests and bring-up
planning.  It packages named local payloads without touching device partitions
or bootloader state.
"""

from __future__ import annotations

import argparse
import struct
import sys
import zlib
from pathlib import Path

MAGIC = b"BOLUNMAINOS\0"
VERSION = 1
MAX_NAME_BYTES = 48


def parse_component(value: str) -> tuple[str, Path]:
    if "=" not in value:
        raise argparse.ArgumentTypeError("component must use name=path syntax")
    name, path_text = value.split("=", 1)
    name = name.strip()
    if not name:
        raise argparse.ArgumentTypeError("component name cannot be empty")
    encoded = name.encode("utf-8")
    if len(encoded) > MAX_NAME_BYTES:
        raise argparse.ArgumentTypeError(f"component name exceeds {MAX_NAME_BYTES} bytes")
    path = Path(path_text)
    if not path.is_file():
        raise argparse.ArgumentTypeError(f"component payload not found: {path}")
    return name, path


def write_padded_name(handle, name: str) -> None:
    encoded = name.encode("utf-8")
    handle.write(encoded)
    handle.write(b"\0" * (MAX_NAME_BYTES - len(encoded)))


def build_image(output: Path, target: str, components: list[tuple[str, Path]]) -> None:
    if not components:
        raise ValueError("at least one component is required")
    target_bytes = target.encode("utf-8")
    if len(target_bytes) > MAX_NAME_BYTES:
        raise ValueError(f"target exceeds {MAX_NAME_BYTES} bytes")

    payloads = [(name, path.read_bytes()) for name, path in components]
    with output.open("wb") as handle:
        handle.write(MAGIC)
        handle.write(struct.pack("<HH", VERSION, len(payloads)))
        write_padded_name(handle, target)
        for name, data in payloads:
            write_padded_name(handle, name)
            handle.write(struct.pack("<II", len(data), zlib.crc32(data) & 0xFFFFFFFF))
        for _, data in payloads:
            handle.write(data)


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description="Build a Bolun MainOS development image")
    parser.add_argument("--target", default="lumia-710", help="Bolun device codename")
    parser.add_argument("--output", required=True, type=Path, help="output image path")
    parser.add_argument("component", nargs="+", type=parse_component, help="component in name=path form")
    args = parser.parse_args(argv)

    try:
        build_image(args.output, args.target, args.component)
    except (OSError, ValueError) as exc:
        print(f"mkmainos: {exc}", file=sys.stderr)
        return 1
    print(f"created {args.output} for {args.target} with {len(args.component)} component(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
