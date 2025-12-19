add_library(SamplePlugin SHARED
        PluginExport.hpp
        SamplePlugin.cpp
        SamplePlugin.hpp
)

target_include_directories(SamplePlugin PRIVATE
        ${SOURCE_ROOT}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(SamplePlugin PRIVATE SAMPLE_PLUGIN_EXPORTS)

set_target_properties(SamplePlugin PROPERTIES
        PREFIX ""
        SUFFIX ".plugin"
)