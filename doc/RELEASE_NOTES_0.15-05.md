# 📝 NotepadNext-MacOS Release 0.15-05

## 🇮🇩 Bahasa Indonesia

Rilis **v0.15-05** menghadirkan pembaruan estetika dan fungsional revolusioner: mesin tema native (*Native Theme Engine*) dengan **75 palet warna profesional di 25 keluarga tema**, pemilih tema visual interaktif `ThemeSwitcherDialog` (`Ctrl+Shift+T`), penyesuaian kontras tinggi teks Scintilla buffer, penambahan tombol penting pada toolbar utama (*Print*, *Preferences*, modern *Exit*), submenu transformasi teks (*Convert Case*), aksi *Paste Plain Text*, pemeriksa update terintegrasi untuk macOS (*Check for Updates*), integrasi SonarQube static analysis dengan generator database kompilasi instan, serta pembersihan bug 13-pilar (CWE-476 null safety).

### 🌟 Sorotan Pembaruan (Changelog dari 0.15-04 ke 0.15-05)

#### 1. 🎨 Mesin Tema Native (`ThemeManager`) & 75 Palet Warna Profesional

- **Mesin Tema Terintegrasi**: Mengimplementasikan arsitektur `ThemeManager` dan `ThemePaletteData` yang mengelola seluruh palet warna antarmuka dan editor secara terpusat.
- **3 Mode Operasi**: Mendukung mode `Auto` (otomatis mengikuti preferensi Light/Dark mode sistem macOS secara real-time), `Force Light`, dan `Force Dark`.
- **75 Palet Pilihan di 25 Keluarga Estetika**: Pilihan tema populer seperti GitHub Dark/Light, Dracula, Monokai, Nord, Solarized, Horizon Dark, One Dark, Catppuccin, dsb.
- **Visual Theme Switcher (`Ctrl+Shift+T`)**: Dialog interaktif `ThemeSwitcherDialog` dengan pencarian instan, swatch viewer 6 warna, preview real-time pada buffer editor, serta tombol Esc untuk membatalkan perubahan tanpa efek samping.
- **Scintilla Buffer High-Contrast (Slots 0..31)**: Mengharmonisasikan warna caret, seleksi teks, margin line number, dan folding indicator agar selalu berkontras tinggi dan bebas dari teks gelap di atas latar gelap.

#### 2. 🛠️ Toolbar Utama yang Diperkaya & Fitur Produktivitas Teks

- **Tombol Toolbar Baru**: Menambahkan tombol aksi cepat *Print*, *Preferences*, dan ikon modern *Exit* pada toolbar utama untuk alur kerja yang lebih ergonomis.
- **Submenu Convert Case**: Menyediakan konversi teks komprehensif pada menu Edit: *UPPERCASE*, *lowercase*, *Title Case*, *Sentence case*, *iNVERT cASE*, *camelCase*, dan *snake_case*.
- **Paste as Plain Text (`Cmd+Shift+V`)**: Menyisipkan teks dari clipboard tanpa menyertakan format rich-text atau artefak HTML.
- **Pemeriksa Pembaruan macOS (Check for Updates)**: Fitur pengecekan pembaruan rilis GitHub langsung dari menu aplikasi pada macOS.

#### 3. 🔍 Pipeline Analisis Statis SonarQube & Generator Database Kompilasi

- **Integrasi SonarQube & SonarLint**: Konfigurasi `sonar-project.properties` untuk inspeksi mutu kode berkelanjutan, keamanan, dan deteksi code smell.
- **Generator Compilation Database Instan**: Menghadirkan skrip [`scripts/generate_compilation_database.py`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/scripts/generate_compilation_database.py) yang mampu menghasilkan `compile_commands.json` (86 translation unit) dalam waktu <50ms tanpa beban CPU tinggi.

#### 4. 🛡️ Audit Mutu Kode 13-Pilar & Hardening Pra-Prosesor

- **Null Pointer Safety (CWE-476)**: Penambahan null guard defensif pada seluruh fungsi antarmuka `MainWindow.cpp` untuk mengeliminasi potensi crash SIGSEGV saat tab ditutup atau dock dilepas.
- **Validasi Filesystem**: Menambahkan pengecekan nilai balik `QDir::mkpath()` di [`src/ScintillaNext.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/ScintillaNext.cpp) guna mendeteksi kegagalan pembuatan direktori penyimpanan.
- **Resiliensi Header & Makro Qt**: Memperbaiki makro `QT_VERSION_CHECK` dan menambahkan header `<QObject>` untuk registrasi `Q_ENUM` pada header file lintas versi Qt.
- **Pemulihan Matrix CI/CD Lengkap**: Mengaktifkan kembali matrix build lengkap untuk Qt 6.5, Qt 6.8, Qt 6.10, Qt 6.11, dan Qt 6.12 pada runner macOS.

---

## 🇬🇧 English

Release **v0.15-05** introduces a revolutionary aesthetic and functional leap: a full-scale **Native Theme Engine (`ThemeManager`) with 75 curated professional palettes across 25 families**, an interactive `ThemeSwitcherDialog` (`Ctrl+Shift+T`), high-contrast Scintilla buffer styling, extended main toolbar actions (*Print*, *Preferences*, modern *Exit*), a comprehensive *Convert Case* text transform submenu, *Paste Plain Text*, native macOS *Check for Updates*, integrated SonarQube static analysis with an instant compilation database generator, and 13-pillar codebase hardening (CWE-476 null safety).

### 🚀 Changelog from 0.15-04 to 0.15-05

#### 1. 🎨 Native Theme Engine (`ThemeManager`) & 75 Professional Palettes

- **Centralized Engine**: Engineered `ThemeManager` and `ThemePaletteData` singletons to orchestrate unified styling across application UI and editor text buffers.
- **3 Operating Modes**: Features `Auto` (real-time dynamic sync with macOS system Light/Dark appearance), `Force Light`, and `Force Dark`.
- **75 Palettes across 25 Aesthetic Families**: Includes GitHub Dark/Light, Dracula, Monokai, Nord, Solarized, Horizon Dark, One Dark, Catppuccin, and more.
- **Interactive Visual Switcher (`Ctrl+Shift+T`)**: `ThemeSwitcherDialog` with instant filtering, a 6-swatch color preview, real-time live canvas styling, and non-destructive Esc key cancellation.
- **High-Contrast Scintilla Styling (Slots 0..31)**: Standardized foreground and background colors for caret, text selections, line number margins, and folding markers, completely eliminating dark-on-dark contrast fatigue.

#### 2. 🛠️ Extended Main Toolbar & Editing Productivity

- **New Toolbar Shortcuts**: Added *Print*, *Preferences*, and modernized *Exit* action icons to the primary toolbar for immediate accessibility.
- **Convert Case Submenu**: Full text transformations under the Edit menu: *UPPERCASE*, *lowercase*, *Title Case*, *Sentence case*, *iNVERT cASE*, *camelCase*, and *snake_case*.
- **Paste as Plain Text (`Cmd+Shift+V`)**: Clean clipboard insertion stripping rich-text styles and formatting tags.
- **macOS Check for Updates**: In-app GitHub release update verification dialog tailored for macOS desktop workflows.

#### 3. 🔍 SonarQube Static Analysis Pipeline & Instant Compilation Database

- **SonarQube & SonarLint Standards**: Configured repository-specific `sonar-project.properties` for continuous security scanning and code smell detection.
- **Instant Compilation Database Generator**: Added [`scripts/generate_compilation_database.py`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/scripts/generate_compilation_database.py) generating complete `compile_commands.json` (86 units) in under 50ms without CPU spikes.

#### 4. 🛡️ 13-Pillar Code Hardening & Preprocessor Safeguards

- **Null Pointer Safety (CWE-476)**: Introduced defensive null guards across `MainWindow.cpp` routines, eliminating SIGSEGV risks during dock teardowns and multi-tab closures.
- **Filesystem Safety**: Added return value verification and diagnostic alerts for `QDir::mkpath()` in [`src/ScintillaNext.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/ScintillaNext.cpp).
- **Preprocessor & Qt Header Guards**: Guarded `QT_VERSION_CHECK` macros and included `<QObject>` for `Q_ENUM` declarations across headers.
- **Full CI/CD Matrix Restored**: Re-established complete automated testing matrix across Qt 6.5, Qt 6.8, Qt 6.10, Qt 6.11, and Qt 6.12 runners.

---

### 📝 Commit Log (0.15-04 ... 0.15-05)

- `7a471a5` fix(preprocessor): guard QT_VERSION_CHECK and include QObject for Q_ENUM across headers (@alsyundawy)
- `8de0740` fix(ci): restore complete Qt 6.5 - Qt 6.12 matrix across macOS runners (@alsyundawy)
- `764cf3f` feat: add Print, Preferences and modern Exit icon to main toolbar (v0.15-05) (@alsyundawy)
- `5598cc6` feat(ui & ci): add Check for Updates on macOS, Convert Case submenu, Paste Plain Text, and fix dark mode contrast (@alsyundawy)
- `1e894b9` fix(theme & linter): fix dark mode pasted text color and resolve all recursive linter issues across src (@alsyundawy)
- `f2648f7` fix(quality): add explicit constructors, const correctness, and modernize loops (@alsyundawy)
- `92e07c5` fix(shortcut): polish indentation and use !sequence.isEmpty() in applyCustomShortcuts (@alsyundawy)
- `75434cb` feat: implement Dark & Light mode theme engine with 75 palettes, SonarQube integration, and 13-pillar hardening (0.15-05) (@alsyundawy)

**Full Changelog**: [0.15-04...0.15-05](https://github.com/alsyundawy/NotepadNext-MacOS/compare/0.15-04...0.15-05)
