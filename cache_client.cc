//Includes and some code: https://www.boost.org/doc/libs/1_72_0/libs/beast/example/http/client/sync/http_client_sync.cpp

#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <iostream>
#include <unordered_map>
#include "cache.hh"

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>


class Cache::Impl {

public:
  std::string host_;
  std::string port_;
  auto HTTPVersion_ = 1.1;
  // The io_context is required for all I/O
  net::io_context ioc_;
  // These objects perform our I/O
  beast::tcp_stream stream_(ioc_);
  tcp::resolver resolver_(ioc_);

  Impl(std::string host, std::string port) {

    host_ = host;
    port_ = port;

    // Look up the domain name
    auto const results = resolver_.resolve(host_, port_);

    // Make the connection on the IP address we get from a lookup
    stream_.connect(results);
  }

  ~Impl(){
    // Gracefully close the socket
      beast::error_code ec;
      stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

      // not_connected happens sometimes
      // so don't bother reporting it.
      if(ec && ec != beast::errc::not_connected)
          throw beast::system_error{ec};

      // If we get here then the connection is closed gracefully
  }

/*
// Check command line arguments.
        if(argc != 4 && argc != 5)
        {
            std::cerr <<
                "Usage: http-client-sync <host> <port> <target> [<HTTP version: 1.0 or 1.1(default)>]\n" <<
                "Example:\n" <<
                "    http-client-sync www.example.com 80 /\n" <<
                "    http-client-sync www.example.com 80 / 1.0\n";
            return EXIT_FAILURE;
        }
        auto const host = argv[1];
        auto const port = argv[2];
        auto const target = argv[3];
        int version = argc == 5 && !std::strcmp("1.0", argv[4]) ? 10 : 11;
*/
  }

  void set(key_type key, val_type val, size_type size) {
    // Set up an HTTP SET request message
    http::request<http::string_body, std::unordered_map> req{http::verb::set, /*(key, value, size)*/, HTTPVersion_};
    req.set(http::field::host, host_);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    http::write(stream_, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(stream_, buffer, res);

    // Write the message to standard out
    std::cout << res << std::endl;
  }

  val_type get(key_type key, size_type& val_size) {
    // Set up an HTTP GET request message
    http::request<http::string_body, std::unordered_map> req{http::verb::get, /*(key, ???)*/, HTTPVersion_};
    req.set(http::field::host, host_);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    http::write(stream_, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(stream_, buffer, res);

    // Write the message to standard out
    std::cout << res << std::endl;
  }

  bool del(key_type key) {
    // Set up an HTTP DELETE request message
    http::request<http::string_body, std::unordered_map> req{http::verb::delete_, /*key*/, HTTPVersion_};
    req.set(http::field::host, host_);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    http::write(stream_, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(stream_, buffer, res);

    // Write the message to standard out
    std::cout << res << std::endl;
  }

  size_type space_used() {
    // Set up an HTTP HEAD request message
    http::request<http::string_body, std::unordered_map> req{http::verb::head, HTTPVersion_};
    req.set(http::field::host, host_);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    http::write(stream_, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(stream_, buffer, res);

    // Write the message to standard out
    std::cout << res << std::endl;
  }

  void reset() {
    // Set up an HTTP POST request message
    http::request<http::string_body, std::unordered_map> req{http::verb::post, "/reset", HTTPVersion_};
    req.set(http::field::host, host_);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    http::write(stream_, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(stream_, buffer, res);

    // Write the message to standard out
    std::cout << res << std::endl;
  }

};

Cache::Cache(size_type maxmem, 
  float max_load_factor, 
  Evictor* evictor, 
  hash_func hasher) { return; }

Cache::Cache(std::string host, std::string port) {
	pImpl_ = (std::make_unique<Impl>(cache_Impl(host, port)));
}

void Cache::set(key_type key, val_type val, size_type size) { pImpl_->set(key, val, size); }
Cache::val_type Cache::get(key_type key, size_type& val_size) const { return pImpl_->get(key, val_size); }
bool Cache::del(key_type key) { return pImpl_->del(key); }
Cache::size_type Cache::space_used() const { return pImpl_->space_used(); }
void Cache::reset() { pImpl_->reset(); }
Cache::~Cache() { pImpl_.reset(); }