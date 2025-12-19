/*
 *  Filename: EnginePlugin.hpp
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
#include "EnginePluginInterface.hpp"

#if defined(ASTERA_PLATFORM_LINUX)
    #include <dlfcn.h>
using LibraryHandle = void*;

    #define PLUGIN_LOAD(name) dlopen(name, RTLD_LAZY)
    #define PLUGIN_UNLOAD(handle) dlclose(handle)
    #define PLUGIN_GET_CREATE_FUNC(handle) (CreatePluginFunc) dlsym(handle, "CreatePlugin")
#elif defined(ASTERA_PLATFORM_WINDOWS)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <Windows.h>
using LibraryHandle = HMODULE;

    #define PLUGIN_LOAD(name) ::LoadLibraryA(name, RTLD_LAZY)
    #define PLUGIN_UNLOAD(handle) ::FreeLibrary(handle)
    #define PLUGIN_GET_CREATE_FUNC(handle) RCAST<CreatePluginFunc>(::GetProcAddress(handle, "CreatePlugin"));
#else
    #error "Plugins are not supported on this platform"
#endif

namespace Astera {
    template<typename Plugin>
    concept ValidPlugin = std::is_base_of_v<IEnginePlugin, Plugin>;

    class Plugin {
        IEnginePlugin* mPluginHandle {nullptr};
        LibraryHandle mLibHandle {nullptr};
        Path mPluginPath;

    public:
        Plugin() = default;

        Result<string> Load(const Path& pluginFile) {
            mLibHandle = PLUGIN_LOAD(pluginFile.c_str());
            if (!mLibHandle) {
                return unexpected(fmt::format("Plugin '{}' failed to load. Ensure the plugin is a valid Astera plugin.",
                                              pluginFile.string()));
            }

            const auto CreatePlugin = PLUGIN_GET_CREATE_FUNC(mLibHandle);
            if (!CreatePlugin) {
                PLUGIN_UNLOAD(mLibHandle);
                return unexpected(
                  "Could not find 'CreatePlugin' symbol in plugin. Make sure it is defined and exported.");
            }

            mPluginHandle = CreatePlugin();
            mPluginPath   = pluginFile;
            return mPluginHandle->GetName();
        }

        i32 Unload() {
            if (mLibHandle) {
                PLUGIN_UNLOAD(mLibHandle);
                mLibHandle = nullptr;
            }

            if (mPluginHandle) {
                mPluginHandle = nullptr;
            }

            mPluginPath = "";
            return 0;
        }

        ASTERA_CLASS_PREVENT_COPIES(Plugin)

        ~Plugin() {
            std::ignore = Unload();
        }

        Plugin(Plugin&& other) noexcept
            : mPluginHandle(std::exchange(other.mPluginHandle, nullptr)),
              mLibHandle(std::exchange(other.mLibHandle, nullptr)) {}

        Plugin& operator=(Plugin&& other) noexcept {
            if (this != &other) {
                std::ignore   = Unload();
                mPluginHandle = std::exchange(other.mPluginHandle, nullptr);
                mLibHandle    = std::exchange(other.mLibHandle, nullptr);
            }
            return *this;
        }

        ASTERA_KEEP const Path& GetPluginPath() const {
            return mPluginPath;
        }

        IEnginePlugin* operator->() const {
            return mPluginHandle;
        }

        template<ValidPlugin Plugin>
        Plugin* operator->() const {
            return DCAST<Plugin*>(mPluginHandle);
        }

        ASTERA_KEEP IEnginePlugin* Get() const {
            return mPluginHandle;
        }

        template<ValidPlugin Plugin>
        Plugin* Get() const {
            return DCAST<Plugin*>(mPluginHandle);
        }
    };
}  // namespace Astera
