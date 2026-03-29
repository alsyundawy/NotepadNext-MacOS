message(STATUS "Configuring macOS packaging")
message(STATUS "macOS Deployment Target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")

# ──────────────────────────────────────────────────────────────
# Paksa deployment target 12.0 (Monterey) jika belum diset dari workflow
# Ini backup safety supaya tidak ikut runner macOS 15
# ──────────────────────────────────────────────────────────────
if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS version" FORCE)
    message(STATUS "Deployment target di-set otomatis ke 12.0 (Monterey)")
endif()

# Pastikan CMake tahu kita build bundle
set_target_properties(NotepadNext PROPERTIES
    MACOSX_BUNDLE TRUE
)

# Custom Info.plist (sudah kamu pakai)
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

# Install rules
set(INSTALL_DIR ${CMAKE_BINARY_DIR}/install)

install(TARGETS NotepadNext
    BUNDLE DESTINATION .
)

install(FILES ${APP_ICON_MACOS}
    DESTINATION NotepadNext.app/Contents/Resources
)

# Custom target untuk install ke folder sementara
add_custom_target(install_local
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_BINARY_DIR}
        --prefix ${INSTALL_DIR}
    DEPENDS NotepadNext
    COMMENT "Installing NotepadNext to ${INSTALL_DIR}"
)

# macdeployqt (harus sudah ada di PATH dari install-qt-action)
find_program(MACDEPLOYQT_EXECUTABLE macdeployqt REQUIRED)

# Custom target dmg (sudah dioptimasi)
add_custom_target(dmg
    COMMAND ${MACDEPLOYQT_EXECUTABLE}
        ${INSTALL_DIR}/NotepadNext.app
        -dmg
        -verbose=2                     # tambahan: biar log lebih jelas saat debug
    COMMAND ${CMAKE_COMMAND} -E rename
        ${INSTALL_DIR}/NotepadNext.dmg
        ${CMAKE_BINARY_DIR}/NotepadNext-v${PROJECT_VERSION}.dmg
    DEPENDS install_local
    COMMENT "Creating DMG with macdeployqt (Deployment Target: ${CMAKE_OSX_DEPLOYMENT_TARGET})"
)
