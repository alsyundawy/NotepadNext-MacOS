#!/usr/bin/env python3
"""
Generate compile_commands.json for SonarQube, SonarLint, Clangd, and C/C++ extensions.
Scans src/ for C, C++, Objective-C++, and header files.
Decomposed into modular functions with cognitive complexity < 10 and zero Bandit warnings.
"""

from __future__ import annotations

import json
import shlex
from pathlib import Path
from typing import Any

CPM_CACHE_DIR = ".cpm-cache"
CLANGXX_PATH = "/usr/local/opt/llvm/bin/clang++"
CLANG_PATH = "/usr/bin/clang"

FLYENV_NODE_LIB = "/Users/alsyundawy/Library/FlyEnv/env/node/lib"
QT_MACOS_LIB = "/Users/alsyundawy/Qt/6.12.0/macos/lib"
QT_MACOS_INCLUDE = "/Users/alsyundawy/Qt/6.12.0/macos/include"
MACOS_SDK_PATH = "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk"


def get_framework_dirs() -> list[str]:
    """Return framework search paths."""
    candidates = [FLYENV_NODE_LIB, QT_MACOS_LIB]
    return [d for d in candidates if Path(d).exists()]


def get_include_dirs(root: Path) -> list[str]:
    """Return all project and dependency include paths."""
    dirs = [
        str(root / "src"),
        str(root / "src" / "decorators"),
        str(root / "src" / "dialogs"),
        str(root / "src" / "docks"),
        str(root / "src" / "features"),
        str(root / "src" / "utils"),
        str(root / "src" / "widgets"),
        str(root / "build" / "src" / "NotepadNext_autogen" / "include"),
        str(root / CPM_CACHE_DIR / "ads" / "fb9d" / "src"),
        str(root / "build" / "_deps" / "ads-build" / "src"),
        str(root / CPM_CACHE_DIR / "qsimpleupdater" / "531e" / "include"),
        str(root / CPM_CACHE_DIR / "qsimpleupdater" / "531e" / "src"),
        str(root / CPM_CACHE_DIR / "singleapplication" / "d282"),
        str(root / CPM_CACHE_DIR / "editorconfig_core_qt" / "87f1"),
        str(root / "thirdparty" / "scintilla" / "include"),
        str(root / "thirdparty" / "scintilla" / "src"),
        str(root / "thirdparty" / "scintilla" / "qt" / "ScintillaEditBase"),
        str(root / "thirdparty" / "scintilla" / "qt" / "ScintillaEdit"),
        str(root / "thirdparty" / "lexilla" / "include"),
        str(root / "thirdparty" / "lexilla" / "access"),
        str(root / "thirdparty" / "lexilla" / "lexlib"),
        str(root / "thirdparty" / "lua" / "src"),
        str(root / "thirdparty" / "uchardet" / "src"),
        # Qt system headers and includes
        QT_MACOS_INCLUDE,
        f"{QT_MACOS_INCLUDE}/QtCore",
        f"{QT_MACOS_INCLUDE}/QtGui",
        f"{QT_MACOS_INCLUDE}/QtWidgets",
        f"{QT_MACOS_INCLUDE}/QtNetwork",
        f"{QT_MACOS_INCLUDE}/QtPrintSupport",
        f"{QT_MACOS_INCLUDE}/QtCore5Compat",
        f"{QT_MACOS_INCLUDE}/QtTest",
        # Qt system framework headers
        f"{FLYENV_NODE_LIB}/QtCore.framework/Headers",
        f"{FLYENV_NODE_LIB}/QtGui.framework/Headers",
        f"{FLYENV_NODE_LIB}/QtWidgets.framework/Headers",
        f"{FLYENV_NODE_LIB}/QtNetwork.framework/Headers",
        f"{FLYENV_NODE_LIB}/QtPrintSupport.framework/Headers",
        f"{FLYENV_NODE_LIB}/QtCore5Compat.framework/Headers",
        f"{FLYENV_NODE_LIB}/QtTest.framework/Headers",
        f"{QT_MACOS_LIB}/QtCore.framework/Headers",
        f"{QT_MACOS_LIB}/QtGui.framework/Headers",
        f"{QT_MACOS_LIB}/QtWidgets.framework/Headers",
        f"{QT_MACOS_LIB}/QtNetwork.framework/Headers",
        f"{QT_MACOS_LIB}/QtPrintSupport.framework/Headers",
        f"{QT_MACOS_LIB}/QtCore5Compat.framework/Headers",
        f"{QT_MACOS_LIB}/QtTest.framework/Headers",
    ]
    return [d for d in dirs if Path(d).exists()]


def get_base_defines() -> list[str]:
    """Return common preprocessor defines."""
    return [
        '-DAPP_VERSION="0.15-07"',
        '-DAPP_DISTRIBUTION="dmg"',
        '-DAPP_COPYRIGHT="Copyright 2019-2026 Justin Dailey"',
        '-DAPP_COPYRIGHT_BUILDER="Copyright 2026 Harry Dertin Sutisna <alsyundawy@gmail.com>"',
        "-DSCI_OWNREGEX",
        "-DADS_STATIC",
        "-DQSIMPLEUPDATER_STATIC",
        "-DQAPPLICATION_CLASS=QApplication",
        "-DQT_CORE_LIB",
        "-DQT_GUI_LIB",
        "-DQT_WIDGETS_LIB",
        "-DQT_NETWORK_LIB",
        "-DQT_PRINTSUPPORT_LIB",
        "-DQT_CORE5COMPAT_LIB",
        "-DSCINTILLA_QT=1",
        "-DSCI_LEXER=1",
        "-D_CRT_SECURE_NO_DEPRECATE=1",
    ]


def collect_source_files(search_paths: list[Path]) -> list[Path]:
    """Find all C++, C, Objective-C++, and header files."""
    valid_exts = {".cpp", ".c", ".mm", ".h", ".hpp"}
    files: list[Path] = []
    for sp in search_paths:
        if sp.exists():
            for p in sp.rglob("*"):
                if p.is_file() and p.suffix in valid_exts:
                    files.append(p)
    files.sort()
    return files


def determine_compiler_and_flags(suffix: str) -> tuple[str, list[str]]:
    """Select compiler and standard flags based on file extension."""
    if suffix == ".c":
        return CLANG_PATH, ["-std=c17"]
    if suffix == ".mm":
        return CLANGXX_PATH, ["-std=c++20", "-x", "objective-c++"]
    if suffix in {".h", ".hpp"}:
        return CLANGXX_PATH, ["-std=c++20", "-x", "c++-header"]
    return CLANGXX_PATH, ["-std=c++20"]


def build_compile_entry(
    sf: Path,
    root: Path,
    inc_flags: list[str],
    fw_flags: list[str],
    base_defines: list[str],
) -> dict[str, Any]:
    """Generate a single compile command dictionary."""
    rel_path = sf.relative_to(root)
    compiler, std_flags = determine_compiler_and_flags(sf.suffix)

    cmd_parts = [
        compiler,
        *std_flags,
        "-isystem",
        "/usr/local/opt/llvm/include/c++/v1",
        "-isysroot",
        MACOS_SDK_PATH,
        *fw_flags,
        *base_defines,
        *inc_flags,
        "-c",
        str(rel_path),
        "-o",
        f"build/{rel_path.stem}.o",
    ]

    return {
        "directory": str(root),
        "command": shlex.join(cmd_parts),
        "arguments": cmd_parts,
        "file": str(sf),
    }


def write_database(entries: list[dict[str, Any]], build_dir: Path, root: Path) -> None:
    """Save compile_commands.json to build/ and maintain root symlink."""
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


def main() -> None:
    """Orchestrate compilation database generation."""
    root = Path(__file__).resolve().parent.parent
    build_dir = root / "build"
    build_dir.mkdir(parents=True, exist_ok=True)

    include_dirs = get_include_dirs(root)
    inc_flags = [f"-I{inc}" for inc in include_dirs]
    framework_dirs = get_framework_dirs()
    fw_flags: list[str] = []
    for fw in framework_dirs:
        fw_flags.extend(["-iframework", fw])
    base_defines = get_base_defines()

    source_files = collect_source_files([root / "src"])
    entries = [
        build_compile_entry(sf, root, inc_flags, fw_flags, base_defines)
        for sf in source_files
    ]

    write_database(entries, build_dir, root)
    print(
        f"Generated compilation database with {len(entries)} entries at {build_dir / 'compile_commands.json'}"
    )


if __name__ == "__main__":
    main()
