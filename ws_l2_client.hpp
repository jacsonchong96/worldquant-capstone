#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include "recorder.hpp"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net   = boost::asio;
namespace ssl   = net::ssl;
using tcp = net::ip::tcp;
using json = nlohmann::json;

class WsL2Client {
public:
  WsL2Client(int idx, std::vector<std::string> product_ids,
             const std::string& data_dir, const std::string& filename)
  : idx_(idx), product_ids_(std::move(product_ids)), rec_(data_dir, filename) {}

  void run() {
    const std::string host = "advanced-trade-ws.coinbase.com";
    const std::string port = "443";

    net::io_context ioc;
    ssl::context ctx{ssl::context::tls_client};
    ctx.set_verify_mode(ssl::verify_none);

    tcp::resolver resolver{ioc};
    websocket::stream<beast::ssl_stream<tcp::socket>> ws{ioc, ctx};

    auto const results = resolver.resolve(host, port);
    net::connect(ws.next_layer().next_layer(), results.begin(), results.end());

    if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host.c_str())) {
      beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
      throw beast::system_error{ec, "SNI failed (ws)"};
    }

    ws.next_layer().handshake(ssl::stream_base::client);
    ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
    ws.handshake(host, "/");

    json sub;
    sub["type"] = "subscribe";
    sub["channel"] = "level2";
    sub["product_ids"] = product_ids_;

    ws.write(net::buffer(sub.dump()));

    beast::flat_buffer buffer;

    for (;;) {
      ws.read(buffer);
      auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
      long long ts_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();

      std::string raw = beast::buffers_to_string(buffer.data());
      rec_.append(raw, ts_ns, idx_);

      buffer.consume(buffer.size());
    }
  }

private:
  int idx_;
  std::vector<std::string> product_ids_;
  Recorder rec_;
};


