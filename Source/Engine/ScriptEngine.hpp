// Author: Jake Rieger
// Created: 12/2/25.
//

#pragma once

#include "CommonPCH.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "Clock.hpp"
#include "Components/Transform.hpp"

namespace Nth {
    struct BehaviorEntity;

    template<typename T, typename = void>
    struct LuaTypeTraits {
        static constexpr std::string_view typeName = T::kLuaTypeName;

        static void RegisterMembers(sol::usertype<T>& usertype) {
            T::RegisterMembers(usertype);
        }
    };

    template<typename T>
    struct LuaTypeRegistration {};

    struct LuaRegisterable {
        virtual ~LuaRegisterable()                          = default;
        virtual void RegisterWithLua(sol::state& lua) const = 0;
    };

    template<typename T>
    struct LuaRegistry final : LuaRegisterable {
        static constexpr std::string_view typeName = LuaTypeTraits<T>::typeName;

        void RegisterWithLua(sol::state& lua) const override {
            auto usertype = lua.new_usertype<T>(std::string {typeName});
            LuaTypeTraits<T>::RegisterMembers(usertype);
        }
    };

    struct BehaviorScriptContext {
        sol::environment env;
        sol::protected_function onAwake;
        sol::protected_function onUpdate;
        sol::protected_function onLateUpdate;
        sol::protected_function onDestroyed;
    };

    // TODO: Add more types of scripts that can be implemented to expand the capabilities of the script engine
    enum class ScriptType : u8 {
        Behavior,
        ScriptTypeCount,
    };

    class ScriptEngine {
    public:
        using ScriptID = u64;

        ScriptEngine()  = default;
        ~ScriptEngine() = default;

        N_CLASS_PREVENT_MOVES_COPIES(ScriptEngine)

        void Initialize();

        void LoadScript(const string& source, ScriptID scriptId, ScriptType type = ScriptType::Behavior);
        void LoadScript(const vector<u8>& bytecode, ScriptID scriptId, ScriptType type = ScriptType::Behavior);

        void CallAwakeBehavior(ScriptID id, const BehaviorEntity& entity);
        void CallUpdateBehavior(ScriptID id, const BehaviorEntity& entity, const Clock& clock);
        void CallLateUpdateBehavior(ScriptID id, const BehaviorEntity& entity);
        void CallDestroyedBehavior(ScriptID id, const BehaviorEntity& entity);

        void ExecuteFile(const fs::path& filename);
        void Execute(const string& source);

        template<typename T>
        void RegisterType() {
            LuaRegistry<T> {}.RegisterWithLua(mLua);
        }

        template<typename... Types>
        void RegisterTypes() {
            (RegisterType<Types>(), ...);
        }

        sol::state& GetLuaState() {
            return mLua;
        }

        bool IsInitialized() const {
            return mInitialized;
        }

    private:
        bool mInitialized {false};
        sol::state mLua;
        unordered_map<ScriptID, BehaviorScriptContext> mBehaviorScriptContexts;
    };
}  // namespace Nth
