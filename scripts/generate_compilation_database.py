#!/usr/bin/env python3
"""Generate compile_commands.json for SonarQube, SonarLint, Clangd, and C/C++ extensions.

Scans src/ for C, C++, and Objective-C++ translation units.
"""

import json
from pathlib import Path


def main():
    root = Path(__file__).resolve().parent.parent
    src_dir = root / "src"
    build_dir = root / "build"
    build_dir.mkdir(parents=True, exist_ok=True)

    include_dirs = [
        str(root / "src"),
        str(root / "src" / "decorators"),
        str(root / "src" / "dialogs"),
        str(root / "src" / "docks"),
        str(root / "src" / "features"),
        str(root / "src" / "utils"),
        str(root / "src" / "widgets"),
        str(root / "thirdparty" / "scintilla" / "include"),
        str(root / "thirdparty" / "scintilla" / "src"),
        str(root / "thirdparty" / "scintilla" / "qt" / "ScintillaEdit"),
        str(root / "thirdparty" / "scintilla" / "qt" / "ScintillaEditBase"),
        str(root / "thirdparty" / "lexilla" / "include"),
        str(root / "thirdparty" / "lua" / "src"),
        str(root / "thirdparty" / "uchardet" / "src"),
    ]
    deps_dir = root / ".deps"
    include_dirs.extend(
        [
            str(deps_dir / "generated"),
            str(deps_dir / "qt_shims"),
            str(deps_dir / "ads" / "src"),
            str(deps_dir / "SingleApplication"),
            str(deps_dir / "QSimpleUpdater" / "include"),
            str(deps_dir / "editorconfig-core-qt" / "src"),
        ]
    )

    base_defines = [
        '-DAPP_VERSION="0.15-07"',
        "-DSCI_OWNREGEX",
        "-DADS_STATIC",
        "-DQSIMPLEUPDATER_STATIC",
        "-DQAPPLICATION_CLASS=QApplication",
    ]

    inc_flags = [f"-I{inc}" for inc in include_dirs]

    entries = []

    # Find all translation units
    extensions = {".cpp", ".c", ".mm"}
    source_files = []
    if src_dir.exists():
        for p in src_dir.rglob("*"):
            if p.is_file() and p.suffix in extensions:
                source_files.append(p)

    source_files.sort()

    for sf in source_files:
        rel_path = sf.relative_to(root)
        if sf.suffix == ".c":
            compiler = "/usr/bin/clang"
            std_flag = "-std=c17"
        elif sf.suffix == ".mm":
            compiler = "/usr/bin/clang++"
            std_flag = "-std=c++20 -x objective-c++"
        else:
            compiler = "/usr/bin/clang++"
            std_flag = "-std=c++20"

        defines = list(base_defines)

        cmd_parts = [
            compiler,
            std_flag,
            *defines,
            *inc_flags,
            "-c",
            str(rel_path),
            "-o",
            f"build/{rel_path.stem}.o",
        ]

        command = " ".join(cmd_parts)

        entries.append(
            {
                "directory": str(root),
                "command": command,
                "file": str(sf),
            }
        )

    out_file = build_dir / "compile_commands.json"
    with open(out_file, "w", encoding="utf-8") as f:
        json.dump(entries, f, indent=2)

    root_symlink = root / "compile_commands.json"
    try:
        if root_symlink.is_symlink() or root_symlink.exists():
            root_symlink.unlink()
        root_symlink.symlink_to("build/compile_commands.json")
    except OSError:
        with open(root_symlink, "w", encoding="utf-8") as f:
            json.dump(entries, f, indent=2)

    print(f"Generated compilation database with {len(entries)} entries at {out_file}")


if __name__ == "__main__":
    main()
