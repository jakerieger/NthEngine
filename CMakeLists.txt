cmake_minimum_required(VERSION 3.14)
project(Astera VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# On Windows, put everything (including import libs) in bin directory to simplify linking
if (WIN32)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # For multi-config generators (Visual Studio), also set per-config directories
    foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG} CONFIG_UPPER)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG})
    endforeach ()
endif ()

set(SOURCE_ROOT ${CMAKE_SOURCE_DIR}/Source)
set(ENGINE_ROOT ${SOURCE_ROOT}/Engine)
set(SHARED_ROOT ${SOURCE_ROOT}/Shared)
set(VENDOR_ROOT ${SOURCE_ROOT}/Vendor)

# Find system-wide dependencies. Windows can use vcpkg.
find_package(fmt REQUIRED)
find_package(pugixml REQUIRED)
find_package(spdlog REQUIRED)
find_package(CLI11 REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUAJIT REQUIRED luajit)
pkg_check_modules(SNDFILE REQUIRED sndfile)

if (MSVC)
    find_package(glfw3 REQUIRED)
    add_compile_options(/utf-8)
else ()
    pkg_check_modules(GLFW REQUIRED glfw3)
endif ()

set(ASTERA_LIBS
        fmt::fmt
        glfw
        spdlog::spdlog
        pugixml
        ${LUAJIT_LIBRARIES}
        ${SNDFILE_LIBRARIES}
)

include_directories(${LUAJIT_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS})
link_directories(${LUAJIT_LIBRARY_DIRS})

include(CMake/DetectPlatform.cmake)
include(${VENDOR_ROOT}/Vendor.cmake)

add_subdirectory(Source/Engine)
add_subdirectory(Source/Tools)
add_subdirectory(Source/Plugins)

# Build demos
add_subdirectory(Demos)

# Install configuration
include(CMake/InstallConfig.cmake)
