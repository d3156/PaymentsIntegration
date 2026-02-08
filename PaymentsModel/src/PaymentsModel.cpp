#include "PaymentsModel.hpp"

void PaymentsModel::run()
{
    server = std::make_unique<d3156::EasyWebServer>(io_, 2347);
    io_.run();
}

void PaymentsModel::postInit()
{
    thread_ = boost::thread([this]() { this->run(); });
}

PaymentsModel::~PaymentsModel()
{
    if (server) server->stop();
}

std::string PaymentsModel::name() { return FULL_NAME; }
