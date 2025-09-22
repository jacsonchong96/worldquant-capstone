#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>
#include <string>

namespace beast = boost::beast;
namespace http= beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

class HttpClient {
public:
    static std::string get(const std::string& host, const std::string& target) {
        net::io_context ioc;
        ssl::context ctx{ssl::context::tls_client};
        ctx.set_verify_mode(ssl::verify_none);

        tcp::resolver resolver{ioc};
        beast::ssl_stream<tcp::socket> stream{ioc, ctx};

        auto const results = resolver.resolve(host, "443");
        net::connect(stream.next_layer(), results.begin(), results.end());
        stream.handshake(ssl::stream_base::client);

        http::request<http::string_body> req{http::verb::get, target, 11};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, "l2_recorder");

        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        return res.body();
    }
};
