# 🍎 NotepadNext For MacOS — Notepad++ Like

> 🇬🇧 A cross-platform reimplementation of Notepad++ — built natively for macOS.
> 🇮🇩 Implementasi ulang Notepad++ lintas platform — dibangun secara native untuk macOS.

![Build Notepad Next](https://github.com/alsyundawy/NotepadNext-MacOS/workflows/Build%20Notepad%20Next/badge.svg)
[![Latest Version](https://img.shields.io/github/v/release/alsyundawy/NotepadNext-MacOS)](https://github.com/alsyundawy/NotepadNext-MacOS/releases)
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
![image](https://github.com/user-attachments/assets/9a53931f-0078-44af-8832-e863c9d57ddf)

## 👨‍💻 Credits / Kredit

> 🇬🇧 This repository is a **macOS-only build** of the original project by **[@dail8859](https://github.com/dail8859)**.
> All credit for the original source code, design, and development goes to the original author.
>
> 🇮🇩 Repository ini merupakan **build khusus macOS** dari proyek asli oleh **[@dail8859](https://github.com/dail8859)**.
> Seluruh kredit untuk kode sumber, desain, dan pengembangan diberikan kepada penulis asli.
> Fork ini hanya menyediakan build macOS (Apple Silicon & Intel) untuk kemudahan pengguna.

| | |
| --- | --- |
| 🏠 **Original Repository** | [github.com/dail8859/NotepadNext](https://github.com/dail8859/NotepadNext) |
| 👤 **Original Author** | [@dail8859](https://github.com/dail8859) |
| 🍎 **macOS Build by** | [@alsyundawy](https://github.com/alsyundawy) |

---

## 📦 Installation / Instalasi

Build aplikasi NotepadNext tersedia untuk tiga arsitektur macOS. Anda dapat mengunduh build terbaru langsung dari halaman [Releases](https://github.com/alsyundawy/NotepadNext-MacOS/releases).

> 💡 **Petunjuk:** Pilih rilis terbaru, lalu lihat bagian **Assets** di bagian bawah untuk mendownload file `.dmg` atau `.zip` yang sesuai dengan arsitektur Mac Anda.

---

### 💻 Apple Silicon (arm64)

| 📁 Package | 🏗️ Architecture | 🍎 Minimum macOS | ⚙️ Qt Version |
| --- | --- | --- | --- |
| `NotepadNext-macOS-arm64-Qt6.5.zip` | Apple Silicon (arm64) | macOS 12.0 Monterey | Qt 6.5 |
| `NotepadNext-macOS-arm64-Qt6.8.zip` | Apple Silicon (arm64) | macOS 13.0 Ventura | Qt 6.8 |
| `NotepadNext-macOS-arm64-Qt6.10.zip` | Apple Silicon (arm64) | macOS 14.0 Sonoma | Qt 6.10 |
| `NotepadNext-macOS-arm64-Qt6.11.zip` | Apple Silicon (arm64) | macOS 15.0 Sequoia | Qt 6.11 |

---

### 💻 Intel (x86_64)

| 📁 Package | 🏗️ Architecture | 🍎 Minimum macOS | ⚙️ Qt Version |
| --- | --- | --- | --- |
| `NotepadNext-macOS-x64-Qt6.5.zip` | Intel (x86_64) | macOS 11.0 Big Sur | Qt 6.5 |
| `NotepadNext-macOS-x64-Qt6.8.zip` | Intel (x86_64) | macOS 12.0 Monterey | Qt 6.8 |
| `NotepadNext-macOS-x64-Qt6.10.zip` | Intel (x86_64) | macOS 13.0 Ventura | Qt 6.10 |
| `NotepadNext-macOS-x64-Qt6.11.zip` | Intel (x86_64) | macOS 14.0 Sonoma | Qt 6.11 |

```bash
sudo xattr -cr "NotepadNext-v0.14.dmg"

sudo xattr -rd com.apple.quarantine "NotepadNext-v0.14.dmg"

sudo codesign --force --deep --sign - "NotepadNext-v0.14.dmg"
```

---

## 🍺 Install via Homebrew

🇬🇧 It can also be installed using Homebrew:
🇮🇩 Dapat juga diinstal menggunakan Homebrew:

```bash
brew tap alsyundawy/notepadnext
brew install --no-quarantine notepadnext
```

---

## 🔧 MacOS Tweaks / Pengaturan Tambahan

### 🔤 Font Smoothing

🇬🇧 By default, macOS enables font smoothing which causes text to appear quite differently from the Windows version. This can be disabled system-wide using the following command:

🇮🇩 Secara default, macOS mengaktifkan font smoothing yang menyebabkan tampilan teks berbeda jauh dari versi Windows. Hal ini dapat dinonaktifkan secara sistem menggunakan perintah berikut:

```bash
defaults -currentHost write -g AppleFontSmoothing -int 0
```

---

## ⚠️ Catatan Penting / Important Notes

> 🇬🇧 Though the application overall is stable and usable, it should **not** be considered safe for critically important work. There are numerous bugs and half working implementations. Pull requests are greatly appreciated.
>
> 🇮🇩 Meskipun aplikasi secara keseluruhan stabil dan dapat digunakan, aplikasi ini **tidak** disarankan untuk pekerjaan yang sangat penting. Masih terdapat banyak bug dan fitur yang belum sempurna. Pull request sangat diterima dengan tangan terbuka.

---

## ☕ Support / Dukungan

> 🇬🇧 If you find this project useful and would like to support its development:
>
> 🇮🇩 Jika Anda merasa proyek ini bermanfaat dan ingin memberikan dukungan / donasi:

- 💳 **PayPal**: [![Donate with PayPal](https://img.shields.io/badge/PayPal-donate-003087?logo=paypal&logoColor=white)](https://www.paypal.me/alsyundawy)
- ☕ **Ko-fi**: [![Donate with Ko-fi](https://img.shields.io/badge/Ko--fi-donate-ff5e5b?logo=ko-fi&logoColor=white)](https://ko-fi.com/alsyundawy)
- 📱 **QRIS**:

![QRIS Donation](https://github.com/user-attachments/assets/a0126f28-6dde-43da-ba14-d7c9a27de0df)

[![Buy Me a Coffee](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/alsyundawy)

---

## 📄 License / Lisensi

🇬🇧 This code is released under the [GNU General Public License version 3](https://www.gnu.org/licenses/gpl-3.0.txt).

🇮🇩 Kode ini dirilis di bawah [GNU General Public License versi 3](https://www.gnu.org/licenses/gpl-3.0.txt).
