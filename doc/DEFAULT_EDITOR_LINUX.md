# Notepad Next Linux Default Editor Architecture & Reference Manual

## 1. Executive Overview

`NotepadNext-Default-Editor-Linux.sh` is an enterprise-grade automation engine designed to set **Notepad Next** (`com.github.alsyundawy.NotepadNext`) as the canonical, default source code and text editor across all major Linux distributions and Desktop Environments (DEs).

The script guarantees standard compliance with:

- **Freedesktop.org XDG Standards** (Desktop Entry Specification 1.5, Shared MIME-Info Specification 2.4, XDG Base Directory Specification).
- **Debian / Ubuntu Alternatives System** (`update-alternatives`).
- **Red Hat / Fedora / SUSE Alternatives System** (`alternatives`).
- **POSIX Shell & Bash 3.2–5.2 Compatibility** (Zero-warning `shellcheck` pass and 100% `shfmt` conformant).

---

## 2. Linux Packaging Architectures: `.deb` vs `.rpm`

Linux distributions primarily divide into two predominant package packaging ecosystems: Debian (`.deb`) and Red Hat Package Manager (`.rpm`).

### 2.1 The Debian Ecosystem (`.deb`)

Debian packages are `ar` archives containing two compressed tarballs and a debian-binary version marker:

1. `debian-binary`: Text string defining the package format version (`2.0`).
2. `control.tar.xz` (or `.gz`): Contains package control metadata (`control`), pre/post installation scripts (`preinst`, `postinst`, `prerm`, `postrm`), and file hashes (`md5sums`).
3. `data.tar.xz` (or `.gz`, `.zst`): Contains the actual filesystem payload installed onto the system.

**Distributions using `.deb`:**

- **Debian**: Debian 12 (Bookworm), Debian 13 (Trixie), Debian Unstable (Sid).
- **Ubuntu**: Ubuntu 22.04 LTS (Jammy Jellyfish), Ubuntu 24.04 LTS (Noble Numbat), Ubuntu 26.04.
- **Linux Mint**: Editions Cinnamon, MATE, XFCE, and LMDE (Linux Mint Debian Edition).
- **Pop!\_OS**: System76's Ubuntu-based distribution with COSMIC desktop.
- **Elementary OS**: Ubuntu-based distribution featuring the Pantheon desktop.
- **Zorin OS**: Ubuntu-based desktop environment targeting Windows/macOS switchers.
- **Kali Linux & Parrot OS**: Security and penetration testing distributions based on Debian Testing.
- **MX Linux & antiX**: SysVinit/systemd flexible Debian-based desktop distributions.
- **Raspberry Pi OS**: Debian optimized for ARM single-board computers.
- **Deepin / UOS**: Debian-based distribution with Deepin Desktop Environment (DDE).

### 2.2 The RPM Ecosystem (`.rpm`)

RPM packages are binary archives consisting of a lead, signature, header metadata, and a payload archive compressed using gzip, xz, or zstd:

1. **Lead**: Legacy identifier block.
2. **Signature Block**: OpenPGP/GPG signatures for package integrity verification.
3. **Header**: Structured metadata (tags for Name, Version, Release, Architecture, Requires, Provides, Conflicts, Obsoletes).
4. **Payload (cpio archive)**: The binary files unpacked into system directories.

**Distributions using `.rpm`:**

- **Fedora**: Fedora Workstation, Fedora Silverblue / Atomic Desktops (Rawhide, 39, 40, 41, 42).
- **Red Hat Enterprise Linux (RHEL)**: RHEL 8, RHEL 9, RHEL 10.
- **CentOS Stream**: Upstream development platform for RHEL.
- **AlmaLinux & Rocky Linux**: Bug-for-bug 1:1 binary compatible enterprise distributions derived from RHEL.
- **openSUSE**: openSUSE Tumbleweed (rolling release) and openSUSE Leap (LTS).
- **SUSE Linux Enterprise**: SLED (Desktop) and SLES (Server).
- **Mageia & OpenMandriva**: Community distributions derived from Mandriva.
- **Amazon Linux**: Cloud-native distribution for AWS EC2 based on Fedora/RHEL.
- **PCLinuxOS**: Rolling-release RPM-based desktop distribution with APT-RPM.

---

## 3. Linux Desktop Environments (DE) Coverage

Different desktop environments employ specific daemons, IPC buses, and cache hierarchies to discover and launch default applications. `NotepadNext-Default-Editor-Linux.sh` natively detects and synchronizes with all of them:

| Desktop Environment                 | Primary Tool / Mechanism        | Default File Association Path | Cache / Refresh Command                               |
| :---------------------------------- | :------------------------------ | :---------------------------- | :---------------------------------------------------- |
| **GNOME** (40–47+)                  | `gio mime`, XDG MIME            | `~/.config/mimeapps.list`     | `update-desktop-database ~/.local/share/applications` |
| **KDE Plasma** (5 & 6)              | `ktraderclient5/6`, KService    | `~/.config/mimeapps.list`     | `kbuildsycoca6` or `kbuildsycoca5`                    |
| **XFCE** (4.16–4.20)                | `exo-preferred-applications`    | `~/.config/mimeapps.list`     | `update-desktop-database`                             |
| **Cinnamon**                        | `cinnamon-settings`, Nemo       | `~/.config/mimeapps.list`     | `update-desktop-database`                             |
| **MATE**                            | `mate-default-applications`     | `~/.config/mimeapps.list`     | `update-desktop-database`                             |
| **LXQt**                            | `lxqt-config-file-associations` | `~/.config/mimeapps.list`     | `update-desktop-database`                             |
| **LXDE**                            | `pcmanfm` / LibFM               | `~/.config/mimeapps.list`     | `update-desktop-database`                             |
| **COSMIC** (System76)               | COSMIC App Library / Rust XDG   | `~/.config/mimeapps.list`     | `update-desktop-database`                             |
| **Pantheon**                        | Elementary Granite / Files      | `~/.config/mimeapps.list`     | `update-desktop-database`                             |
| **Budgie**                          | Budgie Desktop Settings         | `~/.config/mimeapps.list`     | `update-desktop-database`                             |
| **Deepin (DDE)**                    | DDE File Manager                | `~/.config/mimeapps.list`     | `update-desktop-database`                             |
| **Tiling WMs** (i3, Sway, Hyprland) | XDG Utilities (`xdg-mime`)      | `~/.config/mimeapps.list`     | `update-desktop-database`                             |

---

## 4. Technical Architecture: How Default Associations Work

### 4.1 Freedesktop XDG MIME Association Precedence

Freedesktop specifies that user preferences take precedence over system defaults:

1. User override: `~/.config/mimeapps.list` (or `$XDG_CONFIG_HOME/mimeapps.list`)
2. User package additions: `~/.local/share/applications/mimeapps.list` (deprecated by standard, but honored)
3. Desktop-specific system overrides: `/etc/xdg/$desktop-mimeapps.list` (e.g. `gnome-mimeapps.list`)
4. System-wide administrator overrides: `/etc/xdg/mimeapps.list`
5. Distribution defaults: `/usr/share/applications/mimeapps.list`

`NotepadNext-Default-Editor-Linux.sh` writes entries to both `[Default Applications]` and `[Added Associations]` sections:

```ini
[Default Applications]
text/plain=com.github.alsyundawy.NotepadNext.desktop;NotepadNext.desktop;
text/markdown=com.github.alsyundawy.NotepadNext.desktop;NotepadNext.desktop;
...

[Added Associations]
text/plain=com.github.alsyundawy.NotepadNext.desktop;NotepadNext.desktop;
...
```

### 4.2 Terminal & Console Alternatives

To make Notepad Next the default editor for terminal operations (e.g., `git commit`, `crontab -e`, `sudoedit`), the script configures:

1. **Debian / Ubuntu Alternatives System**:

   ```bash
   update-alternatives --install /usr/bin/editor editor /usr/bin/NotepadNext 100
   update-alternatives --set editor /usr/bin/NotepadNext
   ```

2. **RHEL / Fedora / SUSE Alternatives System**:

   ```bash
   alternatives --install /usr/bin/editor editor /usr/bin/NotepadNext 100
   alternatives --set editor /usr/bin/NotepadNext
   ```

3. **User Environment Variables**:

   Appends or exports `EDITOR` and `VISUAL` in `~/.bashrc` and `~/.profile`:

   ```bash
   export EDITOR="NotepadNext"
   export VISUAL="NotepadNext"
   ```

---

## 5. Supported MIME Types Catalog (120+ Formats)

The script comprehensively maps over 120 programming languages, markup formats, scripts, and configuration types:

- **Plain & Formatted Text**: `text/plain`, `text/markdown`, `text/x-markdown`, `text/richtext`, `text/csv`, `text/tab-separated-values`.
- **Web Technologies**: `text/html`, `application/xhtml+xml`, `text/css`, `text/javascript`, `application/javascript`, `application/json`, `application/xml`, `text/xml`, `text/yaml`, `application/x-yaml`, `text/x-typescript`, `application/typescript`.
- **Compiled Languages & Headers**:
  - C / C++: `text/x-c`, `text/x-csrc`, `text/x-chdr`, `text/x-c++`, `text/x-c++src`, `text/x-c++hdr`.
  - Rust & Go: `text/x-rust`, `text/rust`, `text/x-go`, `text/x-golang`.
  - Java, Kotlin, Scala: `text/x-java`, `text/x-java-source`, `text/x-kotlin`, `text/x-scala`.
  - C# & .NET: `text/x-csharp`, `text/x-fsharp`.
  - Pascal, Fortran, Ada: `text/x-pascal`, `text/x-fortran`, `text/x-ada`.
- **Scripting & Dynamic Languages**:
  - Python: `text/x-python`, `text/x-python3`.
  - Ruby: `text/x-ruby`, `application/x-ruby`.
  - PHP: `text/x-php`, `application/x-php`.
  - Perl: `text/x-perl`, `application/x-perl`.
  - Lua: `text/x-lua`.
  - R: `text/x-r`.
- **Shell & Systems Scripting**:
  - Bash, POSIX sh, Zsh, Fish: `text/x-sh`, `application/x-sh`, `text/x-shellscript`, `text/x-zsh`, `text/x-fish`.
  - Windows Batch & PowerShell: `text/x-bat`, `application/x-bat`, `text/x-powershell`, `application/x-powershell`.
- **Configuration & Infrastructure**:
  - INI, TOML, Conf, Properties: `text/x-ini`, `application/toml`, `text/x-toml`, `text/x-config`, `text/x-properties`.
  - DevOps: Dockerfile (`text/x-dockerfile`), CMake (`text/x-cmake`), Makefile (`text/x-makefile`), Meson, QMake.
  - SQL: `text/x-sql`.
  - Diff & Patches: `text/x-diff`, `text/x-patch`.

---

## 6. Usage and CLI Options

`NotepadNext-Default-Editor-Linux.sh` provides a flexible CLI interface:

```text
Usage: NotepadNext-Default-Editor-Linux.sh [COMMAND] [OPTIONS]

Commands:
  set               Configure Notepad Next as the default editor (default action)
  restore           Restore original MIME configurations from the latest backup
  status            Audit current MIME associations and default editor status
  help, -h, --help  Display this comprehensive reference guide

Options:
  --system          Apply configuration system-wide (/etc/xdg/mimeapps.list, requires sudo)
  --user            Apply configuration to the current user only (~/.config/mimeapps.list, default)
  --no-color        Disable ANSI terminal colors
  --verbose         Enable detailed diagnostic logging
  --dry-run         Simulate changes without modifying any files
```

### Example Commands

```bash
# 1. Apply to current user
./NotepadNext-Default-Editor-Linux.sh set

# 2. Apply system-wide for all users
sudo ./NotepadNext-Default-Editor-Linux.sh set --system

# 3. Check current configuration status
./NotepadNext-Default-Editor-Linux.sh status

# 4. Restore previous associations from automated backup
./NotepadNext-Default-Editor-Linux.sh restore
```

---

## 7. Quality & Verification Invariants

- **Shellcheck Validation**: Evaluated against ShellCheck 0.10+ (`shellcheck NotepadNext-Default-Editor-Linux.sh`) with 0 errors and 0 warnings.
- **shfmt Formatting**: Formatted with standard indentation and syntax rules (`shfmt -w NotepadNext-Default-Editor-Linux.sh`).
- **Defensive Error Handling**: Safe execution under `set -eo pipefail`, atomic writes via temporary files, automatic timestamped backups before modifications, and validation of desktop file existence.
- **Reverse-DNS AppStream ID**: Canonical target is `com.github.alsyundawy.NotepadNext.desktop`, ensuring 100% interoperability with modern Flatpak, Snap, AppStream, and standard desktop package releases.
