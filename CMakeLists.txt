cmake_minimum_required(VERSION 3.14)
project(NthEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Detect platform and set engine defines (N_ENGINE_XXX)
include(CMake/DetectPlatform.cmake)

# Fetch 3rd party dependencies
include(CMake/FetchDeps.cmake)
set(NTHENGINE_LIBS
        glfw
        glm::glm
        spdlog::spdlog
        EnTT::EnTT
        fmt::fmt
        pugixml
        luajit
        sol2::sol2
)

add_subdirectory(Source/Engine)
add_subdirectory(Source/Tools)
add_subdirectory(Sandbox/Runtime)

set(NTHENGINE_INSTALL_BINDIR "bin" CACHE PATH "Executables install directory")
set(NTHENGINE_INSTALL_LIBDIR "lib" CACHE PATH "Libary binaries install directory")
set(NTHENGINE_INSTALL_INCLUDEDIR "include" CACHE PATH "Library include install directory")
set(NTHENGINE_INSTALL_CMAKEDIR "lib/cmake/NthEngine" CACHE PATH "CMake configuration install directory")
set(NTHENGINE_INSTALL_CONTENTDIR "." CACHE PATH "Installation directory for EngineContent")

include(CMakePackageConfigHelpers)

# Configure the version file
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/NthEngineConfigVersion.cmake"
        VERSION 1.0.0  # Set your version here
        COMPATIBILITY SameMajorVersion
)

# Configure the config file (lol)
configure_package_config_file(
        "${CMAKE_SOURCE_DIR}/CMake/NthEngineConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/NthEngineConfig.cmake"
        INSTALL_DESTINATION ${NTHENGINE_INSTALL_CMAKEDIR}
)

# Install headers for Engine
install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/Source/Engine
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}/NthEngine
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install common headers
install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/Source/Common
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}/NthEngine
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install vendor headers
install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/Source/Vendor
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}/NthEngine
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install NthEngine targets (the main library)
install(
        TARGETS NthEngine
        EXPORT NthEngineTargets
        LIBRARY DESTINATION ${NTHENGINE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${NTHENGINE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${NTHENGINE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}
)

install(
        TARGETS glfw spdlog EnTT fmt pugixml sol2 glm glm-header-only
        EXPORT NthEngineTargets
        LIBRARY DESTINATION ${NTHENGINE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${NTHENGINE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${NTHENGINE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}
        #        OPTIONAL  # Don't fail if some targets don't exist
)

# Install LuaJIT
InstallLuaJIT(${NTHENGINE_INSTALL_LIBDIR} ${NTHENGINE_INSTALL_INCLUDEDIR})

# Export targets for installation with namespace
install(
        EXPORT NthEngineTargets
        FILE NthEngineTargets.cmake
        NAMESPACE NthEngine::
        DESTINATION ${NTHENGINE_INSTALL_CMAKEDIR}
)

# Install config files
install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NthEngineConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NthEngineConfigVersion.cmake"
        DESTINATION ${NTHENGINE_INSTALL_CMAKEDIR}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/EngineContent
        DESTINATION ${NTHENGINE_INSTALL_CONTENTDIR}
        PATTERN "*.md" EXCLUDE
        PATTERN ".git*" EXCLUDE
)
