#pragma once
#include <boost/asio.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast.hpp>
#include <map>
#include <set>
#include <string>
#include <MetricsModel/Metrics>
#include <PaymentsModel>

namespace FreeKassa
{
    using namespace boost;
    using json_map = std::map<std::string, std::string>;

    struct Settings {
        std::string shop_id_     = "";
        std::string secretword1  = "";
        std::string secretword2  = "";
        std::string shop_link    = "";
        std::string support_link = "";
        std::set<boost::asio::ip::address> freeKassaWhitelist;
        Settings(std::string configPath);
    };

    class Client
    {
    public:
        Client(Settings s);

        std::string createFreeKassaLink(double amount, const std::string &paymentId);

        std::pair<bool, std::string> handle_webhook_request(const beast::http::request<beast::http::string_body> &req,
                                                            const boost::asio::ip::address &client_ip);

        void setPaymentCallback(PaymentsModel::PaymentCallback cb);

    private:
        Metrics::Counter amount;
        Metrics::Counter paymets_count;
        PaymentsModel::PaymentCallback onPayment;
        Settings s;
        std::string htmlPayPage = "";
        std::string renderAnswer(const beast::http::request<beast::http::string_body> &req);
        std::map<std::string, std::string> amountByMerchant;
    };

}