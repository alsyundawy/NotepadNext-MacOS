# Native Build & Packaging Guide — Debian 12+ & Ubuntu 22.04+

## 1. Overview & Lineage

NotepadNext is a fast, versatile native Linux and macOS desktop text editor built on the **Scintilla 5.6.7** and **Lexilla 5.5.4** editing engines with a native **Qt 6** GUI (no Win32 emulation, no Wine layer).

This guide provides step-by-step native compilation and `.deb` packaging instructions for:

- **Debian 12 (Bookworm)** and **Debian 13 (Trixie / Sid)**
- **Ubuntu 22.04 LTS (Jammy Jellyfish)**
- **Ubuntu 24.04 LTS (Noble Numbat)**
- **Ubuntu 26.04 LTS**

> **Lineage & Upstream Reference:** This document incorporates native Linux build strategies established in upstream projects and community reference implementations, notably Jeremie Conte's [`NotepadNext-native-linux`](https://github.com/jeremieconte/NotepadNext-native-linux) (`BUILD-UBUNTU-26.04.md`), adapted for NotepadNext-MacOS & Linux edition.

---

## 2. Build Dependencies

Install the required system compilers, CMake, Qt6 development libraries, and X11/XCB headers.

### 2.1 Ubuntu 24.04 LTS (Noble), Ubuntu 26.04 LTS & Debian 13 (Trixie)

```bash
sudo apt update
sudo apt install -y \
    build-essential cmake ninja-build pkgconf \
    qt6-base-dev qt6-base-private-dev qt6-base-dev-tools \
    qt6-5compat-dev qt6-tools-dev qt6-tools-dev-tools linguist-qt6 \
    libgl1-mesa-dev libxkbcommon-dev libxkbcommon-x11-0 \
    libfontconfig1-dev libfreetype6-dev libxcb1-dev libxcb-cursor-dev \
    dpkg-dev rpm
```

### 2.2 Ubuntu 22.04 LTS (Jammy) & Debian 12 (Bookworm)

On Ubuntu 22.04 and Debian 12, install the standard Qt6 developer packages:

```bash
sudo apt update
sudo apt install -y \
    build-essential g++-12 cmake ninja-build pkg-config \
    qt6-base-dev qt6-base-dev-tools \
    qt6-5compat-dev qt6-tools-dev qt6-tools-dev-tools linguist-qt6 \
    libgl1-mesa-dev libxkbcommon-dev libxkbcommon-x11-0 \
    libfontconfig1-dev libfreetype6-dev libxcb1-dev libxcb-cursor-dev \
    dpkg-dev rpm
```

_(On Ubuntu 22.04, `g++-12` provides optimal C++20 standard library support for Scintilla and Lexilla)._

---

## 3. Vendored & Automatic Dependencies (CPM)

NotepadNext vendors core editing algorithms directly within `thirdparty/` for reproducibility and offline stability:

- **Scintilla 5.6.7** (`thirdparty/scintilla`)
- **Lexilla 5.5.4** (`thirdparty/lexilla`)
- **Lua 5.5.1 / 5.4.x** (`thirdparty/lua`)
- **uchardet** (`thirdparty/uchardet`)

Auxiliary Qt utilities are retrieved automatically via **CPM (CMake Package Manager)** during CMake configuration (internet connection required only on the first configure):

- **Qt-Advanced-Docking-System (ADS)**
- **QSimpleUpdater**
- **SingleApplication**
- **EditorConfig Core Qt**

---

## 4. Technical Portage Notes & Workarounds

### 4.1 Qt-Advanced-Docking-System & `Qt6::GuiPrivate` on Qt ≥ 6.9

As documented in `NotepadNext-native-linux/BUILD-UBUNTU-26.04.md`:

- On Qt ≥ 6.9 (such as Ubuntu 24.10, 26.04, or modern Qt SDKs), ADS links against `Qt6::GuiPrivate` to access `<qpa/qplatformnativeinterface.h>` for X11 floating title bar customization.
- When compiling against system Qt packages on these distributions, the package `qt6-base-private-dev` **must** be installed. If absent, CMake will fail looking for private header paths under `/usr/include/.../QtGui/<version>`.

### 4.2 glibc Backward Compatibility Invariant

- Binaries built on **Ubuntu 22.04 LTS** link against `glibc 2.35`.
- Because the GNU C Library (glibc) maintains strict backward compatibility, an executable built on `glibc 2.35` will seamlessly run on **Ubuntu 22.04**, **Debian 12** (`glibc 2.36`), **Ubuntu 24.04** (`glibc 2.39`), and **Ubuntu 26.04**.
- Conversely, a binary compiled on Ubuntu 24.04 (`glibc 2.39`) **cannot** run on Ubuntu 22.04 or Debian 12 without upgrading the host glibc.

---

## 5. Compilation & Build Steps

```bash
# 1. Clone repository
git clone https://github.com/alsyundawy/NotepadNext-MacOS.git
cd NotepadNext-MacOS

# 2. Configure CMake Release build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# 3. Build executable in parallel
cmake --build build --parallel "$(nproc)"

# 4. Run binary directly
./build/src/NotepadNext
```

Binary hardening flags (`RELRO`, `-z now`, `-fstack-protector-strong`, `-D_FORTIFY_SOURCE=2`) are enabled automatically by the root `CMakeLists.txt`.

---

## 6. Packaging as `.deb` & `.rpm`

```bash
# Generate Debian package (.deb)
cmake --build build --target deb

# Generate Red Hat package (.rpm)
cmake --build build --target rpm
```

The resulting files will be placed in `build/`:

- `build/notepadnext_0.15-07_amd64.deb`
- `build/notepadnext-0.15-07-1.x86_64.rpm`

### 6.1 Inspect Package Metadata

```bash
# Inspect generated Debian package control metadata:
dpkg-deb -I build/notepadnext_0.15-07_amd64.deb

# Verify file contents inside .deb:
dpkg-deb -c build/notepadnext_0.15-07_amd64.deb
```

---

## 7. Installing with Automated Dependency Resolution

To install the `.deb` package while automatically resolving and downloading all required dependencies from APT repositories:

```bash
# ✅ Recommended APT installation command (local file):
sudo apt install ./build/notepadnext_0.15-07_amd64.deb
```

> **Why `apt install ./file.deb` instead of `dpkg -i`?**  
> `dpkg -i` is a low-level unpacker that cannot query APT repository mirrors. `sudo apt install ./file.deb` calculates the full dependency graph and automatically downloads required Qt6 libraries (`libqt6widgets6`, `qt6-qpa-plugins`, etc.) just like `sudo dnf install ./file.rpm` does on Fedora.
