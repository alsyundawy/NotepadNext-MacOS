# 🍎 NotepadNext For MacOS — Notepad++ Like

> 🇬🇧 A cross-platform reimplementation of Notepad++ — built natively for macOS.
>
> 🇮🇩 Implementasi ulang Notepad++ lintas platform — dibangun secara native untuk macOS.

![Build Notepad Next](https://github.com/alsyundawy/NotepadNext-MacOS/workflows/Build%20Notepad%20Next/badge.svg)
[![Latest Version](https://img.shields.io/github/v/release/alsyundawy/NotepadNext-MacOS)](https://github.com/alsyundawy/NotepadNext-MacOS/releases)
[![Language: Indonesian & English](https://img.shields.io/badge/Language-ID%20%7C%20EN-blue.svg)](#localization--language-support)
[![Maintenance Status](https://img.shields.io/maintenance/yes/9999)](https://github.com/alsyundawy/NotepadNext-MacOS/)
[![License](https://img.shields.io/github/license/alsyundawy/NotepadNext-MacOS)](https://github.com/alsyundawy/NotepadNext-MacOS/blob/master/LICENSE)
[![GitHub Issues](https://img.shields.io/github/issues/alsyundawy/NotepadNext-MacOS)](https://github.com/alsyundawy/NotepadNext-MacOS/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/alsyundawy/NotepadNext-MacOS)](https://github.com/alsyundawy/NotepadNext-MacOS/pulls)
[![Donate with PayPal](https://img.shields.io/badge/PayPal-donate-003087?logo=paypal&logoColor=white)](https://www.paypal.me/alsyundawy)
[![Donate with Ko-fi](https://img.shields.io/badge/Ko--fi-donate-ff5e5b?logo=ko-fi&logoColor=white)](https://ko-fi.com/alsyundawy)
[![Donate with QRIS](https://img.shields.io/badge/QRIS-donate-red)](https://github.com/user-attachments/assets/a0126f28-6dde-43da-ba14-d7c9a27de0df)
[![Sponsor with GitHub](https://img.shields.io/badge/GitHub-sponsor-orange)](https://github.com/sponsors/alsyundawy)
[![GitHub Stars](https://img.shields.io/github/stars/alsyundawy/NotepadNext-MacOS?style=social)](https://github.com/alsyundawy/NotepadNext-MacOS/stargazers)
[![GitHub Forks](https://img.shields.io/github/forks/alsyundawy/NotepadNext-MacOS?style=social)](https://github.com/alsyundawy/NotepadNext-MacOS/network/members)
[![GitHub Contributors](https://img.shields.io/github/contributors/alsyundawy/NotepadNext-MacOS?style=social)](https://github.com/alsyundawy/NotepadNext-MacOS/graphs/contributors)

---

![image](https://github.com/user-attachments/assets/60f58d46-56e1-471d-a532-addc959f7c40)
![image](https://github.com/user-attachments/assets/d20a56fc-e371-45dd-bcf3-04895a8538e7)


---

## Overview

> 🇬🇧 **NotepadNext For MacOS** is a native macOS reimplementation of Notepad++ built with modern Qt 6. It provides fast editing performance, syntax highlighting across 264+ file formats, tabbed document workflows, Gatekeeper support, and native Indonesian & English localization.
>
> 🇮🇩 **NotepadNext For MacOS** adalah implementasi ulang native Notepad++ untuk macOS yang dibangun menggunakan Qt 6 modern. Aplikasi ini menghadirkan kecepatan tinggi, penyorotan sintaks untuk 264+ format file, manajemen multi-tab dokumen, integrasi sistem macOS, serta pelokalan penuh Bahasa Indonesia dan Bahasa Inggris.

---

## Quickstart

```bash
# 1. Berikan izin eksekusi skrip / Grant script execute permissions
chmod +x NotepadNext-Default-Editor.sh

# 2. Jadikan NotepadNext sebagai default editor macOS / Set as system default editor
./NotepadNext-Default-Editor.sh -f
```

---

## Installation

> 🇬🇧 NotepadNext application builds are available for macOS. You can download the latest build directly from the [Releases](https://github.com/alsyundawy/NotepadNext-MacOS/releases) page. Choose the latest release, then check the **Assets** section at the bottom to download the `.dmg` or `.zip` file matching your Mac architecture.
>
> 🇮🇩 Build aplikasi NotepadNext tersedia untuk macOS. Anda dapat mengunduh build terbaru langsung dari halaman [Releases](https://github.com/alsyundawy/NotepadNext-MacOS/releases). Pilih rilis terbaru, lalu lihat bagian **Assets** di bagian bawah untuk mendownload file `.dmg` atau `.zip` yang sesuai dengan arsitektur Mac Anda.

Available packages for Apple Silicon (`arm64`):

| Package / Paket | Architecture / Arsitektur | Minimum macOS | Qt Version / Versi Qt |
| :--- | :--- | :--- | :--- |
| `NotepadNext-macOS-arm64-Qt6.5.zip` | Apple Silicon (arm64) | macOS 12.0 Monterey | Qt 6.5 |
| `NotepadNext-macOS-arm64-Qt6.8.zip` | Apple Silicon (arm64) | macOS 13.0 Ventura | Qt 6.8 |
| `NotepadNext-macOS-arm64-Qt6.10.zip` | Apple Silicon (arm64) | macOS 14.0 Sonoma | Qt 6.10 |
| `NotepadNext-macOS-arm64-Qt6.11.zip` | Apple Silicon (arm64) | macOS 15.0 Sequoia | Qt 6.11 |

Available packages for Intel (`x86_64`):

| Package / Paket | Architecture / Arsitektur | Minimum macOS | Qt Version / Versi Qt |
| :--- | :--- | :--- | :--- |
| `NotepadNext-macOS-x64-Qt6.5.zip` | Intel (x86_64) | macOS 11.0 Big Sur | Qt 6.5 |
| `NotepadNext-macOS-x64-Qt6.8.zip` | Intel (x86_64) | macOS 12.0 Monterey | Qt 6.8 |
| `NotepadNext-macOS-x64-Qt6.10.zip` | Intel (x86_64) | macOS 13.0 Ventura | Qt 6.10 |
| `NotepadNext-macOS-x64-Qt6.11.zip` | Intel (x86_64) | macOS 14.0 Sonoma | Qt 6.11 |

To resolve Gatekeeper quarantine warnings on macOS:

```bash
sudo xattr -cr "NotepadNext-v0.14.dmg"
sudo xattr -rd com.apple.quarantine "NotepadNext-v0.14.dmg"
sudo codesign --force --deep --sign - "NotepadNext-v0.14.dmg"
```

To install using Homebrew:

```bash
brew tap alsyundawy/notepadnext
brew install --no-quarantine notepadnext
```

---

## Dependencies

> 🇬🇧 **System Requirements & Dependencies:**
>
> - **Operating System**: macOS 11.0 (Big Sur) or higher.
> - **Architectures**: Apple Silicon (`arm64`) and Intel (`x86_64`).
> - **Framework**: Qt 6.5, 6.8, 6.10, or 6.11 with `Qt6Core5Compat`.
> - **Build Tools**: CMake 3.21+, Ninja, and C++20 compiler (`AppleClang`).
> - **Automation Helper**: `duti` (optional, for LaunchServices file associations).
>
> 🇮🇩 **Kebutuhan Sistem & Dependensi:**
>
> - **Sistem Operasi**: macOS 11.0 (Big Sur) ke atas.
> - **Arsitektur**: Apple Silicon (`arm64`) dan Intel (`x86_64`).
> - **Framework**: Qt 6.5, 6.8, 6.10, atau 6.11 dengan `Qt6Core5Compat`.
> - **Build Tools**: CMake 3.21+, Ninja, dan C++20 compiler (`AppleClang`).
> - **Otomatisasi**: `duti` (opsional, untuk pemetaan asosiasi file LaunchServices).

---

## Configuration

> 🇬🇧 **Force-set NotepadNext as the system-wide default text and code editor on macOS.**
>
> By default, macOS binds hundreds of file formats and extensionless documents to TextEdit. We provide an automated, production-ready script (`NotepadNext-Default-Editor.sh`) that associates 264+ developer file extensions and 38 UTIs, unblocks Gatekeeper quarantine flags, rebuilds the LaunchServices cache database, and restarts Finder & Dock in sub-second time.
>
> 🇮🇩 **Jadikan NotepadNext sebagai editor teks dan kode default seluruh sistem di macOS.**
>
> Secara default, macOS menetapkan ratusan format file dan dokumen tanpa ekstensi ke TextEdit. Kami menyediakan skrip otomatis (`NotepadNext-Default-Editor.sh`) yang memetakan 264+ ekstensi file developer dan 38 UTIs, menghapus karantina Gatekeeper, merefresh database LaunchServices, dan me-restart Finder & Dock secara instan (<0.3 detik).

### Features & Capabilities / Fitur & Keunggulan

| Feature / Fitur | 🇬🇧 Description | 🇮🇩 Deskripsi |
| :--- | :--- | :--- |
| ⚡ **Batch Engine (<0.3s)** | Atomic LaunchServices configuration in a single pass without process spawn lag. | Konfigurasi atomik LaunchServices dalam 1 panggilan cepat tanpa jeda proses. |
| 📄 **264+ Extensions** | Supports 264+ developer file formats. | Mendukung 264+ format file developer. |
| 🧩 **38 System UTIs** | Binds `public.plain-text`, `public.source-code`, `public.script`, `public.data`, etc. | Memetakan UTI teks, skrip, dan dokumen *unknown* tanpa ekstensi. |
| 🛡️ **Gatekeeper Unblock** | Automatically strips `com.apple.quarantine` from the application bundle. | Menghapus atribut karantina Gatekeeper secara otomatis dari bundle aplikasi. |
| 🔒 **TCC / FDA Check** | Validates Full Disk Access to avoid silent macOS permission blocks. | Memverifikasi Full Disk Access untuk mencegah pemblokiran izin macOS. |
| 🔄 **Auto Cache Refresh** | Rebuilds LaunchServices database (`lsregister -kill -r`) & restarts Finder/Dock. | Membangun ulang database LaunchServices dan me-restart Finder & Dock otomatis. |

### Command-line Options / Opsi Perintah

| Option / Opsi | Description / Deskripsi |
| :--- | :--- |
| `-f`, `-y`, `--force` | Force replace defaults non-interactively & auto-restart Finder/Dock. *(Mode paksa & restart UI)* |
| `-d`, `--dry-run` | Preview all 300+ associations without applying any system changes. *(Pratinjau perubahan)* |
| `--rebuild-cache` | Force reset & rebuild LaunchServices database cache (*default: on*). *(Rebuild cache LaunchServices)* |
| `--no-restart` | Apply file associations without restarting Finder and Dock. *(Terapkan tanpa restart UI)* |
| `-h`, `--help` | Show usage options and help information. *(Tampilkan bantuan)* |
| `-v`, `--version` | Display script version. *(Tampilkan versi skrip)* |

To configure font smoothing to match Windows rendering:

🇬🇧 By default, macOS enables font smoothing which causes text to appear quite differently from the Windows version. This can be disabled system-wide using the following command:

🇮🇩 Secara default, macOS mengaktifkan font smoothing yang menyebabkan tampilan teks berbeda jauh dari versi Windows. Hal ini dapat dinonaktifkan secara sistem menggunakan perintah berikut:

```bash
defaults -currentHost write -g AppleFontSmoothing -int 0
```

---

## Localization & Language Support

> 🇬🇧 **Native Full Indonesian & English Localization Support.**
>
> NotepadNext For MacOS features built-in support for **Bahasa Indonesia (`id_ID`)** alongside **English (`en_US`)**, covering all interface components (Menu Bar, Toolbar, Settings dialogs, Find/Replace, Editor context menus, and Shortcuts).
>
> 🇮🇩 **Dukungan Pelokalan Penuh Bahasa Indonesia & Bahasa Inggris.**
>
> NotepadNext For MacOS kini menyediakan pelokalan lengkap untuk **Bahasa Indonesia (`id_ID`)** bersama **Bahasa Inggris (`en_US`)**, mencakup seluruh komponen antarmuka aplikasi (Menu Bar, Toolbar, Dialog Pengaturan, Pencarian/Penggantian, Menu Konteks Editor, dan Pintasan Tombol).

### Cara Mengaktifkan Bahasa Indonesia / How to Switch to Indonesian

1. **Settings / Preferensi**:
   - 🇬🇧 Open **NotepadNext** $\rightarrow$ Click menu **Settings** $\rightarrow$ **Preferences...** (or press `Cmd + ,`).
   - 🇮🇩 Buka aplikasi **NotepadNext** $\rightarrow$ Klik menu **Settings** $\rightarrow$ **Preferences...** (atau tekan `Cmd + ,`).

2. **Localization Dropdown / Menu Bahasa**:
   - 🇬🇧 Under the **General** tab $\rightarrow$ Look for the **Localization** / **Language** dropdown.
   - 🇮🇩 Pada tab **General** $\rightarrow$ Cari menu pilihan dropdown **Localization** / **Language**.

3. **Select Indonesian / Pilih Bahasa Indonesia**:
   - 🇬🇧 Select **Bahasa Indonesia** from the list $\rightarrow$ Click **Close**.
   - 🇮🇩 Pilih **Bahasa Indonesia** dari daftar $\rightarrow$ Klik **Close**.

4. **Restart Application / Mulai Ulang Aplikasi**:
   - 🇬🇧 Restart NotepadNext to apply the Indonesian language across the entire application.
   - 🇮🇩 Mulai ulang (restart) NotepadNext untuk menerapkan Bahasa Indonesia ke seluruh aplikasi.

| Language / Bahasa | Locale Code | Coverage / Cakupan | Maintainer / Penerjemah |
| :--- | :--- | :--- | :--- |
| 🇮🇩 **Bahasa Indonesia** | `id_ID` (`id`) | Lengkap (2.150+ baris string XML) | [@alsyundawy](https://github.com/alsyundawy) |
| 🇬🇧 **English** | `en_US` (`en`) | Base (Native) | [@dail8859](https://github.com/dail8859) |

---

## Running Tests

> 🇬🇧 **Executing Tests & Script Diagnostics:**
>
> ```bash
> # 1. Preview default editor mappings (Dry Run Mode)
> ./NotepadNext-Default-Editor.sh --dry-run
>
> # 2. Build and verify locally with CMake & CTest
> cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
> cmake --build build --parallel
> ctest --test-dir build --output-on-failure
> ```
>
> 🇮🇩 **Menjalankan Pengujian & Diagnostik Skrip:**
>
> ```bash
> # 1. Pratinjau pemetaan editor default (Mode Dry Run)
> ./NotepadNext-Default-Editor.sh --dry-run
>
> # 2. Bangun dan verifikasi pengujian lokal dengan CMake & CTest
> cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
> cmake --build build --parallel
> ctest --test-dir build --output-on-failure
> ```

---

## Contributing

> 🇬🇧 Contributions are welcome! You can contribute bug reports, code improvements, or UI translations:
>
> - **Translations**: Managed on Crowdin at [crowdin.com/project/notepadnext](https://crowdin.com/project/notepadnext) or via direct GitHub Pull Requests for macOS resources.
> - **Pull Requests**: Please ensure all changes pass automated CI checks.
>
> 🇮🇩 Kontribusi sangat terbuka! Anda dapat mengirimkan laporan bug, perbaikan kode, atau penerjemahan antarmuka:
>
> - **Terjemahan**: Dikelola melalui Crowdin di [crowdin.com/project/notepadnext](https://crowdin.com/project/notepadnext) atau langsung melalui Pull Request GitHub.
> - **Pull Requests**: Pastikan semua perubahan lolos verifikasi CI otomatis.

---

## Credits

> 🇬🇧 This repository is a **macOS-only build** of the original project by **[@dail8859](https://github.com/dail8859)**.
>
> All credit for the original codebase goes to the original author.
>
> This fork is maintained at [https://github.com/alsyundawy/NotepadNext-MacOS](https://github.com/alsyundawy/NotepadNext-MacOS).
>
> 🇮🇩 Repository ini merupakan **build khusus macOS** dari proyek asli oleh **[@dail8859](https://github.com/dail8859)**.
>
> Seluruh kredit untuk proyek aslinya diberikan kepada penulis asli.
>
> Fork ini dikelola di [https://github.com/alsyundawy/NotepadNext-MacOS](https://github.com/alsyundawy/NotepadNext-MacOS) untuk menyediakan build macOS (Apple Silicon & Intel).

| Component / Komponen | Link / Tautan |
| :--- | :--- |
| 🏠 **Original Repository / Repositori Asli** | [github.com/dail8859/NotepadNext](https://github.com/dail8859/NotepadNext) |
| 👤 **Original Author / Penulis Asli** | [@dail8859](https://github.com/dail8859) |
| 🍎 **macOS Repository / Repositori macOS** | [github.com/alsyundawy/NotepadNext-MacOS](https://github.com/alsyundawy/NotepadNext-MacOS) |
| 👤 **macOS Maintainer / Pengembang macOS** | [@alsyundawy](https://github.com/alsyundawy) |

---

## Important Notes

> 🇬🇧 Though the application overall is stable and usable, it should **not** be considered safe for critically important work. There are numerous bugs and half working implementations. Pull requests are greatly appreciated.
>
> 🇮🇩 Meskipun aplikasi secara keseluruhan stabil dan dapat digunakan, aplikasi ini **tidak** disarankan untuk pekerjaan yang sangat penting. Masih terdapat banyak bug dan fitur yang belum sempurna. Pull request sangat diterima dengan tangan terbuka.

---

## Support

> 🇬🇧 If you find this project useful and would like to support its development:
>
> 🇮🇩 Jika Anda merasa proyek ini bermanfaat dan ingin memberikan dukungan / donasi:

- 💳 **PayPal**: [![Donate with PayPal](https://img.shields.io/badge/PayPal-donate-003087?logo=paypal&logoColor=white)](https://www.paypal.me/alsyundawy)
- ☕ **Ko-fi**: [![Donate with Ko-fi](https://img.shields.io/badge/Ko--fi-donate-ff5e5b?logo=ko-fi&logoColor=white)](https://ko-fi.com/alsyundawy)
- 📱 **QRIS**:

![QRIS Donation](https://github.com/user-attachments/assets/a0126f28-6dde-43da-ba14-d7c9a27de0df)

[![Buy Me a Coffee](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/alsyundawy)

---

## License

🇬🇧 This code is released under the [GNU General Public License version 3](https://www.gnu.org/licenses/gpl-3.0.txt).

🇮🇩 Kode ini dirilis di bawah [GNU General Public License versi 3](https://www.gnu.org/licenses/gpl-3.0.txt).

![Alt](https://repobeats.axiom.co/api/embed/d2562b8d093a92380646fc849764852e07adb657.svg "Repobeats analytics image")
