project(NthEngine)

function(BuildLuaJIT)
    FetchContent_GetProperties(luajit)

    if (NOT luajit_POPULATED)
        FetchContent_Populate(luajit)

        if (WIN32)
            # Create a custom target for building LuaJIT on Windows
            add_custom_command(
                    OUTPUT
                    "${luajit_SOURCE_DIR}/src/lua51.lib"
                    "${luajit_SOURCE_DIR}/src/lua51.exp"
                    COMMAND cmd /c msvcbuild.bat static
                    WORKING_DIRECTORY ${luajit_SOURCE_DIR}/src
                    COMMENT "Building LuaJIT for Windows"
                    DEPENDS
                    "${luajit_SOURCE_DIR}/src/msvcbuild.bat"
                    "${luajit_SOURCE_DIR}/src/luajit.h"
                    "${luajit_SOURCE_DIR}/src/lua.h"
            )

            # Create a custom target that depends on the output
            add_custom_target(luajit_build
                    DEPENDS "${luajit_SOURCE_DIR}/src/lua51.lib"
            )

            set(LUAJIT_LIB_NAME "lua51.lib")
        elseif (UNIX AND NOT APPLE)
            # Build LuaJIT on Linux
            add_custom_command(
                    OUTPUT
                    "${luajit_SOURCE_DIR}/src/libluajit.a"
                    COMMAND make clean
                    COMMAND make amalg BUILDMODE=static CFLAGS=-fPIC #  Requires fPIC in order to build NthEngine as shared
                    WORKING_DIRECTORY ${luajit_SOURCE_DIR}/src
                    COMMENT "Building LuaJIT for Linux"
                    DEPENDS
                    "${luajit_SOURCE_DIR}/src/Makefile"
                    "${luajit_SOURCE_DIR}/src/luajit.h"
                    "${luajit_SOURCE_DIR}/src/lua.h"
            )

            # Create a custom target that depends on the output
            add_custom_target(luajit_build
                    DEPENDS "${luajit_SOURCE_DIR}/src/libluajit.a"
            )

            set(LUAJIT_LIB_NAME "libluajit.a")
        else ()
            message(FATAL_ERROR "Cannot compile LuaJIT for this system. Only Windows and Linux are supported.")
        endif ()
    endif ()

    add_library(luajit STATIC IMPORTED GLOBAL)
    add_dependencies(luajit luajit_build)

    set_target_properties(luajit PROPERTIES
            IMPORTED_LOCATION "${luajit_SOURCE_DIR}/src/${LUAJIT_LIB_NAME}"
            INTERFACE_INCLUDE_DIRECTORIES "${luajit_SOURCE_DIR}/src"
    )

    # On Linux, LuaJIT requires linking against libdl and libm
    if (UNIX AND NOT APPLE)
        set_target_properties(luajit PROPERTIES
                INTERFACE_LINK_LIBRARIES "dl;m"
        )
    endif ()

    # Make variables available to calling scope
    set(LUAJIT_INCLUDES "${luajit_SOURCE_DIR}/src" PARENT_SCOPE)
    set(LUAJIT_SOURCE_DIR "${luajit_SOURCE_DIR}" PARENT_SCOPE)
    set(LUAJIT_LIBRARY "${luajit_SOURCE_DIR}/src/${LUAJIT_LIB_NAME}" PARENT_SCOPE)
endfunction()

function(InstallLuaJIT install_lib_dir install_include_dir)
    FetchContent_GetProperties(luajit)

    # Install LuaJIT library (platform-specific name)
    if (WIN32)
        set(LUAJIT_LIB_FILE "${luajit_SOURCE_DIR}/src/lua51.lib")
    elseif (UNIX AND NOT APPLE)
        set(LUAJIT_LIB_FILE "${luajit_SOURCE_DIR}/src/libluajit.a")
    endif ()

    install(
            FILES ${LUAJIT_LIB_FILE}
            DESTINATION ${install_lib_dir}
    )

    # Install LuaJIT headers
    install(
            DIRECTORY "${luajit_SOURCE_DIR}/src/"
            DESTINATION ${install_include_dir}/NthEngine/luajit
            FILES_MATCHING PATTERN "*.h"
    )
endfunction()