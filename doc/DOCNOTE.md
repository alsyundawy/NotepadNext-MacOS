# 📘 Documentation Note (DOCNOTE) — NotepadNext-MacOS v0.15-04

**Project**: NotepadNext For MacOS  
**Target Release**: `0.15-04`  
**Date**: October 21, 2026  
**Maintainer / Builder**: Harry DS Alsyundawy ([@alsyundawy](https://github.com/alsyundawy))  
**Upstream Project**: [dail8859/NotepadNext](https://github.com/dail8859/NotepadNext)

---

## 1. Executive Summary & Ecosystem Context

**NotepadNext For MacOS** is a native macOS reimplementation of the renowned Windows editor **Notepad++**, engineered using modern C++20 and the **Qt 6 framework** (with full support across Qt 6.5 LTS, Qt 6.8 LTS, Qt 6.10, Qt 6.11, and Qt 6.12).

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
| 11  | **Maintainability Review**        | Standardized release metadata and application bundle identifier `com.github.alsyundawy.NotepadNext` across `CMakeLists.txt`, `deploy/macos/info.plist`, `updates.json`, `NotepadNext-Default-Editor.sh` (with legacy fallback), and Linux AppStream XML. Ensured transparent configuration for MegaLinter and Trunk linters.                                                     |        PASS         |
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

## 4. Modernized "About Notepad Next" Dialog

The about dialog (`src/dialogs/MainWindow.cpp`) has been elevated into an elegant, informative, modern, and professional presentation:

- **Visual Brand Integration**: Scaled 80×80 high-resolution application icon (`:/icons/NotepadNext.png`).
- **Dynamic System Metadata**: Displays runtime Qt version badge (`qVersion()`), macOS OS release, and host CPU architecture (`QSysInfo::prettyProductName()` and `QSysInfo::currentCpuArchitecture()`).
- **Clear Attribution**: Dual copyright credits for original author **Justin Dailey** and macOS maintainer & builder **Harry DS Alsyundawy**.
- **Architectural Overview**: Highlights core engines (Scintilla buffer, Lexilla lexers, Qt 6 Widgets, QADS advanced docking) and Universal 2 binary support.
- **Interactive Links**: Clickable hyperlinks for macOS edition repository, upstream repository, and GNU GPL v3 license terms.

---

## 5. CI/CD Pipeline & Qt 6.12 for macOS 15+ (Sequoia)

### GitHub Actions Matrix Expansion

In `.github/workflows/build.yml` and `.github/workflows/release.yml`, dedicated matrix runners were added for **Qt 6.12** specifically targeting macOS 15+:

| Architecture      | Runner Name      | Deployment Target | Qt Version | Host Arch |
| :---------------- | :--------------- | :---------------- | :--------- | :-------- |
| **Apple Silicon** | `macos-15`       | `15.0` (Sequoia)  | `6.12`     | `arm64`   |
| **Intel x86_64**  | `macos-15-intel` | `15.0` (Sequoia)  | `6.12`     | `x86_64`  |

Both jobs compile natively, link against `Qt6Core5Compat`, package standalone `.dmg` distributions with `PackagingMac.cmake`, and upload verified release assets.

---

## 6. Tailored MegaLinter Configuration (`.mega-linter.yml`)

A custom `.mega-linter.yml` configuration has been established at repository root to align with the repository stack:

- **Enabled Linters**: `ACTION_ACTIONLINT`, `BASH_SHELLCHECK`, `BASH_SHFMT`, `CMAKE_LINT`, `CPP_CPPCHECK`, `GIT_GIT_DIFF`, `JSON_JSONLINT`, `JSON_PRETTIER`, `MARKDOWN_MARKDOWNLINT`, `YAML_YAMLLINT`, `YAML_PRETTIER`, `XML_XMLLINT`.
- **Disabled Irrelevant Descriptors**: 30+ descriptors disabled (PHP, Ruby, C#, Java, Dart, Terraform, Ansible, Kubernetes, Helm, Dockerfile, etc.).
- **Suppressed False Positives**: Disabled `COPYPASTE` and `SPELL` checkers which generated false alarms on Scintilla/Lexilla vendored lexers and Indonesian localization strings.
- **Path Exclusions**: Filter regex ignores `thirdparty/`, `.deps/`, `build/`, `scratch/`, `.trunk/`, and `i18n/`.

---

## 7. Third-Party Component Stack & Default Context Menu

### Updated Third-Party Stack (from NoteSciPad Integration)

The third-party subsystem under `thirdparty/` has been upgraded and harmonized:

- **Scintilla**: Upgraded from `5.6.6` to **`5.6.7`**, incorporating latest bugfixes for high-DPI font rendering, multi-cursor handling, and line marker decorations.
- **Lexilla**: Upgraded from `5.5.3` to **`5.5.4`**, featuring enhanced lexing accuracy for C++, YAML, HTML, and TCL.
- **Lua**: Upgraded from `5.4.9` to **`5.5.1`**, providing optimized bytecode execution and modern scripting capabilities.
- **Python 3 Integration**: Embedded automated discovery for Python 3.13+ in `thirdparty/CMakeLists.txt` with graceful fallback.

### Default NotepadNext.ini Context Menu

The default context menu specification for the code editor is configured as:

```ini
[Gui]
EditorContextMenu=Undo, Redo, , Cut, Copy, Paste, Delete, , SelectAll, , UpperCase, LowerCase, , Open, Save, Find, , Base64Encode, URLEncode, , Base64Decode, URLDecode
```

In `src/dialogs/MainWindow.cpp`, this sequence serves as the native fallback and includes token whitespace sanitization (`.trimmed()`) to reliably parse comma-space formatted INI entries.

---

## 8. Release Evolution: v0.15-01 through v0.15-04

| Version     | Release Date | Key Milestones & Architectural Advancements                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| :---------- | :----------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **0.15-01** | 2026-09-10   | Full core rebase onto upstream NotepadNext `v0.15`; expanded default editor script (`NotepadNext-Default-Editor.sh` v1.6.0) to 470+ file extensions across 15 categories; resolved modern macOS Settings pane registration; extensive Indonesian translation foundation.                                                                                                                                                                                                               |
| **0.15-02** | 2026-09-25   | Upgraded embedded Lua runtime to 5.4.9; defined `LUA_QL`/`LUA_QS` compatibility macros; added static linkage to scroll wheel event handler; packaged dual-arch release DMGs inside ZIP archives (`arm64` and `x64`).                                                                                                                                                                                                                                                                   |
| **0.15-03** | 2026-10-19   | Upstream sync with `dail8859/NotepadNext`; refined Indonesian translation across 64 UI elements; synchronized packaging workflow for automated macOS DMG distribution.                                                                                                                                                                                                                                                                                                                 |
| **0.15-04** | 2026-10-21   | 13-pillar security audit and hardening (CWE-120 fix, SIGSEGV protection, pointer safety); application bundle ID standardization to `com.github.alsyundawy.NotepadNext`; high-resolution macOS squircle branding icons; modernized rich-text About dialog; comparative release notes v0.15-04 vs v0.15 official; Qt 6.12 CI runners for macOS 15+; tailored MegaLinter config; Scintilla 5.6.7 / Lexilla 5.5.4 / Lua 5.5.1 third-party integration; default context menu configuration. |

---

## 9. Localization Architecture (English & Indonesian)

In accordance with user requirements, the application's translation system has been streamlined exclusively to:

1. 🇬🇧 **English (`en_US`)**: Base source strings and reference translation.
2. 🇮🇩 **Bahasa Indonesia (`id_ID`)**: Native, complete translation with 414 XML strings covering menus, status bar, docks, preferences, find/replace dialogs, and Scintilla contextual menus.

All other language `.ts` files have been removed from `i18n/`, reducing repository bloat while accelerating CMake build and packaging stages.

---

## 10. Verification & Quality Gates

### Linters Executed

- **Trunk**: `actionlint`, `bandit`, `black`, `checkov`, `isort`, `markdownlint`, `oxipng`, `prettier`, `ruff`, `shellcheck`, `shfmt`, `svgo`, `taplo`, `yamllint`.
- **Static Analyzers**: `cppcheck`, `flawfinder`, `actionlint`.
- **Status**: 100% clean passing state across all modified and generated files.
