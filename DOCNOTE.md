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

## 4. Modernized Adaptive "About Notepad Next" Dialog (`AboutDialog`)

The previous basic `QMessageBox` popup has been replaced by a dedicated, multi-tab `AboutDialog` (`src/dialogs/AboutDialog.h` & `src/dialogs/AboutDialog.cpp`):

- **Dynamic System Palette Adaptation**: Adheres strictly to the invariant that **NotepadNext has no in-app dark mode toggle**. Instead of hardcoding colors, the dialog dynamically extracts its palette from `this->palette()` (`QPalette::Window`, `QPalette::WindowText`, `QPalette::Highlight`, `QPalette::Base`, `QPalette::Mid`). It automatically recalculates contrasting foregrounds and background cards on `QEvent::PaletteChange` and `QEvent::ApplicationPaletteChange`, ensuring crystal-clear readability and zero color clashing under both macOS Light and Dark system appearances.
- **Six Specialized Information Tabs**:
  1. **About**: 80×80 application icon (`:/icons/NotepadNext.png`), version & stable badges, official attribution, legal disclaimer, and metadata specifications table.
  2. **Maintainer**: Detailed profile of macOS Maintainer Harry Dertin Sutisna Alsyundawy (`@alsyundawy`), ALSYUNDAWY IT SOLUTION, contact channels, and donation links (PayPal & Indonesian QRIS barcode).
  3. **Credits & Attribution**: Comprehensive attribution for Justin Dailey (`@dail8859`), Neil Hodgson (Scintilla & Lexilla), The Qt Company, uchardet, and NoteSciPad.
  4. **Diagnostics**: Real-time system diagnostics including OS version, Darwin kernel, host & build CPU architecture, Qt runtime version, and Retina high-DPI scaling ratio, with an instant "Copy Diagnostics to Clipboard" action.
  5. **Shortcuts**: Structured table of essential productivity keyboard shortcuts.
  6. **License**: Complete GNU General Public License v3.0 statement with official URL.
- **Stability & Reentrancy Hardening**: Features a strict boolean reentrancy guard (`m_isUpdatingTheme`) preventing cascading recursive event loops during palette changes and stylesheet calculations, eliminating stack-overflow crashes on opening.

---

## 5. Comprehensive File Encoding & uchardet Architecture

Architected and hardened specifically for NotepadNext-MacOS:

- **Multi-Stage Detection Pipeline (`EncodingDetector`)**:
  1. _Stage 1_: Byte Order Mark (BOM) recognition for UTF-8, UTF-16LE, and UTF-16BE.
  2. _Stage 2_: Interleaved NUL byte analysis for UTF-16 without BOM.
  3. _Stage 3_: Document header parsing for XML declaration (`encoding="..."`), HTML `<meta charset="...">`, Python/Ruby coding cookies (`# -*- coding: ... -*-`), and CSS `@charset`.
  4. _Stage 4_: Strict UTF-8 multi-byte validator.
  5. _Stage 5_: Mozilla Universal Charset Detector (`uchardet`) fallback.
  6. _Stage 6_: Locale ANSI fallback (Windows-1252 / ISO-8859-1).
- **Interactive Encoding Dialog (`EncodingSelectionDialog`)**:
  - Live search filter across Unicode, Western European, East Asian, Cyrillic, Central European, and Legacy DOS codepages.
  - Displays detected document encoding, BOM status, detection technique, and confidence percentage.
  - Dual operational pathways: convert current text in buffer or reload file from disk.
- **Top-Level `Encoding` Menu**:
  - Direct conversion to UTF-8 (`Ctrl+Alt+U`), UTF-8 BOM, ANSI (`Ctrl+Alt+A`), UTF-16 LE, and UTF-16 BE.
  - `Reopen with Encoding` submenu with major international encodings.
- **Status Bar Integration**:
  - `unicodeType` StatusLabel displays formatted active encoding and BOM string.
  - Tooltip: `Encoding / BOM (Click to view or convert)`. Clicking triggers `actionShowEncodingInfo` directly.

---

## 6. New Window & Multi-Instance Architecture

To enable opening multiple independent application windows on macOS:

- **New Window Action (`actionNewWindow`)**:
  - Bound to `Ctrl+Shift+N` (`Cmd+Shift+N` on macOS) in the File menu.
  - Launches independent secondary processes without colliding with `SingleApplication` primary instance checks.
- **Native macOS Process Spawning (`/usr/bin/open -n`)**:
  - Automatically discovers the `.app` bundle hierarchy from `QCoreApplication::applicationFilePath()`.
  - Spawns via `/usr/bin/open -n <Bundle.app> --args --multi-instance`, ensuring macOS WindowServer assigns independent window focus, docks, and menu bar handlers.
  - Falls back cleanly to `QProcess::startDetached` for standalone binary executions.
- **Unified Settings Synchronization**:
  - `ApplicationSettings` explicitly targets `"NotepadNext"` organization and application scopes, allowing all concurrent windows to share and update the same `NotepadNext.ini` user configuration without fragmentation.

---

## 7. Redesigned Unsaved Changes & Exit Dialog (`SaveModifiedDialog`)

Replaces the plain `QMessageBox` in `MainWindow::promptForSave` with a dedicated, custom-crafted dialog designed strictly around macOS Human Interface Guidelines:

- **Visual Hierarchy & Header**: Displays the crisp 64×64 squircle Retina application icon, bold contextual headline (`Do you want to save the changes made to the document "..."?`), and a clear warning explaining that unsaved changes will be permanently discarded.
- **Document Preview Cards**:
  - For single-file prompts: Renders a dedicated card displaying the document name and full filesystem path.
  - For multi-document batch closures: Embeds a scrollable list card detailing every modified file with status tags (`[New]` or full path).
- **macOS Button Conventions & Keyboard Shortcuts**:
  - **Don't Save / Don't Save All**: Destructive action placed on the far left, directly accessible via standard macOS `Cmd+D` shortcut.
  - **Cancel**: Neutral action placed in the center-right, triggered via `Escape` or `Cmd+.`.
  - **Save / Save All**: Accent-highlighted default action placed on the far right, triggered via `Return`, `Enter`, or `Cmd+S`.
- **Dynamic System Palette Adaptation**: Inherits active Light/Dark OS appearance styles dynamically via `palette()` with recursion-guarded updates.

---

## 8. CI/CD Pipeline & Qt 6.12 for macOS 15+ (Sequoia)

### GitHub Actions Matrix Expansion

In `.github/workflows/build.yml` and `.github/workflows/release.yml`, dedicated matrix runners were added for **Qt 6.12** specifically targeting macOS 15+:

| Architecture      | Runner Name      | Deployment Target | Qt Version | Host Arch |
| :---------------- | :--------------- | :---------------- | :--------- | :-------- |
| **Apple Silicon** | `macos-15`       | `15.0` (Sequoia)  | `6.12`     | `arm64`   |
| **Intel x86_64**  | `macos-15-intel` | `15.0` (Sequoia)  | `6.12`     | `x86_64`  |

Both jobs compile natively, link against `Qt6Core5Compat`, package standalone `.dmg` distributions with `PackagingMac.cmake`, and upload verified release assets.

---

## 9. Tailored MegaLinter Configuration (`.mega-linter.yml`)

A custom `.mega-linter.yml` configuration has been established at repository root to align with the repository stack:

- **Enabled Linters**: `ACTION_ACTIONLINT`, `BASH_SHELLCHECK`, `BASH_SHFMT`, `CMAKE_LINT`, `CPP_CPPCHECK`, `JSON_JSONLINT`, `JSON_PRETTIER`, `MARKDOWN_MARKDOWNLINT`, `YAML_PRETTIER`, `XML_XMLLINT`.
- **Disabled Irrelevant Descriptors**: 30+ descriptors disabled (PHP, Ruby, C#, Java, Dart, Terraform, Ansible, Kubernetes, Helm, Dockerfile, etc.).
- **Suppressed False Positives**: Disabled `COPYPASTE` and `SPELL` checkers which generated false alarms on Scintilla/Lexilla vendored lexers and Indonesian localization strings.
- **Path Exclusions**: Filter regex ignores `thirdparty/`, `.deps/`, `build/`, `scratch/`, `.trunk/`, and `i18n/`.

---

## 10. Third-Party Component Stack & Default Context Menu

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

## 11. Release Evolution: v0.15-01 through v0.15-04

| Version     | Release Date | Key Milestones & Architectural Advancements                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
| :---------- | :----------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **0.15-01** | 2026-09-10   | Full core rebase onto upstream NotepadNext `v0.15`; expanded default editor script (`NotepadNext-Default-Editor.sh` v1.6.0) to 470+ file extensions across 15 categories; resolved modern macOS Settings pane registration; extensive Indonesian translation foundation.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| **0.15-02** | 2026-09-25   | Upgraded embedded Lua runtime to 5.4.9; defined `LUA_QL`/`LUA_QS` compatibility macros; added static linkage to scroll wheel event handler; packaged dual-arch release DMGs inside ZIP archives (`arm64` and `x64`).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
| **0.15-03** | 2026-10-19   | Upstream sync with `dail8859/NotepadNext`; refined Indonesian translation across 64 UI elements; synchronized packaging workflow for automated macOS DMG distribution.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |
| **0.15-04** | 2026-10-21   | 13-pillar security audit and hardening (CWE-120 fix, SIGSEGV protection, pointer safety); New Window & multi-instance architecture (`/usr/bin/open -n`, `Ctrl+Shift+N`); redesigned macOS HIG Save & Exit prompt (`SaveModifiedDialog`); AboutDialog crash & stack overflow resolution (`m_isUpdatingTheme` reentrancy guard); comprehensive File Encoding menu & multi-stage `uchardet` engine; modernized adaptive `AboutDialog` with dynamic system palette integration; application bundle ID standardization to `com.github.alsyundawy.NotepadNext`; high-resolution macOS squircle branding icons; comparative release notes; Qt 6.12 CI runners for macOS 15+; tailored MegaLinter config; Scintilla 5.6.7 / Lexilla 5.5.4 / Lua 5.5.1 integration; default context menu configuration. |

---

## 12. Localization Architecture (English & Indonesian)

In accordance with user requirements, the application's translation system has been streamlined exclusively to:

1. 🇬🇧 **English (`en_US`)**: Base source strings and reference translation.
2. 🇮🇩 **Bahasa Indonesia (`id_ID`)**: Native, complete translation with 425 XML strings covering menus, status bar, docks, preferences, find/replace dialogs, save prompts, and Scintilla contextual menus.

All other language `.ts` files have been removed from `i18n/`, reducing repository bloat while accelerating CMake build and packaging stages.

---

## 13. Verification & Quality Gates

### Linters Executed

- **Trunk**: `actionlint`, `bandit`, `black`, `checkov`, `isort`, `markdownlint`, `oxipng`, `prettier`, `ruff`, `shellcheck`, `shfmt`, `svgo`, `taplo`, `yamllint`.
- **Static Analyzers**: `cppcheck`, `flawfinder`, `actionlint`.
- **Status**: 100% clean passing state across all modified and generated files.
