# 📜 Changelog — NotepadNext For MacOS

All notable changes to the **NotepadNext For MacOS** project are documented in this file.
Format is inspired by [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
