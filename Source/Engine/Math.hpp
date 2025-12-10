/// @author: Jake Rieger
/// @created: 12/3/2025.
///

#pragma once

#include "Common/CommonPCH.hpp"

#include <sol/sol.hpp>

namespace sol {
    class state;
}  // namespace sol

namespace Nth {
    /// @brief Implements standard math operations and defines common constants
    class Math {
    public:
        inline static constexpr f64 kPi    = 3.141592653589793;
        inline static constexpr f64 kTau   = kPi * kPi;
        inline static constexpr f64 kEuler = 2.718281828459045;

        /// @brief Returns the minimum of two values
        /// @param a First value
        /// @param b Second value
        /// @return The smaller of the two values
        static f32 Min(f32 a, f32 b);

        /// @brief Returns the maximum of two values
        /// @param a First value
        /// @param b Second value
        /// @return The larger of the two values
        static f32 Max(f32 a, f32 b);

        /// @brief Clamps a value between two bounds
        /// @param a First value
        /// @param b Second value
        /// @return The clamped value
        static f32 Clamp(f32 a, f32 b);

        /// @brief Calculates the square root of a value
        /// @param a The value to calculate the square root of
        /// @return The square root of the value
        static f32 Sqrt(f32 a);

        /// @brief Raises a value to a power
        /// @param a The base value
        /// @param b The exponent
        /// @return a raised to the power of b
        static f32 Pow(f32 a, f32 b);

        /// @brief Calculates the sine of an angle
        /// @param a The angle in radians
        /// @return The sine of the angle
        static f32 Sin(f32 a);

        /// @brief Calculates the cosine of an angle
        /// @param a The angle in radians
        /// @return The cosine of the angle
        static f32 Cos(f32 a);

        /// @brief Calculates the tangent of an angle
        /// @param a The angle in radians
        /// @return The tangent of the angle
        static f32 Tan(f32 a);

        /// @brief Rounds a value down to the nearest integer
        /// @param a The value to floor
        /// @return The floored value
        static f32 Floor(f32 a);

        /// @brief Rounds a value up to the nearest integer
        /// @param a The value to ceil
        /// @return The ceiled value
        static f32 Ceil(f32 a);

        /// @brief Rounds a value to the nearest integer
        /// @param a The value to round
        /// @return The rounded value
        static f32 Round(f32 a);

        /// @brief Converts degrees to radians
        /// @param a The angle in degrees
        /// @return The angle in radians
        static f32 DegToRad(f32 a);

        /// @brief Converts radians to degrees
        /// @param a The angle in radians
        /// @return The angle in degrees
        static f32 RadToDeg(f32 a);

        /// @brief Generates a random float between 0.0 and 1.0
        /// @return A random float value
        static f32 Random();

        /// @brief Generates a random integer between min and max
        /// @return A random signed integer value
        static i32 RandomInt(i32 min, i32 max);

        /// @brief Linearly interpolates between two values
        /// @param a The start value
        /// @param b The end value
        /// @param t The interpolation factor (0.0 to 1.0)
        /// @return The interpolated value
        static f32 Lerp(f32 a, f32 b, f32 t);

    private:
        friend class Game;

        static void RegisterLuaGlobals(sol::state& lua);
    };
}  // namespace Nth