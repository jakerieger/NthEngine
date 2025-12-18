#pragma once

#include <AsteraCore.hpp>
#include <EnginePluginInterface.hpp>
#include "PluginExport.hpp"

namespace Astera {
    class SCREENSHOT_API Screenshot final : public IEnginePlugin {
    public:
        Screenshot() {
            this->mName = "Screenshot";
        }

        void OnEngineStart(Game* engine) override;
        void OnEngineStop(Game* engine) override;
        void OnSceneRender(Game* engine) override;

        // Manual capture trigger
        void CaptureScreenshot();

        // Configuration
        void SetOutputDirectory(const std::filesystem::path& dir);
        void SetHotkey(int key);  // GLFW key code

    private:
        std::string GenerateFilename() const;
        static bool SavePixelsToPNG(const std::string& filepath, const std::vector<u8>& pixels, u32 width, u32 height);

        Game* mEngine {nullptr};
        std::filesystem::path mOutputDir {"Screenshots"};
        int mHotkeyCode {GLFW_KEY_F12};
        bool mCaptureRequested {false};
        bool mHotkeyWasPressed {false};
    };
}  // namespace Astera

extern "C" SCREENSHOT_API Astera::IEnginePlugin* CreatePlugin();