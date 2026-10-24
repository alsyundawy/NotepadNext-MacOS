# 📝 NotepadNext-MacOS Release 0.15-02

## 🇮🇩 Bahasa Indonesia

Rilis **v0.15-02** menghadirkan pembaruan besar pada penamaan berkas DMG spesifik arsitektur, mesin scripting internal dengan upgrade ke rilis resmi terbaru **Lua 5.4.9**, perbaikan bug struktural dan inisialisasi memori di seluruh subsistem [`src/`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src), modernisasi toolchain CMake, peningkatan skrip default editor macOS ([`NotepadNext-Default-Editor.sh`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/scripts/NotepadNext-Default-Editor.sh) v1.6.0), pembersihan total tautan donasi Ko-fi, serta penyempurnaan terjemahan resmi Bahasa Indonesia.

### 🌟 Sorotan Pembaruan (Changelog dari 0.15-01 ke 0.15-02)

#### 1. 📦 Penamaan Berkas DMG Mandiri Spesifik Arsitektur

- **Satu Berkas DMG per Arsitektur**: Setiap hasil build runner kini menghasilkan berkas DMG mandiri yang bersih tanpa berkas duplikat `.zip` maupun `.dmg.zip`:
  - **Apple Silicon (ARM64)**: `NotepadNext-v0.15-arm64.dmg`
  - **Intel (x64)**: `NotepadNext-v0.15-x64.dmg`
- Menghilangkan sepenuhnya berkas generik `NotepadNext-v0.15.dmg` dan file kompresi ganda.

#### 2. 🔄 Upgrade Mesin Scripting Lua ke Rilis Resmi Terbaru (Lua 5.4.9)

- **Upgrade Runtime**: Mengganti modul lama Lua 5.3.4 (rilis 2017) dengan rilis resmi **Lua 5.4.9** (rilis Agustus 2026 oleh PUC-Rio / Lua.org).
- **Remediasi Keamanan & CVE**: Menutup celah kerentanan memori lama pada modul Lua (`lparser.c`, `ldebug.c`, `lvm.c` - CVE-2025-52938 dan CVE-2025-52939).
- **Kompatibilitas Penuh**: Mengaktifkan definisi `LUA_COMPAT_5_3` serta menyediakan makro kompabilitas `LUA_QL` dan `LUA_QS` untuk menjaga backward-compatibility 100% pada API internal ([`LuaExtension.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/LuaExtension.cpp)) dan seluruh 87 lexer bahasa.
- **Pembersihan Modul Usang**: Mengeliminasi modul lama `src/lbitlib.c` (digantikan operator bitwise bawaan 64-bit Lua 5.4) dan menambahkan header modern `ljumptab.h`, `lopnames.h`.
- **Zero-Path Security Guard**: Mengunci `LUA_PATH_DEFAULT=""` dan `LUA_CPATH_DEFAULT=""` untuk mencegah eksekusi library liar dari current working directory.

#### 3. 🛡️ Audit Menyeluruh 318 File `src/` & Perbaikan Bug

- **Include Guard [`src/ScintillaEnums.h`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/ScintillaEnums.h)**: Menambahkan `#pragma once` pada header 1.600+ baris konstanta Scintilla untuk mencegah ODR / multiple definition error.
- **Inisialisasi Variabel Primitif [`src/SearchResultsCollector.h`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/SearchResultsCollector.h)**: Menginisialisasi `prevLineNumber = -1`, `prevStartPositionFromBeginning = 0`, `prevEndPositionFromBeginning = 0`, dan `child = nullptr` guna mencegah undefined behavior dari nilai sampah memori.
- **Inisialisasi Struct Primitif [`src/NppImporter.h`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/NppImporter.h) & [`src/NppImporter.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/NppImporter.cpp)**: Memberikan nilai awal default `= 0` pada seluruh field integer struct `LexerStyle`, `Language`, dan `LanguageName`.
- **Const-Correctness [`src/decorators/BookMarkDecorator.h`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/decorators/BookMarkDecorator.h)**: Menstandarkan parameter fungsi `setBookMarkedLines(const QList<int> &lines)` menggunakan const reference.
- **Internal Linkage Scope [`src/ShiftWheelToHorizontalScrollFilter.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/ShiftWheelToHorizontalScrollFilter.cpp)**: Memberikan spesifikasi `static` pada `isHorizontalWheelEvent` agar tidak terjadi tabrakan simbol global.

#### 4. ⚙️ Toolchain & Modernisasi CMake

- **Bahasa Kompilator**: Mendeklarasikan `LANGUAGES C CXX` pada root [`CMakeLists.txt`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/CMakeLists.txt) untuk memastikan deteksi awal toolchain C (Lua) dan C++20.
- **CMake Sub-projek**: Memperbarui [`thirdparty/lua/CMakeLists.txt`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/thirdparty/lua/CMakeLists.txt) ke `cmake_minimum_required(VERSION 3.21)`.
- **Versi Terpusat**: Versi rilis ditingkatkan ke `0.15-02` pada `CMakeLists.txt` dan `updates.json`.

#### 5. 🍎 Skrip Default Editor macOS v1.6.0 ([`NotepadNext-Default-Editor.sh`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/scripts/NotepadNext-Default-Editor.sh))

- **Proteksi Trap Re-entrancy**: Menambahkan `trap - EXIT INT TERM HUP` di dalam `cleanup()` untuk mencegah pemanggilan berulang saat sinyal interupsi.
- **Perbaikan System Settings**: Membatasi `lsregister` ke `-domain user` sehingga tidak merusak atau menyembunyikan panel System Settings pada macOS 13+ (Ventura, Sonoma, Sequoia).
- **Cakupan Ekstensi Maksimal**: Mendukung 471 ekstensi file dan 38 UTI di 15 kategori pengembangan agar tidak diambil alih oleh TextEdit default macOS.
- **Kepatuhan Linter Penuh**: Lolos 100% `shellcheck 0.11.0` (0 warning) dan diformat rapi menggunakan `shfmt`.

#### 6. 🌐 Pembersihan & Lokalisasi

- **Pembersihan Donasi**: Menghapus seluruh tautan dan tombol donasi Ko-fi dari seluruh repositori.
- **Penyempurnaan Bahasa Indonesia**: Menstandarkan terjemahan pada `i18n/NotepadNext_id.ts` sesuai kaidah KBBI tanpa tanda kurung berlebih.

---

## 🇬🇧 English

Release **v0.15-02** introduces major upgrades across packaging, tooling, and core codebase: standalone architecture-specific DMG releases (`NotepadNext-v0.15-arm64.dmg` & `NotepadNext-v0.15-x64.dmg`), official **Lua 5.4.9** runtime upgrade, C++20 memory & header fixes across all 318 files in [`src/`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src), root CMake toolchain modernization, production hardening of the macOS default editor script ([`NotepadNext-Default-Editor.sh`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/scripts/NotepadNext-Default-Editor.sh) v1.6.0), complete removal of legacy Ko-fi links, and pristine Indonesian localization passing all static analysis tools.

### 🚀 Changelog from 0.15-01 to 0.15-02

#### 1. 📦 Standalone Architecture-Specific DMG Output

- **Single DMG per Architecture**: Output DMGs are clean and single per target architecture without redundant `.dmg.zip` or `.zip` files:
  - **Apple Silicon (ARM64)**: `NotepadNext-v0.15-arm64.dmg`
  - **Intel (x64)**: `NotepadNext-v0.15-x64.dmg`
- Completely removed generic `NotepadNext-v0.15.dmg` and nested compression artifacts.

#### 2. 🔄 Embedded Lua Upgrade to Official Release (Lua 5.4.9)

- **Core Runtime**: Upgraded from Lua 5.3.4 (2017) to **Lua 5.4.9** (August 2026, PUC-Rio / Lua.org).
- **Security Hardening**: Remediated CVE-2025-52938 and CVE-2025-52939 out-of-bounds memory vulnerabilities.
- **Full Compatibility**: Enabled `LUA_COMPAT_5_3` and added `LUA_QL`/`LUA_QS` compatibility macros to ensure zero breakage for C++ bindings and all 87 language lexers.
- **Zero-Path Guards**: Configured `LUA_PATH_DEFAULT=""` and `LUA_CPATH_DEFAULT=""` to prevent dynamic library loading hijacking.

#### 3. 🛡️ Exhaustive Audit of 318 `src/` Files & Bug Fixes

- **Missing Include Guard**: Added `#pragma once` to [`src/ScintillaEnums.h`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/ScintillaEnums.h).
- **Uninitialized Member Variables**: Initialized primitive members in [`src/SearchResultsCollector.h`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/SearchResultsCollector.h).
- **Struct Primitives Initialization**: Added default `= 0` initializers to `LexerStyle`, `Language`, and `LanguageName` structs in [`src/NppImporter.h`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/NppImporter.h) & [`.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/NppImporter.cpp).
- **Const-Correctness**: Updated `setBookMarkedLines(const QList<int> &lines)` in [`src/decorators/BookMarkDecorator.h`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/decorators/BookMarkDecorator.h) & [`.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/decorators/BookMarkDecorator.cpp).
- **Internal Linkage**: Added `static` to `isHorizontalWheelEvent` in [`src/ShiftWheelToHorizontalScrollFilter.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/ShiftWheelToHorizontalScrollFilter.cpp).

#### 4. ⚙️ CMake & Toolchain Modernization

- **Languages**: Added `LANGUAGES C CXX` to root [`CMakeLists.txt`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/CMakeLists.txt).
- **Version**: Centralized version `0.15-02` across `CMakeLists.txt` and `updates.json`.

#### 5. 🍎 macOS Default Editor Script v1.6.0

- **Re-entrant Trap Defense**: Added `trap - EXIT INT TERM HUP` in `cleanup()`.
- **System Settings Safeguard**: Scoped `lsregister` cache purging strictly to `-domain user`.
- **471 File Extensions & 38 UTIs**: Full takeover against TextEdit across 15 development categories.
- **Strict Quality**: 100% compliant with `shellcheck 0.11.0` and `shfmt`.

#### 6. 🌐 Clean Up & Localization

- **Donation Purge**: Removed all Ko-fi donation badges and links across the entire repository.
- **Refined Indonesian Translation**: Standardized [`i18n/NotepadNext_id.ts`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/i18n/NotepadNext_id.ts) phrasing according to formal KBBI standards without extraneous parentheses.

---

### 📝 Commit Log (0.15-01 ... 0.15-02)

- `26876ee` fix(ci): sanitize git push target and bot identity in lupdate.yml (@alsyundawy)
- `c2a7475` Update image links in README.md (@༺ Initial H ༻)
- `5b19fc6` fix(release): upload canonical DMG per-arch using Qt 6.8 build (@alsyundawy)
- `4e36eb9` fix(ci): sanitize release automation, isolate CI triggers, and add .env.example (@alsyundawy)
- `0524a8d` fix(ci): harden workflows against token leakages, injection, and linter warnings (@alsyundawy)
- `ae856fa` fix(ci): output only single DMG file per architecture without extra zip archives (@alsyundawy)
- `52ab16b` feat(ci): package architecture-specific DMGs inside ZIP archives (NotepadNext-v0.15-arm64.dmg / NotepadNext-v0.15-x64.dmg) (@alsyundawy)
- `93bb403` fix(src): initialize struct primitives and use const reference in BookMarkDecorator (@alsyundawy)
- `bcbea70` fix(scintilla): add missing #pragma once include guard to ScintillaEnums.h (@alsyundawy)
- `f78e704` refactor(scroll): add static linkage to isHorizontalWheelEvent (@alsyundawy)
- `6397556` fix(lua): define LUA_QL/LUA_QS compatibility macros and fix LuaExtension build on Lua 5.4 (@alsyundawy)
- `a6e5d58` feat(lua): upgrade embedded Lua to latest 5.4.9 release and bump version to 0.15-02 (@alsyundawy)

**Full Changelog**: [0.15-01...0.15-02](https://github.com/alsyundawy/NotepadNext-MacOS/compare/0.15-01...0.15-02)
