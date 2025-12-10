/// @author: Jake Rieger
/// @created: 12/3/2025.
///

#include "Math.hpp"

#include <random>

namespace Nth {
    f32 Math::Min(f32 a, f32 b) {
        return (a < b) ? a : b;
    }

    f32 Math::Max(f32 a, f32 b) {
        return (a > b) ? a : b;
    }

    f32 Math::Clamp(f32 a, f32 b) {
        return (a > b) ? Max(a, b) : Min(a, b);
    }

    f32 Math::Sqrt(f32 a) {
        return sqrt(a);
    }

    f32 Math::Pow(f32 a, f32 b) {
        return pow(a, b);
    }

    f32 Math::Sin(f32 a) {
        return sin(a);
    }

    f32 Math::Cos(f32 a) {
        return cos(a);
    }

    f32 Math::Tan(f32 a) {
        return tan(a);
    }

    f32 Math::Floor(f32 a) {
        return floor(a);
    }

    f32 Math::Ceil(f32 a) {
        return ceil(a);
    }

    f32 Math::Round(f32 a) {
        return round(a);
    }

    f32 Math::DegToRad(f32 a) {
        return a * kPi / 180.f;
    }

    f32 Math::RadToDeg(f32 a) {
        return a * kPi;
    }

    f32 Math::Random() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<f32> dis(0.0f, 1.0f);
        return dis(gen);
    }

    i32 Math::RandomInt(i32 min, i32 max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<i32> dis(min, max);
        return dis(gen);
    }

    f32 Math::Lerp(f32 a, f32 b, f32 t) {
        return a + (b - a) * t;
    }

    void Math::RegisterLuaGlobals(sol::state& lua) {
        lua["Math"]       = lua.create_table();
        auto math         = lua["Math"];
        math["Pi"]        = kPi;
        math["Tau"]       = kTau;
        math["E"]         = kEuler;
        math["Min"]       = [](const sol::object&, f32 a, f32 b) -> f32 { return Min(a, b); };
        math["Max"]       = [](const sol::object&, f32 a, f32 b) -> f32 { return Max(a, b); };
        math["Clamp"]     = [](const sol::object&, f32 a, f32 b) -> f32 { return Clamp(a, b); };
        math["Sqrt"]      = [](const sol::object&, f32 a) -> f32 { return Sqrt(a); };
        math["Pow"]       = [](const sol::object&, f32 a, f32 b) -> f32 { return Pow(a, b); };
        math["Sin"]       = [](const sol::object&, f32 a) -> f32 { return Sin(a); };
        math["Cos"]       = [](const sol::object&, f32 a) -> f32 { return Cos(a); };
        math["Tan"]       = [](const sol::object&, f32 a) -> f32 { return Tan(a); };
        math["Floor"]     = [](const sol::object&, f32 a) -> f32 { return Floor(a); };
        math["Ceil"]      = [](const sol::object&, f32 a) -> f32 { return Ceil(a); };
        math["Round"]     = [](const sol::object&, f32 a) -> f32 { return Round(a); };
        math["DegToRad"]  = [](const sol::object&, f32 a) -> f32 { return DegToRad(a); };
        math["RadToDeg"]  = [](const sol::object&, f32 a) -> f32 { return RadToDeg(a); };
        math["Random"]    = [](const sol::object&) -> f32 { return Random(); };
        math["RandomInt"] = [](const sol::object&, i32 a, i32 b) -> i32 { return RandomInt(a, b); };
        math["Lerp"]      = [](const sol::object&, f32 a, f32 b, f32 t) -> f32 { return Lerp(a, b, t); };
    }
}  // namespace Nth