#pragma once

#include <AsteraCore.hpp>
#include <EnginePluginInterface.hpp>
#include "PluginExport.hpp"

namespace Astera {
    class SAMPLE_PLUGIN_API Screenshot final : public IEnginePlugin {
    public:
        Screenshot() {
            this->mName = "SamplePlugin";
        }

        void OnEngineStart(Game* engine) override;
        void OnEngineStop(Game* engine) override;
    };
}  // namespace Astera

extern "C" SAMPLE_PLUGIN_API Astera::IEnginePlugin* CreatePlugin();