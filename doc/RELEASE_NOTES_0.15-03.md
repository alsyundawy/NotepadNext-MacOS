# 📝 NotepadNext-MacOS Release 0.15-03

## 🇮🇩 Bahasa Indonesia

Rilis **v0.15-03** menghadirkan sinkronisasi perbaikan bug dari upstream resmi v0.15, penyempurnaan stabilitas penanganan streaming encoding berkas non-UTF16, penanganan opsi pencarian Regex yang lebih andal, pembaruan menyeluruh lokalisasi Bahasa Indonesia sesuai kaidah KBBI, serta modernisasi dependensi otomatis pada pipeline GitHub Actions dan CodeQL.

### 🌟 Sorotan Pembaruan (Changelog dari 0.15-02 ke 0.15-03)

#### 1. 🛡️ Perbaikan Crash Pembacaan Berkas Teks Non-UTF16 (Issue #1149)

- **Stabilisasi Buffer Streaming**: Memperbaiki alur pembacaan berkas pada [`src/ScintillaNext.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/ScintillaNext.cpp) dengan mendeklarasikan `QStringEncoder encoder(QStringEncoder::Utf8)` dan `QStringDecoder decoder` di tingkat fungsi.
- **Pencegahan Alokasi Berulang & Crash**: Mengeliminasi instansiasi ulang decoder di dalam loop pembacaan chunk data serta memastikan konversi UTF-16 ke UTF-8 berjalan mulus tanpa merusak buffer memori teks.

#### 2. 🔍 Penyempurnaan Pencarian Ekspresi Reguler / Regex (Issue #1156)

- **Sanitasi Opsi Pencarian**: Pada [`src/dialogs/FindReplaceDialog.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/dialogs/FindReplaceDialog.cpp), saat mode pencarian _Regular Expression (Regex)_ diaktifkan, opsi yang tidak kompatibel seperti _Backwards Direction_ dan _Match Whole Word_ secara otomatis dimatikan (_unchecked_) dan dinonaktifkan (_disabled_).
- **Pencegahan Ambigu & Error Logika**: Menghindarkan pengguna dari error pencarian regex yang ambigu ketika mencoba mencocokkan pola regex dengan batasan kata utuh atau arah mundur yang tidak didukung mesin Scintilla Regex.

#### 3. 🌐 Standardisasi Lokalisasi Bahasa Indonesia & Sinkronisasi Upstream

- **Penyempurnaan Bahasa Indonesia**: Memperbarui berkas terjemahan [`i18n/NotepadNext_id.ts`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/i18n/NotepadNext_id.ts) untuk memastikan seluruh istilah menu, dialog, dan tooltip menggunakan kosakata baku Bahasa Indonesia (KBBI) yang bersih dan konsisten.
- **Sinkronisasi Terjemahan Internasional**: Mengintegrasikan katalog terjemahan terbaru dari upstream Crowdin untuk menjaga keselarasan bahasa antarmuka global.

#### 4. ⚙️ Pembaruan Pipeline CI/CD & Keamanan Otomatis

- **Pembaruan GitHub Actions**: Memperbarui action pipeline ke versi terbaru:
  - `github/codeql-action/init` & `analyze` ke versi `4.38.3`.
  - `actions/upload-artifact` ke versi `7.0.2`.
  - `actions/download-artifact` ke versi `8.0.2`.
  - `vedantmgoyal2009/winget-releaser` ke hash commit terbaru.
- **Versi Terpusat**: Versi rilis ditingkatkan ke `0.15-03` pada [`CMakeLists.txt`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/CMakeLists.txt) dan [`updates.json`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/updates.json).

---

## 🇬🇧 English

Release **v0.15-03** introduces upstream bugfix synchronization from official v0.15, stability fixes for non-UTF16 text file streaming, refined Regular Expression search dialog options, polished Indonesian localization according to formal KBBI standards, and automated CI/CD dependency security bumps across GitHub Actions and CodeQL.

### 🚀 Changelog from 0.15-02 to 0.15-03

#### 1. 🛡️ Non-UTF16 Text File Streaming Crash Fix (Issue #1149)

- **Streaming Buffer Stabilization**: Fixed file read routines in [`src/ScintillaNext.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/ScintillaNext.cpp) by initializing `QStringEncoder encoder(QStringEncoder::Utf8)` and `QStringDecoder decoder` at the function scope.
- **Eliminated Redundant Allocation & Crashes**: Removed per-chunk decoder re-instantiation inside the read loop, ensuring seamless UTF-16 to UTF-8 buffer conversions without memory corruptions.

#### 2. 🔍 Regular Expression Search Dialog Sanitization (Issue #1156)

- **Search Option Invalidation**: In [`src/dialogs/FindReplaceDialog.cpp`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/src/dialogs/FindReplaceDialog.cpp), when _Regular Expression_ search mode is toggled, incompatible search options (_Backwards Direction_ and _Match Whole Word_) are automatically unchecked and disabled.
- **Prevented Ambiguous Searches**: Protects users from syntax conflicts and engine stalls when attempting whole-word or backward regex searches unsupported by the underlying Scintilla regex engine.

#### 3. 🌐 Indonesian Localization Polish & Upstream Translation Sync

- **Refined Indonesian Translation**: Standardized [`i18n/NotepadNext_id.ts`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/i18n/NotepadNext_id.ts) vocabulary to adhere to formal Indonesian standards (KBBI) with clean, natural phrasing across dialogs and menus.
- **Upstream Translation Synchronization**: Synced latest international translation catalogs from upstream Crowdin.

#### 4. ⚙️ CI/CD Pipeline & Automated Security Updates

- **GitHub Actions Security Bumps**: Upgraded workflow actions:
  - `github/codeql-action/init` & `analyze` to `4.38.3`.
  - `actions/upload-artifact` to `7.0.2`.
  - `actions/download-artifact` to `8.0.2`.
  - `vedantmgoyal2009/winget-releaser` updated.
- **Centralized Versioning**: Bumped release version to `0.15-03` across [`CMakeLists.txt`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/CMakeLists.txt) and [`updates.json`](file:///Users/alsyundawy/Downloads/GitHub/NotepadNext-MacOS/updates.json).

---

### 📝 Commit Log (0.15-02 ... 0.15-03)

- `6f66070` chore(release): bump version to 0.15-03, sync upstream, and refine Indonesian translations (@alsyundawy)
- `e8e550f` Merge upstream changes from dail8859/NotepadNext (v0.15) into master (@alsyundawy)
- `5f7f744` Merge pull request #44 from alsyundawy/dependabot/github_actions/actions/download-artifact-8.0.2 (@alsyundawy)
- `feb4587` Merge pull request #43 from alsyundawy/dependabot/github_actions/actions/upload-artifact-7.0.2 (@alsyundawy)
- `af58d29` Merge pull request #42 from alsyundawy/dependabot/github_actions/github/codeql-action/init-4.38.3 (@alsyundawy)
- `e784afd` Merge pull request #41 from alsyundawy/dependabot/github_actions/github/codeql-action/analyze-4.38.3 (@alsyundawy)
- `62114c1` Bump actions/download-artifact from 8.0.1 to 8.0.2 (@dependabot[bot])
- `3d594bd` Bump actions/upload-artifact from 7.0.1 to 7.0.2 (@dependabot[bot])
- `1d0a30b` Bump github/codeql-action/init from 4.38.2 to 4.38.3 (@dependabot[bot])
- `98206e8` Bump github/codeql-action/analyze from 4.38.2 to 4.38.3 (@dependabot[bot])
- `88fd13c` Update translation files (@dail8859)
- `429445f` Merge pull request #40 from alsyundawy/dependabot/github_actions/vedantmgoyal2009/winget-releaser-b7370d4d152efdce803a26cc6ca9eee58cb2ad54 (@alsyundawy)
- `8fcec17` Merge pull request #39 from alsyundawy/dependabot/github_actions/github/codeql-action/analyze-4.38.2 (@alsyundawy)
- `53087b5` Merge pull request #38 from alsyundawy/dependabot/github_actions/github/codeql-action/init-4.38.2 (@alsyundawy)
- `adfd128` Uncheck invalid find options during Regex searching (@dail8859)
- `0616733` Bump vedantmgoyal2009/winget-releaser (@dependabot[bot])
- `6d7f052` Bump github/codeql-action/analyze from 4.38.1 to 4.38.2 (@dependabot[bot])
- `63591a1` Bump github/codeql-action/init from 4.38.1 to 4.38.2 (@dependabot[bot])
- `27076bb` Update translation files (@dail8859)
- `b123ea2` Fix crash when reading non-UTF16 files (#1149) (@amirkiarafiei)
- `7c6dc05` Update jekyll-gh-pages.yml (@dail8859)

**Full Changelog**: [0.15-02...0.15-03](https://github.com/alsyundawy/NotepadNext-MacOS/compare/0.15-02...0.15-03)
