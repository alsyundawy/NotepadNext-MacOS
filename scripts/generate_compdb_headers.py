#!/usr/bin/env python3
from __future__ import annotations

import glob
import json
import os
from typing import Any


def load_compdb(compdb_path: str) -> list[dict[str, Any]] | None:
    if not os.path.exists(compdb_path):
        print("compile_commands.json not found at", compdb_path)
        return None

    try:
        with open(compdb_path, "r", encoding="utf-8") as f:
            content = f.read().strip()
            if not content:
                print("Notice: compile_commands.json is empty, skipping enrichment.")
                return None
            entries = json.loads(content)
    except (json.JSONDecodeError, OSError, UnicodeDecodeError) as err:
        print(
            f"Notice: unable to parse compile_commands.json ({err}), skipping enrichment."
        )
        return None

    if not isinstance(entries, list) or not entries:
        print(
            "Notice: compile_commands.json has no valid entries, skipping enrichment."
        )
        return None

    return entries


def find_sample_entry(entries: list[dict[str, Any]]) -> dict[str, Any] | None:
    sample = next(
        (
            e
            for e in entries
            if isinstance(e, dict) and "MainWindow.cpp" in e.get("file", "")
        ),
        entries[0] if entries else None,
    )
    if not sample or not isinstance(sample, dict) or "command" not in sample:
        print(
            "Notice: compile_commands.json does not contain valid compilation sample, skipping."
        )
        return None
    return sample


def create_header_entry(header_path: str, base_entry: dict[str, Any]) -> dict[str, Any]:
    base_cmd = base_entry["command"]
    base_file = base_entry.get("file", "")

    new_cmd = base_cmd.replace(
        f"-c {base_file}", f"-x c++-header -fsyntax-only {header_path}"
    )
    output = base_entry.get("output")
    if output:
        new_cmd = new_cmd.replace(f"-o {output}", "")

    return {
        "directory": base_entry.get("directory", ""),
        "command": new_cmd,
        "file": header_path,
    }


def enrich_headers(
    entries: list[dict[str, Any]], repo_root: str, default_sample: dict[str, Any]
) -> int:
    existing_files = {
        e.get("file") for e in entries if isinstance(e, dict) and "file" in e
    }
    file_map = {e["file"]: e for e in entries if isinstance(e, dict) and "file" in e}

    src_dir = os.path.join(repo_root, "src")
    headers = glob.glob(os.path.join(src_dir, "**", "*.h"), recursive=True) + glob.glob(
        os.path.join(src_dir, "**", "*.hpp"), recursive=True
    )

    added = 0
    for h in headers:
        if h in existing_files:
            continue
        cpp_candidate = os.path.splitext(h)[0] + ".cpp"
        base_entry = file_map.get(cpp_candidate, default_sample)
        entries.append(create_header_entry(h, base_entry))
        existing_files.add(h)
        added += 1

    return added


def save_compdb(
    repo_root: str, compdb_path: str, entries: list[dict[str, Any]]
) -> None:
    with open(compdb_path, "w", encoding="utf-8") as f:
        json.dump(entries, f, indent=2)

    root_compdb = os.path.join(repo_root, "compile_commands.json")
    if not os.path.islink(root_compdb) and os.path.exists(root_compdb):
        try:
            with open(root_compdb, "w", encoding="utf-8") as f:
                json.dump(entries, f, indent=2)
        except OSError as err:
            print(f"Notice: unable to mirror root compile_commands.json: {err}")


def main() -> None:
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    compdb_path = os.path.join(repo_root, "build", "compile_commands.json")

    entries = load_compdb(compdb_path)
    if entries is None:
        return

    sample = find_sample_entry(entries)
    if sample is None:
        return

    added = enrich_headers(entries, repo_root, sample)
    save_compdb(repo_root, compdb_path, entries)

    print(
        f"Added {added} header entries to compile_commands.json (Total entries: {len(entries)})"
    )


if __name__ == "__main__":
    main()
