project(NthEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools/NthCLI)

add_executable(nth
        ProjectDescriptor.hpp
        ProjectRunner.hpp
        ProjectRunner.cpp
        main.cpp
)

target_link_libraries(nth PRIVATE
        CLI11
        fmt::fmt
        pugixml::static
        NthEngine
)

target_include_directories(nth PRIVATE
        ../../../Source/Common
        ../../../Source/Engine
        ../../../Source/Vendor
)

# Install nth executable to bin directory
install(TARGETS nth
        RUNTIME DESTINATION ${NTHENGINE_INSTALL_BINDIR}
)