# 📝 NotepadNext-MacOS Release 0.15-04

## 🇮🇩 Bahasa Indonesia

Rilis **v0.15-04** merupakan pembaruan arsitektural besar yang memodernisasi komponen inti editor (Scintilla 5.6.7, Lexilla 5.5.4, Lua 5.5.1), menambahkan mesin pendeteksi karakter encoding cerdas (`EncodingDetector` dengan `uchardet`), dukungan multi-jendela (_multi-instance New Window_), perancangan ulang dialog sesuai Apple Human Interface Guidelines (`AboutDialog` dan `SaveModifiedDialog`), penyelarasan Bundle ID ke `com.github.alsyundawy.NotepadNext`, set ikon Retina Squircle baru, serta audit mutu kode 13-pilar (MegaLinter & Trunk).

### 🌟 Sorotan Pembaruan (Changelog dari 0.15-03 ke 0.15-04)

#### 1. 🚀 Pembaruan Mesin Inti Editor: Scintilla 5.6.7, Lexilla 5.5.4 & Lua 5.5.1

- **Scintilla 5.6.7**: Meningkatkan kualitas rendering teks sub-pixel pada layar Retina macOS, multi-cursor selection yang lebih presisi, dan peningkatan responsivitas scrollbar elastis bawaan macOS.
- **Lexilla 5.5.4**: Pembaruan tokenizer modern untuk C++20 (`concept`, `requires`, `consteval`), folding blok YAML multi-baris, dan entitas HTML5.
- **Lua 5.5.1**: Peningkatan runtime scripting dengan manajemen C-closure yang lebih efisien, garbage collection lebih cepat, serta makro kompatibilitas `LUA_QL` dan `LUA_QS`.
- **Integrasi Python 3.13+**: Deteksi otomatis interpreter Python modern pada [`thirdparty/CMakeLists.txt`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/thirdparty/CMakeLists.txt) untuk eksekusi skrip background.

#### 2. 🔤 Mesin Deteksi & Konversi Encoding Komprehensif (`EncodingDetector`)

- **Deteksi Cerdas Bertingkat**: Mengintegrasikan library `uchardet` untuk mendeteksi charset berkas secara otomatis (UTF-8, UTF-16, ISO-8859, Windows-1252, Shift-JIS, GB18030, dll.).
- **Menu Encoding Baru**: Menyediakan opsi _Reopen with Encoding_ dan _Convert to Encoding_ langsung dari antarmuka editor.
- **Status Bar Interaktif**: Menampilkan label encoding aktif pada status bar yang dapat diklik untuk mengganti encoding secara instan.

#### 3. 🪟 Dukungan Multi-Instance & Jendela Mandiri Baru (`Ctrl+Shift+N` / `open -n`)

- **Jendela Terpisah Mandiri**: Menambahkan aksi _New Window_ (`Ctrl+Shift+N` / `open -n`) yang meluncurkan instance proses terpisah untuk multitasking dokumen tanpa bercampur dalam satu tab bar.
- **Isolasi State & Berbagi Preferensi**: Setiap instance jendela berjalan independen dengan konfigurasi preferensi yang tersinkronisasi.

#### 4. 🎨 Desain Ulang Dialog Antarmuka Apple HIG (`AboutDialog` & `SaveModifiedDialog`)

- **Modern Adaptive `AboutDialog`**: Dialog Tentang modern 6 tab yang beradaptasi secara dinamis dengan palet sistem (Light/Dark mode) macOS, dilengkapi diagnostik sistem lengkap dan proteksi reentrancy.
- **Modern `SaveModifiedDialog`**: Dialog konfirmasi simpan perubahan yang dirancang ulang dengan kartu detail dokumen, peringatan data loss visual, dan shortcut keyboard native macOS `Cmd+D` (_Don't Save_).

#### 5. 🏷️ Penyelarasan Bundle Identifier & Ikon Apple HIG Squircle

- **Standardisasi Bundle ID**: Menyelaraskan ID aplikasi menjadi **`com.github.alsyundawy.NotepadNext`** di seluruh `Info.plist`, AppStream Linux, dan LaunchServices macOS.
- **Paket Ikon Squircle Retina**: Menghadirkan set ikon baru bergaya macOS Squircle tajam dari resolusi 16px hingga 512px (ICNS, PNG, dan SVG).

#### 6. 🛡️ Hardening Keamanan, Audit 13-Pilar & CI Runner macOS 15+

- **Remediasi CWE-120 (Buffer Overflow)**: Mengganti fungsi string raw tak terbatas dengan `std::snprintf` pada [`src/IFaceTable.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/IFaceTable.cpp).
- **Keamanan Pointer**: Inisialisasi eksplisit seluruh pointer ke `nullptr` guna mencegah dangling pointer dereference.
- **Resiliensi QTBUG-111538**: Null guard defensif pada `SearchResultsDock` mencegah crash SIGSEGV saat pemulihan tata letak dock.
- **Perbaikan Signal Disconnect**: Memperbaiki pemutusan sinyal pada `QuickFindWidget` agar tidak terjadi memory leak callback.
- **Validasi Rename Berkas**: Menangani kegagalan rename berkas dengan pesan error diagnostik `showSaveErrorMessage()` alih-alih mengabaikannya.
- **Infrastruktur CI/CD**: Menambahkan runner macOS 15+ (`macos-15` ARM64 & `macos-15-intel` x64) dengan Qt 6.12 dan integrasi MegaLinter + Trunk.

---

## 🇬🇧 English

Release **v0.15-04** is a major architectural milestone delivering modernized core editing engines (Scintilla 5.6.7, Lexilla 5.5.4, Lua 5.5.1), an intelligent character encoding detection engine (`EncodingDetector` powered by `uchardet`), native multi-instance New Window support, redesigned Apple HIG dialogs (`AboutDialog` and `SaveModifiedDialog`), bundle ID alignment to `com.github.alsyundawy.NotepadNext`, modern Retina squircle icons, and 13-pillar code hardening with MegaLinter and Trunk.

### 🚀 Changelog from 0.15-03 to 0.15-04

#### 1. 🚀 Modernized Core Engine: Scintilla 5.6.7, Lexilla 5.5.4 & Lua 5.5.1

- **Scintilla 5.6.7**: Resolves sub-pixel text rendering artifacts on macOS Retina displays, refines multi-cursor selections, and improves elastic scrollbar physics.
- **Lexilla 5.5.4**: Upgrades syntax tokenizers for modern languages, including C++20 standard keywords (`concept`, `requires`, `consteval`), multi-line YAML folding, and HTML5 entities.
- **Lua 5.5.1**: Optimized bytecode VM with enhanced garbage collection pacing and compatibility macros (`LUA_QL`, `LUA_QS`).
- **Python 3.13+ Discovery**: Automated modern Python interpreter resolution in [`thirdparty/CMakeLists.txt`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/thirdparty/CMakeLists.txt).

#### 2. 🔤 Comprehensive Character Encoding Architecture (`EncodingDetector`)

- **Multi-Stage Statistical Detection**: Integrated `uchardet` library to automatically identify document charsets (UTF-8, UTF-16, ISO-8859, Windows-1252, Shift-JIS, GB18030, etc.).
- **Dedicated Encoding Menu**: Offers _Reopen with Encoding_ and _Convert to Encoding_ directly from the menu bar.
- **Interactive Status Bar**: Clickable status bar widget displaying the active document encoding with instant switcher presets.

#### 3. 🪟 Native Multi-Instance Support (`Ctrl+Shift+N` / `open -n`)

- **Independent Windows**: Added _New Window_ (`Ctrl+Shift+N` / `open -n`) action allowing users to spawn standalone process instances for distraction-free multi-tasking.
- **State Isolation**: Each window instance runs with its own document tabs while seamlessly sharing user configuration.

#### 4. 🎨 Apple HIG Modernized Dialogs (`AboutDialog` & `SaveModifiedDialog`)

- **Adaptive `AboutDialog`**: 6-tab system-palette aware dialog with real-time light/dark theme adaptation, full diagnostic environment reporting, and reentrancy guards.
- **Refined `SaveModifiedDialog`**: Clean macOS HIG unsaved changes dialog featuring document metadata cards, visual data-loss warnings, and standard macOS `Cmd+D` (_Don't Save_) shortcut.

#### 5. 🏷️ Bundle ID Standardization & Apple HIG Squircle Icon Set

- **Unified Identity**: Aligned application bundle identifier to **`com.github.alsyundawy.NotepadNext`** across macOS `Info.plist`, Linux AppStream, and LaunchServices.
- **Retina Squircle Icons**: High-DPI icon assets crafted according to Apple Human Interface Guidelines across 16px to 512px resolutions (ICNS, PNG, and SVG).

#### 6. 🛡️ 13-Pillar Code Hardening, Security & macOS 15+ CI Pipeline

- **CWE-120 Resolution**: Bounded `std::snprintf` in [`src/IFaceTable.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/IFaceTable.cpp) eliminating buffer overflow vulnerabilities.
- **Pointer Safety**: Explicit `nullptr` initialization across all member pointers, preventing dangling pointer dereferences.
- **QTBUG-111538 Resilience**: Defensive null guard on `SearchResultsDock` eliminating crashes during dock window restoration.
- **Signal Disconnect Fix**: Corrected callback disconnection parameters in `QuickFindWidget` to prevent listener leaks.
- **Filesystem Verification**: Handled `QFile::rename()` failures with diagnostic dialogs (`showSaveErrorMessage()`) instead of discarding errors.
- **macOS 15+ CI Matrix**: Configured automated GitHub Actions runners for `macos-15` ARM64 & Intel with Qt 6.12 and MegaLinter + Trunk pipelines.

---

### 📝 Commit Log (0.15-03 ... 0.15-04)

- `1d43177` fix: resolve AboutDialog crash via reentrancy guard, redesign unsaved changes dialog with SaveModifiedDialog, and update release notes (@alsyundawy)
- `bb5d61d` feat: add New Window multi-instance support, update 0.15-04 release documentation, and sanitize attributions (@alsyundawy)
- `f53be3d` fix(ci): resolve MegaLinter yamllint conflict and tune cppcheck Qt settings (@alsyundawy)
- `a89e5aa` ci: sync MegaLinter workflow and configuration with SparkNotePad (@alsyundawy)
- `f38c7a0` feat: integrate SparkNotePad encoding architecture, modernize adaptive AboutDialog, and update 0.15-04 release documentation (@alsyundawy)
- `e2a4082` feat: align bundle ID to com.github.alsyundawy.NotepadNext, enhance logo branding, and update comparative release documentation (@alsyundawy)
- `4245052` feat: add logo icon to README, update app ID to com.github.alsyundawy.NotepadNext, and add comparative release notes v0.15-04 vs v0.15 (@alsyundawy)
- `6948192` feat(thirdparty): upgrade to Scintilla 5.6.7, Lexilla 5.5.4, Lua 5.5.1, set default EditorContextMenu, and complete 0.15-01 to 0.15-04 release notes (@alsyundawy)
- `d63aef6` feat: modernize About dialog, add Qt 6.12 runners for macOS 15+, refine Indonesian translation, and configure MegaLinter (@alsyundawy)
- `d6c41d9` chore(release): bump version to 0.15-04, modernize branding icons, and harden codebase across 13 pillars (@alsyundawy)
- `197c182` Merge pull request #46 from alsyundawy/dependabot/github_actions/github/codeql-action/analyze-4.39.0 (@alsyundawy)
- `a7fa3fe` Merge pull request #45 from alsyundawy/dependabot/github_actions/github/codeql-action/init-4.39.0 (@alsyundawy)
- `6993189` Bump github/codeql-action/analyze from 4.38.3 to 4.39.0 (@dependabot[bot])
- `0c849fc` Bump github/codeql-action/init from 4.38.3 to 4.39.0 (@dependabot[bot])

**Full Changelog**: [0.15-03...0.15-04](https://github.com/alsyundawy/NotepadNext-MacOS/compare/0.15-03...0.15-04)
