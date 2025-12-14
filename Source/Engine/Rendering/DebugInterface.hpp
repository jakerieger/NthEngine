/*
 *  Filename: DebugInterface.hpp
 *  This code is part of the Astera core library
 *  Copyright 2025 Jake Rieger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#pragma once

#include "EngineCommon.hpp"
#include <memory>
#include <unordered_map>

namespace Astera {
    /// @brief Generic engine event type. To be implemented further.
    struct Event;

    /// @brief Interface for debug overlay implementations
    class IDebugOverlay {
    public:
        virtual ~IDebugOverlay() = default;

        /// @brief Updates the debug overlay state
        /// @param deltaTime Time elapsed since the last update in seconds
        virtual void OnUpdate(f32 deltaTime) = 0;

        /// @brief Renders the debug overlay to the screen
        virtual void OnRender() = 0;

        /// @brief Handles incoming engine events
        /// @param event The event to process
        virtual void OnEvent(const Event& event) = 0;
    };

    /// @brief Manages debug overlays for the engine
    class DebugManager {
    public:
        DebugManager() = default;
        ~DebugManager();

        ASTERA_CLASS_PREVENT_MOVES_COPIES(DebugManager)

        /// @brief Attaches a debug overlay to the manager
        /// @param name Unique identifier for the overlay
        /// @param overlay Pointer to the overlay instance to attach
        void AttachOverlay(const string& name, IDebugOverlay* overlay);

        /// @brief Detaches all registered debug overlays
        void DetachOverlays();

        /// @brief Enables or disables a specific debug overlay
        /// @param name The identifier of the overlay to modify
        /// @param enabled True to enable the overlay, false to disable
        void SetOverlayEnabled(const string& name, bool enabled);

        /// @brief Checks if a specific debug overlay is enabled
        /// @param name The identifier of the overlay to check
        /// @return True if the overlay is enabled, false otherwise
        bool GetOverlayEnabled(const string& name) const;

        /// @brief Updates all enabled debug overlays
        /// @param deltaTime Time elapsed since the last update in seconds
        void Update(float deltaTime) const;

        /// @brief Renders all enabled debug overlays
        void Render() const;

        /// @brief Dispatches an event to all enabled debug overlays
        /// @param event The event to handle
        void HandleEvent(const Event& event) const;

    private:
        /// @brief Internal structure holding overlay state
        struct Overlay {
            /// @brief Pointer to the overlay instance
            IDebugOverlay* overlay {nullptr};
            /// @brief Whether the overlay is currently enabled
            bool enabled {true};
        };

        /// @brief Map of overlay names to their corresponding overlay data
        unordered_map<string, Overlay> mOverlays;
    };
}  // namespace Astera