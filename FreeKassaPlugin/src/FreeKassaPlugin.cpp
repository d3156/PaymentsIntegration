#include "FreeKassaPlugin.hpp"

void FreeKassaPlugin::registerArgs(d3156::Args::Builder &bldr)
{
    bldr.setVersion("FreeKassaPlugin " + std::string(FreeKassaPlugin_VERSION))
        .addOption(configPath, "FreeKassaPath", "path to config for FreeKassa.json");
}

void FreeKassaPlugin::registerModels(d3156::PluginCore::ModelsStorage &models)
{
    model                    = RegisterModel("PaymentsModel", new PaymentsModel(), PaymentsModel);
    MetricsModel::instance() = RegisterModel("MetricsModel", new MetricsModel(), MetricsModel);
}

// ABI required by d3156::PluginCore::Core (dlsym uses exact names)
extern "C" d3156::PluginCore::IPlugin *create_plugin() { return new FreeKassaPlugin(); }

extern "C" void destroy_plugin(d3156::PluginCore::IPlugin *p) { delete p; }