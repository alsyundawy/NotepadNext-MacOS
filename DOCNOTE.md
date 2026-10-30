# 📘 NotepadNext-MacOS Release DocNote — v0.15-07

```text
================================================================================
Release Version     : 0.15-07 (Stability Hardening, Tab Lifecycle Fix, Multi-Qt Matrix & Linter Zero-Error)
Release Date        : 2026-10-31
Lead Maintainer     : Harry Dertin Sutisna Alsyundawy (@alsyundawy)
Organization        : ALSYUNDAWY IT SOLUTION (https://alsyundawy.com)
Repository          : https://github.com/alsyundawy/NotepadNext-MacOS
Core Editing Engine : Scintilla 5.6.7 (Vendored) & Lexilla 5.5.4 (Vendored)
Embedded Lua Core   : Sandboxed Lua 5.5.1
Scripting Runtime   : Python 3.15+ Engine Bridge
GUI Framework       : Qt 6.5 / 6.8 LTS / 6.10 / 6.11 / 6.12 (C++20 Standard)
Target Platforms    : macOS 11.0–26.0 (Apple Silicon & Intel), Linux (Ubuntu, Debian, Fedora, RHEL, openSUSE)
License             : GNU General Public License v3.0 (GPL-3.0-or-later)
Architecture        : Native Scintilla reimplementation of Notepad++ for macOS & Linux
================================================================================
```

---

## 1. Executive Summary: Release v0.15-07

NotepadNext-MacOS **v0.15-07** delivers critical runtime crash fixes, compiler database stabilization, complete multi-Qt Linux and macOS matrix synchronization, and rigorous zero-error linter compliance:

1. **Resolution of Random Crash When Closing Unsaved Tabs**:
   - **Root Cause Investigation**: Identified crash triggers during tab closure:
     1. In `ScintillaNext::getFilePath()`, `getPath()`, and `getFileInfo()`, calls unconditionally executed `Q_ASSERT(isFile())`. When closing an unsaved or untitled document, `SaveModifiedDialog::setupUi()` invoked `getFilePath()`, which failed the assertion and triggered `qt_assert(...)` calling `abort()`.
     2. In `DockedEditor.cpp`, the `QObject::destroyed(QObject *obj)` lambda re-queried `editors()`. Because the underlying `CDockWidget` had not yet detached its widget pointer during early destruction, `editors()` returned the dying editor object (`obj`), re-setting `currentEditor` to the deallocated instance and firing `editorActivated(currentEditor.data())`, causing a critical Use-After-Free / pure-virtual call crash in `updateGui()`.
     3. In `MainWindow::updateEditorPositionBasedUi()`, `dockedEditor->currentDockArea()` was dereferenced unconditionally without checking for `nullptr`. When the last tab or dock area was closed or destroyed, `currentDockArea()` returned null, resulting in an immediate `SIGSEGV`.
     4. In `DockedEditor::switchToEditor()`, missing null-check on `editor` caused null dereferencing of `editor->parentWidget()`.
   - **Remediation**: Replaced assertions in `ScintillaNext::getFilePath()` with safe empty fallbacks, guarded `SaveModifiedDialog::setupUi()` with `isFile()` checks, added null guards in `updateEditorPositionBasedUi()`, filtered out dying `obj` from `edList` in the `destroyed` callback, and safely wrapped `editor` with `QPointer<ScintillaNext>` during modal save prompt unwinding in `closeFile()`.

2. **Fix for 1,288 Clangd / IDE False-Positive Errors & C-Header Parsing**:
   - **Root Cause**: Clang/Clangd defaults `.h` files without corresponding compile units to C language mode. When `.clangd` or SonarLint supplied `-std=c++20`, Clang aborted parsing with `error: invalid argument '-std=c++20' not allowed with 'C'` and reported missing `<type_traits>`, cascading into over 1,200 false syntax errors.
   - **Remediation**:
     1. Created automated Python enrichment tool `scripts/generate_compdb_headers.py` wired directly into `CMakeLists.txt` via `enrich_compile_commands` target, automatically populating `compile_commands.json` with explicit `-x c++-header -std=c++20` entries for all 95 headers in `src/`.
     2. Configured `.vscode/settings.json` with `"sonar.c.file.suffixes": "-"` and `"sonar.cpp.file.suffixes": ".cpp,.cxx,.cc,.h,.hpp,.hxx"`, ensuring SonarLint never routes `.h` C++ Qt headers to the C compiler analyzer.
     3. Updated `.clangd` with explicit two-argument `-x` / `c++-header` rules for `PathMatch: .*\.(h|hpp)$`.
     4. Verified with `clangd --check` on `ActionUtils.h`, `AboutDialog.h`, `DefaultDirectoryManager.cpp`, `MainWindow.cpp`, and `DockedEditor.cpp` achieving 0 errors and 0 syntax warnings.

3. **Resolution of SonarLint Code Smells & Cognitive Complexity Warnings**:
   - **`DefaultDirectoryManager.cpp`**: Applied C++20 `using enum ApplicationSettings::DefaultDirectoryBehaviorEnum;` to reduce verbosity (S6186) and modernized control flow with C++17/C++20 if-init statements `if (const auto behavior = ...; ...)` (S6004).
   - **`MainWindow.cpp` & `MainWindow.h`**: Converted `newEditor` pointers to `const ScintillaNext *` (S5813), `dockArea` to `const ads::CDockAreaWidget *`, and marked `MainWindow::switchToEditor` as a `const` member function in declaration and definition (S5956).
   - **`DockedEditor.cpp`**: Converted `dockAreaCreated` lambda parameter to `const ads::CDockAreaWidget *` (S5813). Extracted helper functions `extractActiveEditor` and `isEditorOpen` to decouple widget hierarchy traversal, eliminating deep nesting (nesting depth <= 2 across all methods, resolving S134) and dropping `getCurrentEditor()` Cognitive Complexity from 29 down to <= 6 (resolving S3776).

4. **Pixel-Crisp Toolbar Tooltips & Native Cocoa Window Transparency**:
   - Enforced 6px rounded corners (`border-radius: 6px;`), compact geometry tight to font size (`lbl->setMargin(0)` and `qproperty-margin: 0; padding: 2px 6px;`).
   - Integrated native Cocoa `NSWindow` transparency (`[window setOpaque:NO]`, `[window setBackgroundColor:[NSColor clearColor]]`, `[window invalidateShadow]`), eliminating opaque black window clipping artifacts on macOS.
   - Suppressed redundant keyboard shortcut duplicates in tooltip labels via `ToolbarTooltipFilter`.

5. **Synchronized Linux Packaging (`cmake/PackagingLinux.cmake`)**:
   - Synchronized Linux packaging with `SparkNotePad/cmake/PackagingLinux.cmake` including multi-Qt version tagging (`PKG_QT_SUFFIX`), dual 64-bit RPM SONAME resolves, automated APT/DNF dependencies, AppStream metadata, and AppImage packaging targets.
   - Created `NotepadNext-Default-Editor-Linux.sh` implementing standard Freedesktop XDG MIME handlers (120+ types), alternatives management, and shell `$EDITOR` integration.

6. **Complete Linux & macOS Release Matrix (Qt 6.5 to 6.12)**:
   - Updated `.github/workflows/release.yml` to include the complete matrix from Qt 6.5, 6.8 LTS, 6.10, 6.11, up to 6.12 across Linux (Ubuntu 22.04 and 24.04), macOS Intel x64, and macOS Apple Silicon arm64 runners.

7. **Empirically Verified Local macOS Release Build & DMG Packaging**:
   - Built locally via Ninja and Qt 6.12.0:

     ```bash
     cmake -S . -B build -G Ninja \
         -DCMAKE_BUILD_TYPE=Release \
         -DAPP_DISTRIBUTION=dmg \
         -DCMAKE_PREFIX_PATH="/Users/alsyundawy/Qt/6.12.0/macos"
     cmake --build build --target NotepadNext -j4
     cmake --build build --target dmg
     ```

   - Generated native disk image: `build/NotepadNext-v0.15-07-x64.dmg` (27 MB, compressed UDBZ).
   - Validated integrity with Apple `hdiutil verify`: Checksum status **VALID** (CRC32 `$7CACB3C2`).

8. **Full Toolchain & Linter Zero-Error Verification**:
   - **Compiler**: Built cleanly with Clang++ and CMake (`exit 0`).
   - **MegaLinter CI**: 100% green across branches `0.15-01` through `0.15-07` and `master`.
   - **Cppcheck**: Zero errors with Qt library support across all source files (`exit 0`).
   - **Clangd**: 100% clean check across all C++ source files (`exit 0`, 0 errors).
   - **ShellCheck & shfmt**: Zero warnings across all repository shell scripts.
   - **Actionlint**: Validated all workflows in `.github/workflows/` (`exit 0`).

---

## 2. Detailed Technical Changelog & File Diffs

### A. `src/DockedEditor.cpp` & `src/DockedEditor.h`

#### Guarded Switch-to-Editor

```cpp
void DockedEditor::switchToEditor(const ScintillaNext *editor) const
{
    if (!editor) {
        return;
    }
    if (auto *dockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget())) {
        dockWidget->setAsCurrentTab();
        dockWidget->raise();
    }
}
```

#### Destroyed Signal UAF Prevention

```cpp
connect(editor, &QObject::destroyed, this, [this](QObject *obj) {
    if (currentEditor == obj) {
        currentEditor = nullptr;
    }
    auto edList = editors();
    edList.removeAll(static_cast<ScintillaNext *>(obj));
    if (!edList.isEmpty()) {
        currentEditor = edList.first();
        emit editorActivated(currentEditor.data());
    } else {
        currentEditor = nullptr;
    }
});
```

### B. `src/dialogs/MainWindow.cpp`

#### Null-Safe Dock Area Handling

```cpp
void MainWindow::updateEditorPositionBasedUi()
{
    const auto *dockArea = dockedEditor->currentDockArea();
    if (!dockArea) {
        ui->actionCloseAllToLeft->setEnabled(false);
        ui->actionCloseAllToRight->setEnabled(false);
        ui->actionCloseAllExceptActive->setEnabled(false);
        return;
    }
    const int index = dockArea->currentIndex();
    const int total = dockArea->dockWidgetsCount();
    ui->actionCloseAllToLeft->setEnabled(index > 0);
    ui->actionCloseAllToRight->setEnabled(index < (total - 1));
    ui->actionCloseAllExceptActive->setEnabled(editorCount() > 1);
}
```

#### QPointer Lifetime Guard in `closeFile`

```cpp
QPointer<ScintillaNext> pEditor = editor;
if (!checkEditorsBeforeClose({editor})) {
    return;
}
if (pEditor) {
    pEditor->close();
}
```

### C. `src/widgets/EditorInfoStatusBar.cpp`

- Guarded status bar label variables (`docSize`, `docPos`, `docType`, `eolFormat`, `unicodeType`, `overType`) when editor is null.

### D. `.clangd` & `compile_commands.json`

- Injected `-x c++-header -std=c++20` for `.h` matching and exported full `compile_commands.json` via `scripts/generate_compdb_headers.py`.

### E. `.github/workflows/release.yml`

- Added Qt 6.12 to Linux build matrix and unified `-DPKG_QT_SUFFIX` across Linux and macOS matrices.

### F. Upstream Harmonization (Commit `4361e31` / PR `#be1e274`)

- Harmonized upstream Default Language preference for new documents (`PreferencesDialog` and `ApplicationSettings`) via linear cherry-pick, ensuring clean Git history without translation churn.

---

## 3. Cumulative Release Evolution Matrix (`0.15-01` to `0.15-07`)

| Release       | Focus / Milestone             | Key Real Deliverables                                                                                                                                               |
| :------------ | :---------------------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **`0.15-01`** | Upstream v0.15 Rebase         | 470+ file format mapping script, macOS System Settings registration fix, initial Indonesian L10n.                                                                   |
| **`0.15-02`** | Runtime & Architecture        | Embedded Lua upgrade, horizontal wheel touchpad smoothing, per-architecture DMG release workflow.                                                                   |
| **`0.15-03`** | UI Ergonomics                 | Find/Mark 1-box input layout, persistent toolbar Preferences & Exit icons, upstream v0.15 sync.                                                                     |
| **`0.15-04`** | Brand & UI Architecture       | Squircle macOS icon, uchardet encoding detector, modern `SaveModifiedDialog`, adaptive `AboutDialog`, multi-instance support.                                       |
| **`0.15-05`** | Theme Engine & Modernization  | Dark/Light mode theme manager with 75 palettes, SonarQube integration, toolbar actions polish.                                                                      |
| **`0.15-06`** | Diagnostics & Platform Polish | Modernized card-based `UpdateDialog`, streamlined id/en i18n, Apple Silicon `arm64` release runner synchronization.                                                 |
| **`0.15-07`** | Crash Fixes & Packaging       | Unsaved tab crash resolution (SIGABRT/UAF), Clangd compdb enrichment, native Cocoa transparent tooltips, Linux DEB/RPM CPack packaging, verified Qt 6.12 DMG build. |
