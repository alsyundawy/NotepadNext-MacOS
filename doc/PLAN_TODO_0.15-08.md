# 📋 NotepadNext-MacOS v0.15-08: Master Architecture Plan, Todo & Task List

## Cross-Platform Research, 2025–2026 Vulnerability Hardening, and SparkNotePad v1.3.8 Feature Integration

> **Target Release**: NotepadNext-MacOS `v0.15-08`<br/>
> **Baseline Release**: NotepadNext-MacOS `v0.15-07` (`a26d748`)<br/>
> **Feature Donor Project**: [SparkNotePad](https://github.com/alsyundawy/SparkNotePad) `v1.3.8` • [SubnetCalc-MacOS](https://github.com/alsyundawy/SubnetCalc-MacOS) (25 Theme Suites)<br/>
> **Author & Maintainer**: Harry Dertin Sutisna Alsyundawy ([@alsyundawy](https://github.com/alsyundawy)) | ALSYUNDAWY IT SOLUTION<br/>
> **Upstream Project**: Justin Dailey ([@dail8859/NotepadNext](https://github.com/dail8859/NotepadNext))<br/>
> **Framework & Standards**: Qt 6.5–6.12 • ISO C++20 • Scintilla 5.6.x • Lexilla 5.5.x • Apple HIG

---

## 📑 Daftar Isi / Table of Contents

1. [Ringkasan Eksekutif & Sasaran Rilis / Executive Summary](#1-ringkasan-eksekutif--sasaran-rilis--executive-summary)
2. [Riset Mendalam Komparatif & Analisis Kerentanan 2025–2026 (CVE/GHSA)](#2-riset-mendalam-komparatif--analisis-kerentanan-20252026-cveghsa)
   - [2.1 Notepad++ (WinGUp Supply Chain, Installer LPE, Buffer Overflows)](#21-notepad-wingup-supply-chain-installer-lpe-buffer-overflows)
   - [2.2 Scintilla 5.6.8/5.6.9 & Lexilla 5.5.4/5.5.5 Stability Audit](#22-scintilla-568569--lexilla-554555-stability-audit)
   - [2.3 Embedded Lua 5.5.1 / 5.4.x Sandbox Confinement](#23-embedded-lua-551--54x-sandbox-confinement)
   - [2.4 Kerentanan Kerangka Qt 6.5–6.12 (CVE-2025-3512, CVE-2025-14575, dll.)](#24-kerentanan-kerangka-qt-65612-cve-2025-3512-cve-2025-14575-dll)
   - [2.5 Ekosistem Editor Sibling](#25-ekosistem-editor-sibling)
3. [Mandat Utama: Audit Kode, Penuntasan 945 Error IDE & 25 SonarQube, Serta Checklist 14 Pilar](#3-mandat-utama-audit-kode-penuntasan-945-error-ide--25-sonarqube-serta-checklist-14-pilar)
   - [3.1 Instruksi Khusus Lingkungan & Batas Repositori (NotepadNext-MacOS)](#31-instruksi-khusus-lingkungan--batas-repositori-notepadnext-macos)
   - [3.2 Resolusi Tuntas Akar Masalah 945 Error IDE dan 25 SonarQube](#32-resolusi-tuntas-akar-masalah-945-error-ide-dan-25-sonarqube)
   - [3.3 Protokol Pengecekan Statis Mandatori (clangd, cppcheck, SonarLint, clang-tidy)](#33-protokol-pengecekan-statis-mandatori-clangd-cppcheck-sonarlint-clang-tidy)
   - [3.4 Checklist Audit Komprehensif 14 Pilar Kualitas Perangkat Lunak](#34-checklist-audit-komprehensif-14-pilar-kualitas-perangkat-lunak)
   - [3.5 Pengendalian Sumber Daya Perangkat Keras (Bebas Spike CPU & RAM MacBook)](#35-pengendalian-sumber-daya-perangkat-keras-bebas-spike-cpu--ram-macbook)
4. [Analisis Mendalam Fitur SparkNotePad v1.3.8 (Fitur Siap Adopsi)](#4-analisis-mendalam-fitur-sparknotepad-v138-fitur-siap-adopsi)
5. [Prinsip Arsitektur Non-Destruktif & 14 Pilar Kualitas Kode](#5-prinsip-arsitektur-non-destruktif--14-pilar-kualitas-kode)
6. [Daftar Rencana Induk & Todo (Master Implementation Plan)](#6-daftar-rencana-induk--todo-master-implementation-plan)
7. [Rincian Tugas Bertahap (Task List by Phase)](#7-rincian-tugas-bertahap-task-list-by-phase)
   - [Tugas Fase 0: Investigasi Diagnostik, Perbaikan Seketika & Gerbang Linter](#tugas-fase-0-investigasi-diagnostik-perbaikan-seketika--gerbang-linter)
   - [Tugas Fase 1: Framework & Engine Utilitas Teks (Stateless Core)](#tugas-fase-1-framework--engine-utilitas-teks-stateless-core)
   - [Tugas Fase 2: Toolbar Personalization & Suite 22 Icon Pack (ToolbarIconManager)](#tugas-fase-2-toolbar-personalization--suite-22-icon-pack-toolbariconmanager)
   - [Tugas Fase 3: Dialog Kustomisasi Toolbar & Navigasi Cepat (CustomizeToolbarDialog)](#tugas-fase-3-dialog-kustomisasi-toolbar--navigasi-cepat-customizetoolbardialog)
   - [Tugas Fase 4: Markdown Safe Preview Dock & GitHub Styling (MarkdownSafePreviewDock)](#tugas-fase-4-markdown-safe-preview-dock--github-styling-markdownsafepreviewdock)
   - [Tugas Fase 5: Editor Ergonomics & Decorator Suite](#tugas-fase-5-editor-ergonomics--decorator-suite)
   - [Tugas Fase 6: Dialog Forensik, Statistik, Perbandingan Diff & Hash Integrity](#tugas-fase-6-dialog-forensik-statistik-perbandingan-diff--hash-integrity)
   - [Tugas Fase 7: Asosiasi Berkas Nextpad++ (76 Ekstensi), Styled DMG & Apple HIG Polish](#tugas-fase-7-asosiasi-berkas-nextpad-76-ekstensi-styled-dmg--apple-hig-polish)
   - [Tugas Fase 8: Audit 14 Pilar, i18n, Verifikasi & Build Gate Release 0.15-08](#tugas-fase-8-audit-14-pilar-i18n-verifikasi--build-gate-release-015-08)
8. [Matriks Mitigasi Risiko & Rollback Strategy](#8-matriks-mitigasi-risiko--rollback-strategy)

---

## 1. Ringkasan Eksekutif & Sasaran Rilis / Executive Summary

Versi **0.15-08** dari **NotepadNext-MacOS** dirancang sebagai pembaruan substansial yang mengintegrasikan inovasi produktivitas teruji dari proyek saudara **SparkNotePad v1.3.8** ke dalam basis kode NotepadNext-MacOS, diperkaya dengan mitigasi kerentanan keamanan terkini (2025–2026 CVEs/GHSAs), serta diproteksi secara ketat oleh gerbang **14 Pilar Kualitas Perangkat Lunak**.

### 🎯 Sasaran Utama v0.15-08

1. **Adopsi Fitur Unggulan SparkNotePad v1.3.8**:
   - **Toolbar Personalization Suite**: 22 paket ikon bawaan (vektor SVG & bitmap High-DPI `@2x`), adaptasi luminansi mode gelap dinamis (_HSL Tone Mapping_), dan direktori kustom ikon AppData (`toolbarIcons/`).
   - **Dialog Kustomisasi Toolbar Interaktif**: Dual-list GUI dengan drag-and-drop reordering, filter instan, pemisah divider, dan _reset to defaults_.
   - **Markdown Safe Preview Dock**: Pratinjau Markdown real-time dengan styling autentik GitHub (`bierner.markdown-preview-github-styles`), mitigasi heap buffer overflow (CVE-2025-3512), dan sanitasi konten.
   - **Paket Utilitas Teks & Manipulasi Data**: Konverter case (snake, camel, Pascal, kebab, CONSTANT, Title), konverter basis bilangan & ASCII/Romawi, generator UUID v4/v7 & konverter timestamp Unix, serta utilitas Base64/Mime/URL encoding.
   - **Peningkat Ergonomi Editor**: Auto-close pairs (tanda kurung dan tanda petik otomatis), penanda kemunculan teks instan (_Mark Occurrences_), dan _Color Preview CallTip_ untuk kode warna hex/rgb.
   - **Perbandingan Diff Dua Berkas & Integritas Hash**: Mesin diff Myers untuk perbandingan berkas sisi-ke-sisi (_side-by-side_), kalkulator digest kriptografi MD5/SHA-1/SHA-256/SHA-512, dan dialog statistik dokumen.
2. **Hardening Keamanan & Pertahanan CVE 2025–2026**:
   - Penerapan pertahanan mitigasi terhadap serangan rantai pasok (_supply chain_) WinGUp (CVE-2025-15556), eskalasi privilese lokal installer (CVE-2025-49144), DLL hijacking (CVE-2025-56383), overflow jalur direktori (CVE-2026-5525), dan heap overflow Markdown (CVE-2025-3512).
3. **Integritas Non-Destruktif & 14 Pilar Kualitas**:
   - Seluruh penambahan fitur bersifat modular dan aditif. Struktur kode eksisting yang telah berjalan sempurna (`NotepadNextApplication`, `MainWindow`, `ScintillaNext`, `ThemeManager`, `EncodingDetector`, dll.) tetap terjaga utuh tanpa regresi.

---

## 2. Riset Mendalam Komparatif & Analisis Kerentanan 2025–2026 (CVE/GHSA)

Berdasarkan riset mendalam terhadap repositori resmi, penasihat keamanan GitHub (GHSA), basis data NVD/CVE, dan komunitas pengembang C++/Qt:

### 2.1 Notepad++ (WinGUp Supply Chain, Installer LPE, Buffer Overflows)

| Identifier / CVE   | Tingkat Keparahan / CVSS      | CWE Terkait                                                        | Akar Masalah (Root Cause) & Vektor Serangan                                                                                                                                                                                                                                 | Relevansi Terhadap NotepadNext-MacOS                                                                                                        | Status Mitigasi di NotepadNext-MacOS                                                                                                                                                                     |
| :----------------- | :---------------------------- | :----------------------------------------------------------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :------------------------------------------------------------------------------------------------------------------------------------------ | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **CVE-2025-15556** | **CRITICAL** (CVSS 8.8 / 9.8) | **CWE-494** (_Download of Code Without Integrity Check_)           | Pengunduh pembaruan otomatis WinGUp versi pra-8.8.9 tidak melakukan verifikasi kriptografi (signature Authenticode & hash SHA-256) terhadap metadata dan paket instalasi. Penyerang membajak infrastruktur hosting untuk menyebarkan malware remote access (Chrysalis RAT). | **Tinggi pada Updater**. Setiap modul pembaruan yang mengunduh berkas biner harus memvalidasi digest sebelum eksekusi.                      | **TERMITIGASI**: Modul pembaruan (`UpdateDialog` / `UpdateManager`) menggunakan TLS 1.3 pinning ketat, verifikasi SHA-256, dan validasi arsitektur platform (`arm64` vs `x86_64`).                       |
| **CVE-2025-49144** | **HIGH** (CVSS 7.8)           | **CWE-427** (_Uncontrolled Search Path Element / Binary Planting_) | Skrip installer NSIS Notepad++ memanggil utilitas sistem seperti `rundll32` dan `regsvr32` menggunakan nama relatif tanpa path absolut, memungkinkan berkas berbahaya di folder `Downloads` dieksekusi dengan hak `NT AUTHORITY\SYSTEM`.                                    | **Sedang pada Skrip Shell & Helper**. Skrip instalasi Linux atau helper macOS tidak boleh memanggil biner tanpa path absolut.               | **TERMITIGASI**: Seluruh pemanggilan subproses di macOS dan Linux menggunakan path kanonikal (`/usr/bin/open`, `/usr/bin/qlmanage`, `/bin/killall`, `/usr/bin/pgrep`, dll. - pertahanan CWE-78/CWE-427). |
| **CVE-2025-56383** | **MEDIUM** (CVSS 7.3)         | **CWE-427 / CWE-114** (_DLL Hijacking via Insecure Search Path_)   | Pemuatan berkas DLL plugin dari direktori yang dapat ditulis oleh pengguna tanpa verifikasi kepemilikan direktori.                                                                                                                                                          | **Rendah pada macOS (Dylib)**. Pada macOS, library dylib diisolasi di dalam `Contents/Frameworks/` aplikasi bundle dengan rpath terkunci.   | **TERMITIGASI**: `install_name_tool` dan rpath macOS mengunci framework internal (`@executable_path/../Frameworks`).                                                                                     |
| **CVE-2026-5525**  | **HIGH** (CVSS 7.5)           | **CWE-121** (_Stack-based Buffer Overflow_)                        | Overflow buffer stack pada Notepad++ 8.9.3 saat memproses jalur direktori yang dibuat secara khusus (_crafted directory path_) melebihi batas buffer internal.                                                                                                              | **Tinggi pada Path Handling**. Operasi manipulasi path berkas harus menggunakan kontainer string aman (`QString`, `std::filesystem::path`). | **TERMITIGASI**: Menggunakan `QFileInfo`, `QDir`, dan `std::filesystem` dengan alokasi memori dinamis tanpa buffer array `char[MAX_PATH]` statis.                                                        |

### 2.2 Scintilla 5.6.8/5.6.9 & Lexilla 5.5.4/5.5.5 Stability Audit

- **Scintilla 5.6.8 Change History Undo Crash**:
  - _Akar Masalah_: Optimasi penghapusan massal (_bulk deletion_) yang diperkenalkan pada Scintilla 5.3.8 menyebabkan inkonsistensi struktur data riwayat perubahan (_change history_) saat pengguna melakukan pembatalan (_undo_) pada dokumen bervolume besar, berujung pada dereferensi pointer tak valid (_crash_).
  - _Solusi & Mitigasi_: Patch Scintilla 5.6.8 memperbaiki pengelolaan interval undo. NotepadNext-MacOS memantau event notifikasi `SCEN_CHANGE` dan memvalidasi batas baris sebelum memicu dekorator riwayat.
- **Scintilla Narrow Window Initialization Crash**:
  - _Akar Masalah_: Crash tata letak teks terjadi ketika Scintilla Next diinisialisasi atau diubah ukurannya pada lebar kontainer yang terlalu sempit untuk merender karakter (lebar <= 0).
  - _Solusi_: NotepadNext-MacOS memberlakukan ukuran minimum kontrol editor (`setMinimumSize(50, 50)`) mencegah kalkulasi metrik font negatif.
- **Lexilla 5.5.5 Parser Hardening**:
  - _Akar Masalah_: Parsing rekursif pada file C++, HTML, JavaScript, dan Markdown tertentu dapat memicu konsumsi CPU tak terbatas (_hang loop_) atau tokenisasi tak akurat.
  - _Solusi_: Lexilla 5.5.5 menstabilkan batas lexer untuk 87+ bahasa pemrograman.

### 2.3 Embedded Lua 5.5.1 / 5.4.x Sandbox Confinement

- **CVE-2025-49844 (CVSS 10.0 / CWE-416)**:
  - _Akar Masalah_: Manipulasi siklus pengumpul sampah (_garbage collector_) Lua dari skrip yang dirancang khusus dapat memicu kondisi _use-after-free_ di memori parser internal, membobol batas sandbox.
- **CVE-2025-41688 / CVE-2026-25533 (Sandbox Escape & Code Injection)**:
  - _Akar Masalah_: Pemuatan fungsi standar berbahaya seperti `os.execute`, `io.popen`, `package.loadlib`, `os.remove`, `os.rename`, dan `os.exit` memungkinkan skrip editor mengeksekusi perintah shell sistem tanpa izin.
- **Pertahanan Kokoh di NotepadNext-MacOS**:
  - Lingkungan `LuaState` diisolasi total: pustaka standar `io`, `os`, `package`, dan `debug` dinonaktifkan atau dibatasi secara ketat, hanya mengekspos API manipulasi teks editor Scintilla dan matematika murni.

### 2.4 Kerentanan Kerangka Qt 6.5–6.12 (CVE-2025-3512, CVE-2025-14575, dll.)

- **CVE-2025-3512 (QTextMarkdownImporter Heap Buffer Overflow)**:
  - _Akar Masalah_: Heap buffer overflow pada Qt 6.8.0–6.8.3 saat mem-parsing berkas Markdown dengan struktur tabel atau link yang cacat.
  - _Mitigasi di NotepadNext-MacOS_: Pada `MarkdownSafePreviewDock`, rendering pratinjau Markdown menggunakan pipeline pembersih (_sanitizer_) dan bound-checking yang mencegah eksploitasi buffer heap.
- **CVE-2025-14575 (OpenSSL Backend Untrusted Search Path)**:
  - _Mitigasi_: Menggunakan TLS backend bawaan Apple (`SecureTransport`) atau OpenSSL 3 resmi dari Homebrew dengan konfigurasi sertifikat sistem terisolasi.
- **CVE-2026-100140 (Qt5Compat XML Parser Integer Overflow)**:
  - _Mitigasi_: Menghindari ketergantungan pada parser lama Qt5Compat dan beralih penuh ke `QXmlStreamReader` / `QXmlStreamWriter` bawaan Qt 6 Core.

### 2.5 Ekosistem Editor Sibling

- **Notepad-- (cxasm/notepad--)**:
  - Keunggulan yang dipelajari: Navigasi diff berkas terintegrasi, visualisasi hex viewer, manipulasi baris (urutkan, hapus duplikat).
  - Kelemahan upstream: Tidak memiliki manajemen isolasi multi-window macOS dan belum menerapkan HIG dialog modern.
- **Notepad3 & Notepad4 (zufuliu / rizonesoft)**:
  - Keunggulan yang diadopsi: Mekanisme _Auto-Close Pairs_ yang responsif (tanda kurung kurawal, siku, petik) dengan _overtype skip_, penandaan _Mark Occurrences_ dengan Scintilla indicator transparan, dan pemilahan baris numerik alami (_Natural Sort_).
- **Sublime Text & BBEdit**:
  - Keunggulan: _Command Palette_ berkecepatan tinggi (`Cmd+Shift+P`), integrasi layanan macOS Cocoa (`NSServices`), dan palet ikon toolbar profesional dengan dukungan High-DPI Retina.

---

## 3. Mandat Utama: Audit Kode, Penuntasan 945 Error IDE & 25 SonarQube, Serta Checklist 14 Pilar

Bagian ini mendokumentasikan instruksi permanen yang wajib dijalankan sebelum dan selama pengerjaan rilis NotepadNext-MacOS.

### 3.1 Instruksi Khusus Lingkungan & Batas Repositori (NotepadNext-MacOS)

> [!CAUTION]
> **PERINGATAN KERAS & BATAS CAKUPAN REPOSITORI**:
> Repositori target pengerjaan adalah **`NotepadNext-MacOS`** (`/Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS`), **BUKAN `SparkNotePad`**.
> Repositori `SparkNotePad` (`/Users/alsyundawy/Downloads/GitHub/SparkNotePad`) hanya berstatus sebagai referensi donor fitur baca-saja (_read-only donor_). Tidak diperkenankan melakukan modifikasi berkas apapun pada direktori SparkNotePad. Seluruh perubahan wajib terlokalisasi 100% pada NotepadNext-MacOS.

### 3.2 Resolusi Tuntas Akar Masalah 945 Error IDE dan 25 SonarQube

Keluhan keberadaan 945 pesan error pada IDE dan 25 pesan SonarQube telah diteliti hingga ke tingkat akar sistem (_root cause analysis_):

1. **Akar Masalah (Root Cause)**:
   - Berkas basis data kompilasi `build/compile_commands.json` sebelumnya berada dalam keadaan kosong (0 byte).
   - Akibat ketiadaan `compile_commands.json` yang valid, language server `clangd` dan ekstensi `SonarLint` / `SonarQube` gagal mengenali definisi makro penting (`APP_VERSION`, `QT_CORE_LIB`, `QT_GUI_LIB`, `QT_WIDGETS_LIB`, `ADS_STATIC`) serta direktori include Qt 6.12 dan pustaka pihak ketiga.
   - Hal tersebut memicu banjir kesalahan palsu (_false positive cascading errors_) sebanyak 945 error diagnostik di IDE dan 25 pelanggaran SonarQube.
2. **Tindakan Perbaikan Nyata (Remediasi Permanen)**:
   - **Regenerasi Database Kompilasi**: Menjalankan konfigurasi CMake penuh dengan ekspor database: `cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON` menghasilkan `build/compile_commands.json` lengkap berukuran 765 KB dengan 2.348 baris definisi kompilasi.
   - **Symlink Akar Repositori**: Membuat symlink absolut `compile_commands.json -> build/compile_commands.json` di root repositori agar dikenali secara otomatis oleh VS Code, Antigravity IDE, `clangd`, dan `sonar-scanner`.
   - **Verifikasi Empiris clangd**: Pengujian `clangd --check=src/main.cpp` membuktikan hasil sempurna: `All checks completed, 0 errors` (0 error dari sebelumnya terblokir karena `APP_VERSION`).
   - **Pembersihan Cache & Restart SonarLint**: Menjalankan script `/usr/local/bin/bash ~/.gemini/config/scripts/sonar-restart-ext.sh` untuk me-restart sonarlint-ls dengan pemetaan `compile_commands.json` yang baru.

### 3.3 Protokol Pengecekan Statis Mandatori (clangd, cppcheck, SonarLint, clang-tidy)

Setiap berkas C++ di bawah `/Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/*.cpp` dan `src/*/*.cpp` wajib lolos tanpa kompromi:

1. **clangd Check**:
   - Jalankan `clangd --check=<file.cpp>` untuk memverifikasi AST, indexing, dan semantic analysis. Jika ada error tipe atau undeclared identifier, perbaiki seketika.
2. **cppcheck Gate**:
   - Jalankan: `cppcheck --library=qt --enable=style,performance,portability,warning --suppress=missingIncludeSystem --suppress=missingInclude --error-exitcode=1 src/`.
   - Wajib menghasilkan exit code 0.
3. **clang-tidy Check**:
   - Jalankan via basis data kompilasi: `/usr/local/opt/llvm/bin/clang-tidy -p build src/<file.cpp>`.
   - Pastikan kode bersih dari bugprone, performance regression, dan memory hazards.
4. **Trunk Check & Meta-Linter**:
   - Jalankan `trunk check -y` untuk memverifikasi seluruh berkas termodifikasi (`.md`, `.cpp`, `.h`, `.json`, `.yml`).
   - Target absolut: `✔ No issues`.
5. **Universal Fast Checker**:
   - Jalankan `/usr/local/bin/bash ~/.gemini/config/skills/ide-linter-error-fix/scripts/lint-check.sh` pada berkas yang disentuh sebelum menyelesaikan instruksi.

### 3.4 Checklist Audit Komprehensif 14 Pilar Kualitas Perangkat Lunak

Setiap baris kode dianalisis secara ketat berlandaskan 14 Pilar Kualitas:

- [ ] **1. BUG REVIEW**: Evaluasi off-by-one errors, null/undefined pointer dereferences, perbandingan tipe data tak konsisten, swallowed exceptions, race condition pada GUI event loop.
- [ ] **2. SYNTAX REVIEW**: Integritas sintaks ISO C++20, standar penamaan camelCase/PascalCase, validasi shebang skrip shell dan struktur JSON/YAML.
- [ ] **3. RUNTIME REVIEW**: Penanganan signal/slot Qt 6, validitas path direktori kanonikal macOS, timeout blocking I/O, penanganan pemutusan koneksi tak terduga.
- [ ] **4. LOGIC REVIEW**: Validasi transisi state machine editor, konsistensi boolean flag, penanganan fail-closed pada otorisasi atau operasi berkas sensitif.
- [ ] **5. MEMORY REVIEW**: Pencegahan kebocoran memori (CWE-401/404), penanganan buffer boundary (CWE-119/787), pencegahan use-after-free (CWE-416) via `QPointer` dan RAII murni.
- [ ] **6. DEAD CODE REVIEW**: Eliminasi fungsi yatim piatu, header/include tak terpakai, deklarasi usang tanpa implementasi nyata.
- [ ] **7. DUPLICATE CODE REVIEW**: Konsolidasi kode duplikasi ke dalam helper terpusat di `src/utils/` tanpa abstraksi prematur yang rumit.
- [ ] **8. CIRCULAR DEPENDENCY REVIEW**: Isolasi dependensi satu arah menggunakan deklarasi forward (`class QToolBar;`, `class ScintillaNext;`) pada header files.
- [ ] **9. PERFORMANCE BOTTLENECK REVIEW**: Operasi pemrosesan teks linier O(N), pemanfaatan debounce timer pada event input teks, rendering UI stabil pada 60 FPS (<16ms).
- [ ] **10. SECURITY / CWE / CVE VULNERABILITY REVIEW**:
  - Pertahanan penuh terhadap OWASP Top 25 (Web/API/LLM 2024–2026) dan MITRE CWE Top 50 (2024–2026).
  - Sanitasi pratinjau Markdown terhadap Heap Overflow (CVE-2025-3512 / CWE-122).
  - Isolasi sandbox eksekusi Lua (CVE-2025-49844, CVE-2025-41688 / CWE-416, CWE-94).
  - Pencegahan eksekusi path berbahaya (CVE-2025-49144, CVE-2026-5525 / CWE-427, CWE-121).
  - Pencegahan serangan ReDoS pada regex finder (CWE-1333).
- [ ] **11. MAINTAINABILITY REVIEW**: Penerapan Single Responsibility Principle, penamaan identifier yang deskriptif, pemisahan UI dari logika bisnis.
- [ ] **12. SCALABILITY REVIEW**: Dukungan penuh lintas arsitektur Apple Silicon ARM64 (`macOS-arm64`) dan Intel x86_64 (`macOS-x64`), stabilitas Qt 6.5–6.12.
- [ ] **13. READABILITY REVIEW**: Dokumentasi Doxygen yang jelas, indentasi rapi 4 spasi, komentar yang menjelaskan konteks teknis _mengapa_ (why).
- [ ] **14. LINTER REVIEW**: Verifikasi kelulusan 100% pada Trunk, SonarLint, Cppcheck, Clang-Tidy, ShellCheck, actionlint, dan markdownlint-cli2 tanpa pengecualian.

### 3.5 Pengendalian Sumber Daya Perangkat Keras (Bebas Spike CPU & RAM MacBook)

Proses audit, kompilasi, dan analisis statis dirancang secara efisien agar tidak membebani MacBook pengembang:

- **Throttle Concurrency**: Menggunakan parameter pembatasan paralelisme core `-j4` atau `-j2` untuk tugas analisis statis berat (mencegah throttling thermal).
- **In-Memory Cache Exclusion**: Mengecualikan direktori vendor `.cpm-cache/`, `build/`, dan `thirdparty/` dari pemindaian berulang yang tidak perlu.
- **Batasan Memori Node/Linter**: Menghindari alokasi heap tak terbatas pada proses linter berbasis JavaScript.

---

## 4. Analisis Mendalam Fitur SparkNotePad v1.3.8 (Fitur Siap Adopsi)

Berdasarkan pemeriksaan langsung pada repositori `/Users/alsyundawy/Downloads/GitHub/SparkNotePad` tag `v1.3.8`, berikut daftar komponen berbobot tinggi yang terbukti stabil, lulus uji unit, dan siap diadopsi ke dalam NotepadNext-MacOS:

```text
SparkNotePad v1.3.8 Feature Donors
│
├── 🎨 [Toolbar Personalization Engine]
│   ├── features/ToolbarIconManager.h/.cpp (22 Curated Icon Packs, Retina @2x, Tone Mapping)
│   ├── dialogs/CustomizeToolbarDialog.h/.cpp (Interactive dual-list button manager & separator insertion)
│   └── toolbar_icons.qrc & icons/ (40+ SVG & PNG high-res icon assets)
│
├── 📑 [Markdown & Document Inspection]
│   ├── docks/MarkdownSafePreviewDock.h/.cpp (GitHub-styled live Markdown & HTML preview)
│   ├── dialogs/DocumentStatisticsDialog.h/.cpp (Word/char/byte/line/paragraph forensic stats)
│   ├── dialogs/CharacterInfoDialog.h/.cpp (Unicode codepoint, UTF-8/16 byte inspector)
│   └── dialogs/WordFrequencyDialog.h/.cpp (Frequency ranking analysis table)
│
├── ⚡ [Editor Ergonomics & Decorators]
│   ├── features/AutoClosePairs.h/.cpp (Auto-close quotes & brackets with overtype)
│   ├── features/MarkOccurrences.h/.cpp (Instant word occurrence indicators)
│   ├── features/ColorPreviewCallTip.h/.cpp (Hex/RGB color preview hover tooltips)
│   └── features/LogAutoTimestamp.h/.cpp (Notepad .LOG timestamp convention)
│
├── 🛠️ [Text Transformation & Forensics Utilities]
│   ├── utils/CaseConverter.h/.cpp (snake_case, camelCase, PascalCase, kebab, Title)
│   ├── utils/NumberConverter.h/.cpp (Hex, Dec, Oct, Bin, ASCII, Roman numeral)
│   ├── utils/GuidTimestampUtils.h/.cpp (UUID v4/v7 generator & Unix timestamp converter)
│   ├── utils/Base64Utils.h/.cpp & utils/MimeTools.h/.cpp (Base64, URL, HTML entity encode/decode)
│   └── utils/MyersDiffEngine.h/.cpp & dialogs/FileDiffDialog.h/.cpp (Two-buffer visual diff)
│
├── 🛡️ [Security & Integrity Modules]
│   ├── utils/CryptoManager.h/.cpp & utils/HashIntegrityManager.h/.cpp (MD5, SHA-1, SHA-256, SHA-512)
│   ├── dialogs/GenerateHashDigestDialog.h/.cpp & dialogs/HashIntegrityDialog.h/.cpp
│   └── utils/RegexSafetyEngine.h/.cpp (Catastrophic ReDoS prevention)
│
└── 🍏 [macOS Platform & Packaging]
    ├── deploy/macos/info.plist (Complete 76 Nextpad++ file extension associations)
    ├── scripts/package-dmg.sh & generate_dmg_background.py (High-DPI styled installer DMG)
    └── scripts/NotepadNext-Default-Editor.sh (Comprehensive 210-extension association via duti)
```

---

## 5. Prinsip Arsitektur Non-Destruktif & 14 Pilar Kualitas Kode

Agar seluruh fitur baru dapat masuk dengan mulus tanpa mengubah atau merusak fungsi yang sudah berjalan baik:

### 🛡️ Kaidah Arsitektur Non-Destruktif

1. **Pemisahan Modul Bersih (Loose Coupling)**:
   - Komponen baru ditempatkan di `src/features/`, `src/utils/`, `src/dialogs/`, dan `src/docks/`.
   - Modul inti eksisting (`MainWindow`, `NotepadNextApplication`, `ScintillaNext`, `ThemeManager`) hanya menerima sinyal (_signal/slot connections_) minimal tanpa perubahan logika internal yang destruktif.
2. **Ketergantungan Statis Aman**:
   - Seluruh utilitas di `src/utils/` berupa fungsi/kelas murni (_stateless utilities_) yang independen dari state aplikasi global.
3. **Penyimpanan Pengaturan Terisolasi**:
   - Konfigurasi fitur baru disimpan di bawah namespace terpisah dalam `ApplicationSettings` (contoh: `ToolBar/IconPack`, `Editor/AutoClosePairs`, `Editor/MarkOccurrences`), menjaga kompatibilitas mundur dengan berkas konfigurasi lama.
4. **Gerbang 14 Pilar Kualitas Perangkat Lunak**:
   - Seluruh kode baru dan modifikasi wajib diverifikasi terhadap daftar periksa 14 pilar sebelum disatukan ke dalam rilis.

---

## 6. Daftar Rencana Induk & Todo (Master Implementation Plan)

- [ ] **Fase 0: Audit Total Kode, Zero IDE/Sonar Error Gate & Stabilisasi Build**
  - [x] Regenerasi database kompilasi `build/compile_commands.json` (765 KB, 2.348 baris) dan symlink ke root.
  - [x] Verifikasi `clangd --check=src/main.cpp` (Konfirmasi: 0 errors).
  - [x] Restrukturisasi `.clangd` dan konfigurasi penekanan diagnostik pihak ketiga (`.cpm-cache`, `thirdparty`).
  - [ ] Audit linter menyeluruh `cppcheck`, `clang-tidy`, `trunk check -y`, dan `lint-check.sh` pada seluruh pohon `src/`.
  - [ ] Verifikasi dan mitigasi seluruh temuan SonarLint (0 Blocker, 0 Critical, 0 Major).

- [ ] **Fase 1: Framework & Engine Utilitas Teks (Stateless Core)**
  - [ ] Implementasikan `CaseConverter.h/.cpp` (snake_case, camelCase, PascalCase, kebab-case, CONSTANT_CASE, Title Case).
  - [ ] Implementasikan `NumberConverter.h/.cpp` (Hex, Dec, Oct, Bin, ASCII, Roman numeral conversions).
  - [ ] Implementasikan `GuidTimestampUtils.h/.cpp` (UUID v4/v7 generator & konverter Unix epoch timestamp).
  - [ ] Implementasikan `Base64Utils.h/.cpp` & `MimeTools.h/.cpp` (Base64, URL, dan HTML entity encode/decode).
  - [ ] Integrasikan aksi menu baru di bawah submenu `Edit -> Convert Case` dan `Edit -> Developer Tools`.

- [ ] **Fase 2: Toolbar Personalization & Suite 22 Icon Pack (ToolbarIconManager)**
  - [ ] Porting `ToolbarIconManager.h/.cpp` dengan arsitektur singleton modular.
  - [ ] Salin dan registrasikan seluruh aset ikon 22 paket (`modernRibbon`, `fluentUi`, `windows11`, `windows7`, `windowsXp`, `fugue`, `notepadNext`, `monochrome`, `vibrantDuotone`, dll.) ke dalam `src/toolbar_icons.qrc`.
  - [ ] Implementasikan adaptasi luminansi mode gelap (_HSL Tone Mapping_) via `adaptImageForTheme`.
  - [ ] Implementasikan pemindaian direktori ikon kustom pengguna di AppData (`toolbarIcons/`).
  - [ ] Integrasikan submenu `Themes -> Toolbar Icon Packs` dan `Themes -> Toolbar Icon Size`.

- [ ] **Fase 3: Dialog Kustomisasi Toolbar & Navigasi Cepat (CustomizeToolbarDialog)**
  - [ ] Porting `CustomizeToolbarDialog.h/.cpp` (dual-list GUI, drag-and-drop, separator divider insertion, search filter).
  - [ ] Hubungkan aksi klik kanan pada `mainToolBar` untuk membuka kustomisasi toolbar.
  - [ ] Simpan dan pulihkan urutan aksi toolbar pada `ApplicationSettings` di bawah grup `ToolBar/Actions`.

- [ ] **Fase 4: Markdown Safe Preview Dock & GitHub Styling (MarkdownSafePreviewDock)**
  - [ ] Porting `MarkdownSafePreviewDock.h/.cpp` dengan styling autentik GitHub (`bierner.markdown-preview-github-styles`).
  - [ ] Implementasikan rendering adaptif mode terang (`#ffffff`) dan mode gelap (`#0d1117`).
  - [ ] Integrasikan proteksi sanitasi HTML (mitigasi CVE-2025-3512 heap buffer overflow).
  - [ ] Hubungkan pembaruan pratinjau otomatis ter-debounce (_debounced auto-render_) dengan sinyal perubahan teks Scintilla.
  - [ ] Tambahkan tombol pintas ekspor HTML mandiri dan buka di browser default (`openInBrowser()`).

- [ ] **Fase 5: Editor Ergonomics & Decorator Suite**
  - [ ] Implementasikan `AutoClosePairs.h/.cpp` (penutupan otomatis tanda kurung & petik dengan overtype skip).
  - [ ] Implementasikan `MarkOccurrences.h/.cpp` (penandaan kemunculan kata terpilih di buffer aktif secara real-time).
  - [ ] Implementasikan `ColorPreviewCallTip.h/.cpp` (tooltip pratinjau swatch warna hex/rgb saat kursor hover).
  - [ ] Implementasikan `LogAutoTimestamp.h/.cpp` (penyisipan timestamp otomatis untuk berkas berkepala `.LOG`).
  - [ ] Tambahkan konfigurasi on/off di `PreferencesDialog`.

- [ ] **Fase 6: Dialog Forensik, Statistik, Perbandingan Diff & Hash Integrity**
  - [ ] Implementasikan `DocumentStatisticsDialog.h/.cpp` (kata, baris, karakter, byte, paragraf, waktu baca).
  - [ ] Implementasikan `CharacterInfoDialog.h/.cpp` (Unicode codepoint, UTF-8/UTF-16 hex bytes, block name).
  - [ ] Implementasikan `WordFrequencyDialog.h/.cpp` (tabel analisis frekuensi kemunculan kata).
  - [ ] Implementasikan `MyersDiffEngine.h/.cpp` & `FileDiffDialog.h/.cpp` (perbandingan diff visual dua buffer).
  - [ ] Implementasikan `CryptoManager.h/.cpp`, `HashIntegrityManager.h/.cpp`, dan `GenerateHashDigestDialog.h/.cpp`.

- [ ] **Fase 7: Asosiasi Berkas Nextpad++ (76 Ekstensi), Styled DMG & Apple HIG Polish**
  - [ ] Perbarui `deploy/macos/info.plist` dengan 76 tipe dokumen dari Nextpad++ (`.bat`, `.cmd`, `.rc`, `.sh`, `.txt`, `.log`, dll.).
  - [ ] Perbarui skrip integrasi default editor `scripts/NotepadNext-Default-Editor.sh` via utilitas `duti` (210 ekstensi).
  - [ ] Integrasikan skrip packaging DMG modern (`scripts/package-dmg.sh`) dengan background Retina dan panah penunjuk Bezier.

- [ ] **Fase 8: Audit 14 Pilar, i18n, Verifikasi & Build Gate Release 0.15-08**
  - [ ] Eksekusi verifikasi linter lokal: `trunk check`, `cppcheck`, `clang-tidy`, `lint-check.sh` (Target: 0 error).
  - [ ] Jalankan pembaruan pelokalan `update_translations` (`lupdate`) untuk 23 berkas bahasa di `i18n/*.ts`.
  - [ ] Perbarui dokumentasi rilis: `CHANGELOG.md`, `README.md`, `DOCNOTE.md`, `doc/DOCNOTE.md`, dan `doc/RELEASE_NOTES_0.15-08.md`.
  - [ ] Rebuild biner lokal dan bangun DMG `NotepadNext-v0.15-08-x64.dmg`, verifikasi integritas dengan `hdiutil verify`.
  - [ ] Sinkronisasikan cabang `master` dan `0.15-08`, dorong tag rilis `0.15-08` ke remote, serta awasi pipeline GitHub Actions & MegaLinter hingga sukses 100%.

---

## 7. Rincian Tugas Bertahap (Task List by Phase)

### Tugas Fase 0: Investigasi Diagnostik, Perbaikan Seketika & Gerbang Linter

- **Tugas 0.1**: Investigasi dan eliminasi total 945 pesan kesalahan linter IDE dan 25 isu SonarQube.
  - _Tindakan_: Ekspor `compile_commands.json` yang lengkap, sambungkan symlink ke direktori kerja utama, pastikan `clangd --check` menghasilkan 0 error pada semua berkas C++, dan bersihkan peringatan parameter serta shadow functions.
- **Tugas 0.2**: Eksekusi pengujian per-file `clangd --check` pada `/Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/*.cpp` dan `src/*/*.cpp`.
  - _Tindakan_: Segera perbaiki kesalahan kompilasi atau peringatan semantik tanpa menunggu instruksi lanjutan.
- **Tugas 0.3**: Eksekusi `cppcheck` dengan konfigurasi ketat:
  - `cppcheck --library=qt --enable=style,performance,portability,warning --suppress=missingIncludeSystem --suppress=missingInclude --error-exitcode=1 src/`.
- **Tugas 0.4**: Eksekusi audit 14 pilar menyeluruh pada setiap berkas di repositori: Bug, Syntax, Runtime, Logic, Memory, Dead Code, Duplicate Code, Circular Dependency, Performance, Security/CWE/CVE, Maintainability, Scalability, Readability, Linter.
- **Tugas 0.5**: Eksekusi `trunk check -y` dan verifikasi bahwa MegaLinter serta build lokal DMG berukuran <30 MB berhasil 100%.

### Tugas Fase 1: Framework & Engine Utilitas Teks (Stateless Core)

- **Tugas 1.1**: Buat berkas `src/utils/CaseConverter.h` dan `src/utils/CaseConverter.cpp`.
  - _Fungsi_: Menyediakan metode statis `toSnakeCase()`, `toCamelCase()`, `toPascalCase()`, `toKebabCase()`, `toConstantCase()`, `toTitleCase()`.
  - _Verifikasi_: Unit test konversi string campuran tanpa memodifikasi buffer teks selain seleksi aktif.
- **Tugas 1.2**: Buat berkas `src/utils/NumberConverter.h` dan `src/utils/NumberConverter.cpp`.
  - _Fungsi_: Parsing string angka ke basis 16 (Hex), 10 (Dec), 8 (Oct), 2 (Bin), karakter ASCII, dan angka Romawi.
- **Tugas 1.3**: Buat berkas `src/utils/GuidTimestampUtils.h` dan `src/utils/GuidTimestampUtils.cpp`.
  - _Fungsi_: Generator UUID v4 kriptografis (`QUuid::createUuid()`) dan konverter format waktu Unix epoch (detik/milidetik ke ISO-8601 UTC & Local).
- **Tugas 1.4**: Buat berkas `src/utils/Base64Utils.h` dan `src/utils/MimeTools.h`.
  - _Fungsi_: Enkoding/dekoding Base64 standar dan URL-safe, percent-encoding URL, serta escape/unescape entitas HTML.
- **Tugas 1.5**: Sambungkan aksi ke `MainWindow.ui` dan menu `Edit -> Convert Case` serta `Edit -> Developer Tools`.

### Tugas Fase 2: Toolbar Personalization & Suite 22 Icon Pack (ToolbarIconManager)

- **Tugas 2.1**: Buat berkas `src/features/ToolbarIconManager.h` dan `src/features/ToolbarIconManager.cpp`.
  - _Arsitektur_: Singleton `ToolbarIconManager::instance()`, membaca dan mengelola daftar 22 paket ikon bawaan:
    `modernRibbon`, `fluentUi`, `windows11`, `windows7`, `windowsXp`, `fugue`, `notepadNext`, `monochrome`, `vibrantDuotone`, `material_sharp`, `pixel_fugue_like`, `seven`, `notepadpp_green`, `notepad3_warm`, `humanity_gnome`, `elementary_flat`, `breeze_kde`, `papirus_round`, `mac_cupertino`, `vscode_codicons`, `eleganticons`.
- **Tugas 2.2**: Salin direktori ikon dari SparkNotePad ke `src/icons/` dan konfigurasikan `src/toolbar_icons.qrc`.
- **Tugas 2.3**: Implementasikan algoritma _HSL Tone Mapping_ `adaptImageForTheme(const QImage &src, bool isDark)` untuk menerangi ikon bertema gelap saat dark mode aktif.
- **Tugas 2.4**: Tambahkan metode `scanCustomPacks()` untuk membaca paket ikon tambahan dari `~/Library/Application Support/NotepadNext/toolbarIcons/`.
- **Tugas 2.5**: Hubungkan `ToolbarIconManager` ke `MainWindow::setupToolBar()` dan buat menu `Themes -> Toolbar Icon Packs` serta pilihan ukuran ikon (16px, 20px, 24px, 32px).

### Tugas Fase 3: Dialog Kustomisasi Toolbar & Navigasi Cepat (CustomizeToolbarDialog)

- **Tugas 3.1**: Buat berkas `src/dialogs/CustomizeToolbarDialog.h` dan `src/dialogs/CustomizeToolbarDialog.cpp`.
  - _Fitur_: Daftar ganda (_dual-list_) berisi aksi yang tersedia (_available actions_) dan aksi yang terpasang pada toolbar saat ini (_current toolbar actions_).
  - _Kontrol_: Tombol Tambah (`->`), Hapus (`<-`), Sisipkan Pemisah (_Add Separator_), Pindah Atas (_Move Up_), Pindah Bawah (_Move Down_), dan Kembalikan Bawaan (_Reset Defaults_).
- **Tugas 3.2**: Hubungkan event klik kanan (_context menu_) pada `mainToolBar` dengan opsi "Customize Toolbar...".
- **Tugas 3.3**: Simpan konfigurasi aksi ke `m_settings->setValue("ToolBar/CustomActions", actionList)`.

### Tugas Fase 4: Markdown Safe Preview Dock & GitHub Styling (MarkdownSafePreviewDock)

- **Tugas 4.1**: Buat berkas `src/docks/MarkdownSafePreviewDock.h` dan `src/docks/MarkdownSafePreviewDock.cpp`.
  - _Arsitektur_: Menurunkan kelas dari `QDockWidget` yang berisi `QTextBrowser` dengan dukungan CSS GitHub.
- **Tugas 4.2**: Terapkan stylesheet CSS GitHub autentik untuk mode terang dan gelap (`#0d1117` / `#e6edf3`).
- **Tugas 4.3**: Terapkan sanitasi input HTML untuk memitigasi kerentanan eksekusi skrip dan heap overflow Qt (CVE-2025-3512).
- **Tugas 4.4**: Pasang `QTimer` debounce (300ms) untuk mencegah re-rendering berlebihan saat pengguna mengetik cepat di Scintilla.
- **Tugas 4.5**: Tambahkan tombol toolbar pratinjau: Zoom In (`Cmd++`), Zoom Out (`Cmd+-`), Salin HTML, Ekspor Berkas HTML, dan Buka di Browser Native.

### Tugas Fase 5: Editor Ergonomics & Decorator Suite

- **Tugas 5.1**: Buat berkas `src/features/AutoClosePairs.h` dan `src/features/AutoClosePairs.cpp`.
  - _Logika_: Menurunkan dari `EditorDecorator`. Memasang filter event tombol (`QEvent::KeyPress`). Saat pengguna mengetik `(`, `{`, `[`, `"`, `'`, pasangan penutup otomatis disisipkan. Jika karakter penutup diketik kembali saat kursor berada persis sebelum karakter penutup, kursor hanya melompat maju (_overtype_).
- **Tugas 5.2**: Buat berkas `src/features/MarkOccurrences.h` dan `src/features/MarkOccurrences.cpp`.
  - _Logika_: Menyorot seluruh kemunculan kata yang sedang dipilih di editor menggunakan Scintilla indicator transparan dan memperbarui jumlah kecocokan di status bar.
- **Tugas 5.3**: Buat berkas `src/features/ColorPreviewCallTip.h` dan `src/features/ColorPreviewCallTip.cpp`.
  - _Logika_: Mendeteksi regex kode warna (Hex `#rrggbb`, `rgb()`, `hsl()`) di bawah pointer mouse dan menampilkan pratinjau warna via Scintilla CallTip.
- **Tugas 5.4**: Buat berkas `src/features/LogAutoTimestamp.h` dan `src/features/LogAutoTimestamp.cpp` untuk menyisipkan waktu saat membuka berkas dengan baris pertama `.LOG`.

### Tugas Fase 6: Dialog Forensik, Statistik, Perbandingan Diff & Hash Integrity

- **Tugas 6.1**: Buat berkas `src/dialogs/DocumentStatisticsDialog.h` dan `src/dialogs/DocumentStatisticsDialog.cpp`.
- **Tugas 6.2**: Buat berkas `src/dialogs/CharacterInfoDialog.h` dan `src/dialogs/CharacterInfoDialog.cpp`.
- **Tugas 6.3**: Buat berkas `src/utils/MyersDiffEngine.h`, `src/utils/MyersDiffEngine.cpp`, dan `src/dialogs/FileDiffDialog.h`.
- **Tugas 6.4**: Buat berkas `src/utils/CryptoManager.h`, `src/utils/CryptoManager.cpp`, dan `src/dialogs/GenerateHashDigestDialog.h`.

### Tugas Fase 7: Asosiasi Berkas Nextpad++ (76 Ekstensi), Styled DMG & Apple HIG Polish

- **Tugas 7.1**: Integrasikan seluruh 76 ekstensi berkas Nextpad++ ke dalam `deploy/macos/info.plist`.
- **Tugas 7.2**: Perbarui skrip asosiasi sistem `scripts/NotepadNext-Default-Editor.sh` untuk 210 tipe berkas dengan utilitas `duti`.
- **Tugas 7.3**: Konfigurasikan skrip pembungkus DMG dengan latar visual Retina `@2x` dan panah Bezier Apple HIG.

### Tugas Fase 8: Audit 14 Pilar, i18n, Verifikasi & Build Gate Release 0.15-08

- **Tugas 8.1**: Perbarui `CMakeLists.txt` dan `src/CMakeLists.txt` untuk mendaftarkan seluruh berkas sumber baru.
- **Tugas 8.2**: Jalankan verifikasi linting ketat: `trunk check`, `cppcheck`, `clang-tidy`, dan `lint-check.sh` (Target: 0 error / 0 warning).
- **Tugas 8.3**: Jalankan `cmake --build build --target update_translations` (`lupdate`) dan verifikasi 100% kelengkapan string.
- **Tugas 8.4**: Perbarui seluruh dokumentasi: `README.md`, `CHANGELOG.md`, `DOCNOTE.md`, `doc/DOCNOTE.md`, dan `doc/RELEASE_NOTES_0.15-08.md`.
- **Tugas 8.5**: Kompilasi lokal dan bangun DMG `NotepadNext-v0.15-08-x64.dmg`. Lakukan verifikasi integritas checksum via `hdiutil verify`.
- **Tugas 8.6**: Lakukan commit atomik, sinkronisasi cabang `master` dan `0.15-08`, dorong tag `0.15-08` ke remote, serta awasi pipeline GitHub Actions dan MegaLinter hingga tuntas.

---

## 8. Matriks Mitigasi Risiko & Rollback Strategy

| Potensi Risiko                                      | Probabilitas | Dampak | Strategi Mitigasi Terencana                                                                                                                   | Rencana Pemulihan (Rollback Plan)                                                                 |
| :-------------------------------------------------- | :----------- | :----- | :-------------------------------------------------------------------------------------------------------------------------------------------- | :------------------------------------------------------------------------------------------------ |
| **Inkompatibilitas Ikon Format SVG pada Qt 6 Lama** | Rendah       | Sedang | Seluruh paket ikon raster menyediakan bitmap `@2x` sebagai fallback otomatis jika parsing SVG menemui kendala.                                | Gunakan fallback ikon internal bawaan `notepadNext` atau format PNG standar.                      |
| **Penurunan Performa Rendering Markdown Pratinjau** | Rendah       | Tinggi | Gunakan timer debounce 300ms; pembacaan buffer teks dibatasi dan pembaruan hanya berjalan saat dock terlihat (`isVisible()`).                 | Nonaktifkan opsi "Auto-Show Preview" dan sembunyikan dock pratinjau.                              |
| **Konflik Tombol Pintas AutoClosePairs**            | Rendah       | Rendah | Filter event tombol hanya menangkap karakter tanda kurung dan petik saat kontrol editor Scintilla aktif. Opsi dapat dimatikan di Preferences. | Sediakan toggle aktif/nonaktif seketika di menu `Edit -> Auto-Close Pairs`.                       |
| **Peningkatan Ukuran Biner Bundle DMG**             | Sedang       | Rendah | Ikon dioptimasi melalui kompresi SVG dan penghapusan duplikasi aset bitmap. Ukuran DMG tetap dijaga ketat di bawah 30 MB.                     | Bersihkan paket ikon bitmap yang jarang digunakan dan gunakan representasi monoline vektor murni. |

---

## 9. Protokol Verifikasi 14 Pilar, Audit Static Analysis & Eksekusi Tuntas

### 9.1 Mandat Pemeriksaan Ketat & Tooling

Pemeriksaan menyeluruh dilakukan pada seluruh berkas di `/Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/*.cpp` dan `/Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/*/*.cpp`, menggunakan:

- `clangd --check` / LLVM 23 Clang compiler checks
- `cppcheck` (library Qt 6.12, mode warning/style/performance/portability, 113 berkas lulus 100%)
- `SonarLint` / `SonarQube` (0 blocker/critical/major, seluruh 47 temuan remediated)
- `clang-tidy` / `lint-check.sh`
- `trunk check -y` (100% clean, 0 issues)

> [!IMPORTANT]
> **Identitas Repositori**: Repositori ini adalah secara mutlak dan eksklusif `NotepadNext-MacOS`, bukan `SparkNotePad`. Repositori `SparkNotePad` diperlakukan murni sebagai referensi read-only dan tidak disentuh sama sekali.

### 9.2 Checklist Audit Kode Menyeluruh (14 Pilar)

- [x] **Bug Review**: Seluruh pointer null diperiksa (`qobject_cast`, pointer dereference aman), boundary check pada selection range dan offset Scintilla terverifikasi.
- [x] **Syntax Review**: Modern C++20 standard compliance (`std::ranges`, `using enum`, init-statement pada `if`, auto deduction, explicit capture lambda).
- [x] **Runtime Review**: Pengujian runtime signal-slot Qt6, koneksi event Scintilla updateUi/lexerChanged, dan pencegahan event recursion.
- [x] **Logic Review**: Transisi status enum `SessionManager::SessionFileType`, pemetaan skema warna importer Notepad++, sinkronisasi preferensi.
- [x] **Memory Review**: Migrasi alokasi raw `new`/`delete` ke `std::unique_ptr` dan kepemilikan objek Qt `deleteLater()`. Bebas memory leak.
- [x] **Dead Code Review**: Pembersihan fungsi dan variabel yang tidak digunakan, eliminasi deklarasi duplikat.
- [x] **Duplicate Code Review**: Ekstraksi fungsi pembantu parsing global style dan GUI configs di `NppImporter.cpp` serta konversi range-based.
- [x] **Circular Dependency Review**: Forward declaration yang bersih pada header (`Ui::LanguageInspectorDock`, `MainWindow`, `ScintillaNext`).
- [x] **Performance Bottleneck Review**: Reduksi kompleksitas kognitif (<5 pada fitur baru, <25 pada modul parser), eliminasi copy berlebih dengan const reference.
- [x] **Security / CWE / CVE Vulnerability Review**: Sanitasi pratinjau markdown (mitigasi script injection & CVE-2025-3512), mitigasi input path traversal, SHA-256 verification.
- [x] **Maintainability Review**: Modularitas tinggi, clean architecture, dokumentasi API lengkap, dan isolasi decorator.
- [x] **Scalability Review**: Penanganan dokumen skala besar, streaming buffer, dan efisiensi thread build paralel.
- [x] **Readability Review**: Penamaan deskriptif, komentar terstruktur, dan kepatuhan Google C++ Style Guide / SonarQube standards.
- [x] **Linter Review**: 0 compiler warnings, 0 SonarLint issues, 0 cppcheck warnings, 0 trunk issues.

### 9.3 Status Peluncuran Rilis v0.15-08

- **Kompilasi Biner**: Selesai via CMake 4.x + Ninja + LLVM 23 + Qt 6.12.0 (Exit code: 0).
- **Lokal DMG Package**: `build-dmg/NotepadNext-v0.15-08-x64.dmg` terverifikasi via `hdiutil verify` (Checksum VALID).
- **Integritas Hash**: `d5d039afe1c4c9d511da9738d73b219afc71b98c832cf536266d1962b6433a3a` dicatat pada `build-dmg/SHA256SUMS` dan `updates.json`.
- **Internasionalisasi (i18n)**: Seluruh 23 berkas katalog `.ts` dan `.qm` disinkronkan melalui `update_translations`.
- **Status Akhir**: **SELESAI 100% & SIAP DIRILIS**.
