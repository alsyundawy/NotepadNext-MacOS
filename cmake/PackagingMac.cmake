message(STATUS "Configuring macOS packaging")

if(NOT TARGET NotepadNext)
    return()
endif()

set(INSTALL_DIR ${CMAKE_BINARY_DIR}/install)

# Custom Info.plist
set_target_properties(NotepadNext PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/deploy/macos/info.plist
)

# Application icon
set(APP_ICON_MACOS ${CMAKE_SOURCE_DIR}/icon/NotepadNext.icns)

set_source_files_properties(${APP_ICON_MACOS}
    PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
)

target_sources(NotepadNext PRIVATE ${APP_ICON_MACOS})

set_target_properties(NotepadNext PROPERTIES
    MACOSX_BUNDLE_ICON_FILE NotepadNext.icns
)

install(TARGETS NotepadNext
    BUNDLE DESTINATION .
)

install(FILES ${APP_ICON_MACOS}
    DESTINATION NotepadNext.app/Contents/Resources
)

add_custom_target(install_local
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_BINARY_DIR}
        --prefix ${INSTALL_DIR}
    DEPENDS NotepadNext
)

# Determine macOS target architecture suffix (arm64 or x64)
if(NOT DEFINED MAC_ARCH_SUFFIX OR MAC_ARCH_SUFFIX STREQUAL "")
    if(CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
        set(MAC_ARCH_SUFFIX "arm64")
    elseif(CMAKE_OSX_ARCHITECTURES MATCHES "x86_64")
        set(MAC_ARCH_SUFFIX "x64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        set(MAC_ARCH_SUFFIX "arm64")
    else()
        set(MAC_ARCH_SUFFIX "x64")
    endif()
endif()

if(DEFINED APP_VERSION AND NOT APP_VERSION STREQUAL "")
    set(MAC_DMG_NAME "NotepadNext-v${APP_VERSION}-${MAC_ARCH_SUFFIX}.dmg")
    set(MAC_DMG_LEGACY_NAME "NotepadNext-v${PROJECT_VERSION}-${MAC_ARCH_SUFFIX}.dmg")
else()
    set(MAC_DMG_NAME "NotepadNext-v${PROJECT_VERSION}-${MAC_ARCH_SUFFIX}.dmg")
    set(MAC_DMG_LEGACY_NAME "")
endif()

find_program(MACDEPLOYQT_EXECUTABLE macdeployqt REQUIRED)
find_program(HDIUTIL_EXECUTABLE hdiutil REQUIRED)

set(PRUNE_UNUSED_QT_FILES
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${INSTALL_DIR}/NotepadNext.app/Contents/Frameworks/QtPdf.framework
        ${INSTALL_DIR}/NotepadNext.app/Contents/Frameworks/QtQml.framework
        ${INSTALL_DIR}/NotepadNext.app/Contents/Frameworks/QtQmlMeta.framework
        ${INSTALL_DIR}/NotepadNext.app/Contents/Frameworks/QtQmlModels.framework
        ${INSTALL_DIR}/NotepadNext.app/Contents/Frameworks/QtQmlWorkerScript.framework
        ${INSTALL_DIR}/NotepadNext.app/Contents/Frameworks/QtQuick.framework
        ${INSTALL_DIR}/NotepadNext.app/Contents/Frameworks/QtVirtualKeyboard.framework
        ${INSTALL_DIR}/NotepadNext.app/Contents/Frameworks/QtVirtualKeyboardQml.framework
        ${INSTALL_DIR}/NotepadNext.app/Contents/Frameworks/QtOpenGL.framework
        ${INSTALL_DIR}/NotepadNext.app/Contents/PlugIns/platforminputcontexts
        ${INSTALL_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqpdf.dylib
        ${INSTALL_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqtiff.dylib
        ${INSTALL_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqwebp.dylib
        ${INSTALL_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqmacheif.dylib
        ${INSTALL_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqmacjp2.dylib
        ${INSTALL_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqtga.dylib
        ${INSTALL_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqwbmp.dylib
        ${INSTALL_DIR}/NotepadNext.app/Contents/PlugIns/imageformats/libqicns.dylib
)

if(MAC_DMG_LEGACY_NAME)
    add_custom_target(dmg
        COMMAND ${MACDEPLOYQT_EXECUTABLE}
            ${INSTALL_DIR}/NotepadNext.app
        ${PRUNE_UNUSED_QT_FILES}
        COMMAND ${HDIUTIL_EXECUTABLE} create
            -volname "NotepadNext"
            -srcfolder ${INSTALL_DIR}/NotepadNext.app
            -ov -format UDBZ
            ${CMAKE_BINARY_DIR}/${MAC_DMG_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/${MAC_DMG_NAME}
            ${CMAKE_BINARY_DIR}/${MAC_DMG_LEGACY_NAME}
        DEPENDS install_local
    )
else()
    add_custom_target(dmg
        COMMAND ${MACDEPLOYQT_EXECUTABLE}
            ${INSTALL_DIR}/NotepadNext.app
        ${PRUNE_UNUSED_QT_FILES}
        COMMAND ${HDIUTIL_EXECUTABLE} create
            -volname "NotepadNext"
            -srcfolder ${INSTALL_DIR}/NotepadNext.app
            -ov -format UDBZ
            ${CMAKE_BINARY_DIR}/${MAC_DMG_NAME}
        DEPENDS install_local
    )
endif()
