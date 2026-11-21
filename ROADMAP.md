# 🗺️ NotepadNext-MacOS: Master Architecture Roadmap & Future Milestones

> **Target Release**: NotepadNext-MacOS `v0.15-09` & Beyond  
> **Current Baseline**: `v0.15-08` (`1c308af`)  
> **Lead Architect & Maintainer**: Harry Dertin Sutisna Alsyundawy ([@alsyundawy](https://github.com/alsyundawy)) | ALSYUNDAWY IT SOLUTION  
> **Upstream Project**: Justin Dailey ([@dail8859/NotepadNext](https://github.com/dail8859/NotepadNext))  
> **Framework & Standards**: Qt 6.5–6.12 • ISO C++20 • Scintilla 5.6.x • Apple HIG • POSIX IEEE 1003.1

---

## 📑 Daftar Isi / Table of Contents

1. [Ringkasan Eksekutif & Sasaran Roadmap / Executive Summary](#1-ringkasan-eksekutif--sasaran-roadmap--executive-summary)
2. [Prinsip Non-Destruktif & Jaminan Integritas Aset Eksisting](#2-prinsip-non-destruktif--jaminan-integritas-aset-eksisting)
3. [14-Skill Operational Stack & 14 Engineering Review Pillars](#3-14-skill-operational-stack--14-engineering-review-pillars)
4. [Rincian Fase Implementasi / Implementation Phases](#4-rincian-fase-implementasi--implementation-phases)
   - [Fase 1: Resilience & Crash Protection Suite](#fase-1-resilience--crash-protection-suite)
   - [Fase 2: Universal Developer Tools Hub](#fase-2-universal-developer-tools-hub)
   - [Fase 3: Tab Ergonomics & Navigation](#fase-3-tab-ergonomics--navigation)
   - [Fase 4: POSIX System & Shell Inspector](#fase-4-posix-system--shell-inspector)
   - [Fase 5: Visual Gutter & Real-Time Navigation](#fase-5-visual-gutter--real-time-navigation)
   - [Fase 6: Workspace Bundles & Engine Modernization](#fase-6-workspace-bundles--engine-modernization)
5. [Matriks Penjaminan Kualitas, Linter & Build Constraints](#5-matriks-penjaminan-kualitas-linter--build-constraints)

---

## 1. Ringkasan Eksekutif & Sasaran Roadmap / Executive Summary

Dokumen ini mendefinisikan peta jalan strategis (*Master Architecture Roadmap*) pengembangan lanjutan untuk **NotepadNext-MacOS** menuju rilis **v0.15-09** dan milestone mendatang.

Setelah rilis stabil **v0.15-08** berhasil menuntaskan integrasi pratinjau Markdown-It dengan 30 tema terisolasi, 28 paket ikon toolbar dinamis, tombol plus (+) native pada tab bar, pengemasan DMG bergaya native macOS, dan eliminasi entri desktop ganda di Linux/FreeBSD, fokus roadmap berikutnya adalah:

- **Resilience**: Memastikan tidak ada data dokumen yang hilang dengan autosave snapshot berkala dan wizard pemulihan pasca-crash.
- **Developer Productivity**: Menyediakan hub peralatan developer 5-tab langsung di dalam editor (kriptografi stream hash 11 algoritma, MIME/Base64/URL converter, radix converter, enkripsi HTML AES-256-GCM, pretty-printer/minifier kode, dan inspektur JWT/UUID).
- **Tab & POSIX Ergonomics**: Pengorganisasian dokumen bertab visual, penggantian nama berkas di tempat (*in-place renaming*), pemulihan tab tertutup, dan inspeksi hak akses shell POSIX.

---

## 2. Prinsip Non-Destruktif & Jaminan Integritas Aset Eksisting

Sesuai dengan instruksi arsitektur permanen dan standar rekayasa perangkat lunak enterprise:

> [!IMPORTANT]
> **Strict Asset Preservation Mandate**:
>
> 1. **Zero Disruption to Existing Themes**: Seluruh tema aplikasi bawaan, 25 tema syntax highlighting kontribusi dari `SubnetCalc-MacOS`, dan 30 tema pratinjau markdown terisolasi pada `PreviewTabWidget` / `MarkdownSafePreviewDock` **TIDAK AKAN DISENTUH, DIUBAH, ATAU DIHAPUS**.
> 2. **Zero Disruption to 28 Toolbar Icon Packs**: Seluruh 28 paket ikon toolbar (`ToolbarIconManager`) dan 6 tingkat ukuran ikon (`16x16` hingga `64x64`) tetap beroperasi 100% normal tanpa perubahan struktur resource.
> 3. **Non-Destructive Integration**: Seluruh fitur baru ditambahkan secara modular melalui kelas terisolasi pada direktori `src/features/`, `src/dialogs/`, dan `src/decorators/` tanpa mengubah logika rendering tema yang sudah stabil.

---

## 3. 14-Skill Operational Stack & 14 Engineering Review Pillars

Setiap kode yang diimplementasikan dalam siklus roadmap ini wajib mematuhi protokol penjaminan mutu 14-Skill Stack dan 14 Pilar Review:

```text
[14 Engineering Review Pillars]
 1. Bug Review                 8. Circular Dependency Review
 2. Syntax Review              9. Performance Bottlenecks Review
 3. Runtime Review            10. Security Vulnerability (OWASP / CWE)
 4. Logic Review              11. Maintainability (CISQ/ISO 5055)
 5. Memory & Resource Review  12. Scalability Review
 6. Dead Code Review          13. Readability Review (Google C++ Guide)
 7. Duplicate Code Review     14. Testability & Empirical Verification
```

---

## 4. Rincian Fase Implementasi / Implementation Phases

### Fase 1: Resilience & Crash Protection Suite

*Menghadirkan perlindungan dokumen proaktif dari kegagalan proses atau pemadaman sistem.*

- **`AutosaveRecoveryManager`** (`src/features/`):
  - Daemon timer di latar belakang yang melakukan snapshot berkala pada interval 10 detik (dapat disesuaikan di Settings).
  - Snapshot disimpan ke direktori sandbox terisolasi (`~/.config/NotepadNext/autosave/`) dengan hak akses POSIX `0700` (folder) dan `0600` (file snapshot).
  - Integritas data diverifikasi menggunakan digest `QCryptographicHash::Sha256` untuk mencegah snapshot yang korup atau rusak.
  - Snapshot dibersihkan secara atomik saat dokumen disimpan atau ditutup secara normal.
- **`CrashRecoveryDialog`** (`src/dialogs/`):
  - Deteksi otomatis snapshot yang belum tersimpan (*orphaned snapshots*) saat aplikasi dijalankan kembali setelah penghentian tidak terduga.
  - Dialog interaktif dengan pratinjau diff baris, informasi waktu snapshot, dan opsi pemulihan granular (*Recover Selected*, *Recover All*, *Discard*).

---

### Fase 2: Universal Developer Tools Hub

*Menyediakan utilitas transformasi data dan kriptografi harian developer tanpa ketergantungan pihak ketiga.*

- **`DeveloperToolsEngine`** (`src/features/`):
  - **Tab 1: Hash Stream Engine**: Perhitungan digest 11 algoritma (MD5, SHA-1, SHA-224, SHA-256, SHA-384, SHA-512, SHA3-224..512, Keccak-256) dengan arsitektur stream chunked beroriantasi memori konstan.
  - **Tab 2: MIME & Encoders**: Base64 standar & URL-safe, Quoted-Printable, URL encoding, encode/decode entitas HTML, serta penampil biner hex dump.
  - **Tab 3: Base Radix Converter**: Konversi langsung dan simultan basis 2 (Biner), basis 8 (Oktal), basis 10 (Desimal), basis 16 (Heksadesimal), serta ASCII-to-Hex byte streams.
  - **Tab 4: HTML Crypto Suite**: Generator enkripsi dan dekripsi berkas HTML mandiri berbasis CryptoJS AES-256-GCM dengan meteran kekuatan kata sandi zxcvbn.
  - **Tab 5: Code Formatter, Minifier & Token Inspector**:
    - Pretty-printer dan minifier untuk JSON, XML, dan HTML.
    - Inspektur token JWT (pembacaan header, klaim payload, dan status masa berlaku token).
    - Generator UUID v4 (single dan bulk).
    - Konverter dua arah Unix Epoch timestamp dan ISO 8601.
- **`DeveloperToolsDialog`** (`src/dialogs/`):
  - Antarmuka tabbed native macOS dengan shortcut `⌥⌘D` (Option+Cmd+D).
  - Aksi instan: *Insert to Active Document*, *Copy to Clipboard*, *Process Selected Text*.

---

### Fase 3: Tab Ergonomics & Navigation

*Meningkatkan kenyamanan pengelolaan dokumen multi-tab.*

- **`TabColorManager`** (`src/features/`):
  - Pewarnaan visual tab dengan 10 preset palet profesional dan custom hex color picker.
  - Indikator strip warna halus pada tab bar (`QTabBar::tab:selected`).
  - Penyimpanan preferensi warna tab per file path di dalam session cache.
- **`RenameTabDialog`** (`src/dialogs/`):
  - Pengubahan nama berkas di disk secara langsung dari tab context menu atau shortcut `⇧⌘R`.
  - Penanganan konflik nama berkas secara aman dan pembaruan path pada session state.
- **`ClosedTabsManager`** (`src/features/`):
  - Stack LIFO (*Last In, First Out*) untuk menyimpan riwayat penutupan tab.
  - Shortcut `⇧⌘T` untuk membuka kembali tab yang baru saja ditutup dengan posisi kursor dan status bookmark yang utuh.

---

### Fase 4: POSIX System & Shell Inspector

*Integrasi mendalam dengan utilitas sistem macOS dan Linux.*

- **`PosixFileInspector`** (`src/`):
  - **Shebang Detection**: Analisis baris pertama berkas untuk mendeteksi interpreter shebang (`#!/bin/bash`, `#!/usr/bin/env python3`, dll.) dan auto-suggest syntax lexer yang sesuai.
  - **POSIX File Permissions**: Tampilan status bar untuk hak akses berkas (oktal dan simbolik, misal: `0644` / `-rw-r--r--`).
  - **Executable Bit Toggle**: Klik pada indikator status bar untuk langsung mengubah izin berkas menjadi executable (`chmod +x` / `0755`) dengan prompt konfirmasi.
  - **Touch File Utility**: Pembaruan stempel waktu modifikasi berkas (*file touch*) menggunakan panggilan sistem `utimes`.

---

### Fase 5: Visual Gutter & Real-Time Navigation

*Feedback visual langsung pada margin editor untuk akselerasi alur kerja pengeditan.*

- **`ChangeHistoryDecorator`** (`src/decorators/`):
  - Indikator margin Scintilla untuk menandai baris yang dimodifikasi sejak berkas dibuka:
    - *Modified / Unsaved*: Batang indikator oranye di margin kiri.
    - *Saved / Committed*: Batang indikator hijau setelah berkas disimpan.
    - *Reverted*: Indikator otomatis hilang saat perubahan di-undo kembali ke keadaan awal.
- **`ScrollbarNavigationMarkers`** (`src/features/`):
  - Overlay penanda pada track scrollbar vertikal untuk bookmark aktif, hasil pencarian teks, dan kesalahan kompilasi/sintaks.
- **`TailMonitoringManager` & `TailOptionsDialog`** (`src/features/` & `src/dialogs/`):
  - Pemantauan berkas log langsung secara terus-menerus (`tail -f`) dengan auto-scroll dan indikator status bar aktif.

---

### Fase 6: Workspace Bundles & Engine Modernization

*Portabilitas konfigurasi antar-lingkungan kerja dan pembaruan engine Scintilla.*

- **`NotepadNextTransferManager`** (`src/features/` & `src/dialogs/`):
  - Ekspor dan impor seluruh konfigurasi NotepadNext (shortcuts kustom, pemetaan bahasa, konfigurasi toolbar, session aktif) dalam satu berkas terkompresi `.npnbundle`.
- **Scintilla 5.6.9 & Lexilla 5.5.5 Engine Alignment**:
  - Evaluasi pembaruan engine Scintilla untuk mengadopsi optimasi rendering teks Retina macOS dan stabilitas pengetikan multi-kursor.

---

## 5. Matriks Penjaminan Kualitas, Linter & Build Constraints

Setiap tahapan eksekusi pada roadmap ini dikawal oleh batasan teknis berikut:

| Parameter | Ketentuan / Standar | Catatan Implementasi |
| :--- | :--- | :--- |
| **Build Generator** | `Unix Makefiles` | Generator `Ninja` dilarang. Wajib menggunakan `cmake -B build -G "Unix Makefiles"`. |
| **Build Runner** | Homebrew GNU Make | `make -j8` untuk build paralel yang stabil dan terkontrol. |
| **Standar Bahasa** | ISO C++20 | Standard kompilator Clang / AppleClang dengan opsi keamanan sanitasi memori. |
| **Pemeriksaan Statis** | 0 Linter Errors | Lulus uji `cppcheck --enable=all`, `clang-tidy`, dan verifikasi SonarLint. |
| **Hak Akses Berkas** | POSIX 0700 / 0600 | Seluruh direktori autosave (`0700`) dan berkas snapshot (`0600`) terlindung dari akses pengguna lain (CWE-732). |
| **Aset Eksisting** | 100% Persisten | 28 paket ikon toolbar dan 55+ tema syntax/preview tetap aktif dan tidak tersentuh. |

---

## 6. Status Eksekusi Roadmap v0.15-09 (100% Selesai)

| Fase | Komponen Utama | File Terkait | Status |
| :--- | :--- | :--- | :---: |
| **Fase 1** | Resilience & Crash Recovery | `AutosaveRecoveryManager`, `CrashRecoveryDialog` | ✅ **SELESAI** |
| **Fase 2** | Universal Developer Tools Hub | `DeveloperToolsEngine`, `DeveloperToolsDialog`, `CodeCompressPretty` | ✅ **SELESAI** |
| **Fase 3** | Tab Ergonomics & Navigation | `TabColorManager`, `ClosedTabsManager`, `RenameTabDialog` | ✅ **SELESAI** |
| **Fase 4** | POSIX System & Shell Inspector | `PosixFileInspector` | ✅ **SELESAI** |
| **Fase 5** | Visual Gutter & Real-Time Navigation | `ChangeHistoryDecorator`, `ScrollbarNavigationMarkers`, `TailMonitoringManager`, `TailOptionsDialog` | ✅ **SELESAI** |
| **Fase 6** | Workspace Bundles & Engine Modernization | `NotepadNextTransferManager`, `TransferBundleDialog`, Scintilla 5.6.7 Retina Engine Evaluation | ✅ **SELESAI** |
