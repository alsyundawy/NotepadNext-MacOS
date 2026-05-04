cmake_minimum_required(VERSION 3.21)

# ============================================================
# CMake Policy Overrides
# Must be declared BEFORE project() to take effect.
#
# CMP0091 - Enables MSVC_RUNTIME_LIBRARY as a per-target
#            property (replaces legacy global variable approach).
# CMP0092 - Prevents CMake from injecting /W3 into
#            CMAKE_CXX_FLAGS by default on MSVC builds,
#            allowing explicit warning level control below.
# ============================================================
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0092 NEW)

# ============================================================
# Project Declaration
#
# Version and description match the upstream repository exactly.
# Latest release: v0.14 (published 2026-04-27).
# Ref: https://github.com/dail8859/NotepadNext/releases/tag/v0.14
# ============================================================
project(NotepadNext
    VERSION 0.14
    DESCRIPTION "NotepadNext, reimplementation of Notepad++ text editor"
    HOMEPAGE_URL "https://github.com/lsyundawy/NotepadNext-MacOS"
    LANGUAGES CXX
)

# ============================================================
# Application Metadata
#
# APP_DISTRIBUTION - Optional distribution channel label
#   (e.g. "Microsoft Store", "Flatpak", "AppImage").
#   Leave empty for standard/generic builds.
#
# APP_COPYRIGHT - Auto-populated using the current calendar year
#   via CMake's string(TIMESTAMP) at configure time.
# ============================================================
set(APP_DISTRIBUTION "" CACHE STRING "Distribution type")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(APP_COPYRIGHT "Copyright 2019-${CURRENT_YEAR} Harry DS Alsyundawy")

# ============================================================
# C++ Language Standard
#
# C++20 is required. Extensions are disabled to enforce
# strict ISO conformance and improve cross-compiler portability.
# ============================================================
set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# ============================================================
# Compiler Cache (ccache)
#
# If ccache is found on the system PATH, it is used as a
# compiler launcher to cache compiled translation units.
# This significantly reduces incremental rebuild times,
# especially on clean CI environments.
#
# Install:
#   Linux:   sudo apt install ccache
#   macOS:   brew install ccache
#   Windows: https://ccache.dev/download.html
# ============================================================
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "ccache found and enabled: ${CCACHE_PROGRAM}")
endif()

# ============================================================
# Default Build Type
#
# Single-config generators (Make, Ninja) require CMAKE_BUILD_TYPE
# to be set explicitly. This guard defaults to "Release" when
# the user has not provided a value.
#
# Multi-config generators (Visual Studio, Xcode) are excluded
# via the GENERATOR_IS_MULTI_CONFIG property check.
# ============================================================
get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT _isMultiConfig AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Build configuration: Debug | Release | RelWithDebInfo | MinSizeRel"
        FORCE)
    message(STATUS "CMAKE_BUILD_TYPE not set — defaulting to Release")
endif()

# ============================================================
# Interprocedural Optimization (IPO / LTO)
#
# When supported, enables whole-program link-time optimization
# for Release builds. This allows the linker to inline, prune
# dead code, and optimize across translation unit boundaries.
# Not all toolchains support this; the check is safe to run
# on all platforms.
# ============================================================
include(CheckIPOSupported)
check_ipo_supported(RESULT _ipo_supported OUTPUT _ipo_output)
if(_ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    message(STATUS "IPO/LTO enabled for Release builds")
else()
    message(STATUS "IPO/LTO not supported: ${_ipo_output}")
endif()

# ============================================================
# Global Compile Definitions
#
# SCI_OWNREGEX - Instructs Scintilla to use its own built-in
#                regular expression engine instead of the
#                platform system regex. Required for consistent
#                regex behavior across Windows, macOS, and Linux.
# ============================================================
add_compile_definitions(SCI_OWNREGEX)

# ============================================================
# Compiler & Platform-Specific Flags
# ============================================================

if(MSVC)
    # ----------------------------------------------------------
    # Workaround: std::mutex crash on first lock() call
    # Affects applications built with recent Visual Studio.
    # Ref: https://stackoverflow.com/questions/78598141/
    #      first-stdmutexlock-crashes-in-application-built-with-latest-visual-studio
    # ----------------------------------------------------------
    add_compile_definitions(_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)

    # ----------------------------------------------------------
    # Control Flow Guard (CFG)
    # Hardens indirect function calls at runtime to mitigate
    # control-flow hijacking attacks (e.g. ROP chains).
    # Must be applied at both compile-time and link-time.
    # ----------------------------------------------------------
    add_compile_options(/guard:cf)
    add_link_options(/guard:cf)

    # ----------------------------------------------------------
    # Warning level, conformance, encoding, and build speed
    #
    # /W4          - Enable most useful warnings (stricter than default /W3)
    # /wd4100      - Suppress: unreferenced formal parameter
    # /wd4127      - Suppress: conditional expression is constant
    # /wd4251      - Suppress: class needs dll-interface (common with Qt exports)
    # /permissive- - Reject non-standard MSVC language extensions
    # /utf-8       - Treat source and execution character set as UTF-8
    # /MP          - Parallel compilation across multiple CPU cores
    # ----------------------------------------------------------
    add_compile_options(
        /W4
        /wd4100
        /wd4127
        /wd4251
        /permissive-
        /utf-8
        /MP
    )

    # ----------------------------------------------------------
    # Additional security hardening (linker)
    #
    # /sdl         - Enable extra Security Development Lifecycle checks
    # /NXCOMPAT    - Mark binary as compatible with DEP (NX/XD bit)
    # /DYNAMICBASE - Enable ASLR (Address Space Layout Randomization)
    # ----------------------------------------------------------
    add_compile_options(/sdl)
    add_link_options(/NXCOMPAT /DYNAMICBASE)

elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall               # Enable standard set of useful warnings
        -Wextra             # Enable additional warnings beyond -Wall
        -Wpedantic          # Enforce strict ISO C++ conformance
        -Wno-unused-parameter  # Suppress unused function parameter warnings
    )

    if(UNIX AND NOT APPLE)
        # ------------------------------------------------------
        # Linux: Security hardening flags
        # These flags match the upstream intent and extend it
        # with -fPIE / -pie for proper ASLR support.
        #
        # -fstack-protector-strong  - Insert stack canaries on
        #   functions with buffers or address-taken locals
        #   (broader coverage than -fstack-protector)
        # -D_FORTIFY_SOURCE=2       - Enable glibc runtime buffer
        #   overflow detection for common libc functions
        # -fPIE                     - Emit position-independent code
        #   for executables; required for full ASLR effectiveness
        # -Wl,-z,relro              - Mark ELF segments read-only
        #   after dynamic linking (partial RELRO)
        # -Wl,-z,now                - Resolve all PLT symbols at
        #   load time, enabling full RELRO and closing GOT attacks
        # -pie                      - Produce a PIE executable
        # ------------------------------------------------------
        add_compile_options(
            -fstack-protector-strong
            -D_FORTIFY_SOURCE=2
            -fPIE
        )
        add_link_options(
            -Wl,-z,relro
            -Wl,-z,now
            -pie
        )
    endif()

    if(APPLE)
        # ------------------------------------------------------
        # macOS: Stack protection only.
        # macOS enforces ASLR and W^X at the OS and linker
        # level by default; no additional PIE flags needed here.
        # ------------------------------------------------------
        add_compile_options(-fstack-protector-strong)
    endif()
endif()

# ============================================================
# Qt6 Dependency
#
# Components listed here exactly match those declared in the
# upstream CMakeLists.txt as of commit ef77498 (v0.14).
# No extra components are added; do not include components
# that are not linked by any target in src/ or thirdparty/.
#
# Core         - QObject, QThread, QFile, QSettings, etc.
# Gui          - QPainter, QFont, QImage, platform abstraction
# Widgets      - QMainWindow, QDialog, QTreeView, etc.
# Network      - QNetworkAccessManager, HTTP, TCP/UDP
# PrintSupport - QPrinter, QPrintDialog integration
# Core5Compat  - Qt5 compatibility APIs still used in this codebase
# ============================================================
find_package(Qt6
    REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
        PrintSupport
        Core5Compat
)

# ============================================================
# Qt Standard Project Setup
#
# qt_standard_project_setup() enables:
#   CMAKE_AUTOMOC ON  - Auto-process Q_OBJECT / Q_GADGET macros
#   CMAKE_AUTORCC ON  - Auto-compile .qrc resource files
#   CMAKE_AUTOUIC ON  - Auto-compile .ui form files
#
# NOTE: The upstream file sets CMAKE_AUTORCC ON manually before
# this call. That line is intentionally omitted here because
# qt_standard_project_setup() already sets it — the manual
# assignment is redundant (confirmed upstream, commit ef77498).
# ============================================================
qt_standard_project_setup()

# ============================================================
# CPM.cmake — Dependency Source Cache
#
# CPM stores downloaded package sources under .cpm-cache/ in
# the repository root. This prevents redundant re-downloads
# across clean builds and CI pipeline runs.
#
# To use a shared global cache across projects:
#   cmake -DCPM_SOURCE_CACHE=/path/to/shared/cache ..
# ============================================================
set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.cpm-cache"
    CACHE PATH "CPM source cache directory")

# ============================================================
# Subdirectories
#
# thirdparty/ - External libraries managed via CPM or bundled
# src/        - NotepadNext application source code
# ============================================================
add_subdirectory(thirdparty)
add_subdirectory(src)

# ============================================================
# Packaging
#
# Delegates to platform-specific packaging scripts:
#   cmake/PackagingWindows.cmake  (Win32)
#   cmake/PackagingMac.cmake      (Apple)
#   cmake/PackagingLinux.cmake    (Linux)
# Each script is included with OPTIONAL so a missing file is
# not a configure-time error (confirmed in cmake/Packaging.cmake).
# ============================================================
include(${CMAKE_SOURCE_DIR}/cmake/Packaging.cmake)

# ============================================================
# Build Configuration Summary
# ============================================================
message(STATUS "")
message(STATUS "======================================")
message(STATUS "  NotepadNext — Build Configuration  ")
message(STATUS "======================================")
message(STATUS "  Version        : ${PROJECT_VERSION}")
message(STATUS "  Build Type     : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler       : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  C++ Standard   : C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Qt6 Version    : ${Qt6_VERSION}")
message(STATUS "  IPO/LTO        : ${_ipo_supported}")
message(STATUS "  ccache         : ${CCACHE_PROGRAM}")
message(STATUS "  Install Prefix : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "======================================")
message(STATUS "")