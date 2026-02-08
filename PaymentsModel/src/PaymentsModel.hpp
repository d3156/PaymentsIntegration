#pragma once
#include <PluginCore/IModel.hpp>
#include <functional>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <EasyHttpLib/EasyWebServer>
#include <memory>
#include <string>
#include <vector>

struct Order {
    const std::string order_id = "";
    size_t amount              = 0;
    std::string &paymentProvider;

    enum class Status { InProcess, Fail, Canceled, Success };
    Status status             = Status::InProcess;
    std::string stauts_string = "InProcess";
};

class PaymentsModel final : public d3156::PluginCore::IModel
{

    boost::thread thread_;
    boost::asio::io_context io_;
    void run();

public:
    using PaymentCallback =
        std::function<void(const std::string &sender, const std::string &order_id, const std::string &amount)>;

    static std::string name();

    std::unique_ptr<d3156::EasyWebServer> server;

    int deleteOrder() override { return 0; }

    void init() override {}

    void postInit() override;

    std::vector<PaymentCallback> calbacks;

    ~PaymentsModel();
};
