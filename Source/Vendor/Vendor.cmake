set(GLAD_SOURCES
    ${VENDOR_ROOT}/glad.c
    ${VENDOR_ROOT}/glad.h
)

set(STB_SOURCES
    ${VENDOR_ROOT}/stb_image.cpp
    ${VENDOR_ROOT}/stb_image.h
    ${VENDOR_ROOT}/stb_image_write.cpp
    ${VENDOR_ROOT}/stb_image_write.h
)

set(MINIAUDIO_SOURCES
    ${VENDOR_ROOT}/miniaudio.c
    ${VENDOR_ROOT}/miniaudio.h
)

file(GLOB IMGUI_SOURCES
    ${VENDOR_ROOT}/im*
    ${VENDOR_ROOT}/backends/imgui_impl_glfw*
    ${VENDOR_ROOT}/backends/imgui_impl_opengl3*
)