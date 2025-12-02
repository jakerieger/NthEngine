// Author: Jake Rieger
// Created: 12/2/2025.
//

#pragma once

#include "ScriptEngine.hpp"

namespace Nth {
    struct BehaviorEntity {
        string name;
        Transform transform;

        explicit BehaviorEntity(const string& name, const Transform& transform) : name(name), transform(transform) {}
    };

    template<>
    struct LuaTypeTraits<BehaviorEntity> {
        static constexpr std::string_view typeName = "Entity";

        static void RegisterMembers(sol::usertype<BehaviorEntity>& usertype) {
            usertype["name"]      = &BehaviorEntity::name;
            usertype["transform"] = &BehaviorEntity::transform;
        }
    };

    template<>
    struct LuaTypeTraits<Vec2> {
        static constexpr std::string_view typeName = "Vec2";

        static void RegisterMembers(sol::usertype<Vec2>& usertype) {
            usertype["x"] = &Vec2::x;
            usertype["y"] = &Vec2::y;
        }
    };

    template<>
    struct LuaTypeTraits<Clock> {
        static constexpr std::string_view typeName = "Clock";

        static void RegisterMembers(sol::usertype<Clock>& usertype) {
            usertype["GetDeltaTimePrecise"] = &Clock::GetDeltaTimePrecise;
            usertype["GetDeltaTime"]        = &Clock::GetDeltaTime;
            usertype["GetTotalTime"]        = &Clock::GetTotalTime;
            usertype["GetFramesPerSecond"]  = &Clock::GetFramesPerSecond;
            usertype["GetRawCounter"]       = &Clock::GetRawCounter;
            usertype["GetCounterFrequency"] = &Clock::GetCounterFrequency;
        }
    };

    template<>
    struct LuaTypeTraits<Transform> {
        static constexpr std::string_view typeName = "Transform";

        static void RegisterMembers(sol::usertype<Transform>& usertype) {
            usertype["Position"] = &Transform::position;
            usertype["Rotation"] = &Transform::rotation;
            usertype["Scale"]    = &Transform::scale;
        }
    };
}  // namespace Nth