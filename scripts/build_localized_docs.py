#!/usr/bin/env python3
"""
Generate localized DOCNOTE and CHANGELOG files for NotepadNext-MacOS v0.15-08.
Produces:
  - DOCNOTE.md, DOCNOTE.txt, DOCNOTE_en.txt, DOCNOTE_id.txt
  - CHANGELOG.md, CHANGELOG.txt, CHANGELOG_en.txt, CHANGELOG_id.txt
Synchronizes across:
  - Repository root (.)
  - doc/
  - src/doc/
"""

import os
import sys

DOCNOTE_ID = """# Catatan Rilis Dokumentasi (DOCNOTE) — NotepadNext-MacOS v0.15-08 (Bahasa Indonesia)

--------------------------------------------------------------------------------
================================================================================
Edisi Aplikasi        : NotepadNext For MacOS — Notepad++ Like
Versi Rilis           : 0.15-08 (Tab Bar Plus, Pratinjau Markdown/HTML Tabbed, Suite 28 Icon Pack, Lokalisasi Bilingual en/id, Preferensi Teks Murni, Atribusi SubnetCalc-MacOS & Kompatibilitas Multi-Qt)
Tanggal Rilis         : 2026-11-08
Pengembang / Builder  : Harry Dertin Sutisna Alsyundawy (@alsyundawy)
Organisasi            : ALSYUNDAWY IT SOLUTION (https://alsyundawy.com)
Repositori Upstream   : https://github.com/dail8859/NotepadNext (Basis: v0.15)
Repositori Edisi macOS: https://github.com/alsyundawy/NotepadNext-MacOS
Proyek Donor Fitur    : SparkNotePad v1.3.8 (https://github.com/alsyundawy/SparkNotePad)
Kontributor Tema      : SubnetCalc-MacOS (https://github.com/alsyundawy/SubnetCalc-MacOS - 25 Tema Sintaksis & UI)
Kerangka Kerja GUI    : Qt 6.5 LTS / 6.8 LTS / 6.10 / 6.11 / 6.12 (C++20 ISO Standard)
Target Platform       : macOS 11.0–26.0 (Apple Silicon arm64 & Intel x86_64), Linux (x86_64 & aarch64)
Lisensi               : GNU General Public License v3.0 (GPL-3.0-or-later)
================================================================================
--------------------------------------------------------------------------------

---

## 1. Ringkasan Eksekutif (Bahasa Indonesia): Rilis v0.15-08

NotepadNext-MacOS v0.15-08 adalah rilis fitur dan stabilitas besar yang menghadirkan inovasi produktivitas terdepan, integrasi mendalam macOS, pemurnian antarmuka, dan pengerasan arsitektur:

1. Tombol Tambah Tab (+) & Ergonomi Tab Modern:
   - Menambahkan tombol '+' persisten pada tab bar editor (DockedEditorTitleBar) dengan estetika native macOS dan status interaksi hover/pressed.
   - Pembuatan tab dokumen baru secara instan melalui satu klik atau pintasan standar (Cmd+N).

2. Pratinjau Tab Interaktif Markdown & HTML (PreviewTabWidget):
   - Merender dokumen Markdown dan HTML di dalam tab editor dengan kotak info GitHub ([!NOTE], [!TIP], [!IMPORTANT], [!WARNING], [!CAUTION]), tipografi devdocs, metrik jumlah kata dan estimasi waktu baca, serta pembesaran dinamis.
   - Menyediakan menu konteks klik-kanan pada tab editor berkas .md dan .html untuk meluncurkan pratinjau tab atau membuka langsung di peramban sistem (openInDefaultBrowser).

3. Konfigurasi 6 Tingkat Ukuran Ikon & Seleksi Icon Pack pada Menu Themes:
   - Memperluas menu Themes dengan submenu pemilihan paket ikon toolbar dan 6 tingkat ukuran ikon (16x16, 20x20, 24x24, 32x32, 48x48, 64x64) dengan tanda centang aktif dan pembaruan visual seketika.

4. Dokumentasi Terintegrasi pada Menu Help (DocNote & Changelog):
   - Aksi DocNote dan Changelog pada menu Help secara otomatis mencari dan membuka berkas DOCNOTE.md dan CHANGELOG.md langsung di dalam tab editor Scintilla.

5. Tab Terdedikasi "Themes & Toolbar" pada Dialog Preferensi:
   - Dialog preferensi ditata ulang dengan tab berbasis teks murni tanpa ikon atau emoji.
   - Memindahkan pengaturan tema, toggle mode gelap/terang, opasitas jendela, pemilihan icon pack, dan ukuran ikon ke dalam tab khusus "Themes & Toolbar" lengkap dengan baris pratinjau ikon langsung yang terang dan jelas.

6. Installer DMG Bergaya Native macOS Resolusi Tinggi:
   - Pipeline packaging kustom (generate_dmg_background.py & package-dmg.sh) menghasilkan kanvas DMG 680x440 resolusi tinggi (standard & Retina @2x), slot aplikasi NotepadNext di kiri, symlink /Applications di kanan, indikator drag-and-drop melengkung, ikon volume, dan verifikasi checksum teruji (hdiutil verify).

7. Suite Personalisasi 28 Icon Pack Terpilih (ToolbarIconManager):
   - Mengintegrasikan koleksi terkurasi 28 paket ikon beresolusi tinggi dan SVG vektor tanpa buram (Modern Ribbon SVG, Fluent, Notepad++ Klasik, ragam Aksara, Retro Windows 7/Aero, dan skeuomorfik WinXP).
   - Menghapus 10 paket ikon redundan untuk menjaga kerapian dan efisiensi bundle aplikasi.

8. Pengakuan Atribusi Proyek Donor & Kontributor Tema:
   - Menyertakan pengakuan atribusi terperinci dan URL resmi untuk SparkNotePad (https://github.com/alsyundawy/SparkNotePad) sebagai donor arsitektur dan fitur utama, serta SubnetCalc-MacOS (https://github.com/alsyundawy/SubnetCalc-MacOS) atas sumbangan 25 rangkaian tema sintaksis dan UI.

9. Pelokalan Bilingual Terpadu (100% Selesai):
   - Sistem bahasa dirampingkan khusus untuk Bahasa Inggris (en) dan Bahasa Indonesia (id) dengan status 100% tuntas (0 entri tertunda), membersihkan 20 berkas katalog terjemahan usang.

10. Dialog Kustomisasi Toolbar Interaktif (CustomizeToolbarDialog):
    - Antarmuka dual-list drag-and-drop untuk menambah, menghapus, mengurutkan aksi toolbar, menyisipkan garis pemisah (separator), dan reset ke default dengan refleksi seketika.

11. Dock Pratinjau Aman Markdown & Ekspor Dokumen (MarkdownSafePreviewDock):
    - Pratinjau langsung berkinerja tinggi dengan dukungan rumus matematika KaTeX dan diagram Mermaid, sinkronisasi scroll dua arah, serta ekspor ke HTML mandiri dan PDF vektor via DocumentExportManager.

12. Suite Ergonomi Editor:
    - AutoClosePairs (penutupan otomatis tanda kurung/kutip cerdas), MarkOccurrences (penyorotan seluruh kemunculan token kata), LogAutoTimestamp (pencatatan otomatis stempel waktu berkas .LOG), dan ColorPreviewCallTip (pratinjau swatch warna Hex/RGB/HSL/CSS real-time).

13. Suite Forensik Teks, Statistik & Integritas Hash:
    - TokenForensicUtils (entropi Shannon, pembaca JWT), DocumentStatisticsDialog (analitik kata/baris/karakter), CharacterInfoDialog (inspeksi mendalam Unicode 16), MyersDiffEngine & FileDiffDialog (perbandingan berkas berdampingan algoritma LCS), dan HashIntegrityManager (pembuatan digest kriptografis streaming CRC32, MD5, SHA-1, SHA-256, SHA-512, SHA3).

14. Integrasi Mendalam macOS (76 Asosiasi Format Berkas):
    - Deklarasi komprehensif 76 tipe dokumen pemrograman dan konfigurasi serta UTType native di deploy/macos/info.plist, lengkap dengan integrasi macOS System Services.

15. Judul Jendela macOS Permanen Dark Aqua:
    - Menegakkan NSAppearanceNameDarkAqua melalui runtime Objective-C Cocoa (objc_msgSend) sehingga bingkai jendela dan tombol traffic lights macOS tetap gelap dan elegan dalam mode apa pun.

16. Pergantian Tema Cepat (< 2 ms):
    - Optimasi rasterisasi ikon via pointer scanline (scanLine()), caching ikon adaptif tema, dan eliminasi parse ulang stylesheet redundan.

17. Kompatibilitas Multi-Qt Penuh (Qt 6.5 LTS hingga Qt 6.12):
    - Perlindungan makro praprosesor (#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)) untuk setColorScheme memastikan kompilasi sukses 100% di semua runner CI dan lingkungan lokal.
"""

DOCNOTE_EN = """# Documentation Note (DOCNOTE) — NotepadNext-MacOS v0.15-08 (English)

--------------------------------------------------------------------------------
================================================================================
Application Edition   : NotepadNext For MacOS — Notepad++ Like
Release Version       : 0.15-08 (Tab Bar Plus, Tabbed Markdown/HTML Preview, 28 Curated Icon Packs, Bilingual Localization en/id, Clean Text-Only Preferences, SubnetCalc-MacOS Attribution & Multi-Qt Compatibility)
Release Date          : 2026-11-08
Lead Maintainer       : Harry Dertin Sutisna Alsyundawy (@alsyundawy)
Organization          : ALSYUNDAWY IT SOLUTION (https://alsyundawy.com)
Upstream Repository   : https://github.com/dail8859/NotepadNext (Baseline: v0.15)
macOS Edition Repo    : https://github.com/alsyundawy/NotepadNext-MacOS
Feature Donor Project : SparkNotePad v1.3.8 (https://github.com/alsyundawy/SparkNotePad)
Theme Contributor     : SubnetCalc-MacOS (https://github.com/alsyundawy/SubnetCalc-MacOS - 25 Theme Suites)
GUI Framework         : Qt 6.5 LTS / 6.8 LTS / 6.10 / 6.11 / 6.12 (C++20 ISO Standard)
Target Platforms      : macOS 11.0–26.0 (Apple Silicon arm64 & Intel x86_64), Linux (x86_64 & aarch64)
License               : GNU General Public License v3.0 (GPL-3.0-or-later)
================================================================================
--------------------------------------------------------------------------------

---

## 1. Executive Summary (English): Release v0.15-08

NotepadNext-MacOS v0.15-08 is a premier enterprise release introducing cutting-edge productivity innovations, deep macOS desktop integration, refined ergonomics, and architectural hardening:

1. Persistent Tab Bar Plus (+) Button & Modern Tab Ergonomics:
   - Added a persistent '+' tool button directly on the editor tab bar (DockedEditorTitleBar) with native macOS styling and responsive hover/pressed interaction states.
   - Create new document tabs instantaneously with a single click or standard shortcut (Cmd+N).

2. Tabbed Interactive Markdown & HTML Live Preview (PreviewTabWidget):
   - Renders Markdown and HTML documents inside editor tabs with GitHub-grade callouts ([!NOTE], [!TIP], [!IMPORTANT], [!WARNING], [!CAUTION]), devdocs typography, word count, reading time estimates, and dynamic zoom controls.
   - Added tab right-click context menu options on .md and .html files to launch in-tab preview or open directly in the default system browser (openInDefaultBrowser).

3. Themes Menu Icon Pack Selection & 6-Tier Sizing Configuration:
   - Expanded the Themes menu with a full toolbar icon pack selector and 6 configurable icon sizes (16x16, 20x20, 24x24, 32x32, 48x48, 64x64) with active checkmarks and live toolbar adaptation.

4. Help Menu In-Editor Documentation (DocNote & Changelog):
   - DocNote and Changelog actions under the Help menu automatically resolve and open DOCNOTE.md and CHANGELOG.md directly in dedicated Scintilla editor tabs.

5. Preferences Dialog Dedicated "Themes & Toolbar" Tab:
   - Restructured the Preferences dialog into a clean, text-only tabbed layout without icons or emojis.
   - Relocated theme styling, dark/light toggle, window opacity, toolbar icon pack selection, and toolbar icon sizing into a dedicated "Themes & Toolbar" tab with a crisp, clear live icon preview row.

6. macOS Native High-DPI Styled DMG Presentation:
   - Engineered custom packaging pipeline (generate_dmg_background.py & package-dmg.sh) producing high-DPI standard and Retina background assets, custom window geometry (680x440), left NotepadNext app slot, right /Applications link, curved drag-and-drop indicator, volume icon, and verified disk image checksums (hdiutil verify).

7. 28 Curated Icon Pack Personalization Suite (ToolbarIconManager):
   - Integrated a refined suite of 28 curated, high-clarity icon packs (Modern Ribbon SVG, Fluent, Classic Notepad++, Aksara variations, Retro Windows 7/Aero, and WinXP skeuomorphic styles).
   - Removed 10 redundant packs to maintain a lean, polished application bundle.

8. Comprehensive Upstream Donor & Contributor Attribution:
   - Full attribution and official URLs provided for SparkNotePad (https://github.com/alsyundawy/SparkNotePad) as foundational architecture and feature donor, and SubnetCalc-MacOS (https://github.com/alsyundawy/SubnetCalc-MacOS) for contributing 25 syntax highlighting and UI theme suites.

9. Streamlined Bilingual Localization (100% Complete):
   - Streamlined translation catalogs to focus strictly on English (en) and Indonesian (id) with 100% finished translation entries (0 unfinished), eliminating 20 obsolete language files.

10. Interactive Toolbar Customization Dialog (CustomizeToolbarDialog):
    - Dual-list drag-and-drop toolbar customization dialog supporting button addition, removal, reordering, separator insertion, and instantaneous toolbar sync with application settings.

11. Markdown Safe Live Preview Dock & Document Export Pipeline (MarkdownSafePreviewDock):
    - High-performance live preview dock supporting KaTeX mathematical expressions and Mermaid diagrams, bidirectional scroll synchronization, and export to standalone GitHub-styled HTML and vector PDF via DocumentExportManager.

12. Editor Ergonomics Suite:
    - AutoClosePairs (intelligent auto-closing of brackets and quotes), MarkOccurrences (Scintilla indicator-based highlight matching across viewport), LogAutoTimestamp (Windows Notepad .LOG header auto-timestamping), and ColorPreviewCallTip (real-time Hex/RGB/HSL/CSS visual calltips and swatches).

13. Text Forensics, Statistics & Cryptographic Hash Integrity:
    - TokenForensicUtils (Shannon entropy, JWT decoding), DocumentStatisticsDialog (character/word/line analytics), CharacterInfoDialog (in-depth Unicode 16 codepoint inspection), MyersDiffEngine & FileDiffDialog (side-by-side LCS file comparison), and HashIntegrityManager (streaming constant-memory cryptographic digest generation for CRC32, MD5, SHA-1, SHA-256, SHA-512, SHA3).

14. Deep macOS System Integration (76 Document Associations):
    - Full declaration of 76 programming and configuration file formats with native macOS UTTypes in deploy/macos/info.plist, complete with macOS System Services integration.

15. Permanent macOS Dark Aqua Titlebar:
    - Enforced NSAppearanceNameDarkAqua via Objective-C Cocoa runtime (objc_msgSend) ensuring window frames and traffic light controls stay dark and sleek in any theme mode.

16. Sub-2ms Fast Theme & Icon Switching:
    - Optimized icon rasterization via scanline pointer arithmetic (scanLine()), cached theme-adapted icons, and deferred action sync reducing switch latency to under 2 ms.

17. Full Multi-Qt Build Compatibility (Qt 6.5 LTS through Qt 6.12):
    - Guarded setColorScheme with preprocessor checks (#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)), ensuring complete, warning-free compilation on both legacy and cutting-edge Qt runners.
"""

CHANGELOG_ID = """# Catatan Perubahan (Changelog) — NotepadNext For MacOS

Semua perubahan penting pada **NotepadNext For MacOS** didokumentasikan di berkas ini.
Format disusun berdasarkan panduan [Keep a Changelog](https://keepachangelog.com/id/1.1.0/)
dan proyek ini mematuhi [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.15-08] - 2026-11-08

### 🌟 Rilis Utama: Tab Bar Plus (+), Pratinjau Markdown/HTML Tabbed, Suite 28 Icon Pack, Lokalisasi Bilingual, Preferensi Teks Murni, Atribusi SubnetCalc-MacOS & Kompatibilitas Multi-Qt

NotepadNext-MacOS v0.15-08 adalah rilis fitur dan stabilitas besar yang menghadirkan inovasi produktivitas terdepan, integrasi mendalam macOS, pemurnian antarmuka, dan pengerasan arsitektur:

#### 🆕 Ditambahkan

- **Tombol Tambah Tab (+) & Ergonomi Tab Modern**:
  - Menambahkan tombol `+` persisten pada tab bar editor (`DockedEditorTitleBar`) dengan estetika macOS asli dan status hover/pressed, memungkinkan pembuatan tab baru secara instan (`Cmd+N`).
- **Pratinjau Tab Markdown & HTML (`PreviewTabWidget`) & Menu Konteks Tab**:
  - Menghadirkan pratinjau tab interaktif untuk dokumen Markdown dan HTML dengan kotak info bergaya GitHub (`[!NOTE]`, `[!TIP]`, `[!IMPORTANT]`, `[!WARNING]`, `[!CAUTION]`), tipografi devdocs, metrik jumlah kata dan estimasi waktu baca, zoom dinamis, ekspor HTML, salin ke clipboard, dan pratinjau di peramban sistem. Menambahkan aksi klik-kanan pada tab `.md` dan `.html`.
- **Konfigurasi Icon Pack & 6 Ukuran Ikon pada Menu Themes**:
  - Memperluas menu Themes dengan submenu pemilihan 28 paket ikon toolbar dan 6 tingkat ukuran ikon (`16x16`, `20x20`, `24x24`, `32x32`, `48x48`, `64x64`) lengkap dengan tanda centang status aktif dan pembaruan visual toolbar seketika.
- **Dokumentasi Terintegrasi pada Menu Help (`DocNote` & `Changelog`)**:
  - Menambahkan aksi `DocNote` dan `Changelog` pada menu Help yang secara otomatis membuka berkas `DOCNOTE.md` dan `CHANGELOG.md` langsung di dalam tab editor Scintilla.
- **Tab Khusus "Themes & Toolbar" pada Dialog Preferensi**:
  - Merestrukturisasi dialog preferensi dengan antarmuka bertab berbasis teks murni (tanpa ikon/emoji), memindahkan pengaturan tema, toggle mode gelap/terang, opasitas jendela, pemilihan paket ikon, dan penyesuaian ukuran ikon ke dalam tab khusus **Themes & Toolbar** lengkap dengan baris pratinjau ikon langsung yang terang dan jelas.
- **Presentasi Installer DMG Bergaya Native macOS**:
  - Merancang pipeline pembuatan DMG kustom (`scripts/generate_dmg_background.py` & `scripts/package-dmg.sh`) yang menghasilkan aset latar belakang resolusi tinggi (standard & Retina @2x), geometri jendela (680x440), slot aplikasi NotepadNext di kiri, tautan `/Applications` di kanan, indikator drag-and-drop melengkung, ikon volume, dan verifikasi checksum citra disk (`hdiutil verify`).
- **Suite Personalisasi 28 Icon Pack Terpilih & Manajemen Toolbar (`ToolbarIconManager`)**:
  - Mengintegrasikan koleksi terkurasi 28 paket ikon (Modern Ribbon SVG, Fluent, Notepad++ Klasik, variasi Aksara, Retro Windows 7/Aero, dan skeuomorfik WinXP) secara optimal, menghapus 10 paket ikon redundan. Menambahkan kemampuan pergantian ikon secara langsung melalui menu konteks klik-kanan toolbar dan dialog Preferensi.
- **Pengakuan Atribusi & Proyek Donor**:
  - Menyertakan tautan resmi dan pengakuan atribusi terperinci untuk proyek saudara [SparkNotePad](https://github.com/alsyundawy/SparkNotePad) sebagai donor arsitektur dan fitur utama, serta [SubnetCalc-MacOS](https://github.com/alsyundawy/SubnetCalc-MacOS) atas kontribusi 25 tema syntax highlighting dan UI.
- **Lokalisasi Bilingual Terpadu & Tab Preferensi Berbasis Teks Murni**:
  - Menyederhanakan sistem translasi hanya pada bahasa Inggris (`en`) dan bahasa Indonesia (`id`) dengan penyelesaian 100% tuntas, membersihkan 20 berkas katalog terjemahan usang. Memperbarui seluruh judul tab pada `PreferencesDialog` menjadi berbasis teks murni tanpa ikon atau emoji.
- **Dialog Kustomisasi Toolbar Interaktif (`CustomizeToolbarDialog`)**:
  - Menghadirkan dialog kustomisasi toolbar interaktif dengan antarmuka dual-list drag-and-drop yang memungkinkan penambahan, pengurangan, pengurutan aksi, penyisipan pemisah (separator), dan reset ke default dengan pembaruan toolbar secara seketika. Dilengkapi pencarian filter aksi dan sinkronisasi otomatis dengan pengaturan aplikasi (`Gui/ToolBar`).
- **Dock Pratinjau Aman Markdown & Pipeline Ekspor Dokumen (`MarkdownSafePreviewDock`)**:
  - Menambahkan dock pratinjau langsung berkinerja tinggi untuk dokumen Markdown dan HTML dengan dukungan formula matematika KaTeX dan diagram Mermaid. Mendukung sinkronisasi scroll dua arah antara Scintilla editor dan penampil pratinjau. Dilengkapi `DocumentExportManager` dan `ExportOptionsDialog` untuk mengekspor dokumen ke berkas HTML mandiri (dengan styling GitHub tersemat) atau dokumen PDF vektor.
- **Suite Ergonomi Editor (Auto-Closing Pairs, Mark Occurrences, Timestamp Otomatis Log & Preview Warna)**:
  - `AutoClosePairs`: Penutupan otomatis tanda kurung dan tanda kutip dengan logika overtyping skip dan penanganan backspace pasangan kosong.
  - `MarkOccurrences`: Penandaan indikator Scintilla untuk menyorot seluruh kemunculan teks yang dipilih pada viewport editor aktif.
  - `LogAutoTimestamp`: Deteksi header `.LOG` ala Windows Notepad yang otomatis menyisipkan stempel tanggal/waktu saat membuka berkas catatan log.
  - `ColorPreviewCallTip`: Deteksi kode warna Hex, RGB, RGBA, HSL, HSLA, dan CSS secara instan dengan pratinjau swatch warna visual pada CallTip editor.
- **Suite Forensik, Statistik, Mesin Diff Myers & Integritas Hash**:
  - `TokenForensicUtils` & `DocumentStatisticsDialog`: Perhitungan entropi Shannon, decoding header/payload token JWT, estimasi waktu membaca/berbicara, serta statistik karakter/kata/baris terperinci.
  - `CharacterInfo` & `CharacterInfoDialog`: Inspeksi mendalam codepoint Unicode 16 dengan kartu visual glyph, urutan escape HTML/C/JSON/CSS, dan metrik karakter dwiarah.
  - `MyersDiffEngine` & `FileDiffDialog`: Perbandingan berkas berdampingan algoritma LCS (Longest Common Subsequence) dengan sinkronisasi scroll dua panel.
  - `HashIntegrityManager`, `GenerateHashDigestDialog` & `HashIntegrityDialog`: Pembuatan digest kriptografis aliran chunk memori konstan (CRC32, MD5, SHA-1, SHA-256, SHA-512, SHA3) dengan indikator status verifikasi real-time.
- **Integrasi Mendalam macOS & Asosiasi 76 Format Berkas**:
  - Memperbarui `deploy/macos/info.plist` dengan deklarasi Uniform Type Identifiers (UTType) komprehensif yang mencakup 76 ekstensi bahasa pemrograman dan markup, serta integrasi macOS System Services.

#### 🛠️ Diperbaiki & Diperkuat

- **Kompatibilitas Multi-Qt Penuh (Qt 6.5 LTS hingga Qt 6.12)**:
  - Membungkus pemanggilan `QStyleHints::setColorScheme` dalam `#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)` sehingga kompilasi berhasil tanpa eror pada seluruh runner CI GitHub Actions dan distro Linux Qt 6.5 LTS.
- **Judul Jendela macOS Permanen Dark Aqua**:
  - Menegakkan `NSAppearanceNameDarkAqua` melalui runtime Objective-C Cocoa (`objc_msgSend`) sehingga bingkai jendela dan tombol traffic lights macOS tetap gelap dan elegan dalam mode apa pun.
- **Pergantian Tema & Icon Cepat (< 2 ms)**:
  - Mengoptimalkan rasterisasi ikon melalui scanline pointer arithmetic (`scanLine()`), caching tema ikon adaptif, dan deferral sinkronisasi aksi jendela.
- **Gerbang Analisis Statis 100% Zero-Error**:
  - Lulus seluruh validasi `trunk check -y` pada 4.023 berkas dengan 0 isu.
  - Eksekusi `cppcheck` bersih pada seluruh berkas C++ baru dan yang dimodifikasi.
  - Pembuatan dan verifikasi paket DMG lokal (`NotepadNext-v0.15-08-x64.dmg`) tervalidasi menggunakan `hdiutil verify`.
"""

CHANGELOG_EN = """# Changelog — NotepadNext For MacOS

All notable changes to the **NotepadNext For MacOS** project are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.15-08] - 2026-11-08

### 🌟 Major Release: Tab Bar Plus (+), Tabbed Markdown/HTML Preview, 28 Curated Icon Packs, Bilingual Localization en/id, Clean Text-Only Preferences, SubnetCalc-MacOS Attribution & Multi-Qt Compatibility

NotepadNext-MacOS v0.15-08 is a major feature and stability release introducing leading productivity innovations, deep macOS desktop integration, refined ergonomics, and architectural hardening:

#### 🆕 Added

- **Tab Bar Plus (+) Button & Tab Ergonomics**:
  - Added a persistent `+` tool button on the dock tab bar (`DockedEditorTitleBar`) with macOS styling and hover/pressed states, allowing users to quickly open a new document tab with a single click or standard shortcut (`Cmd+N`).
- **Tabbed Markdown & HTML Live Preview (`PreviewTabWidget`) & Tab Context Menu**:
  - Introduced tabbed live preview for Markdown and HTML documents with GitHub-grade callouts (`[!NOTE]`, `[!TIP]`, `[!IMPORTANT]`, `[!WARNING]`, `[!CAUTION]`), devdocs typography, word and reading time metrics, dynamic zoom, HTML export, clipboard copy, and browser preview. Added tab right-click context menu actions to launch Markdown and HTML previews directly from open `.md`, `.markdown`, and `.html` files.
- **Themes Menu Icon Pack & 6-Tier Size Configuration**:
  - Expanded the Themes menu to include full selection of 28 toolbar icon packs (`modern ribbon`, `fluent`, `notepad++ classic`, `windows xp`, `retro aero`, `aksara`, etc.) and 6 toolbar icon sizes (`16x16`, `20x20`, `24x24`, `32x32`, `48x48`, `64x64`) with active state checkmarks and dynamic toolbar resizing.
- **Help Menu Tabbed Documentation (`DocNote` & `Changelog`)**:
  - Added `DocNote` and `Changelog` actions under the Help menu that resolve and display `DOCNOTE.md` and `CHANGELOG.md` directly inside dedicated editor tabs.
- **Preferences Dialog "Themes & Toolbar" Dedicated Tab**:
  - Restructured the Preferences dialog into a clean, text-only tabbed layout without icons or emojis. Relocated theme styling, dark/light toggle, window opacity, toolbar icon pack selection, and toolbar icon sizing into a dedicated **Themes & Toolbar** tab with a clear live icon preview row.
- **macOS Native Styled DMG Installer Presentation**:
  - Engineered custom packaging pipeline (`scripts/generate_dmg_background.py` & `scripts/package-dmg.sh`) producing high-DPI standard and Retina background assets, custom window geometry (680x440), left NotepadNext app slot, right `/Applications` link, curved drag-and-drop indicator, volume icon, and verified disk image checksums (`hdiutil verify`).
- **28 Curated Icon Pack Personalization Suite & Toolbar Management (`ToolbarIconManager`)**:
  - Integrated a streamlined suite of 28 curated icon packs (Modern Ribbon SVG, Fluent, Classic Notepad++, Aksara variations, Retro Windows 7/Aero, and WinXP skeuomorphic styles) with zero overhead, removing 10 redundant packs (`aksara-icon-pack-sharp`, `notepad3_warm`, `notepadpp_green`, `material_sharp`, `pixel_fugue_like`, `aksara-icon-pack-bold`, `aksara-icon-pack-duotone`, `aksara-icon-pack-outline`, `aksara-npp-gnome51`, `aksara-npp-kde6`). Added dynamic runtime icon switching directly via toolbar right-click context menu and Preferences dialog.
- **Attribution & Upstream Donor Recognition**:
  - Added explicit URLs and formal attribution for sister project [SparkNotePad](https://github.com/alsyundawy/SparkNotePad) as foundational architecture and feature donor, and [SubnetCalc-MacOS](https://github.com/alsyundawy/SubnetCalc-MacOS) for contributing 25 syntax highlighting and UI themes across the application.
- **Streamlined Bilingual Localization & Text-Only Preferences Tabs**:
  - Refactored translation system to strictly focus on English (`en`) and Indonesian (`id`) with 100% finished translation entries, removing 20 obsolete language files. Updated all tab headers in `PreferencesDialog` to clean text-only labels without icons or emojis.
- **Interactive Toolbar Customization Dialog (`CustomizeToolbarDialog`)**:
  - Introduced a dual-list drag-and-drop toolbar customization dialog allowing users to add, remove, reorder actions, insert separators, and reset to defaults with instantaneous live toolbar reflection. Includes search filtering across available actions and seamless synchronization with application settings (`Gui/ToolBar`).
- **Markdown Safe Live Preview Dock & Document Export Pipeline (`MarkdownSafePreviewDock`)**:
  - Added a high-performance live preview dock for Markdown and HTML documents featuring KaTeX math expression rendering and Mermaid diagram support. Includes bidirectional scroll synchronization between Scintilla editor and preview browser. Paired with `DocumentExportManager` and `ExportOptionsDialog` for exporting documents as standalone self-contained HTML (with embedded GitHub styling) or vector PDF documents.
- **Editor Ergonomics Suite (Auto-Closing Pairs, Mark Occurrences, Log Auto-Timestamp & Color Swatches)**:
  - `AutoClosePairs`: Intelligent bracket and quote auto-closing with overtyping skip logic and empty-pair backspace handling.
  - `MarkOccurrences`: Scintilla indicator-based highlight matching for all occurrences of selected text across the active editor viewport.
  - `LogAutoTimestamp`: Native Windows Notepad `.LOG` header detection automatically appending local date/time stamps when opening log files.
  - `ColorPreviewCallTip`: Automatic detection of Hex, RGB, RGBA, HSL, HSLA, and CSS color codes with live visual CallTip and tooltip color swatches.
- **Forensics, Statistics, Myers Diff Engine & Hash Integrity Suite**:
  - `TokenForensicUtils` & `DocumentStatisticsDialog`: Shannon entropy calculations, JWT header/payload decoding, reading/speaking time estimates, and comprehensive character/word/line analytics.
  - `CharacterInfo` & `CharacterInfoDialog`: In-depth Unicode 16 codepoint inspection with visual glyph cards, HTML/C/JSON/CSS escape sequences, and bidirectional character metrics.
  - `MyersDiffEngine` & `FileDiffDialog`: Longest Common Subsequence (LCS) side-by-side file comparison with synchronized dual-pane scrolling.
  - `HashIntegrityManager`, `GenerateHashDigestDialog` & `HashIntegrityDialog`: Constant-memory stream chunked cryptographic digest generation (CRC32, MD5, SHA-1, SHA-256, SHA-512, SHA3) with real-time verification status indicators.
- **macOS Deep System Integration & 76 File Format Associations**:
  - Modernized `deploy/macos/info.plist` with comprehensive Uniform Type Identifiers (UTType), document type definitions covering 76 programming and markup file extensions, and native macOS System Services integration.

#### 🛠️ Fixed & Hardened

- **Full Multi-Qt Build Compatibility (Qt 6.5 LTS through Qt 6.12)**:
  - Wrapped `QStyleHints::setColorScheme` calls in `#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)`, preventing build failures on Qt 6.5 LTS Linux runners and backward-compatible environments.
- **Permanent macOS Dark Aqua Titlebar**:
  - Enforced `NSAppearanceNameDarkAqua` via Objective-C Cocoa runtime (`objc_msgSend`) so the macOS window chrome, traffic lights, and titlebar remain dark/black in any theme mode matching native Dark Aqua presentation.
- **Sub-2ms Fast Theme & Icon Switching**:
  - Optimized icon rasterization via scanline pointer arithmetic (`scanLine()`), cached theme-adapted icons, removed redundant stylesheet re-parses, and deferred window action sync, reducing theme switch latency from >500 ms to <2 ms.
- **100% Zero-Error Static Analysis Gate**:
  - Cleared all findings across `trunk check -y`, validating 4,023 files with 0 issues.
  - Clean `cppcheck` execution across all newly added and modified C++ source and header files.
  - Complete elimination of shadow parameter warnings and loop modernized to standard STL algorithms (`std::any_of`, `std::all_of`, `std::find_if`, `std::transform`).
  - Built and verified local DMG release (`NotepadNext-v0.15-08-x64.dmg`) with `hdiutil verify` cryptographic checksum validation.
"""


def main():
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    os.chdir(base_dir)

    os.makedirs("doc", exist_ok=True)
    os.makedirs("src/doc", exist_ok=True)

    # Master combined docs
    master_docnote_text = DOCNOTE_ID.strip() + "\n\n---\n\n" + DOCNOTE_EN.strip() + "\n"
    master_changelog_text = CHANGELOG_ID.strip() + "\n\n---\n\n" + CHANGELOG_EN.strip() + "\n"

    # Load whitepaper for DOCNOTE.md
    whitepaper_path = "doc/whitepaper_docnote.md"
    if os.path.exists(whitepaper_path):
        with open(whitepaper_path, "r", encoding="utf-8") as f:
            whitepaper_docnote = f.read()
    else:
        whitepaper_docnote = master_docnote_text

    # Load historical changelog
    hist_path = "doc/historical_changelog.txt"
    if os.path.exists(hist_path):
        with open(hist_path, "r", encoding="utf-8") as f:
            historical_part = f.read().strip()
        full_changelog_md = master_changelog_text + "\n---\n\n" + historical_part + "\n"
    else:
        full_changelog_md = master_changelog_text

    files_to_write = {
        # Root artifacts
        "DOCNOTE.md": whitepaper_docnote,
        "DOCNOTE.txt": master_docnote_text,
        "DOCNOTE_id.txt": DOCNOTE_ID.strip() + "\n",
        "DOCNOTE_en.txt": DOCNOTE_EN.strip() + "\n",
        "CHANGELOG.md": full_changelog_md,
        "CHANGELOG.txt": master_changelog_text,
        "CHANGELOG_id.txt": CHANGELOG_ID.strip() + "\n",
        "CHANGELOG_en.txt": CHANGELOG_EN.strip() + "\n",

        # doc/ artifacts
        "doc/DOCNOTE.md": whitepaper_docnote,
        "doc/DOCNOTE.txt": master_docnote_text,
        "doc/DOCNOTE_id.txt": DOCNOTE_ID.strip() + "\n",
        "doc/DOCNOTE_en.txt": DOCNOTE_EN.strip() + "\n",
        "doc/CHANGELOG.md": full_changelog_md,
        "doc/CHANGELOG.txt": master_changelog_text,
        "doc/CHANGELOG_id.txt": CHANGELOG_ID.strip() + "\n",
        "doc/CHANGELOG_en.txt": CHANGELOG_EN.strip() + "\n",

        # src/doc/ artifacts
        "src/doc/DOCNOTE.md": whitepaper_docnote,
        "src/doc/DOCNOTE.txt": master_docnote_text,
        "src/doc/DOCNOTE_id.txt": DOCNOTE_ID.strip() + "\n",
        "src/doc/DOCNOTE_en.txt": DOCNOTE_EN.strip() + "\n",
        "src/doc/CHANGELOG.md": full_changelog_md,
        "src/doc/CHANGELOG.txt": master_changelog_text,
        "src/doc/CHANGELOG_id.txt": CHANGELOG_ID.strip() + "\n",
        "src/doc/CHANGELOG_en.txt": CHANGELOG_EN.strip() + "\n",
    }

    for path, content in files_to_write.items():
        with open(path, "w", encoding="utf-8") as f:
            f.write(content)
        print(f"Generated: {path} ({len(content)} bytes)")

    # Clean up temporary extraction files
    for temp in [whitepaper_path, hist_path, "doc/changelog_01508.txt"]:
        if os.path.exists(temp):
            os.remove(temp)
            print(f"Cleaned up temporary: {temp}")

    print("\n[SUCCESS] Localized DOCNOTE and CHANGELOG generated and synchronized across root, doc/, and src/doc/.")


if __name__ == "__main__":
    main()
