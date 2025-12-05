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

target_include_directories(NthEngine PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Common>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Source/Vendor>
        $<INSTALL_INTERFACE:include/NthEngine>
)

target_link_libraries(NthEngine PUBLIC
        glfw
        glm::glm
        spdlog::spdlog
        EnTT::EnTT
        fmt::fmt
        pugixml::static
        luajit
        sol2::sol2
)

if (UNIX AND NOT APPLE)
    target_link_libraries(NthEngine PUBLIC rt)
endif ()

if (WIN32)
    add_compile_definitions(
            NOMINMAX           # Prevents min/max macros
            WIN32_LEAN_AND_MEAN # Reduces unnecessary includes
            VC_EXTRALEAN       # Further reduces includes (MSVC)
    )
endif ()