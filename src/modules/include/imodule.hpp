#pragma once

#include <command_entry.hpp>
#include <configuration_parser.hpp>

#include <boost/asio/awaitable.hpp>

#include <nlohmann/json.hpp>

#include <string>

using Co_CommandExecutionResult = boost::asio::awaitable<module_command::CommandExecutionResult>;

class IModule
{
public:
    /// @brief Virtual destructor
    virtual ~IModule() = default;

    /// @brief Runs the module. This function is blocking and will not return until the module is stopped.
    virtual void Run() = 0;

    /// @brief Stops the module.
    virtual void Stop() = 0;

    /// @brief Sets up the module with the given configuration parser.
    /// @param configurationParser The configuration parser to set up the module with
    virtual void Setup(std::shared_ptr<const configuration::ConfigurationParser> configurationParser) = 0;

    /// @brief Executes a command with the given name and arguments.
    /// @param command The name of the command to execute
    /// @param parameters The arguments for the command
    /// @return The result of the command execution
    virtual Co_CommandExecutionResult ExecuteCommand(std::string command, nlohmann::json parameters) = 0;

    /// @brief Returns the name of the module.
    /// @return The name of the module
    virtual const std::string& Name() const = 0;
};
