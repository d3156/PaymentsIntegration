#pragma once
#include <PluginCore/IModel.hpp>

class PaymentsModel final : public d3156::PluginCore::IModel {
public:
    d3156::PluginCore::model_name name() override { return "PaymentsModel"; }
    int deleteOrder() override { return 0; }

    void init() override {
        // TODO: allocate/init resources here
    }

    void postInit() override {
        // TODO: optional
    }
};
