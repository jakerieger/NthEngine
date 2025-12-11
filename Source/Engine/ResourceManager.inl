/*
 *  Filename: ResourceManager.inl
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

#include "ArenaAllocator.hpp"
#include "Rendering/RenderContext.hpp"

#include <typeindex>

namespace Astera {
    class ResourceLoaderBase;

    class ResourceBase {
    public:
        virtual ~ResourceBase() = default;
    };

    template<typename T>
    class Resource : public ResourceBase {
    public:
        T data;

        template<typename... Args>
        // ReSharper disable once CppNonExplicitConvertingConstructor
        Resource(Args&&... args) : data(std::forward<Args>(args)...) {}
    };

    class ResourceLoaderBase {
    public:
        virtual ~ResourceLoaderBase()                                                               = default;
        virtual ResourceBase* Load(RenderContext& context, ArenaAllocator& allocator, const u64 id) = 0;
    };

    template<typename T>
    class ResourceLoader : public ResourceLoaderBase {
    public:
        using ResourceType = T;

        ResourceBase* Load(RenderContext& context, ArenaAllocator& allocator, const u64 id) override {
            void* memory = allocator.Allocate(sizeof(Resource<T>), alignof(Resource<T>));
            if (!memory) return nullptr;
            return new (memory) Resource<T>(LoadImpl(context, id));
        }

    private:
        virtual T LoadImpl(RenderContext& context, const u64 id) = 0;
    };

    template<typename T>
    class ResourceHandle;

    class ResourceManager {
        ASTERA_CLASS_PREVENT_MOVES_COPIES(ResourceManager)

        ArenaAllocator mAllocator;
        RenderContext& mRenderContext;
        unordered_map<u64, ResourceBase*> mResources;
        unordered_map<std::type_index, unique_ptr<ResourceLoaderBase>> mLoaders;

    public:
        explicit ResourceManager(RenderContext& context, const size_t arenaSize = 1_GB)
            : mAllocator(arenaSize), mRenderContext(context) {}

        ~ResourceManager() {
            mAllocator.Reset();
        }

        template<typename... Loaders>
        void RegisterLoaders() {
            (
              [this]() {
                  auto loader                                                       = make_unique<Loaders>();
                  mLoaders[std::type_index(typeid(typename Loaders::ResourceType))] = std::move(loader);
              }(),
              ...);
        }

        template<typename T>
        bool LoadResource(const u64 id) {
            if (mResources.contains(id)) {
                return true;  // asset already exists
            }

            const auto loaderIt = mLoaders.find(std::type_index(typeid(T)));
            if (loaderIt == mLoaders.end()) {
                return false;  // no registered loader for type
            }

            ResourceBase* resource = loaderIt->second->Load(mRenderContext, mAllocator, id);
            if (!resource) {
                return false;  // loading failed
            }

            mResources[id] = resource;
            return true;
        }

        template<typename T>
        ResourceHandle<T> FetchResource(const u64 id) {
            const auto it = mResources.find(id);
            if (it == mResources.end()) { return ResourceHandle<T> {}; }

            Resource<T>* typedResource = DCAST<Resource<T>*>(it->second);
            if (!typedResource) { return {}; }

            return ResourceHandle<T>(this, id, &typedResource->data);
        }

        void Clear() {
            mResources.clear();
            mAllocator.Reset();
        }

        const ArenaAllocator& GetAllocator() {
            return mAllocator;
        }
    };

    template<typename T>
    class ResourceHandle {
        ResourceManager* mManager;
        u64 mId {0};
        T* mData;

    public:
        ResourceHandle() : mManager(nullptr), mData(nullptr) {}

        ResourceHandle(ResourceManager* manager, const u64 id, T* data) : mManager(manager), mId(id), mData(data) {}

        T* Get() {
            return mData;
        }

        const T* Get() const {
            return mData;
        }

        T* operator->() {
            return mData;
        }

        const T* operator->() const {
            return mData;
        }

        [[nodiscard]] bool Valid() const {
            return (mManager != nullptr) && (mData != nullptr) && (mId != 0);
        }
    };
}  // namespace Astera
