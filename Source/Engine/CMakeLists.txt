project(NthEngine)

file(GLOB ENGINE_SOURCES
        *.hpp
        *.cpp
        **/*.hpp
        **/*.cpp
)

add_library(NthEngine STATIC
        ../Common/CommonPCH.hpp
        ../Common/CommonPCH.cpp
        ../Vendor/glad.c
        ../Vendor/stb_image.cpp
        ${ENGINE_SOURCES}
)

target_precompile_headers(NthEngine PRIVATE ../Common/CommonPCH.hpp)

# Remove glad from PCH
set_source_files_properties(../Vendor/glad.c PROPERTIES
        SKIP_PRECOMPILE_HEADERS ON
        LANGUAGE C
)

# Build-time include directories
target_include_directories(NthEngine PRIVATE
        ../Common
        ../Vendor
        ../Engine
)

# Include directories from dependencies (needed for building)
target_include_directories(NthEngine PRIVATE
        ${glfw_SOURCE_DIR}/include
        ${glm_SOURCE_DIR}
        ${spdlog_SOURCE_DIR}/include
        ${entt_SOURCE_DIR}/src
        ${fmt_SOURCE_DIR}/include
        ${pugixml_SOURCE_DIR}/src
        ${sol2_SOURCE_DIR}/include
        ${LUAJIT_INCLUDES}
)

# Install-time include directories
target_include_directories(NthEngine PUBLIC
        $<INSTALL_INTERFACE:${NTHENGINE_INSTALL_INCLUDEDIR}>
        $<INSTALL_INTERFACE:${NTHENGINE_INSTALL_INCLUDEDIR}/NthEngine>
        $<INSTALL_INTERFACE:${NTHENGINE_INSTALL_INCLUDEDIR}/NthEngine/Common>
        $<INSTALL_INTERFACE:${NTHENGINE_INSTALL_INCLUDEDIR}/NthEngine/Vendor>
)

# Link dependencies - use generator expressions to link library files directly
target_link_libraries(NthEngine PRIVATE
        $<BUILD_INTERFACE:glfw>
        $<BUILD_INTERFACE:glm::glm>
        $<BUILD_INTERFACE:spdlog::spdlog>
        $<BUILD_INTERFACE:EnTT::EnTT>
        $<BUILD_INTERFACE:fmt::fmt>
        $<BUILD_INTERFACE:pugixml::static>
        $<BUILD_INTERFACE:luajit>
        $<BUILD_INTERFACE:sol2::sol2>
)

if (UNIX AND NOT APPLE)
    target_link_libraries(NthEngine PRIVATE rt)
endif ()

if (WIN32)
    add_compile_definitions(
            NOMINMAX           # Prevents min/max macros
            WIN32_LEAN_AND_MEAN # Reduces unnecessary includes
            VC_EXTRALEAN       # Further reduces includes (MSVC)
    )
endif ()

# Installation rules for NthEngine
install(TARGETS NthEngine
        EXPORT NthEngineTargets
        ARCHIVE DESTINATION ${NTHENGINE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${NTHENGINE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${NTHENGINE_INSTALL_BINDIR}
)

# Manually install the dependency libraries
install(FILES
        $<TARGET_FILE:glfw>
        $<TARGET_FILE:spdlog>
        $<TARGET_FILE:fmt>
        $<TARGET_FILE:pugixml-static>
        DESTINATION ${NTHENGINE_INSTALL_LIBDIR}
)

# Install headers - Engine
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}/NthEngine
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

# Install headers - Common
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Common/
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}/NthEngine/Common
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

# Install headers - Vendor (glad, stb_image)
install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/glad.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/stb_image.h
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}/NthEngine/Vendor
)

# Install the KHR directory for glad if it exists
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/KHR)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/KHR
            DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}/NthEngine/Vendor
    )
endif ()

# Install dependency headers
# GLFW
install(DIRECTORY ${glfw_SOURCE_DIR}/include/
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
)

# GLM
install(DIRECTORY ${glm_SOURCE_DIR}/glm
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
        PATTERN "*.inl"
)

# spdlog
install(DIRECTORY ${spdlog_SOURCE_DIR}/include/
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
)

# EnTT
install(DIRECTORY ${entt_SOURCE_DIR}/src/
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

# fmt
install(DIRECTORY ${fmt_SOURCE_DIR}/include/
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
)

# pugixml
install(FILES
        ${pugixml_SOURCE_DIR}/src/pugixml.hpp
        ${pugixml_SOURCE_DIR}/src/pugiconfig.hpp
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}
)

# sol2
install(DIRECTORY ${sol2_SOURCE_DIR}/include/
        DESTINATION ${NTHENGINE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp"
)

# Install LuaJIT (uses custom install function)
install_luajit(
        ${NTHENGINE_INSTALL_LIBDIR}
        ${NTHENGINE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT NthEngineTargets
        FILE NthEngineTargets.cmake
        NAMESPACE NthEngine::
        DESTINATION ${NTHENGINE_INSTALL_LIBDIR}/cmake/NthEngine
)

# Create and install config files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/NthEngineConfigVersion.cmake"
        VERSION 1.0.0
        COMPATIBILITY AnyNewerVersion
)

# Generate config file
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/NthEngineConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/NthEngineConfig.cmake"
        @ONLY
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NthEngineConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NthEngineConfigVersion.cmake"
        DESTINATION ${NTHENGINE_INSTALL_LIBDIR}/cmake/NthEngine
)