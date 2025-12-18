#pragma once

#include <string>

namespace Astera {
    class IEnginePlugin {
    public:
        IEnginePlugin()          = default;
        virtual ~IEnginePlugin() = default;

        virtual void OnEngineStart(Game* engine) {}

        virtual void OnEngineStop(Game* engine) {}

        virtual void OnSceneAwake(Game* engine) {}

        virtual void OnSceneUpdate(Game* engine) {}

        virtual void OnSceneRender(Game* engine) {}

        virtual void OnSceneLateUpdate(Game* engine) {}

        virtual void OnSceneDestroyed(Game* engine) {}

        const std::string& GetName() const {
            return mName;
        }

    protected:
        std::string mName;
    };

    typedef void (*OnEngineStartFunc)(Game*);
    typedef void (*OnEngineStopFunc)(Game*);
    typedef void (*OnSceneAwakeFunc)(Game*);
    typedef void (*OnSceneUpdateFunc)(Game*);
    typedef void (*OnSceneRenderFunc)(Game*);
    typedef void (*OnSceneLateUpdateFunc)(Game*);
    typedef void (*OnSceneDestroyedFunc)(Game*);
    typedef IEnginePlugin* (*CreatePluginFunc)();
}  // namespace Astera