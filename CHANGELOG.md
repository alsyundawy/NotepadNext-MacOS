# 📜 Changelog — NotepadNext For MacOS

All notable changes to the **NotepadNext For MacOS** project are documented in this file.
Format is inspired by [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.15-04] - 2026-10-21

### 🎨 App Logo & Brand Icon Modernization / Modernisasi Logo & Ikon Aplikasi

- 🇬🇧 **macOS Squircle Design**: Replaced legacy raster icon with modern high-craft macOS squircle app icon, featuring multi-tone emerald gradient background, prominent N# badge, and detailed angled 3D stylus with drop shadow.
- 🇮🇩 **Desain Squircle macOS**: Memperbarui ikon lama dengan desain squircle native macOS modern, latar belakang gradien hijau emerald bertingkat, emblem N#, dan pensil 3D miring dengan bayangan halus.
- 🇬🇧 **Multi-Resolution Packaging**: Generated high-fidelity transparent PNG icons across all standard desktop resolutions (`16x16`, `24x24`, `32x32`, `48x48`, `64x64`, `128x128`, `256x256`, `512x512`), Apple Retina ICNS (`1024x1024` master canvas via `iconutil`), multi-layer Windows ICO (`16` to `256`), and scalable Linux SVG wrapper.
- 🇮🇩 **Kemasan Multi-Resolusi**: Memproduksi file ikon transparan kualitas tinggi untuk seluruh resolusi desktop standar (`16x16` hingga `512x512`), format Apple Retina ICNS (`1024x1024` kanvas utama melalui `iconutil`), Windows ICO berlapis (`16` hingga `256`), serta pembungkus SVG scalable untuk Linux.

### 🌐 Localization & Language Support / Pelokalan & Dukungan Bahasa

- 🇬🇧 **Streamlined to Indonesian & English**: Purged all deprecated, unmaintained translation files from `i18n/`, exclusively maintaining native English (`en_US`) and 100% complete Bahasa Indonesia (`id_ID`) translations.
- 🇮🇩 **Penyederhanaan Bahasa Indonesia & Inggris**: Menghapus seluruh file terjemahan usang di folder `i18n/`, dan memfokuskan dukungan penuh eksklusif pada Bahasa Indonesia (`id_ID`) dengan cakupan 100% (419 pesan XML terverifikasi) dan Bahasa Inggris (`en_US`).
- 🇬🇧 **Dynamic Language Selector**: Preferences dialog cleanly enumerates `<System Default>`, `Bahasa Indonesia (id_ID)`, and `English (en_US)` without clutter.
- 🇮🇩 **Pemilih Bahasa Dinamis**: Dialog pengaturan secara otomatis menyajikan pilihan `<System Default>`, `Bahasa Indonesia (id_ID)`, dan `English (en_US)` secara ringkas dan rapi.

### 🛡️ 13-Pillar Codebase Hardening & Bug Fixes / Penguatan Kode & Perbaikan Bug

- 🇬🇧 **macOS Dock Restoration SIGSEGV Fix**: Added defensive null pointer check for `SearchResultsDock` in `MainWindow::restoreWindowState()`, eliminating potential segmentation faults when restoring application geometry and window states (`QTBUG-111538` resilience).
- 🇮🇩 **Pencegahan SIGSEGV Restorasi Dock macOS**: Menambahkan pemeriksaan pointer null defensif pada `SearchResultsDock` di `MainWindow::restoreWindowState()`, mencegah potensi crash saat memuat ulang susunan jendela.
- 🇬🇧 **QuickFindWidget Signal Disconnect Bug Fix**: Corrected inverted disconnect logic in `QuickFindWidget::setEditor()`. Previously disconnected incoming parameter rather than `this->editor`, causing stale editor signal retention and memory leak risks.
- 🇮🇩 **Perbaikan Bug Disconnect QuickFindWidget**: Memperbaiki logika pemutusan sinyal pada `QuickFindWidget::setEditor()` yang sebelumnya salah memutuskan parameter baru alih-alih `this->editor` lama.
- 🇬🇧 **Memory Safety & Uninitialized Pointer Fix**: Initialized all raw pointer members in `NotepadNextApplication` (`editorManager`, `recentFilesListManager`, `settings`, `sessionManager`, `translationManager`) explicitly to `nullptr` in class declaration.
- 🇮🇩 **Keamanan Memori Pointer**: Menginisialisasi seluruh pointer pada `NotepadNextApplication` ke `nullptr` pada deklarasi kelas untuk mencegah undefined behavior.
- 🇬🇧 **Buffer Overflow Hardening (CWE-120)**: Replaced unbounded legacy `strcpy` and `strcat` in `src/IFaceTable.cpp` with standard bounded `std::snprintf`.
- 🇮🇩 **Pencegahan Buffer Overflow (CWE-120)**: Mengganti fungsi C lama `strcpy` dan `strcat` di `src/IFaceTable.cpp` dengan `std::snprintf` berbatas aman.
- 🇬🇧 **File Rename Error Handling**: Handled non-zero error return in `MainWindow::on_actionRename_triggered()` with user-facing notification via `showSaveErrorMessage()` instead of discarding the result with `Q_UNUSED`.
- 🇮🇩 **Penanganan Kesalahan Ganti Nama Berkas**: Menambahkan penanganan pesan kesalahan saat penggantian nama berkas gagal pada `MainWindow::on_actionRename_triggered()`.
- 🇬🇧 **Code Hygiene & Shadow Variable Removal**: Fixed constructor overloading ambiguity and missing `explicit` in `TabsQuickActionsBar.h`, and eliminated variable shadowing in `SessionManager::loadUnsavedFileDetails()` and `SearchResultsDock::newSearch()`.
- 🇮🇩 **Pembersihan Kode & Penyamaran Variabel**: Memperbaiki ambiguitas konstruktor pada `TabsQuickActionsBar.h` serta menghilangkan penyamaran variabel (variable shadowing) pada `SessionManager` dan `SearchResultsDock`.

### 💎 Modernized "About NotepadNext" Dialog / Dialog "Tentang NotepadNext" Modern

- 🇬🇧 **Elegant Rich-Text Design**: Completely overhauled the About dialog into a modern, beautifully styled presentation featuring the 512x512 app logo icon, real-time Qt version badge, macOS CPU architecture (`QSysInfo`), dual author attribution (Justin Dailey & Harry DS Alsyundawy), core component breakdown (Scintilla, Lexilla, Qt, QADS), and clickable project links.
- 🇮🇩 **Desain Dialog Elegan & Modern**: Memperbarui antarmuka dialog "Tentang Notepad Next" dengan tampilan modern berformat rich-text elegan, menampilkan ikon aplikasi beresolusi tinggi, lencana versi Qt dinamis, arsitektur CPU macOS (`QSysInfo`), atribusi pembuat & builder, rincian komponen utama, serta tautan resmi proyek.

### ⚡ Build, CI/CD Runners & MegaLinter / Otomasi Build, Runner CI & MegaLinter

- 🇬🇧 **Qt 6.12 Runner Matrix for macOS 15+**: Added dedicated GitHub Actions matrix runners for building and releasing with Qt 6.12 specifically targeting macOS 15+ (Sequoia) across both Apple Silicon (`arm64`) and Intel (`x64`) architectures in `build.yml` and `release.yml`.
- 🇮🇩 **Matrix Runner Qt 6.12 untuk macOS 15+**: Menambahkan runner GitHub Actions untuk kompilasi dan rilis menggunakan Qt 6.12 yang secara spesifik menargetkan macOS 15+ (Sequoia) pada arsitektur Apple Silicon (`arm64`) maupun Intel (`x64`).
- 🇬🇧 **Tailored MegaLinter Configuration (`.mega-linter.yml`)**: Designed a lean, repository-aligned MegaLinter configuration enabling only relevant descriptors (C++, CMake, Shell, Actions, YAML, JSON, Markdown, XML) while explicitly disabling 30+ irrelevant language linters (PHP, Ruby, C#, Java, Dart, Terraform, etc.) and suppressing false-positive spell/copypaste checks on vendored code.
- 🇮🇩 **Konfigurasi MegaLinter Ringkas (`.mega-linter.yml`)**: Membuat konfigurasi MegaLinter yang disesuaikan khusus untuk basis kode C++/Qt/CMake, menonaktifkan 30+ linter bahasa yang tidak relevan, serta mengecualikan berkas thirdparty dan translasi dari pemeriksaan yang tidak perlu.
- 🇬🇧 **Linter Cleanliness**: 100% compliant with `trunk check`, `actionlint`, `yamllint`, `markdownlint`, `prettier`, and `oxipng` image optimization.
- 🇮🇩 **Kepatuhan Linter**: Memenuhi 100% standar pengujian `trunk check`, `actionlint`, `yamllint`, `markdownlint`, `prettier`, dan optimasi gambar `oxipng`.
- 🇬🇧 **Metadata Synchronization**: Maintained version string `0.15-04` across `CMakeLists.txt`, `updates.json`, and Linux AppStream metadata `com.github.dail8859.NotepadNext.metainfo.xml`.
- 🇮🇩 **Sinkronisasi Metadata**: Mempertahankan nomor versi `0.15-04` di `CMakeLists.txt`, `updates.json`, dan metadata AppStream Linux.

---

## [0.15-03] - 2026-10-19

- chore(release): bump version to 0.15-03, sync upstream, and refine Indonesian translations.
- Updated `updates.json` and `CMakeLists.txt`.
- Enhanced string translations across 64 UI elements in `i18n/NotepadNext_id.ts`.

---

## [0.15-02] - 2026-09-25

- fix(ci): sanitize git push target and bot identity in `lupdate.yml`.
- Synchronized packaging workflows for macOS release automation.

---

## [0.14-07] - 2026-08-27

- fix: update builder copyright name, resolve linters, and sync updates metadata.
- Correct builder copyright name to HARRY DS ALSYUNDAWY in `CMakeLists.txt`.
- Fix Markdown formatting and newlines in documentation.
- Refactor `GenerateEnums.py` and `RemoveShortcutTranslations.py` to comply with PEP 8.
- Resolve nodiscard return value warnings in `MainWindow.cpp`, `LuaState.cpp`, and `ScintillaNext.cpp`.
- Fix SC2312 shellcheck warnings in `NotepadNext-Default-Editor.sh`.
