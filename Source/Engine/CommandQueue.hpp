// Author: Jake Rieger
// Created: 11/27/25.
//

#pragma once

#include "CommonPCH.hpp"
#include "Command.hpp"

namespace N {
    /// @brief Command queue for batching and executing rendering commands
    class CommandQueue {
    public:
        CommandQueue()  = default;
        ~CommandQueue() = default;

        N_CLASS_PREVENT_MOVES_COPIES(CommandQueue)

        /// @brief Add a command to the queue
        /// @tparam T Command type (must be one of the types in RenderCommand variant)
        /// @param command The command to enqueue
        template<typename T>
        void Enqueue(T&& command) {
            mCommands.emplace_back(std::forward<T>(command));
        }

        /// @brief Execute all queued commands and clear the queue
        void ExecuteQueue();

        /// @brief Clear all queued commands without executing them
        void Clear();

        /// @brief Get the number of commands in the queue
        N_ND size_t Size() const {
            return mCommands.size();
        }

        /// @brief Check if the queue is empty
        N_ND bool IsEmpty() const {
            return mCommands.empty();
        }

        /// @brief Reserve space for a specific number of commands
        void Reserve(size_t capacity) {
            mCommands.reserve(capacity);
        }

    private:
        /// @brief Execute a single command using visitor pattern
        static void ExecuteCommand(const RenderCommand& command);

        vector<RenderCommand> mCommands;
    };

    /// @brief Command visitor for dispatching commands to their handlers
    class CommandExecutor {
    public:
        void operator()(const ClearCommand& cmd) const;
        void operator()(const DrawSpriteCommand& cmd) const;
        void operator()(const SetViewportCommand& cmd) const;
        void operator()(const BindShaderCommand& cmd) const;
        void operator()(const SetUniformCommand& cmd) const;
    };
}  // namespace N
