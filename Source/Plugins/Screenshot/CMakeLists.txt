add_library(Screenshot SHARED
    PluginExport.hpp
    Screenshot.cpp
    Screenshot.hpp
)

target_include_directories(Screenshot PRIVATE
    ${SOURCE_ROOT}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(Screenshot PRIVATE SCREENSHOT_EXPORTS)