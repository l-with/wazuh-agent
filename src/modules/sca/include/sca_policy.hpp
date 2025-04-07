#pragma once

#include <message.hpp>

#include <filesystem>
#include <functional>

class SCAPolicy
{
public:
    /// @brief Loads a policy from a SCA Policy yaml file
    /// @param path The path to the SCA Policy yaml file
    /// @param pushMessage A function that pushes messages
    /// @returns A SCAPolicy object
    /// @note This function is a placeholder and should be implemented in the actual code
    static SCAPolicy LoadFromFile([[maybe_unused]] const std::filesystem::path& path,
                                  [[maybe_unused]] std::function<int(Message)> pushMessage)
    {
        return {};
    }
};
