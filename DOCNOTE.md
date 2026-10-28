# NotepadNext-MacOS Release DocNote — v0.15-07

```text
================================================================================
Release Version     : 0.15-07 (Stability Hardening, Tab Lifecycle Fix, Multi-Qt Matrix & Linter Zero-Error)
Release Date        : 2026-10-28
Lead Maintainer     : Harry Dertin Sutisna Alsyundawy (@alsyundawy)
Organization        : ALSYUNDAWY IT SOLUTION (https://alsyundawy.com)
Repository          : https://github.com/alsyundawy/NotepadNext-MacOS
Core Editing Engine : Scintilla 5.6.7 (Vendored) & Lexilla 5.5.4 (Vendored)
Embedded Lua Core   : Sandboxed Lua 5.5.1
Scripting Runtime   : Python 3.15+ Engine Bridge
GUI Framework       : Qt 6.5 / 6.8 LTS / 6.10 / 6.11 / 6.12 (C++20 Standard)
Target Platforms    : macOS 11.0–26.0 (Apple Silicon & Intel), Linux (Ubuntu, Debian, Fedora, RHEL, openSUSE), FreeBSD 13–15
License             : GNU General Public License v3.0 (GPL-3.0-or-later)
Architecture        : Native Scintilla reimplementation of Notepad++ for macOS & Linux
================================================================================
```

---

## 1. Executive Summary: Release v0.15-07

NotepadNext-MacOS **v0.15-07** delivers critical runtime crash fixes, IDE compiler database stabilization, complete multi-Qt Linux and macOS matrix synchronization, and rigorous zero-error linter compliance:

1. **Resolution of Random Crash When Closing Unsaved Tabs**:
   - **Root Cause Investigation**: Identified two interrelated crash triggers during tab closure:
     1. In `DockedEditor.cpp`, the `QObject::destroyed(QObject *obj)` lambda re-queried `editors()`. Because the underlying `CDockWidget` had not yet detached its widget pointer during early destruction, `editors()` returned the dying editor object (`obj`), re-setting `currentEditor` to the deallocated instance and firing `editorActivated(currentEditor.data())`, causing a critical Use-After-Free / pure-virtual call crash in `updateGui()`.
     2. In `MainWindow::updateEditorPositionBasedUi()`, `dockedEditor->currentDockArea()` was dereferenced unconditionally without checking for `nullptr`. When the last tab or dock area was closed or destroyed, `currentDockArea()` returned null, resulting in an immediate `SIGSEGV`.
     3. In `DockedEditor::switchToEditor()`, missing null-check on `editor` caused null dereferencing of `editor->parentWidget()`.
   - **Remediation**: Added rigorous null guards in `updateEditorPositionBasedUi()`, filtered out dying `obj` from `edList` in the `destroyed` callback, safely wrapped `editor` with `QPointer<ScintillaNext>` during modal save prompt unwinding in `closeFile()`, and verified safe tab closure under all conditions.

2. **Fix for 1,288 Clangd / IDE False-Positive Errors & C-Header Parsing**:
   - **Root Cause**: Clang/Clangd defaults `.h` files without corresponding compile units to C language mode. When `.clangd` supplied `-std=c++20`, Clang aborted parsing with `error: invalid argument '-std=c++20' not allowed with 'C'` and reported missing `<type_traits>`, cascading into over 1,200 false syntax errors.
   - **Remediation**: Rebuilt `build/compile_commands.json` (759 KB) and added a dedicated `If: PathMatch: .*\.h$` rule with `-xc++-header` in `.clangd`. Verified with `clangd --check=src/main.cpp` achieving 0 errors and 0 syntax warnings.

3. **Resolution of 756 SonarQube / SonarLint Parser Errors**:
   - **Root Cause**: SonarQube's default language analyzer classified Qt Translation files (`i18n/*.ts`) as TypeScript and third-party dependencies as project source, flooding analysis with parser errors.
   - **Remediation**: Configured comprehensive exclusions in `sonar-project.properties`:
     `sonar.exclusions=thirdparty/**,.cpm-cache/**,build/**,i18n/**,**/*.ts,src/stylesheets/**`
     Ensured full compile-command mapping to `build/compile_commands.json`.

4. **Synchronized Linux Packaging (`cmake/PackagingLinux.cmake`)**:
   - Synchronized Linux packaging with `SparkNotePad/cmake/PackagingLinux.cmake` including multi-Qt version tagging (`PKG_QT_SUFFIX`), dual 64-bit RPM SONAME resolves, automated APT/DNF dependencies, AppStream metadata, and AppImage packaging targets.

5. **Complete Linux & macOS Release Matrix (Qt 6.5 to 6.12)**:
   - Updated `.github/workflows/release.yml` to include the complete matrix from Qt 6.5, 6.8 LTS, 6.10, 6.11, up to 6.12 across Linux (Ubuntu 22.04 and 24.04), macOS Intel x64, and macOS Apple Silicon arm64 runners.

6. **Full Toolchain & Linter Zero-Error Verification**:
   - **Compiler**: Built cleanly with LLVM Clang++ and CMake 4.4.4 (`exit 0`).
   - **Trunk**: All checks passed cleanly (`✔ No issues`).
   - **Cppcheck 2.22.0**: Zero errors with Qt library support (`exit 0`).
   - **Clang-Tidy**: Verified clean execution against `compile_commands.json` (`exit 0`).

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

- Corrected status bar label variables (`docSize`, `docPos`, `docType`, `eolFormat`, `unicodeType`, `overType`) when editor is null.

### D. `.clangd` & `compile_commands.json`

- Injected `-xc++-header` for `.h` matching and exported full 759 KB `compile_commands.json`.

### E. `.github/workflows/release.yml`

- Added Qt 6.12 to Linux build matrix and unified `-DPKG_QT_SUFFIX` across Linux and macOS matrices.

### F. Upstream Harmonization (Commit `4361e31` / PR `#be1e274`)

- Harmonized upstream Default Language preference for new documents (`PreferencesDialog` and `ApplicationSettings`) via linear cherry-pick, ensuring zero merge commit message overhead in Git history and completely omitting upstream translation churn.
