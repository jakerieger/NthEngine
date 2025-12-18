set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Plugins)

#add_subdirectory(SamplePlugin)
add_subdirectory(Screenshot)

set(ASTERA_PLUGINS
    #    SamplePlugin
    Screenshot
)

#target_include_directories(SamplePlugin PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}/SamplePlugin
#    ${ENGINE_ROOT}
#    ${VENDOR_ROOT}
#)

target_include_directories(Screenshot PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Screenshot
    ${ENGINE_ROOT}
    ${VENDOR_ROOT}
)