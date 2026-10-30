# macOS Codesign, Quarantine & Notarization Workflow

## 1. Overview

Apple macOS requires distributed applications to be signed with an Apple Developer ID Application certificate and notarized by Apple's Notary Service to run smoothly without Gatekeeper warnings.

NotepadNext provides complete documentation and automated mechanisms for Gatekeeper unblocking, ad-hoc signing, and enterprise developer notarization:

- **Automated Helper Script:** `NotepadNext-Default-Editor.sh` (automatically unblocks quarantine)
- **Local Workaround:** Manual `xattr` quarantine stripping and ad-hoc codesigning
- **Enterprise Pipeline:** Developer ID Application signing and Apple Notary Service submission

---

## 2. Resolving Gatekeeper Warnings (For End Users)

When downloading the pre-compiled `.dmg` package directly from GitHub Releases in Safari or Chrome, macOS marks the application with the `com.apple.quarantine` extended attribute.

### 2.1 Automated Quarantine Removal via Helper Script

Run the included default editor script, which automatically detects and purges quarantine flags:

```bash
chmod +x NotepadNext-Default-Editor.sh
./NotepadNext-Default-Editor.sh -f
```

### 2.2 Manual Terminal Removal

If Gatekeeper prevents opening `NotepadNext.app`:

```bash
# 1. Clear all extended attributes recursively:
sudo xattr -cr "/Applications/NotepadNext.app"

# 2. Specifically remove the quarantine attribute:
sudo xattr -rd com.apple.quarantine "/Applications/NotepadNext.app"

# 3. Re-sign locally with an ad-hoc signature:
sudo codesign --force --deep --sign - "/Applications/NotepadNext.app"
```

---

## 3. Developer ID Signing & Apple Notarization (For Maintainers)

For maintainers distributing officially signed and notarized binaries:

### 3.1 Deep Bundle Signing

```bash
# 1. Sign nested frameworks and dylibs with Hardened Runtime
codesign --force --deep --options runtime --timestamp \
    --sign "Developer ID Application: Your Name (TEAM_ID)" \
    "NotepadNext.app/Contents/Frameworks/"*

# 2. Sign main application bundle
codesign --force --options runtime --timestamp \
    --sign "Developer ID Application: Your Name (TEAM_ID)" \
    "NotepadNext.app"
```

### 3.2 DMG Packaging & Signing

```bash
# Sign the distribution disk image:
codesign --sign "Developer ID Application: Your Name (TEAM_ID)" \
    --timestamp "NotepadNext-v0.15-07-arm64.dmg"
```

### 3.3 Apple Notarization via `notarytool`

```bash
# Submit DMG to Apple Notary Service:
xcrun notarytool submit "NotepadNext-v0.15-07-arm64.dmg" \
    --apple-id "your-apple-id@email.com" \
    --password "app-specific-password" \
    --team-id "TEAM_ID" \
    --wait

# Staple notarization ticket to DMG:
xcrun stapler staple "NotepadNext-v0.15-07-arm64.dmg"

# Verify Gatekeeper assessment:
spctl --assess --type open --context context:primary-signature \
    "NotepadNext-v0.15-07-arm64.dmg"
```

---

## 4. GitHub Actions Configuration

To enable automated signing and notarization in GitHub Releases, configure the following GitHub Repository Secrets:

| Secret Name                   | Description                                                |
| :---------------------------- | :--------------------------------------------------------- |
| `APPLE_CERTIFICATE_BASE64`    | Base64-encoded Developer ID Application `.p12` certificate |
| `APPLE_CERTIFICATE_PASSWORD`  | Password for the `.p12` certificate file                   |
| `APPLE_ID`                    | Apple Developer Account email                              |
| `APPLE_APP_SPECIFIC_PASSWORD` | App-Specific Password generated on appleid.apple.com       |
| `APPLE_TEAM_ID`               | 10-character Apple Developer Team ID (e.g. `ABC123XYZ0`)   |

If these secrets are not configured, the GitHub Actions release workflow builds the DMG and skips signing without failing.
