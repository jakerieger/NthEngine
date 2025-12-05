cmake_minimum_required(VERSION 3.14)
project(NthEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(NTHENGINE_INSTALL_INCLUDEDIR "include" CACHE PATH "Installation directory for header files")
set(NTHENGINE_INSTALL_LIBDIR "lib" CACHE PATH "Installation directory for libraries")
set(NTHENGINE_INSTALL_BINDIR "bin" CACHE PATH "Installation directory for executables")
set(NTHENGINE_INSTALL_CONTENTDIR "." CACHE PATH "Installation directory for EngineContent")

# Detect platform and set engine defines (N_ENGINE_XXX)
include(CMake/DetectPlatform.cmake)

# Fetch 3rd party dependencies
include(CMake/FetchDeps.cmake)

add_subdirectory(Source/Engine)
add_subdirectory(Source/Tools)

add_subdirectory(Sandbox/Runtime)

# Generate documentation target
add_custom_target(CMake_GenerateDocs
        COMMAND ${CMAKE_COMMAND}
        -D "FUNCTION_TO_RUN=GenerateDocs"
        -P "${CMAKE_SOURCE_DIR}/CMake/GenerateDocs.cmake"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Run doxygen and build docs"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/EngineContent
        DESTINATION ${NTHENGINE_INSTALL_CONTENTDIR}
        PATTERN "*.md" EXCLUDE
        PATTERN ".git*" EXCLUDE
)
