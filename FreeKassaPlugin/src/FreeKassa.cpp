#include "FreeKassa.hpp"
#include <PaymentsModel>
#include <exception>
#include <filesystem>
#include <format>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/hex.hpp>
#include <iostream>
#include <ostream>
#include <string>
#include <openssl/md5.h>
#include <boost/algorithm/string.hpp>
extern const std::string_view embedded_html_page;

namespace FreeKassa
{

    Client::Client(Settings sett)
        : s(sett), amount("PaymentsAmount", {{"wallet", "FreeKassa"}}),
          paymets_count("PaymentsCount", {{"wallet", "FreeKassa"}})
    {
        auto handler = [this](const beast::http::request<beast::http::string_body> &req,
                              const boost::asio::ip::address &client_ip) -> std::pair<bool, std::string> {
            return this->handle_webhook_request(req, client_ip);
        };
        if (std::filesystem::exists("./FreeKassa/Page.html"))
            htmlPayPage = (std::ostringstream() << std::ifstream("./FreeKassa/Page.html").rdbuf()).str();
        htmlPayPage.assign(embedded_html_page.data(), embedded_html_page.size());
    }

    std::string md5(const std::string &s)
    {
        unsigned char d[MD5_DIGEST_LENGTH];
        MD5(reinterpret_cast<const unsigned char *>(s.data()), s.size(), d);
        return boost::algorithm::hex_lower(std::string(reinterpret_cast<char *>(d), MD5_DIGEST_LENGTH));
    }

    std::string Client::createFreeKassaLink(double amount, const std::string &paymentId)
    {
        auto amount_s               = std::format("{:.2f}", amount);
        amountByMerchant[paymentId] = amount_s;
        return "https://pay.fk.money/?m=" + s.shop_id_ + "&oa=" + amount_s + "&o=" + paymentId +
               "&s=" + md5(s.shop_id_ + ":" + amount_s + ":" + s.secretword1 + ":RUB:" + paymentId) + "&currency=RUB";
    }

#include <unordered_map>

    std::unordered_map<std::string, std::string> parse_form(const std::string &body)
    {
        std::unordered_map<std::string, std::string> params;
        size_t start = 0;
        while (start < body.size()) {
            auto end = body.find('&', start);
            if (end == std::string::npos) end = body.size();
            auto eq = body.find('=', start);
            if (eq != std::string::npos && eq < end) {
                auto key    = body.substr(start, eq - start);
                auto val    = body.substr(eq + 1, end - eq - 1);
                params[key] = val;
            }
            start = end + 1;
        }
        return params;
    }

    std::pair<bool, std::string>
    Client::handle_webhook_request(const beast::http::request<beast::http::string_body> &req,
                                   const boost::asio::ip::address &client_ip)
    {
        try {
            if (!s.freeKassaWhitelist.contains(client_ip)) return {false, "BadRequest. Client ip not in white list!"};
            if (req.body() == "status_check=1") return {true, "status_check=1"};

            auto rendered = renderAnswer(req);
            if (!rendered.empty()) return {true, rendered};

            auto p        = parse_form(req.body());
            auto it_sign  = p.find("SIGN");
            auto it_mid   = p.find("MERCHANT_ID");
            auto it_amt   = p.find("AMOUNT");
            auto it_order = p.find("MERCHANT_ORDER_ID");

            if (it_sign == p.end() || it_mid == p.end() || it_amt == p.end() || it_order == p.end())
                return {false, "BadRequest. Not founded SIGN or MERCHANT_ID or AMOUNT or MERCHANT_ORDER_ID"};
            std::string sign =
                md5(it_mid->second + ":" + it_amt->second + ":" + s.secretword2 + ":" + it_order->second);
            if (s.shop_id_ != it_mid->second) return {false, "Incorect MERCHANT_ID"};
            if (sign != it_sign->second) return {false, "Bad signature"};

            /// Callback
            // onPayment("FreeKassa", it_order->second, it_amt->second, client_ip);

            amount += std::stoul(it_amt->second);
            paymets_count++;
        } catch (std::exception &e) {
            std::cout << "[FreeKassa] Error in FreeKassa::Client:" << e.what();
        }
        return {true, "OK\n"};
    }

    std::string Client::renderAnswer(const beast::http::request<beast::http::string_body> &req)
    {
        std::string target   = req.target();
        bool is_success_page = false;
        if (target.find("success") != std::string::npos)
            is_success_page = true;
        else if (target.find("fail") != std::string::npos)
            is_success_page = false;
        else
            return "";
        std::map<std::string, std::string> params;
        size_t query_pos = target.find('?');
        if (query_pos == std::string::npos) return "";
        std::string query_string = target.substr(query_pos + 1);
        size_t start             = 0;
        while (start < query_string.length()) {
            size_t eq_pos = query_string.find('=', start);
            if (eq_pos == std::string::npos) break;
            size_t amp_pos = query_string.find('&', eq_pos);
            if (amp_pos == std::string::npos) { amp_pos = query_string.length(); }
            std::string key   = query_string.substr(start, eq_pos - start);
            std::string value = query_string.substr(eq_pos + 1, amp_pos - eq_pos - 1);
            for (size_t i = 0; i < value.length(); ++i) {
                if (value[i] == '+') { value[i] = ' '; }
            }
            params[key] = value;
            start       = amp_pos + 1;
        }
        std::string merchant_order_id = params.count("MERCHANT_ORDER_ID") ? params["MERCHANT_ORDER_ID"] : "";
        std::string amount            = amountByMerchant[merchant_order_id]; // Пример - заменить на реальные данные
        auto res                      = htmlPayPage;
        boost::replace_all(res, "$amount", amount);
        boost::replace_all(res, "$user", merchant_order_id);
        boost::replace_all(res, "$shop", s.shop_link);
        boost::replace_all(res, "$support", s.support_link);
        if (is_success_page) boost::replace_all(res, "showPage(false)", "showPage(true)");
        return res;
    }

    using boost::property_tree::ptree;
    namespace fs = std::filesystem;

    Settings::Settings(std::string configPath)
    {
        if (!fs::exists(configPath)) {
            ptree pt, wl;
            pt.put("shop_id", shop_id_);
            pt.put("secretword1", secretword1);
            pt.put("secretword2", secretword2);
            pt.put("shop_link", shop_link);
            pt.put("support_link", support_link);
            pt.add_child("freeKassaWhitelist", wl);
            fs::create_directories(fs::path(configPath).parent_path());
            write_json(configPath, pt);
            return;
        }
        ptree pt;
        read_json(configPath, pt);
        shop_id_     = pt.get<std::string>("shop_id", "");
        secretword1  = pt.get<std::string>("secretword1", "");
        secretword2  = pt.get<std::string>("secretword2", "");
        shop_link    = pt.get<std::string>("shop_link", "");
        support_link = pt.get<std::string>("support_link", "");
        for (auto &v : pt.get_child("freeKassaWhitelist", ptree{}))
            freeKassaWhitelist.insert(boost::asio::ip::make_address(v.second.get_value<std::string>()));
    }

    void Client::setPaymentCallback(PaymentsModel::PaymentCallback cb) { onPayment = std::move(cb); }
}