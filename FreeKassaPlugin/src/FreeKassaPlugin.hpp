#include "FreeKassa.hpp"
#include <MetricsModel/MetricsModel>
#include <PluginCore/IPlugin>
#include <PluginCore/IModel>

#include <PaymentsModel>
#include <memory>

class FreeKassaPlugin final : public d3156::PluginCore::IPlugin
{
    PaymentsModel *model   = nullptr;
    std::string configPath = "./config/FreeKassa.json";
    std::unique_ptr<FreeKassa::Client> client;

public:
    void registerArgs(d3156::Args::Builder &bldr) override;

    void registerModels(d3156::PluginCore::ModelsStorage &models) override;

    void postInit() override
    {
        // TODO: start threads / async jobs here if needed
    }

    ~FreeKassaPlugin() { client.reset(); }
};
