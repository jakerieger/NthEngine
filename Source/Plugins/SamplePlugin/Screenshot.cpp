#include "Screenshot.hpp"

#include <cstdio>

namespace Astera {
    void Screenshot::OnEngineStart(Game* engine) {
        Log::Warn(mName, "OnEngineStart() called with engine instance: {}", fmt::ptr(engine));
    }

    void Screenshot::OnEngineStop(Game* engine) {
        Log::Warn(mName, "OnEngineStop() called with engine instance: {}", fmt::ptr(engine));
    }
}  // namespace Astera

extern "C" SAMPLE_PLUGIN_API Astera::IEnginePlugin* CreatePlugin() {
    return new Astera::Screenshot();
}