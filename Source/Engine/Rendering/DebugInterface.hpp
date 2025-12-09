// Author: Jake Rieger
// Created: 12/9/25.
//

#pragma once

#include <memory>

namespace Nth {
    /// @brief Generic engine event type. To be implemented further.
    struct Event;

    class IDebugOverlay {
    public:
        virtual ~IDebugOverlay() = default;

        virtual void OnUpdate(float deltaTime)   = 0;
        virtual void OnRender()                  = 0;
        virtual void OnEvent(const Event& event) = 0;
    };

    class DebugManager {
    public:
        ~DebugManager();

        void AttachOverlay(IDebugOverlay* overlay);
        void DetachOverlay();

        void Update(float deltaTime) const;
        void Render() const;
        void HandleEvent(const Event& event) const;

    private:
        IDebugOverlay* mOverlay {nullptr};
    };
}  // namespace Nth
