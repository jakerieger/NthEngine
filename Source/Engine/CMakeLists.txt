project(NthEngine)

file(GLOB ENGINE_SOURCES
        *.hpp
        *.cpp
        **/*.hpp
        **/*.cpp
)

add_library(NthEngine SHARED
        ../Common/CommonPCH.hpp
        ../Common/CommonPCH.cpp
        ../Vendor/glad.c
        ../Vendor/stb_image.cpp
        ${ENGINE_SOURCES}
)
include(GenerateExportHeader)
generate_export_header(NthEngine)

target_precompile_headers(NthEngine PRIVATE ../Common/CommonPCH.hpp)

# Remove glad from PCH
set_source_files_properties(../Vendor/glad.c PROPERTIES
        SKIP_PRECOMPILE_HEADERS ON
        LANGUAGE C
)

target_include_directories(NthEngine PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Common>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Vendor>
        $<INSTALL_INTERFACE:include/NthEngine>
)

target_link_libraries(NthEngine PRIVATE
        ${NTHENGINE_LIBS}
)

if (UNIX AND NOT APPLE)
    target_link_libraries(NthEngine PUBLIC rt dl m)
endif ()

if (WIN32)
    add_compile_definitions(
            NOMINMAX
            WIN32_LEAN_AND_MEAN
            VC_EXTRALEAN
    )
endif ()