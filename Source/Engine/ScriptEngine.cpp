// Author: Jake Rieger
// Created: 12/2/25.
//

#include "ScriptEngine.hpp"
#include "Log.hpp"
#include "ScriptTypeRegistry.hpp"

namespace Nth {
    static void PrintUninitializedError() {
        Log::Error("ScriptEngine", "Attempted to load script before script engine has been initialized!");
    }

    static void PrintScriptNotFoundError(u64 id) {
        Log::Error("ScriptEngine", "Script with id `{}` not found", id);
    }

    void ScriptEngine::Initialize() {
        if (mInitialized) return;
        mLua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::debug);
        mInitialized = true;
    }

    void ScriptEngine::LoadScript(const string& source, ScriptID scriptId, ScriptType type) {
        if (!mInitialized) {
            PrintUninitializedError();
            return;
        }

        try {
            auto env = sol::environment(mLua, sol::create, mLua.globals());
            mLua.script(source, env);

            if (type == ScriptType::Behavior) {
                sol::protected_function awakeFunc      = env["onAwake"];
                sol::protected_function updateFunc     = env["onUpdate"];
                sol::protected_function lateUpdateFunc = env["onLateUpdate"];
                sol::protected_function destroyedFunc  = env["onDestroyed"];

                mBehaviorScriptContexts[scriptId] = {std::move(env),
                                                     std::move(awakeFunc),
                                                     std::move(updateFunc),
                                                     std::move(lateUpdateFunc),
                                                     std::move(destroyedFunc)};

                Log::Debug("ScriptEngine", "Loaded script with id `{}`", scriptId);
            }
        } catch (const sol::error& e) { Log::Error("ScriptEngine", "Error loading script: {}", e.what()); }
    }

    void ScriptEngine::LoadScript(const vector<u8>& bytecode, ScriptID scriptId, const ScriptType type) {
        if (!mInitialized) {
            PrintUninitializedError();
            return;
        }

        try {
            auto env = sol::environment(mLua, sol::create, mLua.globals());

            struct ReaderState {
                const vector<u8>* bytecode {nullptr};
                size_t position {0};
            };

            ReaderState state;
            state.bytecode = &bytecode;

            auto reader = [](lua_State* L, void* data, size_t* size) -> const char* {
                const auto readerState = CAST<ReaderState*>(data);
                if (readerState->position >= readerState->bytecode->size()) {
                    *size = 0;
                    return nullptr;
                }

                *size                 = readerState->bytecode->size() - readerState->position;
                const auto result     = RCAST<const char*>(readerState->bytecode->data() + readerState->position);
                readerState->position = readerState->bytecode->size();
                return result;
            };

            const auto chunkName         = fmt::format("chunk_{}", scriptId);
            sol::load_result loadedChunk = mLua.load(reader, &state, chunkName.c_str(), sol::load_mode::binary);
            if (!loadedChunk.valid()) {
                sol::error err = loadedChunk;
                return;
            }

            if (const auto result = loadedChunk(env); !result.valid()) {
                sol::error err = result;
                return;
            }

            if (type == ScriptType::Behavior) {
                sol::protected_function awakeFunc      = env["onAwake"];
                sol::protected_function updateFunc     = env["onUpdate"];
                sol::protected_function lateUpdateFunc = env["onLateUpdate"];
                sol::protected_function destroyedFunc  = env["onDestroyed"];

                mBehaviorScriptContexts[scriptId] = {std::move(env),
                                                     std::move(awakeFunc),
                                                     std::move(updateFunc),
                                                     std::move(lateUpdateFunc),
                                                     std::move(destroyedFunc)};
            }
        } catch (const sol::error& e) { Log::Error("ScriptEngine", "Error loading script: {}", e.what()); }
    }

    void ScriptEngine::CallAwakeBehavior(const ScriptID id, const BehaviorEntity& entity) {
        if (!mInitialized) {
            PrintUninitializedError();
            return;
        }

        if (!mBehaviorScriptContexts.contains(id)) {
            PrintScriptNotFoundError(id);
            return;
        }

        if (const auto& ctx = mBehaviorScriptContexts[id]; ctx.onAwake.valid()) {
            try {
                std::ignore = ctx.onAwake(entity);
            } catch (const sol::error& e) { Log::Error("ScriptEngine", "{}", e.what()); }
        }
    }

    void ScriptEngine::CallUpdateBehavior(const ScriptID id, const BehaviorEntity& entity, const Clock& clock) {
        if (!mInitialized) {
            PrintUninitializedError();
            return;
        }

        if (!mBehaviorScriptContexts.contains(id)) {
            PrintScriptNotFoundError(id);
            return;
        }

        if (const auto& ctx = mBehaviorScriptContexts[id]; ctx.onAwake.valid()) {
            try {
                std::ignore = ctx.onUpdate(entity, clock);
            } catch (const sol::error& e) { Log::Error("ScriptEngine", "{}", e.what()); }
        }
    }

    void ScriptEngine::CallLateUpdateBehavior(const ScriptID id, const BehaviorEntity& entity) {
        if (!mInitialized) {
            PrintUninitializedError();
            return;
        }

        if (!mBehaviorScriptContexts.contains(id)) {
            PrintScriptNotFoundError(id);
            return;
        }

        if (const auto& ctx = mBehaviorScriptContexts[id]; ctx.onAwake.valid()) {
            try {
                std::ignore = ctx.onLateUpdate(entity);
            } catch (const sol::error& e) { Log::Error("ScriptEngine", "{}", e.what()); }
        }
    }

    void ScriptEngine::CallDestroyedBehavior(const ScriptID id, const BehaviorEntity& entity) {
        if (!mInitialized) {
            PrintUninitializedError();
            return;
        }

        if (!mBehaviorScriptContexts.contains(id)) {
            PrintScriptNotFoundError(id);
            return;
        }

        if (const auto& ctx = mBehaviorScriptContexts[id]; ctx.onAwake.valid()) {
            try {
                std::ignore = ctx.onDestroyed(entity);
            } catch (const sol::error& e) { Log::Error("ScriptEngine", "{}", e.what()); }
        }
    }

    void ScriptEngine::ExecuteFile(const fs::path& filename) {
        if (!mInitialized) {
            PrintUninitializedError();
            return;
        }

        N_ASSERT(exists(filename));

        try {
            mLua.script_file(filename.string());
        } catch (const sol::error& e) { Log::Error("ScriptEngine", "{}", e.what()); }
    }

    void ScriptEngine::Execute(const string& source) {
        if (!mInitialized) {
            PrintUninitializedError();
            return;
        }

        N_ASSERT(!source.empty());

        try {
            mLua.script(source);
        } catch (const sol::error& e) { Log::Error("ScriptEngine", "{}", e.what()); }
    }
}  // namespace Nth