/*
 *  Filename: Coordinates.inl
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

#include <sol/sol.hpp>

// Naming conflicts from minwindef.h
#ifdef near
    #undef near
#endif

#ifdef far
    #undef far
#endif

/// @brief Utility functions for converting from screen space to NDC (Normalized Device Coordinates)
namespace Astera::Coordinates {
    /// @brief Convert screen pixel coordinates to NDC (Normalized Device Coordinates)
    /// @param screenPos Position in screen space (pixels)
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @return Position in NDC space [-1, 1]
    inline static Vec2 ScreenToNDC(const Vec2& screenPos, f32 screenWidth, f32 screenHeight) {
        const f32 ndcX = (screenPos.x / screenWidth) * 2.0f - 1.0f;
        const f32 ndcY = (screenPos.y / screenHeight) * 2.0f - 1.0f;
        return {ndcX, ndcY};
    }

    /// @brief Convert NDC coordinates to screen pixel coordinates
    /// @param ndcPos Position in NDC space [-1, 1]
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @return Position in screen space (pixels)
    inline static Vec2 NDCToScreen(const Vec2& ndcPos, f32 screenWidth, f32 screenHeight) {
        const f32 screenX = (ndcPos.x + 1.0f) * 0.5f * screenWidth;
        const f32 screenY = (ndcPos.y + 1.0f) * 0.5f * screenHeight;
        return {screenX, screenY};
    }

    /// @brief Convert screen pixel coordinates to world space (using orthographic projection)
    /// @param screenPos Position in screen space (pixels)
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @param worldLeft Left bound of world space (typically 0)
    /// @param worldRight Right bound of world space (typically screenWidth)
    /// @param worldBottom Bottom bound of world space (typically 0)
    /// @param worldTop Top bound of world space (typically screenHeight)
    /// @return Position in world space
    inline static Vec2 ScreenToWorld(const Vec2& screenPos,
                                     f32 screenWidth,
                                     f32 screenHeight,
                                     f32 worldLeft   = 0.0f,
                                     f32 worldRight  = 0.0f,
                                     f32 worldBottom = 0.0f,
                                     f32 worldTop    = 0.0f) {
        // Default to 1:1 mapping if world bounds not specified
        if (worldRight == 0.0f) worldRight = screenWidth;
        if (worldTop == 0.0f) worldTop = screenHeight;

        const f32 worldX = worldLeft + (screenPos.x / screenWidth) * (worldRight - worldLeft);
        const f32 worldY = worldBottom + (screenPos.y / screenHeight) * (worldTop - worldBottom);
        return {worldX, worldY};
    }

    /// @brief Convert world space coordinates to screen pixels
    /// @param worldPos Position in world space
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @param worldLeft Left bound of world space
    /// @param worldRight Right bound of world space
    /// @param worldBottom Bottom bound of world space
    /// @param worldTop Top bound of world space
    /// @return Position in screen space (pixels)
    inline static Vec2 WorldToScreen(const Vec2& worldPos,
                                     f32 screenWidth,
                                     f32 screenHeight,
                                     f32 worldLeft   = 0.0f,
                                     f32 worldRight  = 0.0f,
                                     f32 worldBottom = 0.0f,
                                     f32 worldTop    = 0.0f) {
        // Default to 1:1 mapping if world bounds not specified
        if (worldRight == 0.0f) worldRight = screenWidth;
        if (worldTop == 0.0f) worldTop = screenHeight;

        const f32 screenX = ((worldPos.x - worldLeft) / (worldRight - worldLeft)) * screenWidth;
        const f32 screenY = ((worldPos.y - worldBottom) / (worldTop - worldBottom)) * screenHeight;
        return {screenX, screenY};
    }

    /// @brief Convert normalized coordinates [0, 1] to screen pixels
    /// @param normalizedPos Position in normalized space [0, 1]
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @return Position in screen space (pixels)
    inline static Vec2 NormalizedToScreen(const Vec2& normalizedPos, f32 screenWidth, f32 screenHeight) {
        return {normalizedPos.x * screenWidth, normalizedPos.y * screenHeight};
    }

    /// @brief Convert screen pixels to normalized coordinates [0, 1]
    /// @param screenPos Position in screen space (pixels)
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @return Position in normalized space [0, 1]
    inline static Vec2 ScreenToNormalized(const Vec2& screenPos, f32 screenWidth, f32 screenHeight) {
        return {screenPos.x / screenWidth, screenPos.y / screenHeight};
    }

    /// @brief Create an orthographic projection matrix for 2D rendering
    /// @param left Left bound
    /// @param right Right bound
    /// @param bottom Bottom bound
    /// @param top Top bound
    /// @param near Near clipping plane (default -1)
    /// @param far Far clipping plane (default 1)
    /// @return Orthographic projection matrix
    inline static Mat4
    CreateOrthoProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near = -1.0f, f32 far = 1.0f) {
        return glm::ortho(left, right, bottom, top, near, far);
    }

    /// @brief Create an orthographic projection matrix that matches screen dimensions
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @param originTopLeft If true, origin is at top-left; if false, origin is at bottom-left
    /// @return Orthographic projection matrix
    inline static Mat4 CreateScreenProjection(f32 screenWidth, f32 screenHeight, bool originTopLeft = false) {
        if (originTopLeft) {
            // Top-left origin (common for UI and 2D games)
            return glm::ortho(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);
        } else {
            // Bottom-left origin (OpenGL default)
            return glm::ortho(0.0f, screenWidth, 0.0f, screenHeight, -1.0f, 1.0f);
        }
    }

    /// @brief Get the aspect ratio of the screen
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @return Aspect ratio (width / height)
    inline static f32 GetAspectRatio(f32 screenWidth, f32 screenHeight) {
        return screenWidth / screenHeight;
    }

    /// @brief Clamp a position to screen bounds
    /// @param pos Position to clamp
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @return Clamped position
    inline static Vec2 ClampToScreen(const Vec2& pos, f32 screenWidth, f32 screenHeight) {
        return {glm::clamp(pos.x, 0.0f, screenWidth), glm::clamp(pos.y, 0.0f, screenHeight)};
    }

    /// @brief Check if a position is within screen bounds
    /// @param pos Position to check
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @return True if position is within bounds
    inline static bool IsOnScreen(const Vec2& pos, f32 screenWidth, f32 screenHeight) {
        return pos.x >= 0.0f && pos.x <= screenWidth && pos.y >= 0.0f && pos.y <= screenHeight;
    }

    /// @brief Convert mouse coordinates (usually top-left origin) to world space
    /// @param mousePos Mouse position from input system
    /// @param screenWidth Width of the screen in pixels
    /// @param screenHeight Height of the screen in pixels
    /// @param originTopLeft Whether your world space has origin at top-left
    /// @return Position in world space
    inline static Vec2
    MouseToWorld(const Vec2& mousePos, f32 screenWidth, f32 screenHeight, bool originTopLeft = false) {
        if (originTopLeft) {
            // Mouse coords typically have Y down, and our world does too
            return mousePos;
        } else {
            // Mouse Y is down, but our world Y is up, so flip it
            return {mousePos.x, screenHeight - mousePos.y};
        }
    }

    inline static void RegisterLuaGlobals(sol::state& lua) {
        lua["Coordinates"] = lua.create_table();
        auto coords        = lua["Coordinates"];

        coords["ScreenToNDC"] = [](sol::object, const Vec2& screenPos, f32 screenWidth, f32 screenHeight) {
            return ScreenToNDC(screenPos, screenWidth, screenHeight);
        };

        coords["NDCToScreen"] = [](sol::object, const Vec2& ndcPos, f32 screenWidth, f32 screenHeight) {
            return NDCToScreen(ndcPos, screenWidth, screenHeight);
        };

        coords["ScreenToWorld"] = [](sol::object,
                                     const Vec2& screenPos,
                                     f32 screenWidth,
                                     f32 screenHeight,
                                     f32 worldLeft   = 0.0f,
                                     f32 worldRight  = 0.0f,
                                     f32 worldBottom = 0.0f,
                                     f32 worldTop    = 0.0f) {
            return ScreenToWorld(screenPos, screenWidth, screenHeight, worldLeft, worldRight, worldBottom, worldTop);
        };

        coords["WorldToScreen"] = [](sol::object,
                                     const Vec2& worldPos,
                                     f32 screenWidth,
                                     f32 screenHeight,
                                     f32 worldLeft   = 0.0f,
                                     f32 worldRight  = 0.0f,
                                     f32 worldBottom = 0.0f,
                                     f32 worldTop    = 0.0f) {
            return WorldToScreen(worldPos, screenWidth, screenHeight, worldLeft, worldRight, worldBottom, worldTop);
        };

        coords["NormalizedToScreen"] = [](sol::object, const Vec2& normalizedPos, f32 screenWidth, f32 screenHeight) {
            return NormalizedToScreen(normalizedPos, screenWidth, screenHeight);
        };

        coords["ScreenToNormalized"] = [](sol::object, const Vec2& screenPos, f32 screenWidth, f32 screenHeight) {
            return ScreenToNormalized(screenPos, screenWidth, screenHeight);
        };

        coords["CreateOrthoProjection"] =
          [](sol::object, f32 left, f32 right, f32 bottom, f32 top, f32 near = -1.0f, f32 far = 1.0f) {
              return CreateOrthoProjection(left, right, bottom, top, near, far);
          };

        coords["CreateScreenProjection"] =
          [](sol::object, f32 screenWidth, f32 screenHeight, bool originTopLeft = false) {
              return CreateScreenProjection(screenWidth, screenHeight, originTopLeft);
          };

        coords["GetAspectRatio"] = [](sol::object, f32 screenWidth, f32 screenHeight) {
            return GetAspectRatio(screenWidth, screenHeight);
        };

        coords["ClampToScreen"] = [](sol::object, const Vec2& pos, f32 screenWidth, f32 screenHeight) {
            return ClampToScreen(pos, screenWidth, screenHeight);
        };

        coords["IsOnScreen"] = [](sol::object, const Vec2& pos, f32 screenWidth, f32 screenHeight) {
            return IsOnScreen(pos, screenWidth, screenHeight);
        };

        coords["MouseToWorld"] =
          [](sol::object, const Vec2& mousePos, f32 screenWidth, f32 screenHeight, bool originTopLeft = false) {
              return MouseToWorld(mousePos, screenWidth, screenHeight, originTopLeft);
          };
    }
}  // namespace Astera::Coordinates
