cmake_minimum_required(VERSION 3.23)
project(Topposition LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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