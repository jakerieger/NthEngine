set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Demos/SpaceInvaders)

add_executable(Demo-SpaceInvaders main.cpp)

target_link_libraries(Demo-SpaceInvaders PRIVATE
    Astera::Astera
)

target_include_directories(Demo-SpaceInvaders PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SOURCE_ROOT}
    ${ENGINE_ROOT}
    ${VENDOR_ROOT}
    ${COMMON_ROOT}
)

add_custom_target(SpaceInvaders_CopyConfig ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../Config
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Config
    COMMENT "Copying Sandbox config directory to build output"
)

add_custom_target(SpaceInvaders_CopyContent ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../Content
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Content
    COMMENT "Copying Sandbox content directory to build output"
)

add_custom_target(SpaceInvaders_CopyEngineContent ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/EngineContent
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Content/EngineContent
    COMMENT "Copying Engine content directory to build output"
)

add_custom_target(SpaceInvaders_CopyPlugins ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Plugins
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Plugins
)

if (LINUX)
    target_link_options(Demo-SpaceInvaders PRIVATE -rdynamic)
endif ()

add_dependencies(SpaceInvaders_CopyEngineContent SpaceInvaders_CopyContent)
add_dependencies(SpaceInvaders_CopyPlugins SamplePlugin)
add_dependencies(Demo-SpaceInvaders SpaceInvaders_CopyEngineContent)
add_dependencies(Demo-SpaceInvaders SpaceInvaders_CopyConfig)
add_dependencies(Demo-SpaceInvaders SpaceInvaders_CopyPlugins)