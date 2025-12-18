set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Plugins)
add_subdirectory(SamplePlugin)

set(ASTERA_PLUGINS
    SamplePlugin
)

target_include_directories(SamplePlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ENGINE_ROOT}
    ${VENDOR_ROOT}
)