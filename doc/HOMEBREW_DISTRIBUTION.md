# NotepadNext Homebrew Distribution Guide

## 1. Overview

Homebrew is the primary package manager for macOS power users and developers. NotepadNext is distributed via an official Homebrew tap repository: `alsyundawy/notepadnext`.

---

## 2. Installation for End-Users

Users can install NotepadNext with a single command:

```bash
# 1. Tap the official NotepadNext repository:
brew tap alsyundawy/notepadnext

# 2. Install NotepadNext Cask (bypassing Gatekeeper quarantine flags):
brew install --cask --no-quarantine notepadnext

# 3. Launch from terminal:
notepadnext
```

To update NotepadNext via Homebrew:

```bash
brew upgrade --cask notepadnext
```

To uninstall NotepadNext cleanly:

```bash
brew uninstall --cask --zap notepadnext
```

---

## 3. Cask Structure & Artifacts

The Cask recipe features:

- **Dual-Architecture Detection**: Automatic selection between Apple Silicon (`arm64`) and Intel (`x86_64`) DMG packages.
- **Universal CLI Binary Symlink**: The `notepadnext` binary is automatically linked to `/usr/local/bin/notepadnext` (Intel) or `/opt/homebrew/bin/notepadnext` (Apple Silicon).
- **Clean Cleanup Definitions (`zap trash`)**: Removes application caches, preferences (`~/Library/Preferences/com.github.alsyundawy.NotepadNext.plist`), and saved state upon uninstallation.

---

## 4. Maintainer Release Automation

To bump the Cask after cutting a new GitHub Release:

1. Calculate the SHA-256 checksums of the release DMG artifacts:

   ```bash
   shasum -a 256 NotepadNext-v0.15-07-arm64.dmg
   shasum -a 256 NotepadNext-v0.15-07-x64.dmg
   ```

2. Update `version` and `sha256` in the Cask file in the `homebrew-notepadnext` repository.
3. Commit and push changes to trigger automated Homebrew tap updates.
