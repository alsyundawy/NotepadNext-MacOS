# 📝 NotepadNext-MacOS Release 0.15-06

## 🇮🇩 Bahasa Indonesia

Rilis **v0.15-06** merupakan pembaruan produksi unggulan yang membawa arsitektur tipografi dan emoji modern **Unicode 18.0** dengan navigasi atomic grapheme cluster via `QTextBoundaryFinder`, integrasi pintasan pemilih emoji native macOS Character Palette (`Cmd+Ctrl+Space`), porting 6 PR bernilai tinggi dari upstream (IME Korean Hangul, dock layout safety, regex non-hanging loop & backreferences `\0`..`\9`, debounced file watcher, native XML Pretty Print, in-selection search), resiliensi penyimpanan berkas transaksional atomik `QSaveFile`, perkakas manipulasi JSON & konversi indentasi, harmonisasi 87 lexer Lexilla, pembaruan antarmuka `UpdateDialog` modern, dan audit mutu kode SonarQube.

### 🌟 Sorotan Pembaruan (Changelog dari 0.15-05 ke 0.15-06)

#### 1. 😃 Arsitektur Unicode 18.0 Emoji & Tipografi macOS

- **Universal UTF-8 Encoding**: Memastikan pengaturan `setCodePage(SC_CP_UTF8)` aktif di seluruh inisialisasi buffer Scintilla.
- **Cascade Font Apple Color Emoji**: Menghubungkan font `Apple Color Emoji` ke font cascade Scintilla dengan optimasi perataan subpixel LCD (`SC_EFF_QUALITY_LCD_OPTIMIZED`) untuk ketajaman glif warna pada layar Retina dan ProMotion.
- **Navigasi Atomic Grapheme Cluster**: Mengintegrasikan `QTextBoundaryFinder(QTextBoundaryFinder::Grapheme)` pada navigasi kursor dan operasi backspace/delete, sehingga kursor tidak memotong karakter emoji majemuk (seperti keluarga ZWJ 👨‍👩‍👧‍👦, variasi warna kulit 🧑🏽‍💻, atau bendera negara).
- **macOS Character Palette Native (`Cmd+Ctrl+Space`)**: Menambahkan aksi `actionEmojiSymbols` yang langsung memanggil panel karakter AppKit bawaan macOS `[NSApp orderFrontCharacterPalette:nil]`.

#### 2. 🚀 Porting PR Upstream Bernilai Tinggi

- **PR #1166 (IME Composition)**: Memperbaiki penanganan input CJK dan Bahasa Korea (Hangul) di macOS pada `ScintillaEditBase::inputMethodEvent` dengan melacak panjang preedit string, mengabaikan synthetic backspace, dan mengalirkan teks secara bersih tanpa duplikasi suku kata.
- **PR #1153 (Dock Restoration Safety)**: Memeriksa `currentDockObjectNames()` sebelum memanggil `restoreWindowState()` guna mencegah crash SIGSEGV saat layout dock berbeda antarsi rilis, serta menginisialisasi geometri caret secara defensif (`setCurrentPos(0); setAnchor(0); chooseCaretX();`).
- **PR #1165 & #1154 (Regex Engine Resilience)**: Mengeliminasi infinite loop pada `Finder::replaceAll()` ketika pola regex mencocokkan string panjang 0, serta menulis ulang `QRegexSearch::SubstituteByPosition()` untuk mendukung ekspansi grup penangkap `\0` hingga `\9` dan karakter escape (`\n`, `\r`, `\t`, `\\`).
- **PR #1122 (Real-Time File Watcher)**: Menambahkan `QFileSystemWatcher` dengan mekanisme debounce 250ms pada `EditorManager`, memicu sinyal `editorFileChangedOnDisk` dengan dialog prompt konfirmasi saat berkas diperbarui di disk.
- **PR #1096 (XML Formatter)**: Menyediakan format rapi XML (_Pretty Print_) native tanpa library eksternal menggunakan `QXmlStreamReader` dan `QXmlStreamWriter` dengan indentasi yang dapat dikonfigurasi.
- **PR #1045 (In-Selection Search & Replace)**: Menambahkan opsi _In Selection_ (`checkBoxInSelection`) pada `FindReplaceDialog` untuk membatasi pencarian dan penggantian teks hanya di dalam area seleksi.

#### 3. 🍎 Resiliensi macOS & Perbaikan Isu Utama

- **Pintasan Keyboard Internasional (Issues #814 & #667)**: Menghapus pintasan tunggal `Alt+1`..`Alt+0` pada macOS di `MainWindow::applyCustomShortcuts()` sehingga tombol Option dapat digunakan secara normal untuk mengetik karakter khusus internasional (`#`, `€`, `[`, `]`, `~`, `@`).
- **Penyimpanan Berkas Bebas Crash (Issue #650)**: Mengganti overwrite berkas mentah dengan penyimpanan transaksional atomik menggunakan `QSaveFile::commit()` pada [`src/ScintillaNext.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/ScintillaNext.cpp), mencegah berkas 0-byte saat listrik padam atau crash sistem.
- **Perkakas Format & Minify JSON (Issue #1140)**: Menyediakan fungsi native _Format JSON_ dan _Minify JSON_ berbasis `QJsonDocument`.
- **Konversi Indentasi Dinamis (Issues #668 & #844)**: Menambahkan perkakas konversi _Tabs to Spaces_ dan _Spaces to Tabs_ yang menghormati lebar tab editor.

#### 4. 📚 Harmonisasi 87 Lexer Bahasa Lexilla & Fallback Resolver

- **Sinkronisasi ID Lexer Resmi**: Mengaudit seluruh 87 konfigurasi bahasa di `src/languages/*.lua` dan menyelaraskan namanya dengan katalog resmi Lexilla (`au3`, `baan`, `COBOL`, `f77`, `ps`, `lisp`, `cpp`, `hypertext`, `null`).
- **Defensive Fallback Resolver**: Menambahkan pemetaan alias pada `NotepadNextApplication::setEditorLanguage` guna menjamin syntax highlighting tetap aktif meskipun terdapat konfigurasi lama atau kustom.

#### 5. 🔄 Desain Ulang UpdateDialog & Pembersihan SonarQube

- **Modern Adaptive `UpdateDialog`**: Merancang ulang dialog informasi pembaruan dengan badge status rilis, log perubahan ringkas, tombol unduh terintegrasi, dan styling visual yang selaras dengan tema aplikasi.
- **Penyelesaian SonarQube Code Smells**: Mengeliminasi code smell pada `MainWindow.cpp` dan `ActionUtils.cpp`, memperbaiki shadowing variabel pada loop C++20, serta memastikan kepatuhan 100% pada 13-pilar audit mutu kode.

---

## 🇬🇧 English

Release **v0.15-06** is a flagship production release introducing **Unicode 18.0 emoji and typography architecture** with atomic grapheme cluster navigation via `QTextBoundaryFinder`, native macOS Character Palette shortcut integration (`Cmd+Ctrl+Space`), selective backporting of 6 high-value upstream PRs (Korean Hangul IME, dock restoration safety, non-hanging regex loop & backreferences `\0`..`\9`, debounced file watcher, dependency-free XML Pretty Print, in-selection search), crash-safe atomic file transactions via `QSaveFile`, built-in JSON and indentation formatters, complete harmonization across all 87 Lexilla lexers, a modern redesigned `UpdateDialog`, and SonarQube code quality cleanup.

### 🚀 Changelog from 0.15-05 to 0.15-06

#### 1. 😃 Unicode 18.0 Emoji & Typography Architecture

- **Universal UTF-8 Encoding**: Enforced `setCodePage(SC_CP_UTF8)` across Scintilla buffer initializations and file reloads.
- **Apple Color Emoji Font Cascade**: Linked `Apple Color Emoji` into Scintilla's font cascade with subpixel LCD smoothing (`SC_EFF_QUALITY_LCD_OPTIMIZED`) for crisp glyph rendering on Retina and ProMotion displays.
- **Atomic Grapheme Cluster Navigation**: Integrated `QTextBoundaryFinder(QTextBoundaryFinder::Grapheme)` into cursor caret and backspace/delete handlers, ensuring cursor steps never bisect compound emojis (such as ZWJ families 👨‍👩‍👧‍👦, skin tones 🧑🏽‍💻, or flag sequences).
- **Native macOS Character Palette (`Cmd+Ctrl+Space`)**: Bound `actionEmojiSymbols` directly to Cocoa AppKit `[NSApp orderFrontCharacterPalette:nil]` for instant system emoji and symbol access.

#### 2. 🚀 High-Value Upstream PR Backports

- **PR #1166 (IME Composition)**: Refactored `ScintillaEditBase::inputMethodEvent` on macOS to track preedit string lengths, discard synthetic backspace events during composition, and commit text cleanly without syllable duplication in Korean Hangul, Japanese Kanji, or Chinese.
- **PR #1153 (Dock Restoration Safety)**: Added defensive `currentDockObjectNames()` guard prior to `restoreWindowState()` to prevent SIGSEGV crashes when dock configurations differ across versions, along with safe caret initialization (`setCurrentPos(0); setAnchor(0); chooseCaretX();`).
- **PR #1165 & #1154 (Regex Engine Resilience)**: Eliminated infinite loops in `Finder::replaceAll()` when regex patterns match 0 characters, and rewrote `QRegexSearch::SubstituteByPosition()` to expand capture groups `\0` through `\9` alongside escape sequences (`\n`, `\r`, `\t`, `\\`).
- **PR #1122 (Real-Time File Watcher)**: Added `QFileSystemWatcher` with a 250ms debounce in `EditorManager` emitting `editorFileChangedOnDisk` with prompt dialogs when files change on disk.
- **PR #1096 (XML Formatter)**: Implemented native, dependency-free XML Pretty Print formatting via `QXmlStreamReader` and `QXmlStreamWriter` with configurable indentation.
- **PR #1045 (In-Selection Search & Replace)**: Added _In Selection_ option (`checkBoxInSelection`) in `FindReplaceDialog` to restrict search and replace operations strictly within the active selection bounds.

#### 3. 🍎 macOS Hardware & OS Resilience

- **International Option Key Shortcuts (Issues #814 & #667)**: Scoped single `Alt+1`..`Alt+0` shortcuts away on macOS in `MainWindow::applyCustomShortcuts()`, allowing European and international keyboards to type special characters (`#`, `€`, `[`, `]`, `~`, `@`) via Option without menu triggering.
- **Crash-Safe Atomic File Save (Issue #650)**: Replaced raw file overwriting with atomic transactional `QSaveFile::commit()` in [`src/ScintillaNext.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/ScintillaNext.cpp), eliminating 0-byte file corruption during unexpected crashes or power loss.
- **JSON Formatting Tools (Issue #1140)**: Built-in high-performance _Format JSON_ and _Minify JSON_ actions powered by `QJsonDocument`.
- **Indentation Conversions (Issues #668 & #844)**: Added dynamic _Convert Tabs to Spaces_ and _Spaces to Tabs_ tools respecting editor tab widths.

#### 4. 📚 Lexilla 87 Lexer Name Harmonization & Fallback Resolver

- **Aligned Official Lexer IDs**: Audited all 87 language definitions in `src/languages/*.lua` to align identifiers with Lexilla's official catalogue (`au3`, `baan`, `COBOL`, `f77`, `ps`, `lisp`, `cpp`, `hypertext`, `null`).
- **Defensive Fallback Resolver**: Added alias resolution in `NotepadNextApplication::setEditorLanguage` ensuring syntax highlighting remains active for any legacy or custom definitions.

#### 5. 🔄 Redesigned UpdateDialog & SonarQube Clean Code

- **Modern Adaptive `UpdateDialog`**: Overhauled release update dialog with visual release status badges, concise markdown changelog rendering, direct download actions, and theme-harmonized styling.
- **SonarQube Code Smell Remediation**: Cleaned code smells in `MainWindow.cpp` and `ActionUtils.cpp`, fixed C++20 loop variable shadowing, and achieved 100% clean status across 13-pillar audits.

---

### 📝 Commit Log (0.15-05 ... 0.15-06)

- `5b9b40b` fix(quality): resolve SonarQube code smells in MainWindow and ActionUtils (@alsyundawy)
- `5ce6d20` fix(build): resolve undeclared identifier 'i' in applyCaseTransform and actionlint warning in release.yml (@alsyundawy)
- `0475626` feat(release): redesign modern update dialog, streamline i18n to id and en, and fix Apple Silicon build pipeline for 0.15-06 (@alsyundawy)
- `2d8e6b2` fix(quality): resolve C++20 ranges, shadowing, and restore QObject in ScintillaEnums (@alsyundawy)
- `91121e8` fix(code-quality): resolve static analysis diagnostics, shadowing, and offline Qt shims (@alsyundawy)
- `3ab9b9f` chore(scripts): bump APP_VERSION to 0.15-06 in compilation database generator (@alsyundawy)
- `e38082e` feat(release): release version 0.15-06 with Unicode 18.0 emoji, Lexilla harmonization, upstream PRs, and 13-pillar audit (@alsyundawy)

**Full Changelog**: [0.15-05...0.15-06](https://github.com/alsyundawy/NotepadNext-MacOS/compare/0.15-05...0.15-06)
