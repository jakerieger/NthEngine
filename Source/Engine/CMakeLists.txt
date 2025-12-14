file(GLOB ENGINE_SOURCES
    *.cpp
    **/*.cpp
)

file(GLOB ENGINE_INCLUDES
    *.hpp
    **/*.hpp
)

add_library(Astera_Objects OBJECT
    ${IMGUI_SOURCES}
    ${MINIAUDIO_SOURCES}
    ${GLAD_SOURCES}
    ${STB_SOURCES}
    ${ENGINE_INCLUDES}
    ${ENGINE_SOURCES}
)

target_include_directories(Astera_Objects PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SOURCE_ROOT}>
    $<BUILD_INTERFACE:${VENDOR_ROOT}>
    $<BUILD_INTERFACE:${LUAJIT_INCLUDE_DIRS}>
    $<BUILD_INTERFACE:${GLFW_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:include/Astera>
)

target_compile_definitions(Astera_Objects PUBLIC
    $<BUILD_INTERFACE:ASTERA_ENGINE_CONTENT_DIR="${CMAKE_SOURCE_DIR}/EngineContent">
)

target_compile_options(Astera_Objects PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-deprecated-declarations>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4996 /wd4244 /wd4267 /wd4018>
)

# CRITICAL: Enable PIC for the object library
set_target_properties(Astera_Objects PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Link dependencies to the object library
target_link_libraries(Astera_Objects PUBLIC ${ASTERA_LIBS})

# Create shared library from object files
add_library(Astera_Shared SHARED $<TARGET_OBJECTS:Astera_Objects>)
set_target_properties(Astera_Shared PROPERTIES OUTPUT_NAME Astera)

# Create static library from object files
add_library(Astera_Static STATIC $<TARGET_OBJECTS:Astera_Objects>)
set_target_properties(Astera_Static PROPERTIES OUTPUT_NAME Astera)

# Inherit include directories and compile definitions, but use INTERFACE linking
# This breaks the direct dependency chain for exports
target_include_directories(Astera_Shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SOURCE_ROOT}>
    $<BUILD_INTERFACE:${VENDOR_ROOT}>
    $<BUILD_INTERFACE:${LUAJIT_INCLUDE_DIRS}>
    $<BUILD_INTERFACE:${GLFW_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:include/Astera>
)

target_include_directories(Astera_Static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${SOURCE_ROOT}>
    $<BUILD_INTERFACE:${VENDOR_ROOT}>
    $<BUILD_INTERFACE:${LUAJIT_INCLUDE_DIRS}>
    $<BUILD_INTERFACE:${GLFW_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:include/Astera>
)

# Link the actual libraries (not the object library) to dependencies
target_link_libraries(Astera_Shared PUBLIC ${ASTERA_LIBS})
target_link_libraries(Astera_Static PUBLIC ${ASTERA_LIBS})

include(GenerateExportHeader)
generate_export_header(Astera_Shared)

# Aliases
add_library(Astera::Shared ALIAS Astera_Shared)
add_library(Astera::Static ALIAS Astera_Static)
add_library(Astera::Astera ALIAS Astera_Shared)