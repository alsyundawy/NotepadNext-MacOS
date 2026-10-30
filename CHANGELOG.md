# 📜 Changelog — NotepadNext For MacOS

All notable changes to the **NotepadNext For MacOS** project are documented in this file.
Format is inspired by [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.15-02] - 2026-10-15

### 🛡️ CI/CD & Security Hardening / Penguatan CI/CD & Keamanan

- 🇬🇧 **Workflow Hardening**: Hardened GitHub Actions workflows against secret leakage, script injection, and untrusted trigger vectors.
- 🇮🇩 **Penguatan Alur Kerja**: Memperketat workflow GitHub Actions dari kebocoran token rahasia, injeksi skrip, dan vektor pemicu yang tidak tepercaya.
- 🇬🇧 **Translation Bot & Git Identity Sanitization**: Sanitized git committer identity and push target in `lupdate.yml` translation pipeline.
- 🇮🇩 **Sanitasi Identitas Bot Terjemahan**: Menstandarkan identitas committer git dan target push pada workflow pembaruan terjemahan `lupdate.yml`.
- 🇬🇧 **Canonical DMG Release Deployment**: Configured release pipeline to upload canonical DMG artifacts per architecture using clean Qt builds.
- 🇮🇩 **Distribusi DMG Kanonikal**: Mengonfigurasi alur rilis untuk mengunggah artefak DMG kanonikal per arsitektur dengan build Qt yang bersih.
- 🇬🇧 **Local DMG Build & Verification**: Successfully built and cryptographically verified native macOS DMG (`NotepadNext-v0.15-02-x64.dmg`) locally with `hdiutil verify`.
- 🇮🇩 **Build & Verifikasi DMG Lokal**: Berhasil mengompilasi dan memverifikasi integritas berkas DMG macOS (`NotepadNext-v0.15-02-x64.dmg`) secara lokal dengan `hdiutil verify`.
