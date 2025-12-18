add_library(SamplePlugin SHARED
    PluginExport.hpp
    Screenshot.cpp
    Screenshot.hpp
)

target_include_directories(SamplePlugin PRIVATE
    ${SOURCE_ROOT}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(SamplePlugin PRIVATE SAMPLE_PLUGIN_EXPORTS)