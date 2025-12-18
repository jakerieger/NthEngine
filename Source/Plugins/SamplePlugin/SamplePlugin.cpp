#include "SamplePlugin.hpp"

#include <cstdio>

namespace Astera {
    void SamplePlugin::OnEngineStart(Game* engine) {
        Log::Warn(mName, "OnEngineStart() called with engine instance: {}", fmt::ptr(engine));
    }

    void SamplePlugin::OnEngineStop(Game* engine) {
        Log::Warn(mName, "OnEngineStop() called with engine instance: {}", fmt::ptr(engine));
    }
}  // namespace Astera

extern "C" SAMPLE_PLUGIN_API Astera::IEnginePlugin* CreatePlugin() {
    return new Astera::SamplePlugin();
}