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

target_include_directories(NthEngine PRIVATE
        ../Common
        ../Vendor
        ../Engine
)

target_link_libraries(NthEngine PUBLIC
        glfw
        glm::glm
        spdlog::spdlog
        EnTT::EnTT
        fmt::fmt-header-only
)

if (UNIX AND NOT APPLE)
    target_link_libraries(NthEngine PUBLIC rt)
endif ()
