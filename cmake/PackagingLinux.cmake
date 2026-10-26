message(STATUS "Configuring Linux packaging (.deb, .rpm, AppImage)")

# Standard installation paths for Linux desktop
install(TARGETS NotepadNext
    RUNTIME DESTINATION bin
)
install(FILES
    ${PROJECT_SOURCE_DIR}/deploy/linux/NotepadNext.desktop
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
if(EXISTS ${PROJECT_SOURCE_DIR}/deploy/linux/com.github.alsyundawy.NotepadNext.metainfo.xml)
    install(FILES
        ${PROJECT_SOURCE_DIR}/deploy/linux/com.github.alsyundawy.NotepadNext.metainfo.xml
        DESTINATION share/metainfo
    )
endif()

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

# Debian (.deb)
set(CPACK_DEBIAN_PACKAGE_NAME "notepadnext")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DEB_ARCH}")
set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "xdg-utils")
set(CPACK_DEBIAN_FILE_NAME "notepadnext_${PROJECT_VERSION}_${DEB_ARCH}.deb")

# Explicit Debian/Ubuntu package dependencies for automated installation via APT (`sudo apt install ./notepadnext_*.deb`).
# Uses Debian '|' alternative syntax to seamlessly support both classic Qt6 packages (Ubuntu 22.04 LTS, Debian 12 Bookworm)
# and 64-bit time_t transition packages (Ubuntu 24.04 LTS, Ubuntu 26.04, Debian 13 Trixie).
# CPACK_DEBIAN_PACKAGE_SHLIBDEPS dynamically discovers and appends libc6, libstdc++6, etc.
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libqt6core6 (>= 6.2.4) | libqt6core6t64, libqt6gui6 (>= 6.2.4) | libqt6gui6t64, libqt6widgets6 (>= 6.2.4) | libqt6widgets6t64, libqt6network6 (>= 6.2.4) | libqt6network6t64, libqt6printsupport6 (>= 6.2.4) | libqt6printsupport6t64, libqt6core5compat6 (>= 6.2.4) | libqt6core5compat6t64, qt6-qpa-plugins, libglx0, libopengl0"
)

# Red Hat (.rpm)
set(CPACK_RPM_PACKAGE_NAME "notepadnext")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "${RPM_ARCH}")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Editors")
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0-or-later")
set(CPACK_RPM_PACKAGE_AUTOREQ ON)
set(CPACK_RPM_PACKAGE_AUTOPROV ON)
set(CPACK_RPM_FILE_NAME "notepadnext-${PROJECT_VERSION}-1.${RPM_ARCH}.rpm")

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
