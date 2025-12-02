// Author: Jake Rieger
// Created: 12/2/25.
//

#pragma once

#include "CommonPCH.hpp"
#include "SceneDescriptor.hpp"

namespace Nth {
    struct SceneDescriptor;
    class SceneState;

    class SceneParser {
    public:
        static void StateToDescriptor(const SceneState& state, SceneDescriptor& outDescriptor);
        static void DescriptorToState(const SceneDescriptor& descriptor, SceneState& outState);
        static void SerializeDescriptor(const SceneDescriptor& descriptor);
        static void DeserializeDescriptor(const fs::path& filename, SceneDescriptor& outDescriptor);
        static void DeserializeDescriptor(const string& source, SceneDescriptor& outDescriptor);
    };
}  // namespace Nth
