project(NthEngine)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW docs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples")
#set(GLFW_INSTALL OFF CACHE BOOL "Generate GLFW installation target")

# Causes problems on Windows
if (NOT MSVC)
    add_compile_options(-Wno-unused-command-line-argument)
endif ()

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE
)

set(GLM_BUILD_TESTS OFF CACHE BOOL "Build GLM tests")
#set(GLM_BUILD_INSTALL OFF CACHE BOOL "Install GLM")

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.16.0
        GIT_SHALLOW TRUE
)

set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "Build spdlog tests")
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "Build spdlog examples")
#set(SPDLOG_INSTALL OFF CACHE BOOL "Install spdlog")

FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.16.0
)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.1.0
)

FetchContent_Declare(
        pugixml
        GIT_REPOSITORY https://github.com/zeux/pugixml.git
        GIT_TAG v1.15
)

FetchContent_Declare(
        luajit
        GIT_REPOSITORY https://github.com/LuaJIT/LuaJIT.git
        GIT_TAG v2.0.5
)
include(${CMAKE_SOURCE_DIR}/CMake/BuildLuaJIT.cmake)
BuildLuaJIT()

FetchContent_Declare(
        sol2
        GIT_REPOSITORY https://github.com/ThePhD/sol2.git
        GIT_TAG v3.5.0
)

FetchContent_MakeAvailable(
        glfw
        glm
        spdlog
        entt
        fmt
        pugixml
        sol2
)
