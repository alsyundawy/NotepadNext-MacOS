#!/usr/bin/env python3
import glob
import json
import os


def main():
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    compdb_path = os.path.join(repo_root, "build", "compile_commands.json")
    if not os.path.exists(compdb_path):
        print("compile_commands.json not found at", compdb_path)
        return

    try:
        with open(compdb_path, "r", encoding="utf-8") as f:
            content = f.read().strip()
            if not content:
                print("Notice: compile_commands.json is empty, skipping enrichment.")
                return
            entries = json.loads(content)
    except Exception as err:
        print(
            f"Notice: unable to parse compile_commands.json ({err}), skipping enrichment."
        )
        return

    if not isinstance(entries, list) or not entries:
        print(
            "Notice: compile_commands.json has no valid entries, skipping enrichment."
        )
        return

    existing_files = {
        e.get("file") for e in entries if isinstance(e, dict) and "file" in e
    }
    file_map = {e["file"]: e for e in entries if isinstance(e, dict) and "file" in e}

    default_sample = next(
        (
            e
            for e in entries
            if isinstance(e, dict) and "MainWindow.cpp" in e.get("file", "")
        ),
        entries[0] if entries else None,
    )
    if (
        not default_sample
        or not isinstance(default_sample, dict)
        or "command" not in default_sample
    ):
        print(
            "Notice: compile_commands.json does not contain valid compilation sample, skipping."
        )
        return

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

        base_cmd = base_entry["command"]
        base_file = base_entry["file"]

        new_cmd = base_cmd.replace(
            f"-c {base_file}", f"-x c++-header -fsyntax-only {h}"
        )
        if "output" in base_entry and base_entry["output"]:
            new_cmd = new_cmd.replace(f"-o {base_entry['output']}", "")

        new_entry = {
            "directory": base_entry["directory"],
            "command": new_cmd,
            "file": h,
        }
        entries.append(new_entry)
        existing_files.add(h)
        added += 1

    with open(compdb_path, "w") as f:
        json.dump(entries, f, indent=2)

    root_compdb = os.path.join(repo_root, "compile_commands.json")
    if not os.path.islink(root_compdb) and os.path.exists(root_compdb):
        try:
            with open(root_compdb, "w") as f:
                json.dump(entries, f, indent=2)
        except OSError as err:
            print(f"Notice: unable to mirror root compile_commands.json: {err}")

    print(
        f"Added {added} header entries to compile_commands.json (Total entries: {len(entries)})"
    )


if __name__ == "__main__":
    main()
