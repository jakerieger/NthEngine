// Author: Jake Rieger
// Created: 12/9/25.
//

#include "DebugInterface.hpp"

namespace Nth {
    DebugManager::~DebugManager() {
        DetachOverlay();
    }

    void DebugManager::AttachOverlay(IDebugOverlay* overlay) {
        mOverlay = overlay;
    }

    void DebugManager::DetachOverlay() {
        if (!mOverlay) return;
        mOverlay = nullptr;
    }

    void DebugManager::Update(float deltaTime) const {
        mOverlay->OnUpdate(deltaTime);
    }

    void DebugManager::Render() const {
        mOverlay->OnRender();
    }

    void DebugManager::HandleEvent(const Event& event) const {
        mOverlay->OnEvent(event);
    }
}  // namespace Nth