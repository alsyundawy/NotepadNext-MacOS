# NotepadNext Linux Packaging Guide (.deb & .rpm)

## 1. Overview

NotepadNext provides native packaging support for modern Linux distributions via CMake and CPack:

- **Debian / Ubuntu / Linux Mint / Pop!\_OS:** Debian packages (`.deb`)
- **Fedora / RHEL / Rocky Linux / AlmaLinux / openSUSE:** RPM packages (`.rpm`)

All official packages are compiled on LTS base environments (**Ubuntu 22.04 LTS / glibc 2.35**) to guarantee binary backward and forward compatibility across:

- **Ubuntu:** 22.04 LTS, 24.04 LTS, 26.04 LTS
- **Debian:** 12 (Bookworm), 13 (Trixie), Sid
- **Fedora:** 39, 40, 41, 42+
- **RHEL / Rocky / AlmaLinux:** 9, 10+
- **openSUSE:** Leap 15.5+, Tumbleweed

---

## 2. Package Specifications & Dependency Architecture

| Attribute           | Debian (`.deb`)                                                                             | RPM (`.rpm`)                                                         |
| :------------------ | :------------------------------------------------------------------------------------------ | :------------------------------------------------------------------- |
| Package Name        | `notepadnext`                                                                               | `notepadnext`                                                        |
| Output Filename     | `notepadnext_<version>_<arch>.deb`                                                          | `notepadnext-<version>-1.<arch>.rpm`                                 |
| Section / Group     | `editors`                                                                                   | `Applications/Editors`                                               |
| License             | `GPL-3.0-or-later`                                                                          | `GPL-3.0-or-later`                                                   |
| Direct Dependencies | Declared with multi-version fallback (OR logic) for classic Qt6 and t64 (64-bit time_t)     | Auto-detected ELF SONAME requirements (`AutoReq: 1`)                 |
| System Libraries    | `libc6 (>= 2.35)`, `libstdc++6 (>= 11)`, `libgcc-s1` (via `CPACK_DEBIAN_PACKAGE_SHLIBDEPS`) | `libc.so.6`, `libstdc++.so.6` (via DNF provides)                     |
| Recommends          | `xdg-utils`                                                                                 | `xdg-utils`                                                          |
| Desktop Entry       | `/usr/share/applications/NotepadNext.desktop`                                               | `/usr/share/applications/NotepadNext.desktop`                        |
| Reverse DNS Desktop | `/usr/share/applications/com.github.alsyundawy.NotepadNext.desktop`                         | `/usr/share/applications/com.github.alsyundawy.NotepadNext.desktop`  |
| AppStream Metadata  | `/usr/share/metainfo/com.github.alsyundawy.NotepadNext.metainfo.xml`                        | `/usr/share/metainfo/com.github.alsyundawy.NotepadNext.metainfo.xml` |
| Application Icon    | `/usr/share/icons/hicolor/scalable/apps/NotepadNext.svg`                                    | `/usr/share/icons/hicolor/scalable/apps/NotepadNext.svg`             |
| Default Editor Tool | `/usr/bin/NotepadNext-Default-Editor`                                                       | `/usr/bin/NotepadNext-Default-Editor`                                |

---

## 3. Why RPM Automatically Installs Dependencies vs. DEB

### 3.1 The RPM Mechanism (Fedora / DNF)

In RPM-based systems (Fedora, RHEL, openSUSE):

- CPack configures `CPACK_RPM_PACKAGE_AUTOREQ ON`.
- During package assembly, `rpmbuild` inspects the ELF `DT_NEEDED` dynamic tags of the binary (`NotepadNext`).
- It extracts raw shared object names (SONAMEs):
  `libQt6Widgets.so.6`, `libQt6Core.so.6`, `libQt6Gui.so.6`, `libQt6PrintSupport.so.6`, `libQt6Core5Compat.so.6`, `libc.so.6`.
- In Fedora / RHEL, repository metadata indexes which package provides which SONAME (`Provides: libQt6Widgets.so.6()(64bit)`).
- When a user executes:

  ```bash
  sudo dnf install ./notepadnext-0.15-07-1.x86_64.rpm
  ```

  `dnf` queries the repository index for every required SONAME and automatically downloads and installs `qt6-qtbase-gui`, `qt6-qt5compat`, etc.

### 3.2 The Debian/Ubuntu Mechanism (DEB / APT)

In Debian-based systems (Ubuntu, Debian, Mint):

- Debian package management (`dpkg` and `apt`) does **not** query repositories by raw ELF SONAME strings at install time. It requires **explicit package names** in the `Depends:` field of `debian/control`.
- If CPack relies exclusively on `CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON` while Qt was installed from an external SDK or GitHub Actions path (e.g., `/home/runner/work/...`), `dpkg-shlibdeps` cannot map those shared libraries to system Debian package names. Consequently, Qt packages could be omitted from `Depends:`.
- To solve this, NotepadNext defines `CPACK_DEBIAN_PACKAGE_DEPENDS` in `cmake/PackagingLinux.cmake`.

### 3.3 The 64-bit `time_t` Transition (`t64`)

Starting with **Ubuntu 24.04 LTS (Noble)** and **Debian 13 (Trixie)**, Debian implemented the 64-bit `time_t` architecture transition (Y2038 protection), renaming standard runtime libraries:

- `libqt6core6` ➜ `libqt6core6t64`
- `libqt6gui6` ➜ `libqt6gui6t64`
- `libqt6widgets6` ➜ `libqt6widgets6t64`
- `libqt6network6` ➜ `libqt6network6t64`
- `libqt6printsupport6` ➜ `libqt6printsupport6t64`
- `libqt6core5compat6` ➜ `libqt6core5compat6t64`

Meanwhile, **Ubuntu 22.04 LTS (Jammy)** and **Debian 12 (Bookworm)** retain the non-t64 package names. NotepadNext uses Debian's alternative dependency operator (`|`):

```text
libqt6core6 (>= 6.2.4) | libqt6core6t64,
libqt6gui6 (>= 6.2.4) | libqt6gui6t64,
libqt6widgets6 (>= 6.2.4) | libqt6widgets6t64,
libqt6network6 (>= 6.2.4) | libqt6network6t64,
libqt6printsupport6 (>= 6.2.4) | libqt6printsupport6t64,
libqt6core5compat6 (>= 6.2.4) | libqt6core5compat6t64,
qt6-qpa-plugins,
libglx0,
libopengl0
```

This syntax instructs APT to install the classic package on Debian 12 / Ubuntu 22.04, or the `t64` package on Ubuntu 24.04+ / Debian 13+.

---

## 4. How to Install: The Crucial Tool Difference (`apt` vs. `dpkg`)

Many users historically ran:

```bash
# ⚠️ Legacy command: Does NOT resolve or download dependencies from mirrors!
sudo dpkg -i notepadnext_0.15-07_amd64.deb
```

`dpkg` is a low-level package unpacker. It has **no network access** and **cannot query APT repositories**. If any dependency is missing, `dpkg -i` aborts with an `unmet dependencies` error.

### ✅ Recommended Modern Installation Command

On Debian 10+, Ubuntu 18.04+, and all modern derivatives:

```bash
# ✅ Modern high-level APT command (equivalent to 'dnf install ./file.rpm')
sudo apt install ./notepadnext_0.15-07_amd64.deb
```

> **Note:** The leading `./` (or absolute path `/path/to/...`) is mandatory. It tells `apt` that the target is a local file rather than a remote repository package name. `apt` will automatically inspect all dependencies, fetch missing packages from official mirrors, install them, and configure NotepadNext in a single automated step.

---

## 5. Building Packages Locally

### 5.1 System Prerequisites

#### Ubuntu 22.04 LTS / Debian 12 (Bookworm)

```bash
sudo apt update
sudo apt install -y --no-install-recommends \
    build-essential cmake ninja-build pkg-config \
    qt6-base-dev qt6-base-dev-tools qt6-5compat-dev \
    qt6-tools-dev qt6-tools-dev-tools linguist-qt6 \
    libgl1-mesa-dev libxkbcommon-dev libxkbcommon-x11-0 \
    libfontconfig1-dev libfreetype6-dev libxcb-cursor-dev \
    rpm dpkg-dev
```

#### Ubuntu 24.04 LTS (Noble) / Ubuntu 26.04 / Debian 13 (Trixie)

```bash
sudo apt update
sudo apt install -y --no-install-recommends \
    build-essential cmake ninja-build pkg-config \
    qt6-base-dev qt6-base-private-dev qt6-base-dev-tools \
    qt6-5compat-dev qt6-tools-dev qt6-tools-dev-tools linguist-qt6 \
    libgl1-mesa-dev libxkbcommon-dev libxkbcommon-x11-0 \
    libfontconfig1-dev libfreetype6-dev libxcb-cursor-dev \
    rpm dpkg-dev
```

#### Fedora / RHEL / CentOS Stream

```bash
sudo dnf install -y \
    gcc-c++ cmake ninja-build rpm-build pkgconf-pkg-config \
    qt6-qtbase-devel qt6-qt5compat-devel qt6-qttools-devel \
    mesa-libGL-devel libxkbcommon-devel libxkbcommon-x11-devel \
    fontconfig-devel freetype-devel xcb-util-cursor-devel
```

### 5.2 Build via CMake Targets

```bash
# 1. Configure Release build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# 2. Compile application
cmake --build build --parallel

# 3. Assemble Debian package (.deb)
cmake --build build --target deb

# 4. Assemble Red Hat package (.rpm)
cmake --build build --target rpm
```

The resulting packages are generated in the `build/` root:

- `build/notepadnext_0.15-07_amd64.deb`
- `build/notepadnext-0.15-07-1.x86_64.rpm`
