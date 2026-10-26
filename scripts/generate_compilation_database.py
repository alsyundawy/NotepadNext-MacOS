#!/usr/bin/env python3
"""Ensure CMake-generated compile_commands.json is synced to project root."""

import os
import shutil
import subprocess
from pathlib import Path


def main():
    root = Path(__file__).resolve().parent.parent
    build_dir = root / "build"
    build_compile_commands = build_dir / "compile_commands.json"
    root_compile_commands = root / "compile_commands.json"

    if not build_compile_commands.exists():
        print("compile_commands.json not found in build/, running cmake configure...")
        subprocess.run(["cmake", "-B", "build", "-S", "."], cwd=str(root), check=True)

    if build_compile_commands.exists():
        if root_compile_commands.is_symlink() or root_compile_commands.exists():
            try:
                root_compile_commands.unlink()
            except OSError:
                pass
        try:
            root_compile_commands.symlink_to("build/compile_commands.json")
            print(f"Created symlink {root_compile_commands} -> build/compile_commands.json")
        except OSError:
            shutil.copy2(build_compile_commands, root_compile_commands)
            print(f"Copied {build_compile_commands} -> {root_compile_commands}")
    else:
        print("Error: Could not locate build/compile_commands.json")


if __name__ == "__main__":
    main()
