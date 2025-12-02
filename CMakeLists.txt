cmake_minimum_required(VERSION 3.14)
project(NthEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Detect platform and set engine defines (N_ENGINE_XXX)
include(CMake/DetectPlatform.cmake)

# Fetch 3rd party dependencies
include(CMake/FetchDeps.cmake)

add_subdirectory(Source/Engine)
add_subdirectory(Source/Sandbox)
add_subdirectory(Source/Tools)

# Generate documentation
include(CMake/GenerateDocs.cmake)

option(NTH_ENGINE_BUILD_DOCS "" ON)

if (NTH_ENGINE_BUILD_DOCS)
    GenerateDocs()
endif ()