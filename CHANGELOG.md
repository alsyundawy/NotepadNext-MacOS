# 📜 Changelog — NotepadNext For MacOS

All notable changes to the **NotepadNext For MacOS** project are documented in this file.
Format is inspired by [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.15-07] - 2026-10-26

### 🛡️ Security Hardening & 2025–2026 CVE Defenses / Pertahanan Keamanan & Mitigasi CVE 2025–2026

- 🇬🇧 **Auto-Updater Cryptographic Integrity (CVE-2025-15556 / CWE-494)**: Enforced strict HTTPS URL pinning for `updates.json`, integrated SHA-256 cryptographic package integrity checking via `QCryptographicHash`, and added checksum verification indicators in `UpdateDialog`.
- 🇮🇩 **Integritas Kriptografi Pembaruan Otomatis (CVE-2025-15556 / CWE-494)**: Menegakkan pinning URL HTTPS ketat untuk `updates.json`, mengintegrasikan verifikasi integritas paket kriptografis SHA-256 melalui `QCryptographicHash`, dan menambahkan indikator checksum pada `UpdateDialog`.
- 🇬🇧 **Embedded Lua Sandbox Hardening (CVE-2026-42214 / CWE-94)**: Stripped dangerous shell execution and filesystem manipulation functions (`os.execute`, `os.remove`, `os.rename`, `os.exit`, `io.popen`, `package.loadlib`) from the embedded Lua 5.5.1 runtime, preventing arbitrary command injection.
- 🇮🇩 **Penguatan Kotak Pasir (Sandbox) Lua (CVE-2026-42214 / CWE-94)**: Menghapus fungsi eksekusi shell dan manipulasi sistem berkas yang rentan (`os.execute`, `os.remove`, `os.rename`, `os.exit`, `io.popen`, `package.loadlib`) dari runtime Lua 5.5.1 bawaan untuk mencegah injeksi perintah berbahaya.
- 🇬🇧 **Subprocess Canonicalization (CVE-2026-25926 / CWE-78)**: Replaced PATH-dependent subprocess calls with canonical absolute binaries (`/usr/bin/open`, `/usr/bin/qlmanage`, `/usr/bin/killall`, `/usr/bin/pgrep`) across all macOS scripts and controllers.
- 🇮🇩 **Kanonikalisasi Subproses (CVE-2026-25926 / CWE-78)**: Mengganti pemanggilan subproses yang bergantung pada PATH dengan jalur absolut kanonik (`/usr/bin/open`, `/usr/bin/qlmanage`, `/usr/bin/killall`, `/usr/bin/pgrep`) di seluruh skrip dan pengendali macOS.
- 🇬🇧 **Memory Bounds & Lexer Protection (CVE-2025-49144 / CVE-2025-56383)**: Implemented safe bounded C-string inspection with `strnlen` in preprocessor hint lookups and bounded lexer name queries (`lexer.size() <= 64`).
- 🇮🇩 **Proteksi Batas Memori & Lekser (CVE-2025-49144 / CVE-2025-56383)**: Menerapkan inspeksi string-C yang aman dengan batas `strnlen` pada pencarian petunjuk praprosesor serta validasi panjang nama lekser (`lexer.size() <= 64`).

### ⚡ Sibling Text Editor Innovations & Productivity / Inovasi & Produktivitas Editor Sibling

- 🇬🇧 **Scintilla 5.6.7 Pixel-Aligned Retina Scaling**: Activated `SCI_SETSCALETECHNIQUE` with `SCALE_TECHNIQUE_PIXEL_ALIGNED` on macOS Retina displays to eliminate subpixel micro-jitter and glyph blurriness.
- 🇮🇩 **Penskalaan Retina Sejajar Piksel Scintilla 5.6.7**: Mengaktifkan `SCI_SETSCALETECHNIQUE` dengan mode `SCALE_TECHNIQUE_PIXEL_ALIGNED` pada layar Retina macOS untuk mengeliminasi jitter mikro subpixel dan keburaman glif font.
- 🇬🇧 **Natural Locale-Aware Numeric Line Sorting**: Integrated `QCollator` (numeric mode & case-insensitive) for natural line sorting (e.g. `Item 2` before `Item 10`) with Ascending and Descending actions.
- 🇮🇩 **Pengurutan Baris Numerik Alami Peka-Lokal**: Mengintegrasikan `QCollator` (mode numerik dan tidak peka huruf besar/kecil) untuk pengurutan baris alami (misal `Item 2` mendahului `Item 10`) dengan menu Menaik dan Menurun.
- 🇬🇧 **Inline Math Expression Evaluator (`Cmd+Shift+M`)**: Added instant mathematical expression evaluation in the active buffer using the embedded Lua 5.5.1 engine.
- 🇮🇩 **Evaluator Ekspresi Matematika Langsung (`Cmd+Shift+M`)**: Menambahkan evaluasi ekspresi matematika instan langsung pada buffer aktif menggunakan mesin Lua 5.5.1 bawaan.
- 🇬🇧 **Multi-Line Paste Auto-Escaping in Find & Replace**: Intercepted clipboard paste events in `FindReplaceDialog` combo boxes to automatically sanitize and escape newlines into `\n` or single spaces according to search mode.
- 🇮🇩 **Auto-Escape Baris Baru pada Tempel Cari & Ganti**: Menangkap event tempel papan klip pada kotak kombo `FindReplaceDialog` untuk membersihkan dan mengonversi baris baru menjadi `\n` atau spasi sesuai mode pencarian.
- 🇬🇧 **Side-by-Side Two-Buffer File Comparison**: Built instant two-buffer diff comparison with automated split view docking (`splitToRight`) and visual diff highlight indicators.
- 🇮🇩 **Perbandingan Dua Buffer Bersebelahan**: Membangun perbandingan diff dua buffer secara instan dengan pembagian tampilan dok otomatis (`splitToRight`) dan indikator penyorotan perbedaan visual.
- 🇬🇧 **Read-Only Hexadecimal / Binary Viewer**: Added toggleable binary/hex view mode rendering 16-byte offsets, hex byte formatting, and an ASCII side-column.
- 🇮🇩 **Tampilan Hex / Biner Hanya-Baca**: Menambahkan mode tampilan heksadesimal/biner yang dapat dialihkan, menampilkan offset 16-bita, format byte hex, dan kolom samping ASCII.

### 🍏 macOS Native Architecture & UI/UX Polish / Arsitektur Native macOS & Pemolesan UI/UX

- 🇬🇧 **macOS Menu Bar Extra / Dock Status Mode**: Integrated `QSystemTrayIcon` enabling background residency with quick access menu to show/hide application and quit.
- 🇮🇩 **Menu Bar Extra / Ikon Status Dock macOS**: Mengintegrasikan `QSystemTrayIcon` yang memungkinkan mode residen di latar belakang dengan menu akses cepat untuk menampilkan/menyembunyikan aplikasi dan keluar.
- 🇬🇧 **Tab Label Middle-Ellipsis (`elideMode`)**: Implemented configurable middle-ellipsis truncation on editor tab labels, preserving tab bar layout geometry even with 30+ open documents.
- 🇮🇩 **Elipsis Tengah Judul Tab (`elideMode`)**: Menerapkan pemotongan judul tab panjang dengan elipsis tengah yang dapat dikonfigurasi, menjaga stabilitas geometri bilah tab meski membuka 30+ dokumen.
- 🇬🇧 **Horizon Dark Contrast Optimization**: Fine-tuned dark/light horizon themes for maximum contrast and readability on Apple Retina displays.
- 🇮🇩 **Optimalisasi Kontras Horizon Dark**: Menyetel palet tema gelap/terang horizon untuk kontras dan keterbacaan optimal pada layar Apple Retina.
- 🇬🇧 **Find & Replace Form Input Precision**: Enforced strict 1-box form input (`comboFind`) for Tab Find and Tab Mark, completely hiding the Replace row and shrinking dialog height via `adjustSize()`, reserving the 2-box input strictly for Tab Replace.
- 🇮🇩 **Presisi Formulir Input Cari & Ganti**: Menegakkan format 1 kotak input (`comboFind`) pada Tab Find dan Tab Mark, menyembunyikan baris Replace sepenuhnya dan menyusutkan tinggi dialog via `adjustSize()`, serta membatasi 2 kotak input hanya untuk Tab Replace.
- 🇬🇧 **Persistent Toolbar Preferences & Exit Actions**: Enforced persistent Settings/Preferences (cog icon) and Exit (exit icon) on `mainToolBar`, ensuring icons are never lost when restoring saved toolbars from `QSettings`.
- 🇮🇩 **Retensi Permanen Ikon Toolbar Preferensi & Keluar**: Memastikan persistensi aksi Preferensi (ikon roda gigi) dan Keluar (ikon keluar) pada `mainToolBar`, mencegah hilangnya ikon saat memuat ulang konfigurasi dari `QSettings`.
- 🇬🇧 **macOS In-Process Multi-Window & Dock Integration**: Unified multi-window architecture within a single process via `NotepadNextApplication`, ensuring secondary window invocations and Cmd+N remain responsive to macOS Dock activations and Cmd+` window cycling.
- 🇮🇩 **Arsitektur Multi-Jendela In-Process & Integrasi Dock macOS**: Menyatukan arsitektur multi-jendela dalam satu proses tunggal via `NotepadNextApplication`, memastikan pembuatan jendela baru dan Cmd+N tetap responsif terhadap klik ikon Dock macOS dan pergantian jendela Cmd+`.
- 🇬🇧 **macOS Dock Right-Click Quit Save Prompt (`QEvent::Close`)**: Intercepted application-level close events on macOS Dock context menu quit and Cmd+Q, iterating through all tracked windows and invoking `checkEditorsBeforeClose()` to guarantee dirty and untitled tabs prompt for saving before exit.
- 🇮🇩 **Dialog Konfirmasi Simpan Berkas saat Keluar dari Menu Dock macOS (`QEvent::Close`)**: Menangkap event penutupan tingkat aplikasi saat keluar melalui klik kanan menu Dock atau Cmd+Q, menelusuri seluruh jendela dan memanggil `checkEditorsBeforeClose()` guna memastikan tab yang dimodifikasi atau baru selalu meminta konfirmasi simpan sebelum aplikasi ditutup.
- 🇬🇧 **Multi-View Split Closure & Unicode Emoji Rendering Stability**: Fortified tab closure routines in horizontal/vertical split views and eliminated text layout crashes on high-volume documents (>10,000 lines) containing complex compound emojis via bounded PlatQt measurement.
- 🇮🇩 **Stabilitas Penutupan Tab Tampilan Pisah & Rendering Emoji Unicode**: Memperkuat rutinitas penutupan tab pada mode belah horizontal/vertikal dan mencegah crash tata letak teks pada dokumen bervolume besar (>10.000 baris) dengan emoji majemuk melalui pengukuran berbatas PlatQt.

### 🔍 13-Pillar Code Quality, SonarLint & Static Analysis Audit / Audit Kualitas Kode 13 Pilar, SonarLint & Analisis Statis

- 🇬🇧 **100% Zero-Defect SonarLint & IDE Diagnostics Resolution**: Resolved all 36 flagged IDE and SonarLint warnings across `AboutDialog`, `HighlightedScrollBar`, `RecentFilesListManager`, `RecentFilesListMenuBuilder`, `RtfConverter`, and `EditorInfoStatusBar` (defaulted destructors `= default`, C++17 `if` init-statements, explicit lambda captures `[this]`, C++20 `using enum Scintilla::Update`, default cases in switch statements, pointer-to-const parameter qualifiers, extraction of nested ternary operators, and explicit `static_cast<int>` on `sptr_t` and `qsizetype`).
- 🇮🇩 **Penyelesaian Penuh Diagnostik SonarLint & Linter IDE Bebas Cacat 100%**: Menyelesaikan seluruh 36 peringatan linter IDE dan SonarLint pada `AboutDialog`, `HighlightedScrollBar`, `RecentFilesListManager`, `RecentFilesListMenuBuilder`, `RtfConverter`, dan `EditorInfoStatusBar` (destruktor defaulted `= default`, C++17 `if` init-statements, penangkapan lambda eksplisit `[this]`, C++20 `using enum Scintilla::Update`, case default pada switch, kualifikasi pointer-to-const pada parameter, pemisahan operator ternary bersarang, serta `static_cast<int>` eksplisit untuk `sptr_t` dan `qsizetype`).
- 🇬🇧 **Upstream PR Harmonization**: Cleanly ported and harmonized upstream PR #8a0a615 (Trim Trailing Whitespace buffer operation with full Indonesian localization), PR #e08fc92 (defensive message handler deregistration in `DebugManager`), and PR #90d6174 (centralized tooltip generation in `ScintillaNext::getToolTip`).
- 🇮🇩 **Harmonisasi PR Upstream**: Mengintegrasikan dan mengharmonisasikan PR upstream #8a0a615 (Aksi Pangkas Spasi Akhir dengan pelokalan Indonesia penuh), PR #e08fc92 (pelepasan handler pesan secara defensif pada `DebugManager`), serta PR #90d6174 (pembuatan tooltip terpusat via `ScintillaNext::getToolTip`).
- 🇬🇧 **100% Zero-Defect Codebase Hardening**: Resolved all SonarLint and static analysis warnings across `URLFinder`, `FindReplaceDialog`, `MacroEditorDialog`, `MainWindow`, `UpdateDialog`, `EditorInspectorDock`, `EditorManager`, `ThemeManager`, `LuaExtension`, `LuaState`, `NotepadNextApplication`, and `ScintillaNext`.
- 🇮🇩 **Penguatan Basis Kode Bebas Cacat 100%**: Menyelesaikan seluruh peringatan SonarLint dan analisis statis di seluruh file inti (`URLFinder`, `FindReplaceDialog`, `MacroEditorDialog`, `MainWindow`, `UpdateDialog`, `EditorInspectorDock`, `EditorManager`, `ThemeManager`, `LuaExtension`, `LuaState`, `NotepadNextApplication`, dan `ScintillaNext`).
- 🇬🇧 **Cognitive Complexity & Nesting Reductions**: Refactored monolithic functions in `MainWindow.cpp`, `LuaExtension.cpp`, `EditorManager.cpp`, and `ScintillaNext.cpp` into modular, single-responsibility helpers, dropping cognitive complexity well below 25 and maximum control flow nesting <= 3.
- 🇮🇩 **Reduksi Kompleksitas Kognitif & Penumpukan Kontrol (Nesting)**: Memfaktorkan ulang fungsi monolitik pada `MainWindow.cpp`, `LuaExtension.cpp`, `EditorManager.cpp`, dan `ScintillaNext.cpp` menjadi fungsi pembantu modular dengan tanggung jawab tunggal, menurunkan kompleksitas kognitif jauh di bawah ambang 25 serta nesting kontrol <= 3.
- 🇬🇧 **Modern RAII & Memory Modernization**: Converted raw UI and component allocations to `std::unique_ptr` with `std::make_unique` (`Ui::MainWindow`, `Ui::FindReplaceDialog`, `Ui::MacroEditorDialog`, `Ui::EditorInspectorDock`, and `LuaState`), eliminating manual delete statements.
- 🇮🇩 **Modernisasi Memori & RAII Modern**: Mengonversi alokasi pointer mentah pada komponen UI ke `std::unique_ptr` dengan `std::make_unique` (`Ui::MainWindow`, `Ui::FindReplaceDialog`, `Ui::MacroEditorDialog`, `Ui::EditorInspectorDock`, dan `LuaState`), menghapus operasi delete manual.
- 🇬🇧 **Type Safety & Scoped Enums**: Upgraded legacy enums (`FileStateChange`, `BufferType`, `TabIndex`) to C++20 `enum class` with `using enum` syntax and eliminated integer precision loss via explicit `static_cast` for Scintilla `sptr_t`, `Sci_Position`, and Qt `qsizetype`.
- 🇮🇩 **Keamanan Tipe & Enum Terlingkup (enum class)**: Memperbarui enum lama (`FileStateChange`, `BufferType`, `TabIndex`) ke `enum class` C++20 dengan sintaks `using enum` serta mengeliminasi potensi kehilangan presisi integer melalui `static_cast` eksplisit untuk `sptr_t`, `Sci_Position`, dan `qsizetype`.
- 🇬🇧 **Const-Correctness & Lambda Captures**: Enforced const qualifiers across methods and pointers, converted default lambda captures `[&]` to explicit scoped captures, and mapped offline `.clangd` SchemaStore definitions to eliminate offline schema warnings.
- 🇮🇩 **Koreksi Const & Penangkapan Lambda Eksplisit**: Menegakkan kualifikasi const pada method dan pointer, mengubah penangkapan lambda implisit `[&]` menjadi penangkapan terlingkup eksplisit, serta memetakan skema `.clangd` offline untuk menghilangkan peringatan skema.
- 🇬🇧 **Flylint & Sonar Diagnostics Resolution**: Resolved IDE linter issues by configuring `.vscode/settings.json` (disabling standalone `c-cpp-flylint.clang` fallback on Qt headers, excluding `.cpm-cache/**`, mapping offline `.clangd` schema), eliminating the most vexing parse in `UpdateDialog.cpp`, adopting C++17 `if` init-statements and auto-types with `static_cast` in `FindReplaceDialog.cpp`, adding missing `<functional>` header in `EditorInspectorDock.h`, and resolving header guard formatting.
- 🇮🇩 **Penyelesaian Masalah Flylint & Diagnostik Sonar**: Mengeliminasi seluruh pesan kesalahan linter IDE melalui perbaikan konfigurasi `.vscode/settings.json` (menonaktifkan parser standalone `c-cpp-flylint.clang` pada header Qt, mengecualikan `.cpm-cache/**`, memetakan skema `.clangd` offline), memperbaiki ambiguitas most vexing parse pada `UpdateDialog.cpp`, menerapkan C++17 `if` init-statement dan deduplikasi tipe auto pada `FindReplaceDialog.cpp`, menyertakan pustaka `<functional>` pada `EditorInspectorDock.h`, serta merapikan header guard.

### 🌐 Complete i18n Modernization & Standardization / Modernisasi & Standardisasi Pelokalan i18n Penuh

- 🇬🇧 **100% Translation Coverage (0 Unfinished Entries)**: Resolved all unfinished translation strings in both Indonesian (`NotepadNext_id.ts` & `SparkNotePad_id.ts`) and English (`NotepadNext_en.ts` & `SparkNotePad_en.ts`) catalogs, generating 646 finished entries in NotepadNext and 1,391 finished entries in SparkNotePad.
- 🇮🇩 **Cakupan Terjemahan 100% (0 Entri Belum Selesai)**: Menyelesaikan seluruh string translasi yang belum selesai pada katalog bahasa Indonesia (`NotepadNext_id.ts` & `SparkNotePad_id.ts`) dan bahasa Inggris (`NotepadNext_en.ts` & `SparkNotePad_en.ts`), menghasilkan 646 entri selesai di NotepadNext dan 1.391 entri selesai di SparkNotePad.
- 🇬🇧 **Indonesian Standardization per EYD V, PUEBI & Mozilla L10n**: Rebuilt Indonesian translations complying with official guidelines from Badan Pengembangan dan Pembinaan Bahasa Kemendikdasmen (ejaan.kemendikdasmen.go.id, PASTI glossary) and the Mozilla Indonesian Style Guide. Standardized terminology (_Berkas_, _Markah_, _Bilah Alat_, _Bilah Status_, _Pintasan_, _Makro_, _Pemeriksa_, _Pengodean_, _Hanya Baca_, _Opak_).
- 🇮🇩 **Standardisasi Bahasa Indonesia sesuai EYD V, PUEBI & Mozilla L10n**: Membangun ulang terjemahan bahasa Indonesia sesuai pedoman resmi Badan Bahasa Kemendikdasmen (ejaan.kemendikdasmen.go.id, glosarium PASTI) dan Panduan Gaya Pelokalan Mozilla. Menstandarkan istilah resmi (_Berkas_, _Markah_, _Bilah Alat_, _Bilah Status_, _Pintasan_, _Makro_, _Pemeriksa_, _Pengodean_, _Hanya Baca_, _Opak_).
- 🇬🇧 **Removal of Dual-Language Parenthetical Artifacts**: Cleaned up legacy translation artifacts containing embedded English terms in parentheses (e.g. `Po&tong (Cut)` -> `Po&tong`, `&Salin (Copy)` -> `&Salin`, `Ubah Nama (Rename)...` -> `Ganti Nama...`, `Read error (Gagal membaca file)` -> `Kesalahan membaca berkas`).
- 🇮🇩 **Pembersihan Anotasi Ganda Inggris dalam Kurung**: Membersihkan artefak terjemahan lama yang mengandung istilah bahasa Inggris dalam kurung (misal `Po&tong (Cut)` -> `Po&tong`, `&Salin (Copy)` -> `&Salin`, `Ubah Nama (Rename)...` -> `Ganti Nama...`, `Read error (Gagal membaca file)` -> `Kesalahan membaca berkas`).
- 🇬🇧 **Zero-Linter Defect Architecture**: Fixed `.clangd` configuration, excluded Qt translation `.ts` XML files from JavaScript/TypeScript language server parsers via `tsconfig.json` and `biome.json`, and eliminated all IDE warnings (`trunk check` passes 100% with 0 issues).
- 🇮🇩 **Arsitektur Bebas Cacat Linter 100%**: Memperbaiki konfigurasi `.clangd`, mengecualikan berkas XML `.ts` translasi Qt dari parser bahasa JavaScript/TypeScript melalui `tsconfig.json` dan `biome.json`, serta mengeliminasi seluruh peringatan IDE (`trunk check` lolos 100% dengan 0 masalah).

### 🧪 LLVM Clang-Tidy, Cppcheck & Zero-Defect Static Analysis Audit / Audit Analisis Statis LLVM Clang-Tidy, Cppcheck & Bebas-Cacat

- 🇬🇧 **100% Clean Clang-Tidy, Cppcheck & Clangd Audit**: Passed comprehensive static analysis across all 88 translation units and header files with 0 errors and 0 warnings.
- 🇮🇩 **Audit Clang-Tidy, Cppcheck & Clangd Bebas Cacat 100%**: Berhasil lolos verifikasi analisis statis komprehensif di seluruh 88 unit translasi dan file header tanpa peringatan maupun pesan kesalahan.
- 🇬🇧 **Null-Pointer Dereference Bugfix in QuickFindWidget**: Added critical null guard `if (!editor) return;` in `QuickFindWidget::initializeEditorIndicator`, eliminating null pointer dereference during quick search initialization.
- 🇮🇩 **Perbaikan Bug Null-Pointer Dereference pada QuickFindWidget**: Menambahkan proteksi defensif `if (!editor) return;` pada `QuickFindWidget::initializeEditorIndicator`, mengeliminasi potensi crash null pointer saat inisialisasi pencarian cepat.
- 🇬🇧 **Modern Algorithm std::find_if Loop Modernization**: Replaced raw `for` loops in `NotepadNextApplication::activeMainWindow()` with modern C++ standard algorithm `std::find_if`, enhancing readability and branch efficiency.
- 🇮🇩 **Modernisasi Loop dengan Algoritma Modern std::find_if**: Mengganti loop `for` konvensional pada `NotepadNextApplication::activeMainWindow()` menggunakan algoritma standar C++ `std::find_if`, meningkatkan keterbacaan kode dan efisiensi eksekusi cabang.
- 🇬🇧 **Scintilla Template Compound Statement Bracing**: Fixed missing braces on single-line branch in `ScintillaNext.h:223` (`forEachMatchInRange`), eliminating compiler template warning propagation across translation units.
- 🇮🇩 **Pemberian Kurung Kurawal Pernyataan Majemuk Templat Scintilla**: Memperbaiki kurung kurawal yang terlewat pada percabangan satu baris di `ScintillaNext.h:223` (`forEachMatchInRange`), mengeliminasi propagasi peringatan templat ke seluruh unit translasi.

### 📦 macOS DMG Packaging Pipeline & Artifact Size Optimization (< 30MB) / Pengemasan DMG & Optimasi Ukuran macOS (< 30MB)

- 🇬🇧 **Elimination of Qt Dynamic Module Bloat**: Enhanced `cmake/PackagingMac.cmake` to strip non-dependent frameworks (`QtPdf`, `QtQml`, `QtQuick`, `QtVirtualKeyboard`, `QtOpenGL`, `platforminputcontexts`, unneeded `imageformats`), reducing bundle size from 112 MB to ~65 MB.
- 🇮🇩 **Eliminasi Bloat Modul Dinamis Qt**: Memperbarui `cmake/PackagingMac.cmake` untuk menghapus pustaka kerangka kerja dinamis yang tidak digunakan (`QtPdf`, `QtQml`, `QtQuick`, `QtVirtualKeyboard`, `QtOpenGL`, `platforminputcontexts`, `imageformats` yang tidak terpakai), memangkas ukuran bundle dari 112 MB menjadi ~65 MB.
- 🇬🇧 **Native bzip2 DMG Compression (`hdiutil -format UDBZ`)**: Built ultra-compact release DMGs of ~28 MB (28,144,826 bytes), well beneath the strict 30 MB ceiling, while maintaining full Retina assets and native macOS integration.
- 🇮🇩 **Kompresi DMG bzip2 Native (`hdiutil -format UDBZ`)**: Menghasilkan berkas distribusi rilis DMG ultra-ringkas berukuran ~28 MB (28.144.826 bita), jauh di bawah batas maksimal 30 MB, dengan tetap mempertahankan aset visual Retina dan integrasi native macOS penuh.
- 🇬🇧 **CMake Packaging Target Guard**: Added `if(NOT TARGET NotepadNext) return() endif()` to prevent standalone analyzer evaluation errors.
- 🇮🇩 **Proteksi Target CMake Packaging**: Menambahkan pengaman `if(NOT TARGET NotepadNext) return() endif()` guna mencegah kesalahan evaluasi penganalisis skrip saat diperiksa secara mandiri.

---

## [0.15-06] - 2026-10-23

### 🌈 Unicode 18.0 Emoji & macOS Typography Architecture / Arsitektur Emoji Unicode 18.0 & Tipografi macOS

- 🇬🇧 **Universal UTF-8 Encoding Enforcement**: Enforced `SC_CP_UTF8` across all Scintilla buffer initializations and disk read routines, guaranteeing lossless encoding for modern multi-byte characters and compound emoji sequences.
- 🇮🇩 **Penegakan Enkodis UTF-8 Universal**: Menegakkan `SC_CP_UTF8` pada seluruh inisialisasi buffer Scintilla dan pembacaan disk, memastikan tidak ada distorsi karakter multi-byte atau urutan emoji majemuk.
- 🇬🇧 **Native Apple Color Emoji Fallback Cascade**: Integrated `"Menlo, Apple Color Emoji"` font fallback cascade and enabled subpixel LCD quality (`SC_EFF_QUALITY_LCD_OPTIMIZED`) for crystal-clear glyph rendering on macOS Retina displays.
- 🇮🇩 **Kaskade Font Native Apple Color Emoji**: Mengintegrasikan kaskade font `"Menlo, Apple Color Emoji"` dan mengaktifkan kualitas LCD subpixel (`SC_EFF_QUALITY_LCD_OPTIMIZED`) untuk ketajaman tampilan pada layar Retina macOS.
- 🇬🇧 **Atomic Grapheme Cluster Traversal (`QTextBoundaryFinder`)**: Intercepted cursor navigation and deletion (Backspace, Delete, Arrow keys) using `QTextBoundaryFinder::Grapheme`. Compound emojis with Zero-Width-Joiners (`\u200D`), skin-tone modifiers, and flags are deleted and navigated atomically without leaving orphan surrogate pairs or broken question-mark glyphs.
- 🇮🇩 **Penelusuran Kluster Grafem Atomik (`QTextBoundaryFinder`)**: Menghubungkan navigasi dan penghapusan kursor menggunakan `QTextBoundaryFinder::Grapheme`. Emoji majemuk dengan Zero-Width-Joiner (`\u200D`), pemodifikasi warna kulit, dan bendera dihapus dan dilewati secara atomik tanpa menyisakan pasangan pengganti atau glif tanda tanya rusak.
- 🇬🇧 **macOS Native Character Palette (`Cmd+Ctrl+Space`)**: Added `actionEmojiSymbols` under Edit menu with shortcut `Cmd+Ctrl+Space`, dispatching native Cocoa `[NSApp orderFrontCharacterPalette:nil]` via Objective-C runtime messaging.
- 🇮🇩 **Palet Karakter Native macOS (`Cmd+Ctrl+Space`)**: Menambahkan menu `Emoji & Karakter Khusus` di bawah menu Edit dengan tombol pintas `Cmd+Ctrl+Space`, memanggil `orderFrontCharacterPalette` Cocoa secara langsung.

### 🔔 Modern, Elegant & Informative Update Dialog / Desain Dialog Pembaruan Modern, Elegan & Informatif

- 🇬🇧 **Native macOS Card-Based Update Dialog (`UpdateDialog.h/.cpp`)**: Replaced the legacy plain QMessageBox with a modern, elegant, and informative macOS card-based dialog featuring real-time architecture detection (Apple Silicon `arm64` vs Intel `x86_64`), dynamic status badges (`✓ Up to Date`, `🚀 Update Available`, `ℹ Check Offline`), clean metadata cards, release highlights, and direct download actions.
- 🇮🇩 **Dialog Pembaruan Native macOS Modern & Elegan (`UpdateDialog.h/.cpp`)**: Menggantikan QMessageBox standar dengan dialog native macOS modern dan elegan berbasis kartu yang dilengkapi deteksi arsitektur real-time (Apple Silicon `arm64` vs Intel `x86_64`), lencana status dinamis (`✓ Sudah Terbaru`, `🚀 Pembaruan Tersedia`), kartu metadata informatif, dan tombol unduh langsung.

### 🌐 Streamlined to Indonesian & English / Penyederhanaan Bahasa Indonesia & Inggris

- 🇬🇧 **Streamlined Exclusively to Indonesian & English (Hapus Selain Bahasa Indonesia & Inggris)**: Purged all unmaintained/redundant translation catalogs from `i18n/`, exclusively maintaining 100% complete native English (`en_US`) and high-polish Bahasa Indonesia (`id_ID`) harmonized with Apple Human Interface Guidelines terminology (_Preferensi..._, _Bungkus Teks_, _Cari dan Ganti_, _Format Berkas_).
- 🇮🇩 **Penyederhanaan Bahasa Indonesia & Inggris**: Menghapus seluruh berkas terjemahan selain Bahasa Indonesia dan Bahasa Inggris dari direktori `i18n/`, dan memfokuskan dukungan penuh eksklusif pada Bahasa Indonesia (`id_ID`) dengan standar Apple Human Interface Guidelines dan Bahasa Inggris (`en_US`).

### 🍏 Apple Silicon (arm64) & Universal Build Pipeline / Saluran Build Apple Silicon & Universal

- 🇬🇧 **Resolved CI Build Compilation Error in `LuaExtension.cpp`**: Fixed `StringResultType::string` scoping error on line 366 that previously caused compiler failures on macOS runners.
- 🇮🇩 **Perbaikan Kompilasi CI pada `LuaExtension.cpp`**: Memperbaiki cakupan `StringResultType::string` pada baris 366 yang sebelumnya menyebabkan kegagalan kompilasi di runner macOS.
- 🇬🇧 **Harmonized GitHub Actions Runners & Packaging**: Standardized Apple Silicon runners on `macos-15`, updated `PackagingMac.cmake` to output both `NotepadNext-v0.15-06-arm64.dmg` and `NotepadNext-v0.15-arm64.dmg`, and configured `updates.json` with dedicated `macos-arm64` and `macos-x64` keys.
- 🇮🇩 **Penyelarasan Runner GitHub Actions & Paket Rilis**: Menstandarkan runner Apple Silicon ke `macos-15`, memperbarui `PackagingMac.cmake` untuk menghasilkan `NotepadNext-v0.15-06-arm64.dmg` dan `NotepadNext-v0.15-arm64.dmg`, serta menyinkronkan `updates.json` untuk arsitektur `macos-arm64` dan `macos-x64`.

### 🧩 Selective Porting of High-Value Upstream Pull Requests / Porting Selektif Pull Request Upstream

- 🇬🇧 **Korean & CJK IME Composition Fix (PR #1166)**: Fixed `QInputMethodEvent` handling in `ScintillaEditBase` on macOS. Prevents character duplication when committing Hangul/Kanji syllables, drops rogue backspace commitments, and preserves pending compositions when ASCII symbols are typed.
- 🇮🇩 **Perbaikan Komposisi IME Korea & CJK (PR #1166)**: Memperbaiki penanganan `QInputMethodEvent` pada `ScintillaEditBase` di macOS. Mencegah duplikasi karakter saat mengetik suku kata Hangul/Kanji, mengabaikan commit backspace palsu, dan menjaga komposisi yang sedang aktif saat mengetik simbol ASCII.
- 🇬🇧 **Crash Prevention on First Keypress & Dock Layout Desync (PR #1153)**: Stored `currentDockObjectNames` alongside saved window state. Bypasses `QMainWindow::restoreState()` if saved dock layout references obsolete or renamed dock widgets (QTBUG-111538), preventing null-pointer crashes on first keypress.
- 🇮🇩 **Pencegahan Crash Tombol Pertama & Desinkronisasi Dock (PR #1153)**: Menyimpan nama objek dock bersama windowState. Melewati `QMainWindow::restoreState()` jika dock yang tersimpan tidak cocok dengan dock aktif saat ini, mencegah crash dereferensi null separator pada penekanan tombol pertama.
- 🇬🇧 **Regex Replace All Hang & Backreference Expansion Fix (PR #1165 & #1154)**: Resolved infinite loop in `Finder::replaceAll()` on zero-length regex matches by advancing position via `editor->positionAfter()`. Rewrote `QRegexSearch::SubstituteByPosition()` to expand capture groups (`\0`..`\9`) and escape sequences (`\n`, `\r`, `\t`, `\\`) directly from captured matches.
- 🇮🇩 **Perbaikan Perulangan Buntu Regex & Ekspansi Referensi Balik (PR #1165 & #1154)**: Mengatasi infinite loop pada pencarian regex berpanjang-nol di `Finder::replaceAll()` dengan melajukan posisi kursor. Menulis ulang `QRegexSearch::SubstituteByPosition()` agar mendukung backreference (`\0`..`\9`) dan escape sequences (`\n`, `\r`, `\t`, `\\`).
- 🇬🇧 **Real-Time External File Modification Watcher (PR #1122)**: Replaced polling with a centralized `QFileSystemWatcher` (leveraging macOS `FSEvents`) in `EditorManager`. Immediately prompts users when the active foreground file is modified on disk, with safety warnings if local unsaved edits exist.
- 🇮🇩 **Pemantau Perubahan Berkas Eksternal Real-Time (PR #1122)**: Menggantikan polling dengan `QFileSystemWatcher` terpusat (memanfaatkan macOS `FSEvents`) pada `EditorManager`. Menampilkan konfirmasi muat ulang secara instan jika berkas aktif diubah di luar aplikasi.
- 🇬🇧 **XML Tools: Pretty Print XML (PR #1096)**: Added dependency-free XML formatting under `Edit -> XML Tools -> Pretty Print XML` using native `QXmlStreamReader` and `QXmlStreamWriter` with single-step undo grouping.
- 🇮🇩 **Alat XML: Format XML Cantik (PR #1096)**: Menambahkan format XML bebas ketergantungan eksternal di bawah `Edit -> XML Tools -> Pretty Print XML` menggunakan `QXmlStreamReader` dan `QXmlStreamWriter`.
- 🇬🇧 **Find & Replace In Selection (PR #1045)**: Added `In selection` checkbox in `FindReplaceDialog`. Restricts search, count, find all, replace, replace all, and mark all strictly to the captured selection bounds.
- 🇮🇩 **Cari & Ganti dalam Seleksi (PR #1045)**: Menambahkan opsi `In selection` pada dialog Cari/Ganti untuk membatasi ruang lingkup pencarian dan penggantian hanya pada teks yang diseleksi.

### 🍏 Critical macOS Usability & Stability Enhancements / Peningkatan Stabilitas & Kegunaan macOS

- 🇬🇧 **Option / Alt Key Shortcut Conflict Resolution (Issues #814 & #667)**: Scoped single `Alt` menu shortcuts (`Alt+1`..`Alt+0` folding shortcuts) on macOS, allowing European and international Mac keyboard layouts to seamlessly type characters like `#` (`Option+3`), `€` (`Option+2`), `[`, `]`, `~`, and `@` without menu shortcut interception.
- 🇮🇩 **Penyelesaian Konflik Tombol Option / Alt (Issues #814 & #667)**: Menghapus jalan pintas menu `Alt` tunggal pada macOS, memungkinkan papan ketik Eropa dan internasional mengetik karakter penting seperti `#` (`Option+3`), `€` (`Option+2`), `[`, `]`, `~`, dan `@` tanpa terhalang menu lipat kode.
- 🇬🇧 **Atomic Session Serialization with `QSaveFile` (Issue #650)**: Replaced direct `QFile` writes with `QSaveFile` commit transactions in `ScintillaNext::writeToDisk` and `SessionManager`, completely eliminating 0-byte corrupted session and document files during abrupt macOS shutdowns or power losses.
- 🇮🇩 **Penyimpanan Sesi Atomik via `QSaveFile` (Issue #650)**: Mengganti penulisan `QFile` mentah dengan transaksi commit atomik `QSaveFile` pada penyimpanan berkas dan sesi, mencegah berkas rusak berukuran 0-byte saat macOS mati mendadak.
- 🇬🇧 **Native JSON Tools: Format & Minify (Issue #1140)**: Added native `Format JSON` (indented) and `Minify JSON` (compact) utilities under `Edit -> JSON Tools` using high-performance `QJsonDocument`.
- 🇮🇩 **Alat JSON Native: Format & Minifikasi (Issue #1140)**: Menambahkan utilitas `Format JSON` (berindentasi rapi) dan `Minify JSON` (kompak ringkas) di bawah `Edit -> JSON Tools` menggunakan `QJsonDocument`.
- 🇬🇧 **Tab & Space Indentation Conversion (Issues #668 & #844)**: Added `Convert Tabs to Spaces` and `Convert Spaces to Tabs` under `Edit -> Indentation`, dynamically respecting the current editor tab width.
- 🇮🇩 **Konversi Tab & Spasi (Issues #668 & #844)**: Menambahkan `Convert Tabs to Spaces` dan `Convert Spaces to Tabs` pada `Edit -> Indent`, menyesuaikan dengan lebar tab aktif secara dinamis.

### ⚡ Lexilla Lexer Synchronization & Language Architecture / Sinkronisasi Lexer Lexilla & Arsitektur Bahasa

- 🇬🇧 **Lexilla Lexer Name Harmonization**: Fixed and synchronized lexer identifier mapping across all 87 language definitions in `src/languages/`, updating `autoit` -> `au3`, `baanc` -> `baan`, `cobol` -> `COBOL`, `fortran77` -> `f77`, `postscript` -> `ps`, `scheme` -> `lisp`, `objc` -> `cpp`, `rc` -> `cpp`, `swift` -> `cpp`, `asp` -> `hypertext`, and `nfo` -> `null`. Guarantees 100% active syntax highlighting without silent lexer initialization failures.
- 🇮🇩 **Penyelarasan Nama Lexer Lexilla**: Memperbaiki dan menyinkronkan pemetaan pengidentifikasi lexer pada seluruh 87 definisi bahasa di `src/languages/`, memperbarui `autoit` -> `au3`, `baanc` -> `baan`, `cobol` -> `COBOL`, `fortran77` -> `f77`, `postscript` -> `ps`, `scheme` -> `lisp`, `objc` -> `cpp`, `rc` -> `cpp`, `swift` -> `cpp`, `asp` -> `hypertext`, dan `nfo` -> `null`. Menjamin 100% penyorotan sintaks aktif tanpa kegagalan inisialisasi lexer.
- 🇬🇧 **Defensive Lexer Alias & Case Fallback Resolution**: Implemented a resilient fallback lookup table in `NotepadNextApplication::setEditorLanguage` that automatically maps legacy or lowercase lexer requests (`autoit`, `cobol`, `swift`, `objc`, etc.) to valid Lexilla lexer modules, preventing syntax highlighting loss.
- 🇮🇩 **Resolusi Alias & Fallback Kasus Lexer Defensif**: Mengimplementasikan tabel pencarian fallback yang tangguh pada `NotepadNextApplication::setEditorLanguage` yang secara otomatis memetakan permintaan lexer lama atau huruf kecil ke modul lexer Lexilla yang valid.
- 🇬🇧 **13-Pillar Clean Code & SonarQube Enterprise Readiness**: Completed full-scale 13-pillar repository audit with `cppcheck`, `trunk check`, and SonarQube configuration (`sonar-project.properties` bumped to `0.15-06`), achieving 0 errors and 0 warnings across all 414 repository files.
- 🇮🇩 **Kesiapan Enterprise 13-Pilar & SonarQube**: Menyelesaikan audit repositori 13-pilar secara penuh dengan `cppcheck`, `trunk check`, dan konfigurasi SonarQube (`sonar-project.properties` diperbarui ke `0.15-06`), mencapai 0 error dan 0 warning di seluruh 414 berkas repositori.

### 🎨 Theme Studio Polish & Visual Enhancements / Pemolesan Theme Studio & Visual

- 🇬🇧 **Default Dark Horizon Palette**: Updated default dark theme to the refined `"horizonDark"` palette, offering superior contrast and syntax readability.
- 🇮🇩 **Palet Gelap Horizon Default**: Memperbarui tema gelap bawaan ke palet `"horizonDark"` yang elegan dan berdaya kontras tinggi.
- 🇬🇧 **Dynamic Checkmarks in Themes Menu**: Connected `menuTheme->aboutToShow` signal to dynamically synchronize checkmarks on all 75 theme actions against the active palette ID.
- 🇮🇩 **Tanda Centang Dinamis pada Menu Tema**: Menghubungkan sinyal `menuTheme->aboutToShow` untuk menyinkronkan centang aktif pada seluruh 75 aksi palet tema.

---

## [0.15-05] - 2026-10-23

### 🎨 Dark & Light Mode Theme Engine / Engine Tema Mode Gelap & Terang

- 🇬🇧 **Dynamic Dark & Light Mode Support**: Engineered a full native Theme Manager architecture (`ThemeManager` singleton) ported and enhanced from `SparkNotePad`, supporting three distinct operating modes: Auto (synchronized with macOS system color scheme via `QPalette`), Force Light, and Force Dark.
- 🇮🇩 **Dukungan Mode Gelap & Terang Dinamis**: Mengembangkan arsitektur Theme Manager native lengkap (`ThemeManager` singleton) yang diadaptasi dan ditingkatkan dari `SparkNotePad`, mendukung tiga mode operasi: Otomatis (sinkronisasi dengan skema warna sistem macOS melalui `QPalette`), Paksa Terang, dan Paksa Gelap.
- 🇬🇧 **75 Curated Professional Palettes**: Bundled 75 studio-grade color palettes across 25 aesthetic families, including NotepadNext Default Light, Dracula Default Dark, Catppuccin (Mocha, Macchiato, Frappé, Latte), Nord, Tokyo Night, Gruvbox (Dark/Light), One Dark/Light, GitHub (Dark/Light), Solarized (Dark/Light), Monokai, Moonlight, Horizon, Rose Pine, SynthWave, and more.
- 🇮🇩 **75 Palet Warna Profesional**: Menyertakan 75 palet warna tingkat studio dari 25 famili estetika, mencakup NotepadNext Default Light, Dracula Default Dark, Catppuccin (Mocha, Macchiato, Frappé, Latte), Nord, Tokyo Night, Gruvbox (Dark/Light), One Dark/Light, GitHub (Dark/Light), Solarized (Dark/Light), Monokai, Moonlight, Horizon, Rose Pine, SynthWave, dan lainnya.
- 🇬🇧 **Interactive Theme Switcher Dialog (`ThemeSwitcherDialog`)**: Created an interactive visual theme picker dialog (`Ctrl+Shift+T` / `Cmd+Shift+T`) featuring real-time fuzzy search, category filtering (All, Light, Dark), 6-swatch palette color previews, live preview on selection, and instantaneous Esc key cancellation.
- 🇮🇩 **Dialog Pemilih Tema Interaktif (`ThemeSwitcherDialog`)**: Membuat dialog pemilih tema visual interaktif (`Ctrl+Shift+T` / `Cmd+Shift+T`) dengan pencarian cepat, filter kategori (Semua, Terang, Gelap), pratinjau 6 blok warna palet, pratinjau langsung saat memilih, dan pembatalan instan dengan tombol Esc.
- 🇬🇧 **High-Contrast Scintilla Editor Styling**: Applied semantic theme colors across Scintilla styles (slots 0..31), caret, selection, line number margin, fold markers, indent guides, and active line highlighting, completely eliminating low-contrast dark-on-dark text artifacts.
- 🇮🇩 **Pewarnaan Editor Scintilla Kontras Tinggi**: Menerapkan warna tema semantik pada gaya Scintilla (slot 0..31), kursor caret, seleksi teks, margin nomor baris, penanda lipatan kode, panduan indentasi, dan penyorotan baris aktif, mencegah teks gelap di atas latar gelap.
- 🇬🇧 **Comprehensive QSS UI Theming**: Full Qt stylesheet coverage for menus, menu bars, toolbars, status bars, docks, tab bars, comboboxes, spinboxes, pushbuttons, and dialog headers with native SVG checkmark badges (`checkmark.svg`).
- 🇮🇩 **Pewarnaan UI Menyeluruh via QSS**: Cakupan stylesheet Qt lengkap untuk menu, bilah menu, bilah alat, bilah status, dock, bilah tab, kotak kombo, spinbox, tombol tekan, dan header dialog dengan ikon centang SVG native (`checkmark.svg`).
- 🇬🇧 **Dedicated Themes Menu & Preferences Integration**: Added top-level `&Themes` menu to `MainWindow` with one-click toggling (`Ctrl+Alt+T` / `Cmd+Alt+T`), Light/Dark submenus, and Family submenus. Integrated a modern `Themes & Visual Styling` panel at the top of the General Preferences page.
- 🇮🇩 **Menu Tema Terdedikasi & Integrasi Pengaturan**: Menambahkan menu utama `&Themes` pada `MainWindow` dengan tombol pintas (`Ctrl+Alt+T` / `Cmd+Alt+T`), submenu Terang/Gelap, dan submenu Famili Tema. Mengintegrasikan panel `Themes & Visual Styling` modern pada bagian teratas dialog Pengaturan Umum.

### 🔄 Auto-Update & Help Menu Integration / Integrasi Pembaruan Otomatis & Menu Bantuan

- 🇬🇧 **Native macOS Update Check**: Enabled "Check for Updates" under Help menu on macOS via `QSimpleUpdater`, querying `https://github.com/alsyundawy/NotepadNext-MacOS/raw/master/updates.json` with dedicated platform key `"macos"`.
- 🇮🇩 **Pemeriksaan Pembaruan Native macOS**: Mengaktifkan "Periksa Pembaruan" pada menu Bantuan di macOS via `QSimpleUpdater`, menghubungkan ke `updates.json` dengan platform key `"macos"`.

### 🛠️ Toolbar Enhancements & Exit Action / Peningkatan Bilah Alat & Aksi Keluar

- 🇬🇧 **Integrated Missing Toolbar Actions**: Added standard supported actions to `mainToolBar`: `actionPrint` (Print...) with `printer.png` in the file operations block, and `actionPreferences` (Preferences...) with `cog.png` in the settings block, aligning toolbar accessibility with Notepad++, Notepad2, and modern code editors.
- 🇮🇩 **Penambahan Aksi Bilah Alat Standar**: Menambahkan aksi yang didukung ke `mainToolBar`: `actionPrint` (Cetak...) dengan ikon `printer.png` pada grup operasi berkas, dan `actionPreferences` (Pengaturan...) dengan ikon `cog.png` pada grup pengaturan, menyelaraskan aksesibilitas bilah alat dengan Notepad++, Notepad2, dan editor kode modern.
- 🇬🇧 **Dedicated Exit Icon on Toolbar**: Created modern vector `exit.svg` and pixel-crisp 16x16 `exit.png` icons, registered them in `resources.qrc`, and positioned `actionExit` preceded by a separator at the very end of `mainToolBar`.
- 🇮🇩 **Ikon Keluar Khusus pada Bilah Alat**: Membuat ikon vektor modern `exit.svg` dan ikon 16x16 `exit.png` beresolusi tajam, mendaftarkannya pada `resources.qrc`, dan menempatkan `actionExit` yang didahului pemisah pada ujung akhir `mainToolBar`.
- 🇬🇧 **Const-Reference Optimization & Header Hardening**: Updated `MainWindow::buildMenu` parameter to `const QStringList &` for zero-copy efficiency, and included `<QtGlobal>` to guarantee clean evaluation of Qt version check macros.
- 🇮🇩 **Optimalisasi Referensi Konstan & Penguatan Header**: Memperbarui parameter `MainWindow::buildMenu` menjadi `const QStringList &` untuk efisiensi zero-copy, serta menyertakan `<QtGlobal>` untuk memastikan evaluasi makro pengecekan versi Qt berjalan mulus.

### 🔤 Convert Case & Context Menu Enhancement / Peningkatan Menu Konversi Huruf & Konteks

- 🇬🇧 **Complete Convert Case Submenu**: Added `Convert Case >` submenu to the editor right-click context menu and Edit menu, featuring `UPPER CASE`, `lower case`, separator, `Title Case`, `Sentence case`, and `iNVERT cASE`.
- 🇮🇩 **Submenu Konversi Huruf Lengkap**: Menambahkan submenu `Convert Case >` pada menu klik kanan editor dan menu Edit, mencakup `UPPER CASE`, `lower case`, pemisah, `Title Case`, `Sentence case`, dan `iNVERT cASE`.
- 🇬🇧 **Multi-Selection & Unicode Safe**: Supports multi-selection ranges, full UTF-8 Unicode letter handling, and single atomic undo grouping across all case transformations.
- 🇮🇩 **Mendukung Multi-Seleksi & Aman Unicode**: Mendukung rentang seleksi ganda, penanganan huruf UTF-8 Unicode lengkap, dan satu aksi pembatalan (undo) atomik.
- 🇬🇧 **Paste Plain Text / Without Formatting**: Added `Paste Plain Text` (`actionPasteWithoutFormat`) with `Cmd+Shift+V` / `Ctrl+Shift+V` shortcut in Edit and editor context menus.
- 🇮🇩 **Tempel Teks Polos Tanpa Format**: Menambahkan `Tempel Teks Polos` (`actionPasteWithoutFormat`) dengan pintasan `Cmd+Shift+V` / `Ctrl+Shift+V` pada menu Edit dan menu konteks editor.

### 🌓 Dark Mode Contrast & Pasted Text Fix / Perbaikan Kontras & Warna Teks Tempel Mode Gelap

- 🇬🇧 **Elimination of White Background Blocks**: Removed legacy hardcoded `bgColor = rgb(0xFFFFFF)` in `src/languages/markdown.lua`, eliminating white background rectangles when pasting bullet lists or headers.
- 🇮🇩 **Pelenyapan Kotak Latar Putih**: Menghapus kode lama `bgColor = rgb(0xFFFFFF)` di `src/languages/markdown.lua`, menghilangkan kotak putih saat menempel teks daftar atau judul.
- 🇬🇧 **Luminance Safety Sweep in ThemeManager**: Implemented dual luminance scanning across all 128 Scintilla style slots in dark mode, automatically elevating dark text (`lum < 140`) to `txtPri` and resetting bright backgrounds (`lum > 90`) to canvas.
- 🇮🇩 **Pemindaian Luminansi di ThemeManager**: Menerapkan pemindaian luminansi ganda pada 128 slot gaya Scintilla dalam mode gelap, mengangkat teks gelap (`lum < 140`) dan mereset latar terang (`lum > 90`).
- 🇬🇧 **Lua Init Style Protection**: Enhanced `init.lua` with dynamic luminance checks before applying language styles in dark mode.
- 🇮🇩 **Proteksi Gaya Lua Init**: Menyempurnakan `init.lua` dengan pemeriksaan luminansi dinamis sebelum menerapkan gaya bahasa pada mode gelap.

### 🚀 CI/CD Pipeline & GitHub Actions Stabilization / Stabilisasi Pipeline CI/CD & GitHub Actions

- 🇬🇧 **Comprehensive Multi-Qt Matrix (Qt 6.5 - Qt 6.12)**: Standardized `.github/workflows/build.yml` and `release.yml` with the full matrix spanning Qt 6.5, Qt 6.8, Qt 6.10, Qt 6.11, and Qt 6.12 across macOS Intel (`macos-15-intel`) and Apple Silicon (`macos-15`, `macos-26`), matching the deployment specification in README.md.
- 🇮🇩 **Matriks Multi-Qt Menyeluruh (Qt 6.5 - Qt 6.12)**: Menstandarisasi `.github/workflows/build.yml` dan `release.yml` dengan matriks lengkap mencakup Qt 6.5, Qt 6.8, Qt 6.10, Qt 6.11, dan Qt 6.12 pada macOS Intel (`macos-15-intel`) dan Apple Silicon (`macos-15`, `macos-26`), selaras dengan spesifikasi rilis pada README.md.
- 🇬🇧 **Fork Protection for Post-Release & CodeQL**: Added repository guards to prevent winget publish errors on forks and configured CodeQL triggers to prevent conflicts with GitHub Code Scanning default setup.
- 🇮🇩 **Proteksi Fork untuk Post-Release & CodeQL**: Menambahkan pengaman repositori untuk mencegah kegagalan winget pada fork dan menyelaraskan konfigurasi CodeQL dengan pengaturan default GitHub.

### 🛡️ 13-Pillar Codebase Hardening & Bug Fixes / Penguatan Kode 13 Pilar & Perbaikan Bug

- 🇬🇧 **Null Pointer Defense (CWE-476)**: Added defensive null pointer guards across `MainWindow.cpp` (`updateGui`, `updateFileStatusBasedUi`, `updateEOLBasedUi`, `updateSaveStatusBasedUi`, `updateLanguageBasedUi`, `updateDocumentBasedUi`, `updateSelectionBasedUi`, `updateContentBasedUi`, `detectLanguage`, `activateEditor`), preventing potential segmentation faults during dock tear-downs and tab close events.
- 🇮🇩 **Pencegahan Null Pointer (CWE-476)**: Menambahkan pengecekan null pointer defensif pada seluruh fungsi pembaruan UI di `MainWindow.cpp`, mencegah potensi crash segmentasi saat penutupan tab atau pelepasan dock.
- 🇬🇧 **Buffer Safety (CWE-120/CWE-126)**: Replaced raw `memcpy` with safe `std::copy_n` in `ByteArrayUtils.h`, and added null checks and safe string length handling in `LuaConsoleDock.cpp`.
- 🇮🇩 **Keamanan Buffer (CWE-120/CWE-126)**: Mengganti `memcpy` mentah dengan `std::copy_n` di `ByteArrayUtils.h`, serta menambahkan validasi null pointer dan penanganan string aman di `LuaConsoleDock.cpp`.
- 🇬🇧 **Filesystem Return Code Verification**: Enhanced `ScintillaNext.cpp` to verify the return status of `QDir::mkpath()`, logging diagnostic warnings upon filesystem failures instead of silently ignoring errors.
- 🇮🇩 **Verifikasi Return Code Sistem Berkas**: Menyempurnakan `ScintillaNext.cpp` untuk memeriksa status kembalian fungsi `QDir::mkpath()` dan mencatat peringatan diagnostik ketika pembuatan direktori gagal.
- 🇬🇧 **Variable Shadowing Elimination**: Resolved shadowed variables `markerAppDecorator`, `closeEv`, `showEv`, `targetEd`, and `totalReplaced` across `MainWindow.cpp`, `AboutDialog.cpp`, and `FindReplaceDialog.cpp`.
- 🇮🇩 **Pembersihan Penyamaran Variabel**: Memperbaiki variabel yang tersamar `markerAppDecorator`, `closeEv`, `showEv`, `targetEd`, dan `totalReplaced` pada `MainWindow.cpp`, `AboutDialog.cpp`, dan `FindReplaceDialog.cpp`.
- 🇬🇧 **Tab Bar Visual Artifact Fix**: Disabled base line drawing (`tabBar->setDrawBase(false)`) across tabbed dialogs (`FindReplaceDialog`, `AboutDialog`) to prevent unstyled line clipping artifacts under dark themes.
- 🇮🇩 **Perbaikan Garis Bawah Tab Bar**: Menonaktifkan penggambaran garis dasar (`tabBar->setDrawBase(false)`) pada dialog bertab (`FindReplaceDialog`, `AboutDialog`) untuk mencegah garis kliping yang tidak terwarnai pada tema gelap.

### 🔍 SonarQube & Compilation Database Pipeline / Pipeline SonarQube & Basis Data Kompilasi

- 🇬🇧 **SonarQube Static Analysis Configuration**: Configured repository-specific `sonar-project.properties` specifying project key, C/C++ source directories, exclusions for third-party libraries, and compilation database integration.
- 🇮🇩 **Konfigurasi Analisis Statis SonarQube**: Mengonfigurasi `sonar-project.properties` khusus repositori yang memuat project key, direktori sumber C/C++, pengecualian pustaka pihak ketiga, dan integrasi compilation database.
- 🇬🇧 **Compilation Database Generator**: Created `scripts/generate_compilation_database.py` that generates an accurate `compile_commands.json` (86 translation units) for SonarQube, SonarLint, and clangd static analysis without requiring a full local Qt build.
- 🇮🇩 **Generator Basis Data Kompilasi**: Membuat skrip `scripts/generate_compilation_database.py` untuk memproduksi `compile_commands.json` yang presisi (86 unit translasi) bagi SonarQube, SonarLint, dan clangd tanpa memerlukan kompilasi lokal Qt secara utuh.
- 🇬🇧 **SonarLint IDE Integration**: Added `.vscode/settings.json` configuring `sonarlint.pathToCompileCommands`, `sonarqube.pathToCompileCommands`, and `clangd` arguments, locking the default terminal profile to bash.
- 🇮🇩 **Integrasi SonarLint IDE**: Menambahkan `.vscode/settings.json` yang mengonfigurasi jalur `compile_commands.json` untuk SonarLint dan SonarQube, argumen `clangd`, serta mengunci profil terminal default ke bash.

### ⚡ Version Bump & Packaging / Peningkatan Versi & Pengemasan

- 🇬🇧 **Version Bump to 0.15-05**: Updated application version to `0.15-05` across `CMakeLists.txt`, `src/CMakeLists.txt`, `updates.json`, and documentation notes.
- 🇮🇩 **Peningkatan Versi ke 0.15-05**: Memperbarui versi aplikasi menjadi `0.15-05` pada `CMakeLists.txt`, `src/CMakeLists.txt`, `updates.json`, dan catatan dokumentasi.
- 🇬🇧 **Linter Cleanliness**: 100% compliant with `trunk check`, `cppcheck`, `markdownlint`, `prettier`, and MegaLinter quality gates.
- 🇮🇩 **Kepatuhan Linter Penuh**: Memenuhi 100% standar pengujian `trunk check`, `cppcheck`, `markdownlint`, `prettier`, dan gerbang kualitas MegaLinter.

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

### 🪟 New Window & Multi-Instance Architecture / Jendela Baru & Arsitektur Multi-Instance

- 🇬🇧 **New Window Menu Action**: Added `New Window` (`actionNewWindow`) to the File menu with shortcut `Ctrl+Shift+N` (`Cmd+Shift+N` on macOS), enabling users to launch independent secondary editor instances.
- 🇮🇩 **Aksi Menu Jendela Baru**: Menambahkan opsi `Jendela Baru` (`actionNewWindow`) pada menu Berkas dengan pintasan `Ctrl+Shift+N` (`Cmd+Shift+N` di macOS) untuk meluncurkan jendela aplikasi sekunder independen.
- 🇬🇧 **Native macOS Multi-Instance Handling**: Integrated macOS bundle detection using `/usr/bin/open -n <Bundle.app> --args --multi-instance`, ensuring seamless WindowServer focus and isolation from the primary process.
- 🇮🇩 **Penanganan Multi-Instance Native macOS**: Mengintegrasikan deteksi bundle macOS melalui `/usr/bin/open -n <Bundle.app> --args --multi-instance` untuk isolasi proses dan fokus WindowServer yang optimal.
- 🇬🇧 **Unified Settings Persistence**: Configured `ApplicationSettings` to explicitly bind to `"NotepadNext"`, guaranteeing that multi-instance processes share and preserve unified user preferences and configurations from `NotepadNext.ini`.
- 🇮🇩 **Persistensi Pengaturan Terpadu**: Mengonfigurasi `ApplicationSettings` agar mengikat eksplisit ke `"NotepadNext"`, menjamin semua jendela sekunder berbagi dan memperbarui file konfigurasi `NotepadNext.ini` yang sama.

### 🔤 Comprehensive File Encoding & uchardet Engine / Menu Encoding & Engine Deteksi Karakter uchardet

- 🇬🇧 **Multi-Stage Character Encoding Detection Engine**: Implemented full multi-stage encoding and BOM detection in `src/utils/EncodingDetector`, powered by Mozilla Universal Charset Detector (`uchardet`), interleaved NUL heuristics for UTF-16, and regex-based header declaration sniffers (XML, HTML, Python/Ruby cookies).
- 🇮🇩 **Engine Deteksi Encoding Karakter Multi-Tahap**: Menerapkan arsitektur deteksi encoding bertingkat dan BOM dalam `src/utils/EncodingDetector`, didukung Mozilla Universal Charset Detector (`uchardet`), heuristik interleaved NUL untuk UTF-16, serta pencari header deklarasi (XML, HTML, coding cookie Python/Ruby).
- 🇬🇧 **Dedicated Encoding Menu in MainWindow**: Activated top-level `Encoding` menu featuring quick conversion actions (UTF-8, UTF-8 BOM, ANSI Windows-1252, UTF-16 LE, UTF-16 BE) and a comprehensive `Reopen with Encoding` submenu with major East Asian, Cyrillic, Central European, and DOS codepages.
- 🇮🇩 **Menu Encoding Terdedikasi pada MainWindow**: Mengaktifkan menu utama `Encoding` lengkap dengan aksi konversi cepat (UTF-8, UTF-8 BOM, ANSI Windows-1252, UTF-16 LE, UTF-16 BE) dan submenu `Reopen with Encoding` untuk membuka ulang berkas menggunakan berbagai codepage (Asia Timur, Sirilik, Eropa Tengah, dan DOS).
- 🇬🇧 **Interactive Encoding Selection Dialog (`EncodingSelectionDialog`)**: Added interactive encoding dialog with real-time fuzzy filtering, document encoding confidence scores, quick preset buttons, and dual conversion vs reopening modes.
- 🇮🇩 **Dialog Pemilih Encoding Interaktif (`EncodingSelectionDialog`)**: Menyediakan dialog pemilihan encoding interaktif dengan pencarian cepat, skor keyakinan deteksi, tombol pintas preset, dan dua mode operasi (konversi isi dokumen vs muat ulang dari disk).
- 🇬🇧 **Status Bar Encoding Interaction**: Upgraded status bar encoding label (`unicodeType`) to dynamically display active encoding & BOM, with informative tooltip and one-click access to the Encoding Selection Dialog.
- 🇮🇩 **Interaksi Status Bar untuk Encoding**: Label status bar encoding (`unicodeType`) kini menampilkan nama encoding dan BOM secara dinamis serta dapat diklik langsung untuk memunculkan dialog pemilihan encoding.

### 💎 Modernized Adaptive "About NotepadNext" Dialog / Dialog "Tentang NotepadNext" Modern & Adaptif

- 🇬🇧 **Full Adaptive About Dialog (`AboutDialog`)**: Replaced legacy `QMessageBox` with a dedicated, tabbed `AboutDialog` that dynamically derives high-contrast colors from the system palette (`QPalette`), adapting seamlessly to macOS light and dark appearances without requiring an in-app dark mode engine.
- 🇮🇩 **Dialog Tentang NotepadNext Adaptif (`AboutDialog`)**: Menggantikan `QMessageBox` lama dengan dialog bertab modern (`AboutDialog`) yang menyesuaikan warna secara dinamis terhadap palet sistem (`QPalette`), bekerja sempurna pada tampilan macOS Terang maupun Gelap tanpa memerlukan engine dark mode internal.
- 🇬🇧 **AboutDialog Stability & Reentrancy Hardening**: Neutralized a critical stack-overflow crash (SIGSEGV) triggered on opening the About dialog by implementing strict reentrancy guards (`m_isUpdatingTheme`) against recursive `QEvent::PaletteChange` / `setStyleSheet` cycles, correcting widget parentage hierarchies across all tab pages, and eliminating string placeholder formatting issues.
- 🇮🇩 **Pencegahan Crash & Penguatan Stabilitas AboutDialog**: Menghilangkan crash fatal (SIGSEGV) saat membuka dialog About dengan menerapkan pengaman reentrancy (`m_isUpdatingTheme`) terhadap loop rekursif `QEvent::PaletteChange`, memperbaiki hierarki parent widget pada seluruh tab, serta menuntaskan masalah pemformatan string.
- 🇬🇧 **Six Structured Tabs**: Features dedicated tabs for About (logo, badges, legal note, metadata table), Maintainer (Harry DS Alsyundawy, ALSYUNDAWY IT SOLUTION, PayPal & QRIS donations), Credits & Attribution (Justin Dailey, Neil Hodgson, Scintilla/Lexilla, Qt, uchardet, NoteSciPad), Diagnostics (OS, kernel, CPU arch, Qt version, Retina DPI scale with one-click clipboard copy), Shortcuts (essential keyboard combinations), and License (GPLv3).
- 🇮🇩 **Enam Tab Terstruktur**: Menyediakan tab khusus Tentang, Pemelihara (Harry DS Alsyundawy, ALSYUNDAWY IT SOLUTION, donasi PayPal & QRIS), Kredit & Atribusi (Justin Dailey, Neil Hodgson, Scintilla/Lexilla, Qt, uchardet, NoteSciPad), Diagnostik (spesifikasi OS, CPU, Qt, rasio Retina dengan tombol salin ke clipboard), Pintasan Keyboard, dan Lisensi (GPLv3).

### 🎨 Redesigned Save & Exit Prompt Dialog (`SaveModifiedDialog`) / Dialog Konfirmasi Simpan & Keluar Modern

- 🇬🇧 **Apple HIG Compliant Save Prompt**: Replaced the plain, unstyled `QMessageBox` in `MainWindow::promptForSave` with a dedicated, custom-crafted `SaveModifiedDialog` matching modern macOS human interface guidelines.
- 🇮🇩 **Dialog Konfirmasi Simpan Standar Apple HIG**: Menggantikan `QMessageBox` sederhana pada `MainWindow::promptForSave` dengan dialog `SaveModifiedDialog` modern yang dirancang khusus mengikuti standar Apple Human Interface Guidelines.
- 🇬🇧 **Contextual Layout & Document Details**: Features the 64×64 application Retina squircle icon, bold contextual headline, explanatory data loss warning, and a document preview card showing the file path for single files or a scrollable list with status tags for multi-document closing.
- 🇮🇩 **Tata Letak Kontekstual & Detail Dokumen**: Menampilkan ikon aplikasi Retina 64×64, judul tebal kontekstual, peringatan risiko kehilangan data, serta kartu pratinjau dokumen yang menampilkan path berkas untuk dokumen tunggal atau daftar berkas yang dapat digulir untuk penutupan multi-dokumen.
- 🇬🇧 **macOS Button Hierarchy & Keyboard Navigation**: Implements macOS standard button positioning with "Don't Save" / "Don't Save All" on the left, "Cancel" (`Esc` / `Cmd+.`), and "Save" / "Save All" on the right as the high-contrast default action (`Return` / `Cmd+S`). Dedicated keyboard handler captures `Cmd+D` for instant "Don't Save" discarding.
- 🇮🇩 **Hierarki Tombol macOS & Navigasi Keyboard**: Menerapkan tata letak tombol standar macOS dengan opsi "Jangan Simpan" di sebelah kiri, "Batal" (`Esc` / `Cmd+.`), dan "Simpan" sebagai aksi utama di sebelah kanan (`Return` / `Cmd+S`). Pintasan keyboard `Cmd+D` langsung membuang perubahan tanpa harus menggunakan mouse.
- 🇬🇧 **Dynamic Palette Harmony**: Automatically harmonizes background, text, borders, and buttons with the active macOS system palette (Light and Dark appearance) with built-in reentrancy protection.
- 🇮🇩 **Harmonisasi Palet Dinamis**: Menyesuaikan warna latar, teks, batas, dan tombol secara otomatis dengan palet sistem macOS aktif (tampilan Terang dan Gelap) dengan proteksi reentrancy terintegrasi.

### ⚡ Build, CI/CD Runners & MegaLinter / Otomasi Build, Runner CI & MegaLinter

- 🇬🇧 **Qt 6.12 Runner Matrix for macOS 15+**: Added dedicated GitHub Actions matrix runners for building and releasing with Qt 6.12 specifically targeting macOS 15+ (Sequoia) across both Apple Silicon (`arm64`) and Intel (`x64`) architectures in `build.yml` and `release.yml`.
- 🇮🇩 **Matrix Runner Qt 6.12 untuk macOS 15+**: Menambahkan runner GitHub Actions untuk kompilasi dan rilis menggunakan Qt 6.12 yang secara spesifik menargetkan macOS 15+ (Sequoia) pada arsitektur Apple Silicon (`arm64`) maupun Intel (`x64`).
- 🇬🇧 **Tailored MegaLinter Configuration (`.mega-linter.yml`)**: Designed a lean, repository-aligned MegaLinter configuration enabling only relevant descriptors (C++, CMake, Shell, Actions, YAML, JSON, Markdown, XML) while explicitly disabling 30+ irrelevant language linters (PHP, Ruby, C#, Java, Dart, Terraform, etc.) and suppressing false-positive spell/copypaste checks on vendored code.
- 🇮🇩 **Konfigurasi MegaLinter Ringkas (`.mega-linter.yml`)**: Membuat konfigurasi MegaLinter yang disesuaikan khusus untuk basis kode C++/Qt/CMake, menonaktifkan 30+ linter bahasa yang tidak relevan, serta mengecualikan berkas thirdparty dan translasi dari pemeriksaan yang tidak perlu.

### 🧩 Third-Party Components & Context Menu / Komponen Pihak Ketiga & Menu Konteks

- 🇬🇧 **Third-Party Stack Upgrade**: Integrated and refined components from NoteSciPad: Scintilla `5.6.7`, Lexilla `5.5.4`, and Lua `5.5.1`. Added intelligent Python 3.13+ interpreter discovery to `thirdparty/CMakeLists.txt`.
- 🇮🇩 **Pembaruan Komponen Pihak Ketiga**: Mengintegrasikan dan menyesuaikan komponen dari NoteSciPad: Scintilla `5.6.7`, Lexilla `5.5.4`, dan Lua `5.5.1`. Menambahkan deteksi otomatis interpreter Python 3.13+ pada `thirdparty/CMakeLists.txt`.
- 🇬🇧 **Default Editor Context Menu**: Set default `EditorContextMenu` in GUI settings to `Undo, Redo, , Cut, Copy, Paste, Delete, , SelectAll, , UpperCase, LowerCase, , Open, Save, Find, , Base64Encode, URLEncode, , Base64Decode, URLDecode`. Added token whitespace sanitization (`.trimmed()`) for seamless INI parsing.
- 🇮🇩 **Menu Konteks Editor Default**: Mengonfigurasi `EditorContextMenu` default pada pengaturan GUI sesuai urutan standar lengkap dengan pembersihan spasi token otomatis untuk keandalan parsing INI.
- 🇬🇧 **Linter Cleanliness**: 100% compliant with `trunk check`, `actionlint`, `yamllint`, `markdownlint`, `prettier`, and `oxipng` image optimization.
- 🇮🇩 **Kepatuhan Linter**: Memenuhi 100% standar pengujian `trunk check`, `actionlint`, `yamllint`, `markdownlint`, `prettier`, dan optimasi gambar `oxipng`.
- 🇬🇧 **Metadata & Bundle Identifier Alignment**: Standardized application bundle and AppStream identifier to `com.github.alsyundawy.NotepadNext` across macOS `deploy/macos/info.plist`, Linux metadata `deploy/linux/com.github.alsyundawy.NotepadNext.metainfo.xml`, and system scripts `NotepadNext-Default-Editor.sh` (with legacy fallback).
- 🇮🇩 **Penyelarasan Pengenal Bundle & Metadata**: Menstandarkan identifier bundle aplikasi dan AppStream menjadi `com.github.alsyundawy.NotepadNext` di seluruh `deploy/macos/info.plist`, metadata Linux `com.github.alsyundawy.NotepadNext.metainfo.xml`, dan skrip integrasi `NotepadNext-Default-Editor.sh` (dengan fallback kompatibilitas).
- 🇬🇧 **Comparative Release Documentation**: Authored `doc/RELEASE_NOTES_0.15-04.md` providing a comprehensive architectural comparison against official upstream v0.15 and modern desktop text editors (Notepad2, Notepad3, Notepad4, Notepad++, Sublime Text, BBEdit, CotEditor, VS Code).
- 🇮🇩 **Dokumentasi Catatan Rilis Komparatif**: Menyusun `doc/RELEASE_NOTES_0.15-04.md` yang memuat perbandingan arsitektural komprehensif terhadap versi resmi hulu v0.15 dan ekosistem text editor modern (Notepad2, Notepad3, Notepad4, Notepad++, Sublime Text, BBEdit, CotEditor, VS Code).
- 🇬🇧 **README Logo Icon Header**: Elevated project presentation in `README.md` with centered high-craft macOS squircle app icon, release notes badge, and dual-language descriptions.
- 🇮🇩 **Header Logo Ikon README**: Mempercantik tampilan proyek pada `README.md` dengan ikon squircle native macOS terpusat beresolusi tinggi, lencana catatan rilis, dan deskripsi bilingual.

---

## [0.15-03] - 2026-10-19

### 🌐 Upstream Synchronization & Localization / Sinkronisasi Upstream & Pelokalan

- 🇬🇧 **Upstream Sync**: Merged latest updates and stability patches from upstream `dail8859/NotepadNext (v0.15)`.
- 🇮🇩 **Sinkronisasi Upstream**: Menggabungkan pembaruan terbaru dan perbaikan stabilitas dari repositori hulu `dail8859/NotepadNext (v0.15)`.
- 🇬🇧 **Indonesian Translation Polish**: Thoroughly refined Indonesian translations across 64 UI strings, ensuring natural terminology for search, replacement, zoom, and document management.
- 🇮🇩 **Penyempurnaan Terjemahan Bahasa Indonesia**: Memperhalus 64 string antarmuka Bahasa Indonesia pada pencarian, penggantian, pengaturan zoom, dan pengelolaan dokumen.
- 🇬🇧 **Metadata & Packaging**: Bumped version to `0.15-03` across `updates.json` and `CMakeLists.txt`; synchronized macOS packaging workflow for automated DMG asset deployment.
- 🇮🇩 **Metadata & Pengemasan**: Menaikkan versi ke `0.15-03` di `updates.json` dan `CMakeLists.txt`; menyelaraskan alur kerja pengemasan otomatis DMG macOS.

---

## [0.15-02] - 2026-09-25

### ⚙️ Lua Runtime & Build Automation / Runtime Lua & Otomasi Build

- 🇬🇧 **Embedded Lua Upgrade**: Upgraded embedded Lua runtime to version 5.4.9 with compatibility macros (`LUA_QL` / `LUA_QS`) in `luaconf.h` and resolved compilation warnings in `LuaExtension.cpp`.
- 🇮🇩 **Pembaruan Runtime Lua**: Memperbarui pustaka runtime Lua ke versi 5.4.9 dengan makro kompatibilitas (`LUA_QL` / `LUA_QS`) di `luaconf.h` serta memperbaiki peringatan kompilasi pada `LuaExtension.cpp`.
- 🇬🇧 **Scroll & Wheel Handling**: Added static linkage to `isHorizontalWheelEvent` in scroll handlers to prevent cross-translation-unit collision and enhance touchpad gestures.
- 🇮🇩 **Penanganan Gulir & Trackpad**: Menambahkan static linkage pada fungsi `isHorizontalWheelEvent` untuk mencegah konflik simbol dan meningkatkan responsivitas gestur trackpad macOS.
- 🇬🇧 **Dual Architecture Packaging**: Engineered release packaging to bundle standalone architecture-specific DMGs inside ZIP archives (`NotepadNext-v0.15-arm64.dmg` and `NotepadNext-v0.15-x64.dmg`).
- 🇮🇩 **Pengemasan Dua Arsitektur**: Mengotomatiskan pembuatan bundel rilis DMG terpisah untuk masing-masing arsitektur Apple Silicon (`arm64`) dan Intel (`x64`) dalam format ZIP.
- 🇬🇧 **CI Sanitation**: Sanitized bot identity and git push targets in translation update workflows (`lupdate.yml`).
- 🇮🇩 **Sanitasi CI**: Membersihkan identitas bot dan target push git pada alur kerja pembaruan translasi (`lupdate.yml`).

---

## [0.15-01] - 2026-09-10

### 🚀 Major v0.15 Core Upgrade & macOS System Integration / Peningkatan Inti v0.15 & Integrasi Sistem macOS

- 🇬🇧 **v0.15 Core Rebase**: Completed full rebase onto upstream NotepadNext `v0.15`, bringing latest Scintilla editing core improvements, session persistence upgrades, and modern Qt 6 widgets.
- 🇮🇩 **Rebase Inti v0.15**: Menyelesaikan rebase penuh ke NotepadNext upstream `v0.15`, menghadirkan pembaruan inti penyuntingan Scintilla, peningkatan persistensi sesi, dan widget Qt 6 modern.
- 🇬🇧 **Expanded Default Editor Script (v1.6.0)**: Upgraded `NotepadNext-Default-Editor.sh` to comprehensively map 470+ developer file extensions across 15 categories, including 38 system UTIs.
- 🇮🇩 **Ekspansi Skrip Default Editor (v1.6.0)**: Memperbarui `NotepadNext-Default-Editor.sh` untuk memetakan 470+ ekstensi berkas pengembang lintas 15 kategori dan 38 UTI sistem secara instan.
- 🇬🇧 **macOS Sequoia / Sonoma Settings Compatibility**: Resolved System Settings pane disappearance issues on modern macOS releases by fixing LaunchServices bundle identifier registration.
- 🇮🇩 **Kompatibilitas Pengaturan macOS Sequoia / Sonoma**: Mengatasi masalah hilangnya panel Pengaturan Sistem pada macOS modern melalui perbaikan registrasi bundle identifier LaunchServices.
- 🇬🇧 **Localization Overhaul**: Comprehensive initial overhaul of Bahasa Indonesia (`id_ID`) translation files with extensive menu and dialog coverage.
- 🇮🇩 **Pembaruan Menyeluruh Pelokalan**: Pembaruan awal komprehensif pada berkas terjemahan Bahasa Indonesia (`id_ID`) mencakup seluruh menu dan dialog utama.
- 🇬🇧 **Repository Cleanup**: Removed deprecated Ko-fi links and harmonized all documentation with standard Markdown headings.
- 🇮🇩 **Pembersihan Repositori**: Menghapus tautan donasi usang dan menyelaraskan seluruh struktur heading dokumentasi Markdown standar.

---

## [0.14-07] - 2026-08-27

- fix: update builder copyright name, resolve linters, and sync updates metadata.
- Correct builder copyright name to HARRY DS ALSYUNDAWY in `CMakeLists.txt`.
- Fix Markdown formatting and newlines in documentation.
- Refactor `GenerateEnums.py` and `RemoveShortcutTranslations.py` to comply with PEP 8.
- Resolve nodiscard return value warnings in `MainWindow.cpp`, `LuaState.cpp`, and `ScintillaNext.cpp`.
- Fix SC2312 shellcheck warnings in `NotepadNext-Default-Editor.sh`.
