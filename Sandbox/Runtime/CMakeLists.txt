project(NthEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Sandbox)

if (WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(Sandbox WIN32
            main.cpp
    )
else ()
    add_executable(Sandbox
            main.cpp
    )
endif ()

target_link_libraries(Sandbox PRIVATE NthEngine)

target_include_directories(Sandbox PRIVATE
        ../../Source/Common
        ../../Source/Engine
        ../../Source/Vendor
)

add_custom_target(CMake_CopySandboxContent ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Sandbox/Content
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Content
        COMMENT "Copying Sandbox content directory to build output"
)

add_custom_target(CMake_CopyEngineContent ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/EngineContent
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Content/EngineContent
        COMMENT "Copying Engine content directory to build output"
)

# The final hierarchy should be GamePath/Content/EngineContent
add_dependencies(CMake_CopyEngineContent CMake_CopySandboxContent)
add_dependencies(Sandbox CMake_CopyEngineContent)