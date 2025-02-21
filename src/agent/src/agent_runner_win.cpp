#include <agent_runner.hpp>

/// Command-line options
static const auto OPT_INSTALL_SERVICE {"install-service"};
static const auto OPT_REMOVE_SERVICE {"remove-service"};
static const auto OPT_RUN_SERVICE {"run-service"};

void AgentRunner::AddPlatformSpecificOptions()
{
    cmdParser.add_options()(OPT_INSTALL_SERVICE, "Use this option to install Wazuh as a Windows service")(
        OPT_REMOVE_SERVICE, "Use this option to remove Wazuh Windows service")(
        OPT_RUN_SERVICE, "Use this option to run Wazuh as a Windows service");
}
