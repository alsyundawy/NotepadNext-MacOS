# NotepadNext — Operating System & Platform Support Matrix

- **Release Target:** NotepadNext v0.15-07+
- **Baseline Standard:** C++20 / Qt 6.5–6.12
- **Binary Baseline:** Ubuntu 22.04 LTS (glibc 2.35, GCC 12) & macOS 11.0+ (AppleClang)
- **Lead Maintainer:** Harry Dertin Sutisna Alsyundawy (`@alsyundawy`)
- **Original Author & Upstream:** Justin Dailey (`@dail8859`)

---

## 1. Executive Summary

NotepadNext is an ultra-lightweight, cross-platform source code and text editor engineered for high performance on **macOS**, **Linux**, and **FreeBSD**.

Because NotepadNext requires modern C++20 and Qt 6.5+, official native `.deb` and `.rpm` packages are compiled against an established glibc baseline (**Ubuntu 22.04 LTS with glibc 2.35**). Binaries compiled on this baseline maintain forwards-compatibility and execute seamlessly across modern downstream Linux distributions. On macOS, native DMG bundles are built and optimized with multi-Qt coverage from Qt 6.5 up to Qt 6.12 across Apple Silicon (`arm64`) and Intel (`x86_64`).

---

## 2. Comprehensive OS & Distro Support Matrix

| Platform / Distro      | Version Range                           | Official Distro Qt 6 Support   | Recommended Delivery / Package                     | Technical & Architectural Notes                                                     |
| :--------------------- | :-------------------------------------- | :----------------------------- | :------------------------------------------------- | :---------------------------------------------------------------------------------- |
| **Ubuntu**             | 20.04 LTS (Focal)                       | None (Qt 5.12 only)            | AppImage / Build from source                       | Base repositories lack `qt6-base`. Use Qt 6.2+ PPA or AppImage.                     |
|                        | 22.04 LTS (Jammy)                       | Qt 6.2.4                       | Native `.deb` (Canonical Release Baseline)         | glibc 2.35 baseline. Supports `libxcb-cursor0` and core Qt6 runtime.                |
|                        | 24.04 LTS (Noble)                       | Qt 6.4.2                       | Native `.deb`                                      | Fully compatible with Debian 64-bit `time_t` transition packages.                   |
|                        | 26.04 LTS (Resolute)                    | Qt 6.10.x / 6.11.x             | Native `.deb`                                      | Verified ABI compatibility with next-generation LTS toolchains.                     |
| **Debian**             | 11 (Bullseye)                           | Limited (backports only)       | AppImage / Build from source                       | Official repos lack full Qt 6 suite. AppImage recommended.                          |
|                        | 12 (Bookworm)                           | Qt 6.4.x                       | Native `.deb`                                      | Fully supported via multi-distro dependency resolution in `PackagingLinux.cmake`.   |
|                        | 13 (Trixie)                             | Qt 6.8.x                       | Native `.deb`                                      | Verified multi-arch library paths and GCC 14 compatibility.                         |
|                        | 14 (Forky)                              | Qt 6.10+ / 6.11                | Native `.deb`                                      | Rolling development testing target.                                                 |
| **FreeBSD**            | 13.x – 15.x                             | `x11-toolkits/qt6` metapackage | Native FreeBSD port / `pkg install`                | Native execution on KDE Plasma 6 and XFCE without Linuxulator/Wine.                 |
| **Fedora**             | 39 – 40                                 | Qt 6.6 – 6.8                   | Native `.rpm`                                      | Verified automatic dependency resolution via `dnf install`.                         |
|                        | 41 – 44                                 | Qt 6.8 – 6.12                  | Native `.rpm`                                      | Clean compilation on modern GCC 14/15 with strict `-Werror` flags.                  |
| **Alma / Rocky Linux** | 8                                       | None in BaseOS                 | AppImage / Source build                            | Enterprise BaseOS lacks Qt 6. AppImage recommended for servers.                     |
|                        | 9                                       | Qt 6.6+ (via EPEL 9)           | Native `.rpm`                                      | Enterprise Linux 9 workstation target with EPEL repository enabled.                 |
|                        | 10 – 11                                 | Qt 6.8+                        | Native `.rpm`                                      | Next-generation Enterprise Linux workstation baseline.                              |
| **openSUSE**           | Leap 15.x                               | Community (`KDE:Qt6`)          | Zypper package / AppImage                          | Official openSUSE Leap requires community Qt 6 repo; AppImage works out of the box. |
|                        | Tumbleweed                              | Rolling Latest (Qt 6.12+)      | Native RPM (`zypper install`)                      | Continuously validated against rolling Qt snapshots.                                |
| **Slackware**          | 15.0 / Current                          | SlackBuilds / Manual           | SlackBuild from source                             | Standard SlackBuild script compilation against installed Qt 6.                      |
| **macOS**              | 11 (Big Sur)                            | Qt 6.2 – 6.5                   | Native `.dmg` (Intel x86_64)                       | Minimum supported macOS version for legacy Intel hardware.                          |
|                        | 12 – 26 (Monterey through modern macOS) | Qt 6.5 – 6.12                  | Native `.dmg` (Apple Silicon arm64 & Intel x86_64) | Full AppKit Cocoa integration, Dock menu, gatekeeper unblock, and native styling.   |

---

## 3. Binary Baseline & Portability Strategy

### 3.1 The glibc 2.35 Baseline Principle

Linux binaries dynamically linked against `glibc` require the target host to provide a `glibc` version equal to or newer than the build host. By anchoring NotepadNext's canonical release runners to **Ubuntu 22.04 LTS (glibc 2.35)**:

1. **Forwards Compatibility:** The resulting `.deb` and `.rpm` binaries run without modification on Ubuntu 22.04, 24.04, 26.04, Debian 12, Debian 13, Fedora 39+, and RHEL/Rocky 9+.
2. **SONAME Requirements:** All package dependencies specify SONAME requirements (`libc.so.6(GLIBC_2.35)(64bit)` and `libxcb-cursor.so.0()(64bit)`), preventing accidental installation on unsupported legacy systems without explicit error messages.

### 3.2 Universal Portable Fallbacks: AppImage & Flatpak

For older distributions or air-gapped workstations where system Qt 6 packages cannot be upgraded:

- **AppImage:** Bundles `libQt6Core.so`, `libQt6Gui.so`, `libQt6Widgets.so`, and all necessary QPA platform plugins (`libqxcb.so`) into an immutable, self-contained executable.
- **Flatpak:** Encapsulates NotepadNext inside the official `org.kde.Platform` or `org.freedesktop.Platform` runtime, ensuring 100% sandboxed isolation from host library discrepancies.

---

## 4. Desktop Environment Validation

NotepadNext has been empirically verified across the following desktop environments:

- **macOS (AppKit / Cocoa):** Native Aqua UI, Dark Mode / Light Mode auto-switching, NSWindow tooltip transparency, Dock right-click quit save prompt, and High-DPI Retina scaling.
- **KDE Plasma 5 & Plasma 6 (X11 & Wayland):** Full integration with `kwriteconfig6` and sycoca file associations via `NotepadNext-Default-Editor-Linux.sh`.
- **GNOME (X11 & Wayland):** GIO MIME database synchronization, Wayland scaling, and client-side window decoration compliance.
- **XFCE (X11):** Native Thunar file manager integration and standard FreeDesktop desktop entry registration.
- **MATE, Cinnamon, COSMIC & Budgie:** Full support via `gio mime`, `update-alternatives`, and standard POSIX shell scripts.

---

## 5. Official Distribution & Packaging Specifications

### 5.1 Packaging Artifact Matrix

| Format            | Target Platforms                        | Package Generator                       | Configuration File            |
| :---------------- | :-------------------------------------- | :-------------------------------------- | :---------------------------- |
| **macOS DMG**     | macOS 11–26 (Apple Silicon + Intel)     | CPack `DragNDrop` / Custom DMG Script   | `CMakeLists.txt` / `scripts/` |
| **Debian (.deb)** | Ubuntu 22.04–26.04, Debian 12–14        | CPack `DEB`                             | `cmake/PackagingLinux.cmake`  |
| **RedHat (.rpm)** | Fedora 39–44, Alma/Rocky 9–11, openSUSE | CPack `RPM`                             | `cmake/PackagingLinux.cmake`  |
| **AppImage**      | Universal portable Linux (glibc 2.35+)  | `linuxdeploy` / `linuxdeploy-plugin-qt` | `deploy/linux/`               |
| **FreeBSD Port**  | FreeBSD 13.x–15.x                       | Native `ports` / `pkg`                  | `editors/notepadnext`         |

---

## 6. Continuous Compatibility Verification

NotepadNext executes automated CI matrix builds validating:

- Apple Clang on macOS (Intel x86_64 & Apple Silicon arm64).
- GCC 11 through 15 across Debian, Ubuntu, and Fedora distributions.
- Qt 6.5 LTS through Qt 6.12 development snapshots.
- Zero-warning compilation with strict security flags (`-fstack-protector-strong`, `-D_FORTIFY_SOURCE=2`, `-Wl,-z,relro,-z,now`).
