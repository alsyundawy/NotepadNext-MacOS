# 📝 NotepadNext-MacOS Release 0.15-01

## 🇮🇩 Bahasa Indonesia

Rilis **v0.15-01** menghadirkan pembaruan besar yang menggabungkan peningkatan terbaru dari upstream NotepadNext v0.15, penyempurnaan menyeluruh pelokalan Bahasa Indonesia (standar baku KBBI & terminologi teknologi modern), perbaikan kritis pada skrip default editor ([`NotepadNext-Default-Editor.sh`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/scripts/NotepadNext-Default-Editor.sh) v1.6.0) yang kini aman untuk ExtensionKit macOS modern (Ventura, Sonoma, Sequoia) tanpa merusak panel System Settings, ekspansi asosiasi ke 470+ format file dan 38 UTIs, serta sinkronisasi metadata pembaruan.

### 🌟 Sorotan Pembaruan (Changelog dari 0.14-07 ke 0.15-01)

#### 1. 🔄 Sinkronisasi Penuh Upstream NotepadNext v0.15

- **Dukungan Encoding UTF-16**: Menambahkan dukungan penuh untuk format teks UTF-16 Big Endian (`UTF-16 BE`) dan UTF-16 Little Endian (`UTF-16 LE`).
- **Pembaruan Advanced Docking System (ADS)**: Mengintegrasikan ADS v5.1.1 untuk stabilitas panel, docking tabs, dan performa UI yang lebih responsif.
- **Navigasi Teks Khusus macOS**: Menetapkan pintasan navigasi teks native macOS yang lebih alami bagi pengguna Mac.
- **Perbaikan Stabilitas & Crash**: Memperbaiki masalah crash saat startup ketika berkas sesi (*session file*) rusak atau tidak terbaca.
- **Dukungan C++20 & Kompatibilitas POSIX**: Memperbaiki peringatan capture implicit `this` pada lambda C++20 serta menambahkan guard `#ifndef` untuk variabel environment Lua di POSIX.

#### 2. 🌐 Pelokalan Menyeluruh Bahasa Indonesia (Standar Baku KBBI)

- **Penyempurnaan `i18n/NotepadNext_id.ts`**: Menyelaraskan lebih dari 2.200 baris string XML ke dalam Bahasa Indonesia yang baik, benar, dan profesional.
- **Eliminasi Tanda Kurung Berlebih**: Menghapus teks terjemahan kaku seperti `(Wrap)`, `(EoL)`, `(Undo)`, `(Redo)` dan menggantinya dengan istilah baku yang elegan (*Bungkus Baris*, *Batal*, *Ulangi*, *Akhir Baris*).
- **Kelengkapan Numerus & Dialog**: Menyelesaikan seluruh entri numerus jamak/tunggal serta dialog pencarian, penanda (*bookmarks*), makro, dan konfigurasi bahasa pemrograman.

#### 3. 🍎 Optimalisasi Skrip Default Editor v1.6.0 ([`NotepadNext-Default-Editor.sh`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/scripts/NotepadNext-Default-Editor.sh))

- **Perbaikan Kritis System Settings macOS Modern**: Membatasi pembersihan cache LaunchServices hanya pada `domain user` (`lsregister -kill -r -domain user`), sehingga ExtensionKit (`/System/Library/ExtensionKit/Extensions/*.appex`) pada macOS 13+ (Ventura, Sonoma, Sequoia) tetap aman dan tidak menyebabkan panel System Settings hilang.
- **Auto-Healing Tanpa Reboot**: Skrip secara otomatis memindai dan meregistrasi ulang *ExtensionKit* serta *System Settings.app*, memulihkan panel yang sempat hilang akibat skrip lama secara instan tanpa perlu me-reboot macOS.
- **Ekspansi Cakupan 470+ Ekstensi & 38 UTIs**: Mendukung 470+ format developer lintas 15 kategori (teks, markdown/MDX, konfigurasi, skrip shell/PowerShell, diagram PlantUML/Mermaid/DOT, CAD/CNC/G-code, smart contracts Solidity/Cairo/Move, kunci kriptografi/sertifikat SSL, log crash report macOS `.ips`).
- **Pembersihan Desktop Anggun**: Menghilangkan penghentian paksa `SystemUIServer` yang sebelumnya membuat menu bar berkedip, digantikan dengan penutupan anggun aplikasi System Settings jika sedang aktif.

#### 4. 🛡️ Pembersihan Donasi & Harmonisasi Lisensi

- **Pembersihan Donasi**: Menghapus seluruh tautan dan tombol donasi Ko-fi dari seluruh repositori.
- **Hak Cipta Builder Terpelihara**: Mempertahankan informasi lisensi `Copyright 2026 HARRY DS ALSYUNDAWY (MacOS-Builder)` pada dialog About dan CMake konfigurasi.
- **Sinkronisasi Metadata Rilis**: Memperbarui berkas [`updates.json`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/updates.json) ke versi `0.15-01`.

---

## 🇬🇧 English

Release **v0.15-01** brings a major milestone combining upstream NotepadNext v0.15 advancements, a complete overhaul of the Indonesian localization with pristine native phrasing, critical stability and safety fixes in the macOS default editor script ([`NotepadNext-Default-Editor.sh`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/scripts/NotepadNext-Default-Editor.sh) v1.6.0) ensuring zero ExtensionKit corruption on modern macOS (Ventura, Sonoma, Sequoia), massive expansion to 470+ file extensions and 38 UTIs, and updated release metadata.

### 🚀 Changelog from 0.14-07 to 0.15-01

#### 1. 🔄 Full Upstream NotepadNext v0.15 Synchronization

- **UTF-16 Encoding Support**: Full support for UTF-16 Big Endian (`UTF-16 BE`) and UTF-16 Little Endian (`UTF-16 LE`) text streams.
- **Advanced Docking System (ADS) Upgrade**: Integrated ADS v5.1.1 for enhanced dock panel stability, responsive tabs, and silky UI layout transitions.
- **macOS Text Navigation Shortcuts**: Assigned native macOS text editing shortcuts for seamless navigation.
- **Startup Crash Resolution**: Resolved startup SIGSEGV crashes caused by unreadable or corrupted session files.
- **C++20 Compliance & POSIX Lua**: Fixed deprecated implicit `this` capture in C++20 lambdas and guarded Lua default environment paths with `#ifndef` on POSIX.

#### 2. 🌐 Comprehensive Indonesian Localization (Formal KBBI Standard)

- **Refined `i18n/NotepadNext_id.ts`**: Aligned over 2,200 XML string lines with professional, natural Indonesian phrasing.
- **Stripped Redundant Parentheses**: Replaced stiff parenthesized labels like `(Wrap)`, `(EoL)`, `(Undo)`, `(Redo)` with natural Indonesian equivalents (*Bungkus Baris*, *Batal*, *Ulangi*, *Akhir Baris*).
- **Pluralization & Complete Dialogs**: Fully translated pluralization forms, find/replace dialogs, bookmarks, macros, and syntax language configurations.

#### 3. 🍎 macOS Default Editor Script v1.6.0 Optimization

- **Modern macOS System Settings Safeguard**: Restricted LaunchServices database rebuilding strictly to user domain (`lsregister -kill -r -domain user`), protecting ExtensionKit (`/System/Library/ExtensionKit/Extensions/*.appex`) on macOS 13+ (Ventura, Sonoma, Sequoia) and eliminating vanished settings panes.
- **Zero-Reboot Auto-Healing**: Automatically scans and re-registers damaged ExtensionKit bundles and System Settings, recovering missing panes instantly without restarting.
- **Expanded 470+ Extensions & 38 UTIs**: Extended association takeover against TextEdit across 15 development categories (source code, markdown/MDX, shell scripts, CAD/CNC, smart contracts, cryptographic certificates, macOS `.ips` crash logs).
- **Graceful Desktop Cleanup**: Eliminated jarring `killall SystemUIServer` desktop flashes in favor of clean application quits.

#### 4. 🛡️ Donation Link Purge & Repository Polish

- **Ko-fi Purge**: Completely removed all Ko-fi donation badges and links across repository documentation and UI dialogs.
- **Builder License Integrity**: Preserved `Copyright 2026 HARRY DS ALSYUNDAWY (MacOS-Builder)` licensing across About metadata and CMake configurations.
- **Metadata Synchronization**: Centralized release version to `0.15-01` in [`updates.json`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/updates.json).

---

### 📝 Commit Log (0.14-07 ... 0.15-01)

- `2a920f4` docs: remove all Ko-fi donation links from repository (@alsyundawy)
- `6ac3714` docs: harmonize Overview with 470+ supported file formats (@alsyundawy)
- `af8dfa7` feat(scripts): expand comprehensive file formats to 470+ extensions (v1.6.0) (@alsyundawy)
- `65d2388` fix(scripts): resolve System Settings pane disappearance on modern macOS (v1.6.0) (@alsyundawy)
- `9e63d23` i18n: sempurnakan dan optimalkan terjemahan Bahasa Indonesia (id_ID) (@alsyundawy)
- `4536a63` Merge remote-tracking branch 'origin/master' into master (@alsyundawy)
- `7e0d2a2` Merge upstream changes from dail8859/NotepadNext (v0.15) into master (@alsyundawy)
- `ba2eccc` Merge pull request #37 from alsyundawy/dependabot/github_actions/jurplel/install-qt-action-4.4.1 (@༺ Initial H ༻)
- `ad828e3` Merge pull request #36 from alsyundawy/dependabot/github_actions/github/codeql-action/init-4.38.1 (@༺ Initial H ༻)
- `b45aead` Merge pull request #35 from alsyundawy/dependabot/github_actions/github/codeql-action/analyze-4.38.1 (@༺ Initial H ༻)
- `6cfb2cc` Update updates.json (@dail8859)
- `377999a` Bump jurplel/install-qt-action from 4.3.1 to 4.4.1 (@dependabot[bot])
- `75842ae` Update create_release.yml (@dail8859)
- `fc4c807` v0.15 release (@GitHub Action)
- `3c86324` Fix release workflow checkout auth failure by removing brittle custom token override (#1141) (@Copilot)
- `76bf636` New Crowdin updates (#1129) (@dail8859)
- `251bca3` Bump github/codeql-action/init from 4.37.9 to 4.38.1 (@dependabot[bot])
- `239d28a` Bump github/codeql-action/analyze from 4.37.9 to 4.38.1 (@dependabot[bot])
- `42d5bf9` Update translation files (@dail8859)
- `a74152a` Add UTF16BE and UTF16LE support (#1139) (@dail8859)
- `e4dc456` Update ADS to v5.1.1 (#1138) (@dail8859)
- `50cd3ec` Add Windows ARM64 support (#1130) (@dail8859)
- `6eb01b5` Improve MacOS build (#1119) (@dail8859)
- `58a6c85` New Crowdin updates (#1111) (@dail8859)
- `e5b7c00` Fix deprecated implicit `this` capture in `[=]` lambdas (C++20) (#1121) (@Physiologie des Cellules Cardiaques et Vasculaires - Université de Tours)
- `99ca772` Fix spelling error in zoom out comment (#1124) (@hemanpa)
- `e2e17cd` Assign macOS text navigation key bindings (#1125) (@hemanpa)
- `1014c2a` Fix crash on startup when a session file cannot be read (#1123) (@hemanpa)
- `dd5d221` Merge pull request #31 from alsyundawy/dependabot/github_actions/oxsecurity/megalinter-10.1.0 (@༺ Initial H ༻)
- `5336a5c` Merge pull request #30 from alsyundawy/dependabot/github_actions/jdpurcell/install-qt-action-d715fb7cdd4e5661191a794764231398e2c952f2 (@༺ Initial H ༻)
- `4f066a2` Merge pull request #29 from alsyundawy/dependabot/github_actions/actions/deploy-pages-5.0.1 (@༺ Initial H ༻)
- `01780f9` Merge pull request #28 from alsyundawy/dependabot/github_actions/github/codeql-action/analyze-4.37.9 (@༺ Initial H ༻)
- `bb86b39` Merge pull request #27 from alsyundawy/dependabot/github_actions/github/codeql-action/init-4.37.9 (@༺ Initial H ༻)
- `66af615` Bump oxsecurity/megalinter from 10.0.0 to 10.1.0 (@dependabot[bot])
- `d96b555` Bump jdpurcell/install-qt-action (@dependabot[bot])
- `5e62753` Bump actions/deploy-pages from 5.0.0 to 5.0.1 (@dependabot[bot])
- `e667163` Bump github/codeql-action/analyze from 4.37.8 to 4.37.9 (@dependabot[bot])
- `26f6380` Bump github/codeql-action/init from 4.37.8 to 4.37.9 (@dependabot[bot])
- `e69fd77` Update translation files (@alsyundawy)
- `f57db52` Update translation files (@dail8859)
- `355b1db` Guard LUA_PATH_DEFAULT/LUA_CPATH_DEFAULT with #ifndef on POSIX (#1117) (@Physiologie des Cellules Cardiaques et Vasculaires - Université de Tours)
- `199f896` Merge pull request #26 from alsyundawy/dependabot/github_actions/actions/labeler-7.0.0 (@༺ Initial H ༻)
- `9f446b6` Bump actions/labeler from 5.0.0 to 7.0.0 (@dependabot[bot])

**Full Changelog**: [0.14-07...0.15-01](https://github.com/alsyundawy/NotepadNext-MacOS/compare/0.14-07...0.15-01)
