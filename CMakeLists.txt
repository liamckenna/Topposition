cmake_minimum_required(VERSION 3.23)
project(Topposition LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(RELEASE_DIR "${CMAKE_SOURCE_DIR}/release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RELEASE_DIR}")

# 1. Automatically find all source and header files
# CONFIGURE_DEPENDS ensures CMake notices when you add or delete files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.h")

# 2. Pass the variables containing your files to the executable
add_executable(Topposition WIN32
    icon.rc
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(Topposition PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

target_link_libraries(Topposition PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)

if(WIN32)
    file(MAKE_DIRECTORY "${RELEASE_DIR}")
    set(RELEASE_LAUNCHER "${RELEASE_DIR}/run_topposition.bat")
    file(GENERATE OUTPUT "${RELEASE_LAUNCHER}" CONTENT
        "@echo off\r\nset \"PATH=%~dp0libs;%PATH%\"\r\n\"%~dp0$<TARGET_FILE_NAME:Topposition>\" %*\r\n"
    )

    # Locate the MinGW bin directory from the compiler path so we can copy
    # runtime DLLs (libgcc_s_seh-1.dll, libstdc++-6.dll, libwinpthread-1.dll)
    # that are NOT captured by TARGET_RUNTIME_DLLS.
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    add_custom_command(TARGET Topposition POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/libs"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/textures"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}/fonts"
        # Recursively collect every transitive DLL dependency and copy to libs/.
        # Also removes any DLLs auto-dropped into the release/ root by SDL3's
        # cmake package config so all DLLs live only in release/libs/.
        COMMAND ${CMAKE_COMMAND}
            "-DEXE_PATH=$<TARGET_FILE:Topposition>"
            "-DMINGW_BIN_DIR=${MINGW_BIN_DIR}"
            "-DRELEASE_DIR=${RELEASE_DIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyRuntimeDeps.cmake"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Textures" "${RELEASE_DIR}/textures"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Fonts" "${RELEASE_DIR}/fonts"
        COMMAND_EXPAND_LISTS
        COMMENT "Packaging Topposition release folder"
    )
endif()
