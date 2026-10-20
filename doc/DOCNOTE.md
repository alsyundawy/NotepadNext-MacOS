# 📘 Documentation Note (DOCNOTE) — NotepadNext-MacOS v0.15-04

**Project**: NotepadNext For MacOS  
**Target Release**: `0.15-04`  
**Date**: October 21, 2026  
**Maintainer / Builder**: Harry DS Alsyundawy ([@alsyundawy](https://github.com/alsyundawy))  
**Upstream Project**: [dail8859/NotepadNext](https://github.com/dail8859/NotepadNext)

---

## 1. Executive Summary & Ecosystem Context

**NotepadNext For MacOS** is a native macOS reimplementation of the renowned Windows editor **Notepad++**, engineered using modern C++20 and the **Qt 6 framework** (with support across Qt 6.5 LTS, Qt 6.8 LTS, Qt 6.10, and Qt 6.11).

While alternative text editors each occupy specific architectural niches:

- **Notepad2 / Notepad2-mod / Notepad3 / Notepad4**: Lightweight, single-file Windows Win32 editors based on Scintilla; minimal memory footprint but bound to the Windows API without cross-platform macOS support.
- **Notepad++ (NPP)**: Highly extensible Windows desktop standard built on pure Win32 API and Scintilla; lacks native macOS rendering and Cocoa/AppKit integration.
- **Sublime Text**: Proprietary, ultra-fast custom UI toolkit and Python plugin engine; excellent macOS integration, but closed-source and commercial.
- **BBEdit**: Premier macOS-exclusive commercial editor deeply integrated with AppleScript, macOS Text Services, and AppKit; not cross-platform.
- **NotepadNext**: Bridges this gap by combining Scintilla's powerful syntax buffer engine with Qt 6's cross-platform graphics and macOS native Cocoa event handling, delivering true native macOS performance with Notepad++ muscle.

---

## 2. 13-Pillar Codebase Audit & Verification

| #   | Pillar                            | Audit Findings & Resolution                                                                                                                                                                                                                                                                                                                                                      | Verification Status |
| :-- | :-------------------------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :-----------------: |
| 1   | **Bug Review**                    | **Fixed**: Corrected inverted disconnect in `QuickFindWidget::setEditor()`. Disconnecting incoming editor instead of previously attached instance caused persistent signal listeners on old buffers. **Fixed**: Added defensive null checks in `MainWindow::restoreWindowState()` for `SearchResultsDock` to prevent `SIGSEGV` during windowState restorations (`QTBUG-111538`). |        PASS         |
| 2   | **Syntax Review**                 | Fully verified against ISO C++20 (`-std=c++20`). Ensured strict compliance with CMake 3.21+ standards, eliminated constructor ambiguity in `TabsQuickActionsBar.h`, and enforced `explicit` on single-parameter constructors.                                                                                                                                                    |        PASS         |
| 3   | **Runtime Review**                | Evaluated application lifecycle: startup sequence in `NotepadNextApplication::init()`, session restoration in `SessionManager`, single-instance IPC via `SingleApplication`, and clean exit teardown. Handled potential write failures in file renaming with `showSaveErrorMessage()`.                                                                                           |        PASS         |
| 4   | **Logic Review**                  | Streamlined translation initialization to prioritize explicit language choice, falling back to System Default. Guaranteed dynamic dropdown in preferences cleanly filters between Indonesian and English.                                                                                                                                                                        |        PASS         |
| 5   | **Memory Review**                 | Initialized all raw member pointers (`editorManager`, `recentFilesListManager`, `settings`, `sessionManager`, `translationManager`) explicitly to `nullptr` in class definitions. Verified Qt parent-child object tree ownership for zero memory leaks.                                                                                                                          |        PASS         |
| 6   | **Dead Code Review**              | Purged 21 unmaintained legacy `.ts` translation files from `i18n/`, reducing repository weight and eliminating build overhead during `lrelease`. Added `scratch/**` to `.gitignore` and `.trunk/trunk.yaml`.                                                                                                                                                                     |        PASS         |
| 7   | **Duplicate Code Review**         | Unified icon generation pipeline: standardizing all output from a single high-resolution master asset into standard desktop square dimensions, avoiding multiple conflicting icon copies.                                                                                                                                                                                        |        PASS         |
| 8   | **Circular Dependency Review**    | Audited header inclusion hierarchy in `src/`. Verified clean forward declarations for `TranslationManager`, `SessionManager`, `EditorManager`, and `RecentFilesListManager`.                                                                                                                                                                                                     |        PASS         |
| 9   | **Performance Bottleneck Review** | Replaced naive string concatenations with direct bounded operations. Configured non-blocking, multi-threaded static checking (`-j 2`) to ensure macOS host CPU stability without thread saturation.                                                                                                                                                                              |        PASS         |
| 10  | **Security Vulnerability Review** | **CWE-120 Resolution**: Replaced unchecked legacy `strcpy` and `strcat` in `src/IFaceTable.cpp` with bounded `std::snprintf`. Scanned entire tree with `flawfinder` and `cppcheck` (0 remaining vulnerabilities).                                                                                                                                                                |        PASS         |
| 11  | **Maintainability Review**        | Standardized release metadata across `CMakeLists.txt`, `updates.json`, and Linux AppStream XML. Ensured transparent configuration for MegaLinter and Trunk linters.                                                                                                                                                                                                              |        PASS         |
| 12  | **Scalability Review**            | Streamlined language catalog: maintaining 100% complete translation parity (419 XML translation messages) between English and Indonesian.                                                                                                                                                                                                                                        |        PASS         |
| 13  | **Readability Review**            | Enhanced documentation, bilingual comments (English & Indonesian), formatted markdown files with `prettier` and `markdownlint`.                                                                                                                                                                                                                                                  |        PASS         |

---

## 3. Brand Icon & Packaging Pipeline

### Visual Asset Specifications

- **Master Asset**: Extracted from high-resolution 2080x2048 master artwork.
- **Background Extraction**: Floodfill matte transparency with 14% fuzz threshold, isolating the emerald squircle, N# emblem, and pencil stylus with natural soft drop shadow.
- **macOS HIG Conformity**: Centered inside a 1:1 canvas with subtle proportional margin to guarantee harmonious docking alongside macOS system icons.

### Generated Icon Matrix

| File Path                   | Format            | Dimensions                   | Purpose                                          |
| :-------------------------- | :---------------- | :--------------------------- | :----------------------------------------------- |
| `src/icons/NotepadNext.png` | PNG (32-bit RGBA) | 512 × 512                    | Qt application window icon & About dialog        |
| `icon/NotepadNext.icns`     | Apple ICNS        | 16 to 1024 Retina            | macOS App Bundle (`Info.plist`)                  |
| `icon/NotepadNext.ico`      | Windows ICO       | 16, 24, 32, 48, 64, 128, 256 | Windows executable icon                          |
| `icon/NotepadNext.svg`      | SVG               | 512 × 512                    | Scalable Linux desktop icon (`hicolor/scalable`) |
| `icon/icon_16x16.png`       | PNG               | 16 × 16                      | Small taskbar / status icon                      |
| `icon/icon_24x24.png`       | PNG               | 24 × 24                      | Tool palette icon                                |
| `icon/icon_32x32.png`       | PNG               | 32 × 32                      | Standard desktop icon                            |
| `icon/icon_48x48.png`       | PNG               | 48 × 48                      | Large list icon                                  |
| `icon/icon_64x64.png`       | PNG               | 64 × 64                      | HiDPI list icon                                  |
| `icon/icon_128x128.png`     | PNG               | 128 × 128                    | Dock icon (Low DPI)                              |
| `icon/icon_256x256.png`     | PNG               | 256 × 256                    | High-DPI icon                                    |
| `icon/icon_512x512.png`     | PNG               | 512 × 512                    | Ultra High-DPI / Store icon                      |

All PNG assets have been losslessly compressed with `oxipng` and verified with `trunk check`.

---

## 4. Localization Architecture (English & Indonesian)

In accordance with user requirements, the application's translation system has been streamlined exclusively to:

1. 🇬🇧 **English (`en_US`)**: Base source strings and reference translation.
2. 🇮🇩 **Bahasa Indonesia (`id_ID`)**: Native, complete translation with 419 XML strings covering menus, status bar, docks, preferences, find/replace dialogs, and Scintilla contextual menus.

All other language `.ts` files have been removed from `i18n/`, reducing repository bloat while accelerating CMake build and packaging stages.

---

## 5. Verification & Quality Gates

### Linters Executed

- **Trunk**: `actionlint`, `bandit`, `black`, `checkov`, `isort`, `markdownlint`, `oxipng`, `prettier`, `ruff`, `shellcheck`, `shfmt`, `svgo`, `taplo`, `yamllint`.
- **Static Analyzers**: `cppcheck`, `flawfinder`.
- **Status**: 100% clean passing state across all modified and generated files.
