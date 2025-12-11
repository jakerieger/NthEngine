cmake_minimum_required(VERSION 3.14)
project(Astera VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(SOURCE_ROOT ${CMAKE_SOURCE_DIR}/Source)
set(COMMON_ROOT ${SOURCE_ROOT}/Common)
set(ENGINE_ROOT ${SOURCE_ROOT}/Engine)
set(VENDOR_ROOT ${SOURCE_ROOT}/Vendor)

# Find system-wide dependencies. Windows can use vcpkg.
find_package(fmt REQUIRED)
find_package(pugixml REQUIRED)
find_package(spdlog REQUIRED)
find_package(CLI11 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUAJIT REQUIRED luajit)
pkg_check_modules(GLFW REQUIRED glfw3)

set(ASTERA_LIBS
    fmt::fmt
    glfw
    spdlog::spdlog
    pugixml
    ${LUAJIT_LIBRARIES}
)

include_directories(${LUAJIT_INCLUDE_DIRS})
link_directories(${LUAJIT_LIBRARY_DIRS})

include(CMake/DetectPlatform.cmake)
include(${VENDOR_ROOT}/Vendor.cmake)

add_subdirectory(Source/Engine)
add_subdirectory(Source/Tools)

# Build demos
add_subdirectory(Demos)

# Install configuration
include(CMake/InstallConfig.cmake)