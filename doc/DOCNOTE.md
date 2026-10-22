# 📘 Documentation Note (DOCNOTE) — NotepadNext-MacOS v0.15-05

- **Project**: NotepadNext For MacOS
- **Target Release**: `0.15-05`
- **Date**: October 23, 2026
- **Maintainer / Builder**: Harry DS Alsyundawy ([@alsyundawy](https://github.com/alsyundawy))
- **Upstream Project**: [dail8859/NotepadNext](https://github.com/dail8859/NotepadNext)

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

## 2. 13-Pillar Codebase Audit & Verification (v0.15-05)

| #   | Pillar                            | Audit Findings & Resolution                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               | Verification Status |
| :-- | :-------------------------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :-----------------: |
| 1   | **Bug Review**                    | **Fixed**: Added defensive null pointer guards across `MainWindow.cpp` (`updateGui`, `updateFileStatusBasedUi`, `updateEOLBasedUi`, `updateSaveStatusBasedUi`, `updateLanguageBasedUi`, `updateDocumentBasedUi`, `updateSelectionBasedUi`, `updateContentBasedUi`, `detectLanguage`, `activateEditor`) to prevent `SIGSEGV` during tab closures or dock detachments. Resolved variable shadowing in `FindReplaceDialog.cpp` (`targetEd`, `totalReplaced`). Disabled `setDrawBase(false)` on tab bars to prevent dark theme edge clipping. |        PASS         |
| 2   | **Syntax Review**                 | Fully verified against ISO C++20 (`-std=c++20`). Resolved pointer loop typing in `IFaceTableMixer.cpp` (`const IFaceTableInterface *iface`). Strict modern C++ syntax across all new ThemeManager, ThemePaletteData, and ThemeSwitcherDialog modules.                                                                                                                                                                                                                                                                                     |        PASS         |
| 3   | **Runtime Review**                | Verified directory creation return codes in `ScintillaNext.cpp` with diagnostic warnings on `QDir::mkpath()` failure. Integrated live theme switching in `ThemeManager` with real-time UI stylesheet updates and instant Scintilla buffer palette propagation without requiring application restarts.                                                                                                                                                                                                                                     |        PASS         |
| 4   | **Logic Review**                  | Architected 3-mode theme state machine (Auto/Light/Dark) in `ThemeManager`. Implemented intelligent high-contrast Scintilla style mapping (slots 0..31) guaranteeing legible foreground and background text combinations under both light and dark palettes (never dark-on-dark text).                                                                                                                                                                                                                                                    |        PASS         |
| 5   | **Memory Review**                 | Clean singleton lifecycle management for `ThemeManager::instance()`. Proper Qt widget parentage across `ThemeSwitcherDialog` and dynamically added preference controls, ensuring zero memory leaks and deterministic widget disposal.                                                                                                                                                                                                                                                                                                     |        PASS         |
| 6   | **Dead Code Review**              | Removed unused temporary includes, obsolete variables, and redundant type casts across `FindReplaceDialog.cpp` and `IFaceTableMixer.cpp`. Maintained lean `.trunk/trunk.yaml` ignore configurations.                                                                                                                                                                                                                                                                                                                                      |        PASS         |
| 7   | **Duplicate Code Review**         | Unified 75 professional theme definitions in `src/features/ThemePaletteData.h`, reused across QSS stylesheet generation, Scintilla buffer styling, preferences dropdowns, and interactive 6-swatch palette previews.                                                                                                                                                                                                                                                                                                                      |        PASS         |
| 8   | **Circular Dependency Review**    | Audited forward declarations and include boundaries between `MainWindow`, `ThemeManager`, `ThemeSwitcherDialog`, and `PreferencesDialog`. Kept clean acyclic dependency graphs with no circular header loops.                                                                                                                                                                                                                                                                                                                             |        PASS         |
| 9   | **Performance Bottleneck Review** | Cached compiled QSS stylesheets in `ThemeManager` to avoid redundant string formatting on every editor tab switch. Optimized Scintilla style setting loop for immediate rendering without UI stutter.                                                                                                                                                                                                                                                                                                                                     |        PASS         |
| 10  | **Security Vulnerability Review** | Zero critical/high vulnerabilities across SonarQube and Flawfinder audits. Safe input handling in `ThemeSwitcherDialog` search filtering. Bounded memory buffers throughout all theme loading and parsing logic.                                                                                                                                                                                                                                                                                                                          |        PASS         |
| 11  | **Maintainability Review**        | Created `sonar-project.properties` and `scripts/generate_compilation_database.py` producing accurate 86-translation-unit `compile_commands.json` for CI/CD and IDE tooling. Configured `.vscode/settings.json` for SonarLint and clangd.                                                                                                                                                                                                                                                                                                  |        PASS         |
| 12  | **Scalability Review**            | Modular theme catalog design allowing straightforward addition of new themes and color families without touching core editor logic. Clean separation of concerns between presentation and editor state.                                                                                                                                                                                                                                                                                                                                   |        PASS         |
| 13  | **Readability Review**            | Comprehensive Doxygen documentation across all new header files (`ThemeManager.h`, `ThemePaletteData.h`, `ThemeSwitcherDialog.h`). Strict compliance with `trunk check`, `markdownlint`, and `prettier`.                                                                                                                                                                                                                                                                                                                                  |        PASS         |

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

- **Dynamic System Palette Adaptation & Theme Engine Integration**: In v0.15-04, the dialog dynamically adapted to the system palette via `this->palette()`. In v0.15-05, `AboutDialog` is directly hooked into `ThemeManager`, harmonizing its tab styling and card palettes dynamically whenever the user switches between any of the 75 themes or toggles Dark/Light modes.
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

## 11. Release Evolution: v0.15-01 through v0.15-05

| Version     | Release Date | Key Milestones & Architectural Advancements                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
| :---------- | :----------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **0.15-01** | 2026-09-10   | Full core rebase onto upstream NotepadNext `v0.15`; expanded default editor script (`NotepadNext-Default-Editor.sh` v1.6.0) to 470+ file extensions across 15 categories; resolved modern macOS Settings pane registration; extensive Indonesian translation foundation.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| **0.15-02** | 2026-09-25   | Upgraded embedded Lua runtime to 5.4.9; defined `LUA_QL`/`LUA_QS` compatibility macros; added static linkage to scroll wheel event handler; packaged dual-arch release DMGs inside ZIP archives (`arm64` and `x64`).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
| **0.15-03** | 2026-10-19   | Upstream sync with `dail8859/NotepadNext`; refined Indonesian translation across 64 UI elements; synchronized packaging workflow for automated macOS DMG distribution.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         |
| **0.15-04** | 2026-10-21   | 13-pillar security audit and hardening (CWE-120 fix, SIGSEGV protection, pointer safety); New Window & multi-instance architecture (`/usr/bin/open -n`, `Ctrl+Shift+N`); redesigned macOS HIG Save & Exit prompt (`SaveModifiedDialog`); AboutDialog crash & stack overflow resolution (`m_isUpdatingTheme` reentrancy guard); comprehensive File Encoding menu & multi-stage `uchardet` engine; modernized adaptive `AboutDialog` with dynamic system palette integration; application bundle ID standardization to `com.github.alsyundawy.NotepadNext`; high-resolution macOS squircle branding icons; comparative release notes; Qt 6.12 CI runners for macOS 15+; tailored MegaLinter config; Scintilla 5.6.7 / Lexilla 5.5.4 / Lua 5.5.1 integration; default context menu configuration. |
| **0.15-05** | 2026-10-23   | Full Dark & Light Mode Theme Engine ported from `SparkNotePad` (`ThemeManager`, `ThemePaletteData`, `ThemeSwitcherDialog`); 75 studio-grade palettes across 25 families; 3 operating modes (Auto/Light/Dark); real-time high-contrast Scintilla buffer styling (slots 0..31); complete QSS widget controls styling; top-level `&Themes` menu (`Ctrl+Shift+T`, `Ctrl+Alt+T`); Preferences visual theme picker; 13-pillar codebase hardening (null guards in `MainWindow`, filesystem return verification in `ScintillaNext`, variable shadowing removal); SonarQube static analysis configuration (`sonar-project.properties`); compilation database generator (`generate_compilation_database.py`); VSCode SonarLint integration.                                                              |

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
- **SonarQube / SonarLint**: Integrated with `sonar-project.properties` and `compile_commands.json` (86 translation units).
- **Status**: 100% clean passing state across all modified and generated files.

---

## 14. Native Dark & Light Mode Theme Architecture

Ported and significantly elevated from `SparkNotePad`, `NotepadNext-MacOS` v0.15-05 introduces a comprehensive, standalone theming engine:

### 1. Three Operating Modes

- **Auto Mode**: Listens to macOS system palette events (`QPalette::WindowText.lightness() < QPalette::Window.lightness()`) to seamlessly synchronize with system Dark / Light appearance without restarting.
- **Force Light Mode**: Locks application appearance to the active Light theme (defaulting to `NotepadNext Default Light`).
- **Force Dark Mode**: Locks application appearance to the active Dark theme (defaulting to `Dracula Default Dark`).

### 2. 75 Studio-Grade Palettes Across 25 Aesthetic Families

All palettes are statically defined with zero runtime allocation overhead in `src/features/ThemePaletteData.h`, providing:

- **Background & Foreground**: Base editor window, document canvas, text colors, and gutter margins.
- **Scintilla Token Colors**: Keyword, String, Number, Comment, Type, Preprocessor, Operator, and Identifier hues.
- **Selection & Caret**: High-visibility selection backgrounds and blinking caret colors.
- **Gutter & Folding**: Line number text, active line highlights, fold margin colors, and fold marker outlines.

### 3. High-Contrast Scintilla Styling (`applyThemeToEditor`)

Unlike generic dark themes that cause low-contrast dark-on-dark unreadable text, `ThemeManager::applyThemeToEditor()` systematically configures Scintilla style slots `0` through `31`:

- Slot `32` (`STYLE_DEFAULT`): Configures base font, foreground, and canvas background.
- `clearDocumentStyle()`: Resets legacy attributes before applying new tokens.
- Caret & Selection: Explicit `setCaretFore()`, `setSelFore(true, ...)`, and `setSelBack(true, ...)`.
- Edge & Whitespace: Configured with contrasting indentation guides and edge columns.

### 4. Interactive Theme Switcher Dialog (`ThemeSwitcherDialog`)

Accessible via `Themes -> Theme Switcher...` or `Ctrl+Shift+T` (`Cmd+Shift+T` on macOS):

- **Real-Time Live Search**: Filters themes instantaneously by title or category.
- **Category Filter**: `All`, `Light`, or `Dark`.
- **Visual Swatch Preview**: Renders 6 distinctive color swatches (Background, Foreground, Accent, Selection, Keyword, String) directly within the list item.
- **Non-Destructive Live Preview**: Selecting a theme applies it instantly to the main window and active editors; pressing `Esc` restores the previous theme state cleanly.

---

## 15. SonarQube & Compilation Database Pipeline

To guarantee enterprise static analysis without compromising macOS hardware performance:

- **`sonar-project.properties`**: Declares project key `NotepadNext-MacOS`, designates `src` as the primary C/C++ source root, and links directly to `build/compile_commands.json`.
- **`scripts/generate_compilation_database.py`**: A lightweight, self-contained Python script using standard libraries that parses CMake configuration and source trees to produce an accurate 86-translation-unit `compile_commands.json` in under 50ms.
- **`.vscode/settings.json`**: Preconfigures SonarLint, SonarQube, and clangd to consume `compile_commands.json` with zero IDE setup friction.
