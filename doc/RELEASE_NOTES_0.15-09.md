# 📋 Release Notes: NotepadNext-MacOS v0.15-09 vs. Official v0.15 Upstream

> **Edition**: NotepadNext For MacOS<br/>
> **Release Version**: `0.15-09`<br/>
> **Official Baseline**: `dail8859/NotepadNext v0.15` (Tag: `v0.15`)<br/>
> **Previous Release**: `0.15-08` (`1c308af`)<br/>
> **Date**: November 21, 2026<br/>
> **Maintainer & Builder**: Harry Dertin Sutisna ([@alsyundawy](https://github.com/alsyundawy)) | ALSYUNDAWY IT SOLUTION<br/>
> **Upstream Repository**: [dail8859/NotepadNext](https://github.com/dail8859/NotepadNext)<br/>
> **macOS Edition Repository**: [alsyundawy/NotepadNext-MacOS](https://github.com/alsyundawy/NotepadNext-MacOS)<br/>
> **Feature Donor Project**: [SparkNotePad](https://github.com/alsyundawy/SparkNotePad) `v1.3.8` • [SubnetCalc-MacOS](https://github.com/alsyundawy/SubnetCalc-MacOS) (25 Theme Suites)<br/>
> **Comprehensive Whitepaper**: [UPSTREAM_COMPARISON.md](UPSTREAM_COMPARISON.md) • [DOCNOTE.md](DOCNOTE.md)

---

## 🌟 Executive Summary

**NotepadNext-MacOS v0.15-09** is a landmark feature release delivering comprehensive developer utilities, high-reliability background autosave crash protection, live log tail streaming, POSIX mode auditing, and Scintilla change history tracking—all while strictly preserving 100% of the existing 25 SubnetCalc theme suites, 30 isolated markdown preview themes, and 28 toolbar icon packs.

This release successfully completes all 6 architectural phases defined in the Roadmap:

1. **Phase 1: Resilience & Crash Protection (`AutosaveRecoveryManager` & `CrashRecoveryDialog`)**:
   - 10-second non-blocking background snapshot timer for dirty and untitled buffers.
   - SHA-256 integrity verification preventing corrupted snapshot loading.
   - POSIX 0700/0600 sandbox file permissions (`~/.local/share/NotepadNext/autosave/`).
   - Startup orphaned snapshot detection wizard with differential preview, multi-selection recovery, and clean discard.
2. **Phase 2: Developer Tools & Utilities Hub (`DeveloperToolsDialog`, `DeveloperToolsEngine`, `CodeCompressPretty`)**:
   - Dedicated 5-tab developer hub (`Ctrl+Alt+D` / `Cmd+Alt+D`):
     - *Hash Hub*: MD5, SHA-1, SHA-256, SHA-384, SHA-512, Keccak-256, Keccak-512 with hex/base64 outputs, file digest computation, and instant hash comparator.
     - *MIME & Transcoding*: Base64, URL percent-encoding, HTML entity encoding, and Quoted-Printable bidirectional codecs.
     - *Radix & Text*: Bin, Oct, Dec, Hex converters, ASCII-to-Hex tables, Title Case, Invert Case, and Sentence Case.
     - *Crypto & Security*: NIST-standard password strength evaluation and password-based HTML AES encryption/decryption.
     - *Code Formatting & Minification*: Fast lossless minifiers and pretty-printers for JSON, XML, HTML, CSS, JavaScript, plus JWT token header/payload parsers and UUID v4 generators.
3. **Phase 3: Live Log Tail Monitoring (`TailMonitoringManager` & `TailOptionsDialog`)**:
   - Native non-blocking `tail -f` log streaming without modal reload dialogs.
   - Dynamic configurable poll intervals (250ms, 500ms, 750ms, 1s, 2s, 5s).
   - Auto-scroll to end tracking with user scroll lock detection.
   - Read-only write protection lock during active monitoring.
   - Automatic file truncation and log rotation detection.
4. **Phase 4: POSIX File Inspector & Shebang Dialect Engine (`PosixFileInspector`)**:
   - Real-time POSIX file permission inspection (octal `0755`/`0644`/`0600` and symbolic `rwxr-xr-x`).
   - Instant executable bit toggling (`chmod +x` / `chmod -x`) via menu and shortcut.
   - POSIX timestamp update (`touch`) preserving existing file content.
   - Intelligent script interpreter and dialect detection from first-line shebangs (Bash, Zsh, POSIX sh, Dash, Ksh, FreeBSD rc.d, Python, Lua).
5. **Phase 5: Change History Tracking Margin (`ChangeHistoryDecorator`)**:
   - Native Scintilla margin 3 change tracking with continuous bar markers (`SC_MARK_BAR`).
   - Four distinct status states: unsaved modifications (amber `#E5A50A`), saved changes (emerald green `#22C55E`), reverted to origin (sky blue `#3B82F6`), and reverted to modified (orange-red `#F97316`).
   - Jump to next/previous change navigation.
6. **Phase 6: Tab Ergonomics, Custom Coloring & In-Place Rename (`ClosedTabsManager`, `TabColorManager`, `RenameTabDialog`, `DockedEditor`)**:
   - 25-item LIFO closed-tab recovery stack (`Ctrl+Shift+T` / `Cmd+Shift+T`) restoring cursor positions, visible lines, untitled buffers, and associated tab color.
   - Dedicated keyboard shortcut deconfliction: `Ctrl+Shift+T` (`⇧⌘T`) reserved exclusively for restoring closed tabs; Theme Switcher reassigned to `Ctrl+Alt+Shift+T` (`⌥⇧⌘T`); Rename tab bound to `Ctrl+Shift+R` (`⇧⌘R`).
   - Tab right-click context menu palette coloring with 10 vibrant studio presets (Red, Orange, Yellow, Green, Teal, Cyan, Blue, Purple, Magenta, Coral) with circular anti-aliased preview indicators.
   - Interactive `QColorDialog` Custom Color picker ("Custom Color...") allowing arbitrary RGB/Hex selection.
   - Dynamic stylesheet integration in `DockedEditor` (`CDockWidgetTab`) with theme palette reactivity (`ThemeManager::themeChanged` and `paletteChanged`), applying top accent borders, translucent tints, and active tab highlights across all 25 SubnetCalc theme palettes.
   - Automatic color migration on in-place file rename (`editorRenamed`).
   - In-place file renaming dialog with cross-platform character sanitization and real-time disk collision protection (`Ctrl+Shift+R` / `⇧⌘R` / tab context menu Rename).
7. **Phase 7: Workspace System Transfer & Portable Bundles (`NotepadNextTransferManager` & `TransferBundleDialog`)**:
   - Portable `.npnbundle` workspace archive export and restoration across macOS, FreeBSD, and Linux.
   - Includes preferences, custom themes, sessions, and scripts with SHA-256 integrity validation and optional AES-256-CBC encryption.
8. **Phase 8: Scintilla 5.6.9 & Lexilla 5.5.5 Engine Modernization**:
   - Upgraded core Scintilla editing engine from v5.6.7 to v5.6.9 LTS, and Lexilla lexer library from v5.5.4 to v5.5.5 LTS.
   - Enforced UTF-8 codeUnits bounds protection in `PlatQt.cpp` (`std::min(fit, ui + codeUnits)`).
   - Hardened Lua 5.5.1 runtime.
9. **Phase 9: 42 Modern Programming Language Syntaxes & Lexers**:
   - Expanded language definitions to 134 total programming and configuration languages:
     - TypeScript, Dart, Zig, Mojo, Solidity, Julia, Kotlin, LLVM IR, WebAssembly (WAT), Crystal, Cangjie.
     - Dockerfile, Terraform, Bicep, Crontab, Configuration File (conf), TOML, JSON5, Protobuf.
     - Elixir, Clojure, F#, Racket, Raku, OCaml, Stata, GDScript, Modula, GLSL, HLSL, AsciiDoc, BibTeX, GraphQL, Mermaid, Gettext PO, POV-Ray, Troff, Eiffel, Groovy, Haxe, Hex Dump.
10. **Phase 10: Full Document Smart Auto-Highlighting & Ergonomics (`SmartHighlighter` & `HighlightedScrollBar`)**:
    - Added 100ms debouncing (`debounceTimer`) to `SmartHighlighter` eliminating CPU spikes during rapid keystrokes while maintaining unconstrained full-document search across all file sizes without line or match limits.
    - Precalculated track metrics in `HighlightedScrollBar` for smooth, zero-latency rendering of scrollbar markers across the entire document buffer.
11. **Phase 11: Build Toolchain Modernization & Ninja Prohibition**:
    - Total and permanent elimination of the legacy Ninja generator across all build scripts, IDE configurations, and documentation.
    - CMake top-level `FATAL_ERROR` guard blocking Ninja generator invocations, strictly enforcing standard `Unix Makefiles` (`make -j6`).
    - Hardened OpenSSL 3.x include paths on macOS (`${OPENSSL_INCLUDE_DIR}` and `/usr/local/opt/openssl@3/include`).

---

## 🛡️ Code Quality & Verification Gate

| Metric / Check | Target Standard | Status | Evidence |
| :--- | :--- | :--- | :--- |
| **Compiler Build Gate** | 0 compiler errors, 0 warnings | ✅ PASSED | `make -j6` (Unix Makefiles) compiled with 100% clean exit |
| **Binary Verification** | Native execution check | ✅ PASSED | `NotepadNext 0.15-09` verified on macOS x86_64 |
| **Zero Disruption Bar** | 25 SubnetCalc Themes & 28 Icon Packs | ✅ PASSED | 100% preserved and fully operational |
| **POSIX Sandbox Security** | Private snapshots permissions | ✅ PASSED | Strictly 0700 dir / 0600 file modes |
| **Local DMG Packaging** | Verified DMG `< 35 MB` | ✅ PASSED | `NotepadNext-v0.15-09-x64.dmg` (35 MB) verified via `hdiutil` |

---

## 📦 Distribution Packages (Full 58-Asset Matrix)

- **macOS Universal & Versioned Disk Images (28 DMGs)**:
  - Canonical: `NotepadNext-v0.15-09-x64.dmg`, `NotepadNext-v0.15-x64.dmg`, `NotepadNext-v0.15-09-arm64.dmg`, `NotepadNext-v0.15-arm64.dmg`
  - Multi-Qt Matrix: Qt 6.5, Qt 6.8, Qt 6.10, Qt 6.11, Qt 6.12 across x64 and arm64
- **Linux Packages (26 Packages)**:
  - Debian (`.deb`): `notepadnext_0.15-09_amd64.deb`, `arm64.deb`, plus Qt 6.5–6.12 matrix
  - RedHat / Fedora (`.rpm`): `notepadnext-0.15-09-1.x86_64.rpm`, `aarch64.rpm`, plus Qt 6.5–6.12 matrix
- **Integration Scripts & Reports**:
  - `NotepadNext-Default-Editor.sh` (macOS default editor script)
  - `NotepadNext-Default-Editor-Linux.sh` (Linux MIME association script)
  - `0.15-09-*-test-report.log` (Verification test report)
  - `SHA256SUMS_0.15-09.txt` (Complete SHA-256 digests)
