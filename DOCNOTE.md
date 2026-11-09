# 📘 Documentation Note (DOCNOTE) — NotepadNext-MacOS v0.15-02

- **Project**: NotepadNext For MacOS
- **Target Release**: `0.15-02`
- **Date**: October 15, 2026
- **Maintainer / Builder**: Harry DS Alsyundawy ([@alsyundawy](https://github.com/alsyundawy))
- **Upstream Project**: [dail8859/NotepadNext](https://github.com/dail8859/NotepadNext)

---

## 1. Executive Summary

NotepadNext-MacOS **v0.15-02** hardens automation pipelines, eliminates token leak risks in CI workflows, sanitizes Git bot identities, and stabilizes DMG packaging across architectures.

### Key Highlights

1. **CI Security Hardening**: Restriced GitHub token permissions and secured release workflows.
2. **Translation Pipeline Fix**: Addressed identity issues in `lupdate.yml`.
3. **Local DMG Build & Verification**: Built `NotepadNext-MacOS-0.15-02-bcfba4f.dmg` locally and verified with Apple `hdiutil verify`.
4. **Linter & Code Quality Hardening**: Resolved variable shadowing in `NppImporter.cpp`, modernized algorithms to `std::transform`, cleaned Python scripts per ruff, and synchronized `.clang-tidy`.
