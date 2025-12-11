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

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../Content
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(COPY ${CMAKE_SOURCE_DIR}/EngineContent
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Content
)
