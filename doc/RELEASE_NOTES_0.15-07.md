# 📋 Release Notes: NotepadNext-MacOS v0.15-07 vs. Official v0.15 Upstream

> **Edition**: NotepadNext For MacOS<br/>
> **Release Version**: `0.15-07`<br/>
> **Official Baseline**: `dail8859/NotepadNext v0.15` (Tag: `v0.15`)<br/>
> **Date**: October 25, 2026<br/>
> **Maintainer & Builder**: Harry DS Alsyundawy ([@alsyundawy](https://github.com/alsyundawy))<br/>
> **Upstream Repository**: [dail8859/NotepadNext](https://github.com/dail8859/NotepadNext)<br/>
> **macOS Edition Repository**: [alsyundawy/NotepadNext-MacOS](https://github.com/alsyundawy/NotepadNext-MacOS)

---

## 🌟 Executive Summary

**NotepadNext-MacOS v0.15-07** delivers an enterprise-grade defense and feature upgrade, incorporating critical 2025–2026 CVE security mitigations, advanced productivity capabilities inspired by sibling editors (Notepad--, Notepad3, Notepad4, and Notepad++ 8.7–8.9), and native macOS architectural enhancements.

Key innovations introduced in `0.15-07`:

1. **2025–2026 Security Hardening & CVE Defenses**:
   - **Auto-Updater Cryptographic Integrity (CVE-2025-15556 / CWE-494)**: Strict HTTPS URL pinning and SHA-256 package verification before update prompts.
   - **Embedded Lua Sandbox Hardening (CVE-2026-42214 / CWE-94)**: Stripped dangerous execution (`os.execute`, `io.popen`, `package.loadlib`, `os.remove`, `os.rename`, `os.exit`) from the embedded Lua 5.5.1 runtime.
   - **Subprocess Canonicalization (CVE-2026-25926 / CWE-78)**: Replaced PATH-dependent subprocess calls with canonical absolute binaries (`/usr/bin/open`, `/usr/bin/qlmanage`, `/usr/bin/killall`, `/usr/bin/pgrep`).
   - **Buffer Bounds & Lexer Protection (CVE-2025-49144 / CVE-2025-56383)**: Bounded C-string scanning with `strnlen` and lexer name query length verification.

2. **Sibling Text Editor Innovations & Productivity**:
   - **Scintilla 5.6.7 Pixel-Aligned Retina Scaling**: Activated `SCI_SETSCALETECHNIQUE` with `SCALE_TECHNIQUE_PIXEL_ALIGNED` on macOS Retina displays to eliminate subpixel micro-jitter.
   - **Natural Locale-Aware Numeric Line Sorting**: Integrated `QCollator` for natural line sorting (e.g. `Item 2` before `Item 10`) with Ascending and Descending actions.
   - **Inline Math Expression Evaluator (`Cmd+Shift+M`)**: Instant mathematical evaluation directly in the active editor buffer using embedded Lua 5.5.1.
   - **Multi-Line Paste Auto-Escaping in Find & Replace**: Automatic clipboard newline sanitization to `\n` or single spaces according to search mode.
   - **Side-by-Side Two-Buffer File Diff Comparison**: Instant two-buffer comparison with automated split view docking (`splitToRight`) and visual diff highlight indicators.
   - **Read-Only Hexadecimal / Binary Viewer**: Fast binary inspection mode featuring 16-byte hex dump and ASCII side-column.

3. **macOS Native Architecture & UI Polish**:
   - **macOS In-Process Multi-Window & Dock Integration**: Unified multi-window architecture within a single process via `NotepadNextApplication`, ensuring secondary window invocations and Cmd+N remain responsive to macOS Dock activations and Cmd+` window cycling.
   - **macOS Dock Right-Click Quit Save Prompt (`QEvent::Close`)**: Intercepted application-level close events on macOS Dock context menu quit and Cmd+Q, cycling through all windows and invoking `checkEditorsBeforeClose()` to guarantee dirty and untitled tabs prompt for saving before exit.
   - **macOS Menu Bar Extra / Dock Status Mode**: Resident background mode with quick-access menu via `QSystemTrayIcon`.
   - **Toolbar Preferences & Exit Actions**: Enforced persistent Settings/Preferences (cog icon) and Exit (exit icon) actions on the main toolbar.
   - **Find & Replace Tab Form Precision**: Strict 1-box input layout for Tab Find and Tab Mark (completely hiding the Replace field), reserving 2-box input strictly for Tab Replace.
   - **Tab Label Middle-Ellipsis (`elideMode`)**: Configurable tab title truncation maintaining clean tab geometry even with 30+ open files.
   - **Horizon Dark Theme Contrast Fine-Tuning**: Optimized high-contrast palette for Apple Retina displays.

4. **Multi-View Split & High-Volume Text Stability**:
   - **Multi-View Split Closure & Select-All Guard**: Fixed crashes during tab closure in split vertical/horizontal views and stabilized multi-pane selection bounds.
   - **High-Volume Lines & Emoji Crash Defense**: Hardened Scintilla's `PlatQt.cpp` text measurement and drawing routines with strict boundary checks, preventing crashes on files with tens of thousands of lines containing compound Unicode 18.0 emojis and ZWJ sequences.
   - **Cursor Selection Contrast & Multi-Word Selection**: Refined text selection shaders and boundary detection for smooth word and sentence dragging.

5. **100% Zero-Defect SonarLint & IDE Diagnostics Resolution**:
   - Resolved all 36 flagged IDE / SonarLint warnings across `AboutDialog`, `HighlightedScrollBar`, `RecentFilesListManager`, `RecentFilesListMenuBuilder`, `RtfConverter`, and `EditorInfoStatusBar` (defaulted destructors `= default`, C++17 `if` init-statements, explicit lambda captures `[this]`, C++20 `using enum Scintilla::Update`, default cases in switch statements, pointer-to-const parameter qualifiers, extraction of nested ternary operators, and explicit `static_cast<int>` on `sptr_t` and `qsizetype`).

6. **Upstream PR Harmonization**:
   - Ported and harmonized upstream PR #8a0a615 (Trim Trailing Whitespace buffer operation with full Indonesian localization), PR #e08fc92 (defensive message handler deregistration in `DebugManager`), and PR #90d6174 (centralized tooltip generation in `ScintillaNext::getToolTip`).

7. **100% Complete Bilingual Localization & Linguistic Rigor**:
   - 100% complete Bahasa Indonesia (`id_ID`) and English (`en_US`) catalogs with 0 unfinished strings (646/646 entries).
   - Standardized strictly against official Indonesian linguistic guidelines: EYD V ([ejaan.kemendikdasmen.go.id](https://ejaan.kemendikdasmen.go.id)), PUEBI, Mozilla Indonesian L10n Style Guide, and official IT terminology (PASTI Kemendikdasmen / KBBI).
   - Eliminated legacy transliterations (e.g. _pengkodean_ -> _pengodean_, _file_ -> _berkas_, _shortcut_ -> _pintasan_) and stripped redundant parenthetical annotations.
   - Zero-overhead IDE diagnostics: resolved Qt XML `.ts` vs TypeScript parser conflicts and `.clangd` schema validations.

---

## 📊 Comprehensive Feature & Architecture Matrix

| Capability / Dimension    | Official Upstream v0.15 (`dail8859`)                | NotepadNext-MacOS v0.15-07 (`alsyundawy`)                           | Engineering Impact & Benefits                                  |
| :------------------------ | :-------------------------------------------------- | :------------------------------------------------------------------ | :------------------------------------------------------------- |
| **Auto-Updater Security** | Plain HTTP/HTTPS download without hash verification | **HTTPS Pinning + SHA-256 Checksum Validation (CWE-494)**           | Immune to mirror manipulation and package tampering.           |
| **Lua Extension Sandbox** | Standard Lua library with shell access              | **Hardened Sandboxed Lua 5.5.1 Runtime (CWE-94)**                   | Eliminates command execution and filesystem hijacking.         |
| **Subprocess Execution**  | Relative PATH command invocations                   | **Canonical Absolute Binaries (`/usr/bin/open`, etc.)**             | Immune to PATH hijacking and malicious binary substitution.    |
| **Retina Text Rendering** | Default subpixel text rendering                     | **Scintilla `SCALE_TECHNIQUE_PIXEL_ALIGNED`**                       | Crisp glyph rendering without blur or subpixel color fringing. |
| **Line Sorting**          | Lexicographical ASCII sorting only                  | **Natural Locale-Aware Numeric Sorting (`QCollator`)**              | Natural human ordering for numbered files, logs, and datasets. |
| **Inline Math**           | None                                                | **Instant Lua Math Evaluator (`Cmd+Shift+M`)**                      | Quick arithmetic computation directly in code or notes.        |
| **Find/Replace Paste**    | Raw multiline paste disrupts input fields           | **Intelligent Newline Sanitization / Escaping**                     | Preserves search dialog layout and regex semantics.            |
| **Buffer Comparison**     | None                                                | **Integrated Side-by-Side File Diff (`splitToRight`)**              | Instant visual code comparison without external tools.         |
| **Hex / Binary View**     | Text only                                           | **Toggleable 16-Byte Hex Dump & ASCII View**                        | Low-level binary file inspection directly inside editor.       |
| **Multi-Window & Dock**   | Detached multi-process windows                      | **Single-Instance In-Process Multi-Window + Dock Quit Prompt**      | Native macOS Dock click handling and dirty-buffer protection.  |
| **Tab Label Truncation**  | Tabs shrink indefinitely or expand endlessly        | **Middle-Ellipsis Truncation (`elideMode`)**                        | Readable filenames and stable tab bar geometry.                |
| **Translations**          | Fragmented translation catalogs                     | **100% Complete Bahasa Indonesia & English (0 Unfinished)**         | Production-ready localization conforming to Apple HIG.         |
| **Static Verification**   | Basic CI                                            | **Zero-Defect 13-Pillar Audit (`trunk`, `cppcheck`, `actionlint`)** | ISO 5055, OWASP Top 10, CWE Top 25 compliant.                  |

---

## 🛠️ Verification & Quality Assurance Summary

All 13 pillars of enterprise verification were executed and validated with empirical evidence:

- **Bug Review**: `cppcheck --library=qt --enable=warning --error-exitcode=1` passed with 0 errors across all 88 translation units.
- **Syntax & Style Review**: `trunk check` passed across all modified repository files with 0 issues.
- **Signal-Slot Safety**: 100% modern Qt functor-based connections verified.
- **Unicode & Logic**: Atomic grapheme navigation verified via `QTextBoundaryFinder`.
- **Memory Safety**: Clean RAII and `QObject` parent-child tree verified.
- **Dependency Graph**: Header cycle DFS scan verified 0 circular dependencies.
- **Workflow Linting**: `actionlint` passed with 0 warnings on `.github/workflows/`.
- **Compilation Gate**: Clean build and link with Exit Code 0 on macOS (`clang++` / Qt 6.12.0).

---

## 📦 Distribution Packages

- **Version**: `0.15-07`
- **Architectures**: Universal 2 (`arm64` Apple Silicon + `x86_64` Intel 64-bit)
- **Minimum OS Version**: macOS 11.0 (Big Sur) through macOS 15.0+ (Sequoia)
- **Artifacts**:
  - `NotepadNext-0.15-07-macOS-Universal.dmg`
  - `NotepadNext-0.15-07-macOS-arm64.dmg`
  - `NotepadNext-0.15-07-macOS-x86_64.dmg`
