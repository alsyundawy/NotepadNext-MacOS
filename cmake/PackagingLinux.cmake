if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    message(STATUS "Configuring FreeBSD desktop installation")
else()
    message(STATUS "Configuring Linux packaging (.deb, .rpm, AppImage)")
endif()

# Standard installation paths for Linux & BSD desktops
install(TARGETS NotepadNext
    RUNTIME DESTINATION bin
)
install(FILES
    ${PROJECT_SOURCE_DIR}/deploy/linux/NotepadNext.desktop
    ${PROJECT_SOURCE_DIR}/deploy/linux/com.github.alsyundawy.NotepadNext.desktop
    DESTINATION share/applications
)
install(FILES
    ${PROJECT_SOURCE_DIR}/icon/NotepadNext.svg
    DESTINATION share/icons/hicolor/scalable/apps
)
install(FILES
    ${PROJECT_SOURCE_DIR}/icon/NotepadNext.svg
    DESTINATION share/icons/hicolor/scalable/mimetypes
)
if(EXISTS ${PROJECT_SOURCE_DIR}/NotepadNext-Default-Editor-Linux.sh)
    install(PROGRAMS
        ${PROJECT_SOURCE_DIR}/NotepadNext-Default-Editor-Linux.sh
        DESTINATION bin
        RENAME NotepadNext-Default-Editor
    )
endif()
if(EXISTS ${PROJECT_SOURCE_DIR}/deploy/linux/com.github.alsyundawy.NotepadNext.metainfo.xml)
    install(FILES
        ${PROJECT_SOURCE_DIR}/deploy/linux/com.github.alsyundawy.NotepadNext.metainfo.xml
        DESTINATION share/metainfo
    )
endif()

if(NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
# ─── CPack Configuration (.deb & .rpm) ─────────────────────────────
set(CPACK_PACKAGE_NAME "notepadnext")
set(CPACK_PACKAGE_VENDOR "Harry Dertin Sutisna / Justin Dailey")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform reimplementation of Notepad++ for macOS and Linux")
set(CPACK_PACKAGE_DESCRIPTION "Notepad Next is a fast, versatile text and source code editor built with Scintilla, Lexilla, and Qt.")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_CONTACT "Harry Dertin Sutisna <alsyundawy@gmail.com>")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/alsyundawy/NotepadNext-MacOS")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

# Architecture resolution for Linux packages
if(NOT DEFINED LINUX_PKG_ARCH OR LINUX_PKG_ARCH STREQUAL "")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
        set(DEB_ARCH "amd64")
        set(RPM_ARCH "x86_64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(DEB_ARCH "arm64")
        set(RPM_ARCH "aarch64")
    else()
        set(DEB_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
        set(RPM_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
    endif()
endif()

# Optional Qt version tag for multi-Qt release matrices (e.g., -qt6.5, -qt6.8)
if(DEFINED PKG_QT_SUFFIX AND NOT PKG_QT_SUFFIX STREQUAL "")
    set(PKG_QT_TAG "-${PKG_QT_SUFFIX}")
else()
    set(PKG_QT_TAG "")
endif()

# Debian (.deb)
set(CPACK_DEBIAN_PACKAGE_NAME "notepadnext")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DEB_ARCH}")
set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "xdg-utils")
set(CPACK_DEBIAN_FILE_NAME "notepadnext_${PROJECT_VERSION}${PKG_QT_TAG}_${DEB_ARCH}.deb")

# Explicit Debian/Ubuntu package dependencies for automated installation via APT (`sudo apt install ./notepadnext_*.deb`).
# Uses Debian '|' alternative syntax to seamlessly support both classic Qt6 packages (Ubuntu 22.04 LTS, Debian 12 Bookworm)
# and 64-bit time_t transition packages (Ubuntu 24.04 LTS, Ubuntu 26.04, Debian 13 Trixie).
# Also includes libxcb-cursor0 required by the Qt 6.5+ XCB platform plugin on modern desktop installations.
# CPACK_DEBIAN_PACKAGE_SHLIBDEPS dynamically discovers and appends libc6, libstdc++6, etc.
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libqt6core6 (>= 6.2.4) | libqt6core6t64, libqt6gui6 (>= 6.2.4) | libqt6gui6t64, libqt6widgets6 (>= 6.2.4) | libqt6widgets6t64, libqt6network6 (>= 6.2.4) | libqt6network6t64, libqt6printsupport6 (>= 6.2.4) | libqt6printsupport6t64, libqt6core5compat6 (>= 6.2.4) | libqt6core5compat6t64, qt6-qpa-plugins, libxcb-cursor0, libglx0, libopengl0"
)

# Red Hat (.rpm)
set(CPACK_RPM_PACKAGE_NAME "notepadnext")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "${RPM_ARCH}")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Editors")
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0-or-later")
set(CPACK_RPM_PACKAGE_AUTOREQ ON)
set(CPACK_RPM_PACKAGE_AUTOPROV ON)
set(CPACK_RPM_FILE_NAME "notepadnext-${PROJECT_VERSION}${PKG_QT_TAG}-1.${RPM_ARCH}.rpm")

# Architecture-specific ELF SONAME suffix for 64-bit RPM architectures
if(RPM_ARCH MATCHES "x86_64|aarch64|ppc64le|s390x")
    set(RPM_SONAME_SUFFIX "()(64bit)")
else()
    set(RPM_SONAME_SUFFIX "")
endif()

# Explicit RPM package dependencies for automated installation via DNF/Zypper (`sudo dnf install ./notepadnext-*.rpm`).
# Uses standard ELF SONAME provides recognized across Fedora, RHEL, CentOS, Rocky Linux, AlmaLinux, and openSUSE:
# - Fedora/RHEL automatically maps these to `qt6-qtbase-gui` and `qt6-qt5compat`
# - openSUSE automatically maps these to `libQt6Widgets6`, `libQt6Gui6`, and `libQt6Core5Compat6`
# DNF/Zypper queries repository indexes, resolves, and prompts users to download all missing Qt6 packages.
set(CPACK_RPM_PACKAGE_REQUIRES
    "libQt6Core.so.6${RPM_SONAME_SUFFIX}, libQt6Gui.so.6${RPM_SONAME_SUFFIX}, libQt6Widgets.so.6${RPM_SONAME_SUFFIX}, libQt6Network.so.6${RPM_SONAME_SUFFIX}, libQt6PrintSupport.so.6${RPM_SONAME_SUFFIX}, libQt6Core5Compat.so.6${RPM_SONAME_SUFFIX}, libGLX.so.0${RPM_SONAME_SUFFIX}, libOpenGL.so.0${RPM_SONAME_SUFFIX}, xdg-utils"
)

# Convenient custom build targets
add_custom_target(deb
    COMMAND ${CMAKE_CPACK_COMMAND} -G DEB
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS NotepadNext
)

add_custom_target(rpm
    COMMAND ${CMAKE_CPACK_COMMAND} -G RPM
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS NotepadNext
)

include(CPack)

# ─── AppImage Support (Optional when APP_DISTRIBUTION=AppImage) ────
if(APP_DISTRIBUTION STREQUAL "AppImage")
    set(APPDIR ${CMAKE_BINARY_DIR}/AppDir)
    set(APPDIR_USR ${APPDIR}/usr)
    set(LINUXDEPLOY ${CMAKE_BINARY_DIR}/linuxdeploy-x86_64.AppImage)
    set(LINUXDEPLOY_QT ${CMAKE_BINARY_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage)
    set(APPIMAGE_ENV_VARS LDAI_OUTPUT=NotepadNext-v${PROJECT_VERSION}-x86_64.AppImage)

    if(DEFINED ENV{QMAKE} AND NOT "$ENV{QMAKE}" STREQUAL "")
        set(APPIMAGE_QMAKE "$ENV{QMAKE}")
    else()
        find_program(APPIMAGE_QMAKE NAMES qmake)
    endif()

    if(APPIMAGE_QMAKE)
        list(APPEND APPIMAGE_ENV_VARS QMAKE=${APPIMAGE_QMAKE})
        add_custom_target(appdir
            COMMAND ${CMAKE_COMMAND} --install . --prefix ${APPDIR_USR}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS NotepadNext
        )
        add_custom_target(download_linuxdeploy
            COMMAND wget --no-verbose -O ${LINUXDEPLOY} https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
            COMMAND wget --no-verbose -O ${LINUXDEPLOY_QT} https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
            COMMAND chmod +x ${LINUXDEPLOY} ${LINUXDEPLOY_QT}
        )
        add_custom_target(appimage
            COMMAND ${CMAKE_COMMAND} -E env ${APPIMAGE_ENV_VARS} ${LINUXDEPLOY}
                --appdir ${APPDIR}
                --executable ${APPDIR_USR}/bin/NotepadNext
                --desktop-file ${APPDIR_USR}/share/applications/NotepadNext.desktop
                --icon-file ${APPDIR_USR}/share/icons/hicolor/scalable/apps/NotepadNext.svg
                --plugin qt
                --output appimage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS appdir download_linuxdeploy
        )
    endif()
endif()
endif()
