#include "Screenshot.hpp"

#include <stb_image_write.h>

namespace Astera {
    namespace fs = std::filesystem;

    void Screenshot::OnEngineStart(Game* engine) {
        mEngine = engine;

        if (!fs::exists(mOutputDir)) {
            fs::create_directories(mOutputDir);
        }

        Log::Warn(mName, "Initialized. Press F12 to capture screenshots.");
        Log::Warn(mName, "Output directory: {}", fs::absolute(mOutputDir).string());
    }

    void Screenshot::OnEngineStop(Game* engine) {
        mEngine = nullptr;
        Log::Warn(mName, "Shutdown complete.");
    }

    void Screenshot::OnSceneRender(Game* engine) {
        if (!engine)
            return;

        // Check hotkey (with edge detection to prevent repeated captures)
        auto* window       = engine->GetHandle();
        bool hotkeyPressed = glfwGetKey(window, mHotkeyCode) == GLFW_PRESS;

        if (hotkeyPressed && !mHotkeyWasPressed) {
            mCaptureRequested = true;
        }
        mHotkeyWasPressed = hotkeyPressed;

        // Perform capture if requested
        if (mCaptureRequested) {
            mCaptureRequested = false;

            auto& renderTarget = engine->GetMainRenderTarget();
            u32 width, height;
            renderTarget.GetSize(width, height);

            // Allocate buffer for pixel data (RGBA)
            std::vector<u8> pixels(width * height * 4);

            // Read pixels from the current framebuffer
            glReadPixels(0,
                         0,
                         static_cast<GLsizei>(width),
                         static_cast<GLsizei>(height),
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         pixels.data());

            // OpenGL reads bottom-to-top, flip vertically for correct orientation
            std::vector<u8> flipped(pixels.size());
            const u32 rowSize = width * 4;
            for (u32 y = 0; y < height; ++y) {
                std::memcpy(&flipped[y * rowSize], &pixels[(height - 1 - y) * rowSize], rowSize);
            }

            // Generate filename and save
            std::string filepath = GenerateFilename();
            if (SavePixelsToPNG(filepath, flipped, width, height)) {
                Log::Warn(mName, "Screenshot saved: {}", filepath);
            } else {
                Log::Error(mName, "Failed to save screenshot: {}", filepath);
            }
        }
    }

    void Screenshot::CaptureScreenshot() {
        mCaptureRequested = true;
    }

    void Screenshot::SetOutputDirectory(const std::filesystem::path& dir) {
        mOutputDir = dir;
        if (!fs::exists(mOutputDir)) {
            fs::create_directories(mOutputDir);
        }
    }

    void Screenshot::SetHotkey(int key) {
        mHotkeyCode = key;
    }

    std::string Screenshot::GenerateFilename() const {
        const auto now  = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);
        const auto ms   = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::tm tm;
#if defined(ASTERA_PLATFORM_LINUX)
        localtime_r(&time, &tm);  // Use localtime_s on Windows
#elif defined(ASTERA_PLATFORM_WINDOWS)
        localtime_s(&tm, &time);
#endif

        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &tm);

        return (mOutputDir / fmt::format("screenshot_{}_{:03d}.png", buffer, ms.count())).string();
    }

    bool
    Screenshot::SavePixelsToPNG(const std::string& filepath, const std::vector<u8>& pixels, u32 width, u32 height) {
        const int result = stbi_write_png(filepath.c_str(),
                                          static_cast<int>(width),
                                          static_cast<int>(height),
                                          4,  // RGBA
                                          pixels.data(),
                                          static_cast<int>(width * 4));
        return result != 0;
    }
}  // namespace Astera

extern "C" SCREENSHOT_API Astera::IEnginePlugin* CreatePlugin() {
    return new Astera::Screenshot();
}