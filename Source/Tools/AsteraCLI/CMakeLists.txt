add_executable(astera-cli
    main.cpp
)

target_link_libraries(astera-cli PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    CLI11::CLI11
    pugixml
    Astera::Static
)

target_include_directories(astera-cli PRIVATE
    ${SOURCE_ROOT}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

target_compile_options(astera-cli PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-deprecated-declarations>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4996>
)