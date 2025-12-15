/*
 *  Filename: Color.hpp
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
#pragma warning(disable: 4244)

#include "EngineCommon.hpp"

#include <iomanip>
#include <sstream>
#include <imgui.h>

namespace Astera {
    /// @brief Encapsulation for representing and manipulating colors
    ///
    /// Provides a comprehensive color class supporting multiple color spaces (RGB, HSV),
    /// various construction methods (float, int, hex), color operations (blending, lerping),
    /// and conversions to different formats. Colors are stored internally as normalized
    /// floating-point RGBA values in the range [0.0, 1.0].
    class Color {
    public:
        /// @brief Default constructor, creates a transparent black color (0, 0, 0, 1)
        Color() = default;

        /// @brief Constructs a color from normalized RGBA components
        /// @param r Red component [0.0, 1.0]
        /// @param g Green component [0.0, 1.0]
        /// @param b Blue component [0.0, 1.0]
        /// @param a Alpha component [0.0, 1.0], defaults to 1.0 (opaque)
        Color(f32 r, f32 g, f32 b, f32 a = 1.0f);

        /// @brief Constructs a grayscale color
        /// @param v Grayscale value [0.0, 1.0] applied to R, G, and B
        /// @param a Alpha component [0.0, 1.0], defaults to 1.0 (opaque)
        explicit Color(f32 v, f32 a = 1.0f);

        /// @brief Constructs a color from a 32-bit ARGB packed integer
        /// @param color 32-bit color value in ARGB format (0xAARRGGBB)
        explicit Color(u32 color);

        /// @brief Constructs a color from a hexadecimal string
        /// @param hex Hex color string (e.g., "#FF5733" or "FF5733")
        explicit Color(const string& hex);

        /// @brief Constructs a color from 8-bit RGBA components
        /// @param r Red component [0, 255]
        /// @param g Green component [0, 255]
        /// @param b Blue component [0, 255]
        /// @param a Alpha component [0, 255], defaults to 255 (opaque)
        explicit Color(u8 r, u8 g, u8 b, u8 a = 255);

        /// @brief Constructs a color from a float array
        /// @param color Float array containing RGBA values [0.0, 1.0]
        explicit Color(const f32* color);

        /// @brief Copy constructor
        /// @param other Color to copy from
        Color(const Color& other);

        /// @brief Copy assignment operator
        /// @param other Color to copy from
        /// @return Reference to this color
        Color& operator=(const Color& other);

        /// @brief Move constructor
        /// @param other Color to move from
        Color(Color&& other) noexcept;

        /// @brief Move assignment operator
        /// @param other Color to move from
        /// @return Reference to this color
        Color& operator=(Color&& other) noexcept;

        /// @brief Equality comparison operator
        /// @param other Color to compare against
        /// @return True if colors are equal, false otherwise
        bool operator==(const Color& other) const;

        /// @brief Inequality comparison operator
        /// @param other Color to compare against
        /// @return True if colors are not equal, false otherwise
        bool operator!=(const Color& other) const;

        // Component setters

        /// @brief Creates a new color with modified alpha channel
        /// @param a New alpha value [0.0, 1.0]
        /// @return New color with modified alpha
        ASTERA_KEEP Color WithAlpha(f32 a) const;

        /// @brief Creates a new color with modified blue channel
        /// @param b New blue value [0.0, 1.0]
        /// @return New color with modified blue
        ASTERA_KEEP Color WithBlue(f32 b) const;

        /// @brief Creates a new color with modified green channel
        /// @param g New green value [0.0, 1.0]
        /// @return New color with modified green
        ASTERA_KEEP Color WithGreen(f32 g) const;

        /// @brief Creates a new color with modified red channel
        /// @param r New red value [0.0, 1.0]
        /// @return New color with modified red
        ASTERA_KEEP Color WithRed(f32 r) const;

        // Modifiers

        /// @brief Adjusts the brightness of the color
        /// @param factor Brightness multiplier (1.0 = no change, >1.0 brighter, <1.0 darker)
        /// @return New color with adjusted brightness
        ASTERA_KEEP Color Brightness(f32 factor) const;

        /// @brief Converts the color to grayscale
        /// @return Grayscale version of the color
        ASTERA_KEEP Color Greyscale() const;

        /// @brief Increases the saturation of the color
        /// @param factor Saturation factor (1.0 = no change, >1.0 more saturated)
        /// @return New color with increased saturation
        ASTERA_KEEP Color Saturate(f32 factor) const;

        /// @brief Decreases the saturation of the color
        /// @param factor Desaturation factor (1.0 = no change, >1.0 more desaturated)
        /// @return New color with decreased saturation
        ASTERA_KEEP Color Desaturate(f32 factor) const;

        // Conversions

        /// @brief Converts the color to a hexadecimal string
        /// @return Hex color string (e.g., "#FF5733")
        ASTERA_KEEP string ToString() const;

        /// @brief Converts the color to a 32-bit ARGB packed integer
        /// @return 32-bit color value in ARGB format (0xAARRGGBB)
        ASTERA_KEEP u32 ToU32() const;

        /// @brief Converts the color to a 32-bit ABGR packed integer
        /// @return 32-bit color value in ABGR format (0xAABBGGRR)
        ASTERA_KEEP u32 ToU32_ABGR() const;

        /// @brief Writes the color components to a float array
        /// @param color Output float array (must have space for 4 floats: RGBA)
        void ToFloatArray(f32* color) const;

        /// @brief Converts the color to HSV color space
        /// @param h Output hue value [0.0, 360.0]
        /// @param s Output saturation value [0.0, 1.0]
        /// @param v Output value/brightness [0.0, 1.0]
        void ToHSV(f32& h, f32& s, f32& v) const;

        /// @brief Generic template conversion method
        /// @tparam T Target type to convert to
        /// @return Converted color value
        template<typename T>
        ASTERA_KEEP T To() const {
            return T {};
        }

        // Components

        /// @brief Gets the red component
        /// @return Red value [0.0, 1.0]
        ASTERA_KEEP f32 R() const;

        /// @brief Gets the green component
        /// @return Green value [0.0, 1.0]
        ASTERA_KEEP f32 G() const;

        /// @brief Gets the blue component
        /// @return Blue value [0.0, 1.0]
        ASTERA_KEEP f32 B() const;

        /// @brief Gets the alpha component
        /// @return Alpha value [0.0, 1.0]
        ASTERA_KEEP f32 A() const;

        /// @brief Calculates the perceived luminance of the color
        /// @return Luminance value [0.0, 1.0] using relative luminance formula
        ASTERA_KEEP f32 Luminance() const;

        // Static methods

        /// @brief Blends two colors using alpha compositing
        /// @param foreground Foreground color with alpha
        /// @param background Background color
        /// @return Blended color result
        static Color AlphaBlend(const Color& foreground, const Color& background);

        /// @brief Linear interpolation between two colors
        /// @param a Start color
        /// @param b End color
        /// @param t Interpolation factor [0.0, 1.0]
        /// @return Interpolated color
        static Color Lerp(const Color& a, const Color& b, f32 t);

        /// @brief Multiply blend mode
        /// @param a First color
        /// @param b Second color
        /// @return Multiplied color (darker result)
        static Color Multiply(const Color& a, const Color& b);

        /// @brief Screen blend mode
        /// @param a First color
        /// @param b Second color
        /// @return Screened color (lighter result)
        static Color Screen(const Color& a, const Color& b);

        /// @brief Overlay blend mode
        /// @param a First color
        /// @param b Second color
        /// @return Overlay blended color (combines multiply and screen)
        static Color Overlay(const Color& a, const Color& b);

        /// @brief Soft light blend mode
        /// @param a First color
        /// @param b Second color
        /// @return Soft light blended color (subtle lighting effect)
        static Color SoftLight(const Color& a, const Color& b);

        /// @brief Hard light blend mode
        /// @param a First color
        /// @param b Second color
        /// @return Hard light blended color (strong lighting effect)
        static Color HardLight(const Color& a, const Color& b);

        /// @brief Color dodge blend mode
        /// @param a First color
        /// @param b Second color
        /// @return Color dodge result (brightens based on blend color)
        static Color ColorDodge(const Color& a, const Color& b);

        /// @brief Color burn blend mode
        /// @param a First color
        /// @param b Second color
        /// @return Color burn result (darkens based on blend color)
        static Color ColorBurn(const Color& a, const Color& b);

        /// @brief Creates a color from HSV color space
        /// @param h Hue [0.0, 360.0]
        /// @param s Saturation [0.0, 1.0]
        /// @param v Value/brightness [0.0, 1.0]
        /// @param a Alpha [0.0, 1.0], defaults to 1.0 (opaque)
        /// @return Color created from HSV values
        static Color FromHSV(f32 h, f32 s, f32 v, f32 a = 1.0f);

    private:
        /// @brief Red component [0.0, 1.0]
        f32 mRed {0.0f};

        /// @brief Green component [0.0, 1.0]
        f32 mGreen {0.0f};

        /// @brief Blue component [0.0, 1.0]
        f32 mBlue {0.0f};

        /// @brief Alpha component [0.0, 1.0]
        f32 mAlpha {1.0f};

        /// @brief Linearizes a color component for gamma-correct operations
        /// @param v Component value [0.0, 1.0]
        /// @return Linearized component value
        /// @see https://stackoverflow.com/questions/61138110/what-is-the-correct-gamma-correction-function
        static f32 LinearizeComponent(f32 v);

        /// @brief Converts a normalized float to 8-bit integer
        /// @param v Float value [0.0, 1.0]
        /// @return 8-bit integer [0, 255]
        static u32 FloatToU32(f32 v);

        /// @brief Converts an 8-bit integer to normalized float
        /// @param v Integer value [0, 255]
        /// @return Normalized float [0.0, 1.0]
        static f32 U32ToFloat(u32 v);
    };

    /// @brief Specialization to convert Color to 32-bit ARGB packed integer
    /// @return 32-bit color value in ARGB format (0xAARRGGBB)
    template<>
    ASTERA_KEEP inline u32 Color::To() const {
        const auto redByte   = CAST<u8>(FloatToU32(mRed));
        const auto greenByte = CAST<u8>(FloatToU32(mGreen));
        const auto blueByte  = CAST<u8>(FloatToU32(mBlue));
        const auto alphaByte = CAST<u8>(FloatToU32(mAlpha));
        return (alphaByte << 24) | (redByte << 16) | (greenByte << 8) | blueByte;
    }

    /// @brief Specialization to convert Color to hexadecimal string
    /// @return Hex color string (e.g., "#FF5733")
    template<>
    ASTERA_KEEP inline string Color::To() const {
        const u32 r = U32ToFloat(mRed + 0.5f);
        const u32 g = U32ToFloat(mGreen + 0.5f);
        const u32 b = U32ToFloat(mBlue + 0.5f);
        std::stringstream ss;
        ss << "#" << std::hex << std::setfill('0') << std::setw(2) << (r & 0xFF) << std::setw(2) << (g & 0xFF)
           << std::setw(2) << (b & 0xFF);
        return ss.str();
    }

    /// @brief Specialization to convert Color to ImGui's ImVec4 format
    /// @return ImVec4 containing RGBA components
    template<>
    ASTERA_KEEP inline ImVec4 Color::To() const {
        return ImVec4 {mRed, mGreen, mBlue, mAlpha};
    }

    /// @brief Namespace containing predefined color constants
    namespace Colors {
        /// @brief Pure white (1.0, 1.0, 1.0, 1.0)
        static Color White {1.0f, 1.0f, 1.0f};

        /// @brief Pure black (0.0, 0.0, 0.0, 1.0)
        static Color Black {0.0f, 0.0f, 0.0f};

        /// @brief Pure red (1.0, 0.0, 0.0, 1.0)
        static Color Red {1.0f, 0.0f, 0.0f};

        /// @brief Pure green (0.0, 1.0, 0.0, 1.0)
        static Color Green {0.0f, 1.0f, 0.0f};

        /// @brief Pure blue (0.0, 0.0, 1.0, 1.0)
        static Color Blue {0.0f, 0.0f, 1.0f};

        /// @brief Yellow (1.0, 1.0, 0.0, 1.0)
        static Color Yellow {1.0f, 1.0f, 0.0f};

        /// @brief Magenta (1.0, 0.0, 1.0, 1.0)
        static Color Magenta {1.0f, 0.0f, 1.0f};

        /// @brief Cyan (0.0, 1.0, 1.0, 1.0)
        static Color Cyan {0.0f, 1.0f, 1.0f};

        /// @brief Light grey (0.75, 0.75, 0.75, 1.0)
        static Color LightGrey {0.75f, 0.75f, 0.75f};

        /// @brief Medium grey (0.5, 0.5, 0.5, 1.0)
        static Color Grey {0.5f, 0.5f, 0.5f};

        /// @brief Dark grey (0.25, 0.25, 0.25, 1.0)
        static Color DarkGrey {0.25f, 0.25f, 0.25f};

        /// @brief White with 25% opacity
        static Color White25 {1.0f, 1.0f, 1.0f, 0.25f};

        /// @brief White with 50% opacity
        static Color White50 {1.0f, 1.0f, 1.0f, 0.5f};

        /// @brief White with 75% opacity
        static Color White75 {1.0f, 1.0f, 1.0f, 0.75f};

        /// @brief Black with 25% opacity
        static Color Black25 {0.0f, 0.0f, 0.0f, 0.25f};

        /// @brief Black with 50% opacity
        static Color Black50 {0.0f, 0.0f, 0.0f, 0.5f};

        /// @brief Black with 75% opacity
        static Color Black75 {0.0f, 0.0f, 0.0f, 0.75f};

        /// @brief Fully transparent color (0.0, 0.0, 0.0, 0.0)
        static Color Transparent {0.0f, 0.0f};
    }  // namespace Colors
}  // namespace Astera

#ifndef ASTERA_COLOR_HASH_SPECIALIZATION
    #define ASTERA_COLOR_HASH_SPECIALIZATION
/// @brief std::hash specialization for Color to allow usage in unordered containers
///
/// Enables Color to be used as a key in std::unordered_map, std::unordered_set, etc.
/// Hash is computed from the 32-bit ARGB representation.
template<>
struct std::hash<Astera::Color> {
    /// @brief Computes hash value for a Color
    /// @param color Color to hash
    /// @return Hash value based on 32-bit ARGB representation
    std::size_t operator()(const Astera::Color& color) const noexcept {
        return std::hash<Astera::u32> {}(color.ToU32());
    }
};
#endif