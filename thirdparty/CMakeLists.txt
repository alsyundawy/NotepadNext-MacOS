include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME ads
    GITHUB_REPOSITORY githubuser0xFFFF/Qt-Advanced-Docking-System
    GIT_TAG v5.1.1
    OPTIONS
        "BUILD_STATIC ON"
        "BUILD_EXAMPLES OFF"
)

CPMAddPackage(
    NAME QSimpleUpdater
    GITHUB_REPOSITORY alex-spataru/QSimpleUpdater
    GIT_TAG c5c136c0f79ca6423b0914a2676ae3d0e3811c5b
    OPTIONS
        "QSIMPLE_UPDATER_BUILD_TESTS OFF"
)

set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication" FORCE)
CPMAddPackage(
    NAME SingleApplication
    GITHUB_REPOSITORY itay-grudev/SingleApplication
    GIT_TAG v3.5.6
)

CPMAddPackage(
    NAME editorconfig_core_qt
    GITHUB_REPOSITORY editorconfig/editorconfig-core-qt
    GIT_TAG ab62f0554abf2bbe4d45427b36a8b2f81ca7b4ab
)

add_subdirectory(lua)
add_subdirectory(scintilla)
add_subdirectory(lexilla)
add_subdirectory(uchardet)

find_package(Python3 3.13 QUIET COMPONENTS Interpreter)
if(NOT Python3_FOUND)
    find_package(Python3 QUIET COMPONENTS Interpreter)
endif()
if(Python3_FOUND)
    message(STATUS "NotepadNext: Python interpreter found: ${Python3_EXECUTABLE} (version ${Python3_VERSION})")
endif()

target_link_libraries(lexilla PRIVATE scintilla)
