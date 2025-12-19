add_library(Screenshot SHARED
    ${STB_SOURCES}
    PluginExport.hpp
    Screenshot.cpp
    Screenshot.hpp
)

target_include_directories(Screenshot PRIVATE
    ${SOURCE_ROOT}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Screenshot PRIVATE
    ${ASTERA_LIBS}
    Astera::Shared
)

target_compile_definitions(Screenshot PRIVATE SCREENSHOT_EXPORTS)

set_target_properties(Screenshot PROPERTIES
    PREFIX ""
    SUFFIX ".plugin"
)