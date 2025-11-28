project(NthEngine)

file(GLOB ENGINE_SOURCES *.hpp *.cpp)

add_library(NthEngine STATIC
        ../Common/CommonPCH.hpp
        ../Common/CommonPCH.cpp
        ../Vendor/glad.c
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
)

target_link_libraries(NthEngine PUBLIC
        glfw
        glm::glm
        spdlog::spdlog
)

if (UNIX AND NOT APPLE)
    target_link_libraries(NthEngine PUBLIC rt)
endif ()
