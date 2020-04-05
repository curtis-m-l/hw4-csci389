//Includes and some code: https://www.boost.org/doc/libs/1_72_0/libs/beast/example/http/client/sync/http_client_sync.cpp

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <iostream>
#include "cache.hh"

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

class Cache::Impl {

public:
    std::string host_;
    std::string port_;
    double HTTPVersion_ = 1.1;
    beast::tcp_stream* stream_ ;
    auto const results_;

    Impl(std::string host, std::string port){
        std::cout << "Impl constructor start\n";
        host_ = host;
        port_ = port;
        std::cout << "Impl constructor end\n";
    }

    ~Impl() {
        // Gracefully close the socket
        
        beast::error_code ec;
        stream_->socket().shutdown(tcp::socket::shutdown_both, ec);
        
        // not_connected happens sometimes
        // so don't bother reporting it.
        /*
        if (ec && ec != beast::errc::not_connected) {
            throw beast::system_error{ ec };
        }
        */
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

    void set(key_type key, val_type val, size_type size) {
        // Set up an HTTP SET request message
        stream_->connect(results_);
        std::string requestBody = "/" + key + "/" + val + "/" + std::to_string(size);
        http::request<http::string_body> req{ http::verb::put, requestBody, HTTPVersion_ };
        req.set(http::field::host, host_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(*stream_, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::string_body> res;

        // Receive the HTTP response
        http::read(*stream_, buffer, res);

        // Write the message to standard out
        std::cout << res << std::endl;
    }

    val_type get(key_type key, size_type& val_size) {
        // Set up an HTTP GET request message
        stream_->connect(results_);
        std::string requestBody = "/" + key;
        http::request<http::string_body> req{ http::verb::get, requestBody, HTTPVersion_ };
        req.set(http::field::host, host_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(*stream_, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::string_body> res;

        // Receive the HTTP response
        http::read(*stream_, buffer, res);

        // Write the message to standard out
        std::cout << res << std::endl;
        //https://github.com/boostorg/beast/issues/819
        std::vector<std::string> splitBody;
        boost::split(splitBody, res.body(), boost::is_any_of("/"));
        val_type answer = splitBody[1].c_str();
        std::stringstream ss(splitBody[2]);
        ss >> val_size;
        return answer;
    }

    bool del(key_type key) {
        // Set up an HTTP DELETE request message
        stream_->connect(results_);
        std::string requestBody = "/" + key;
        http::request<http::string_body> req{ http::verb::delete_, requestBody, HTTPVersion_ };
        req.set(http::field::host, host_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(*stream_, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::string_body> res;

        // Receive the HTTP response
        http::read(*stream_, buffer, res);

        // Write the message to standard out
        std::cout << res << std::endl;
        std::string result = res.body();
        bool answer = true;
        if (result == "False") {
            answer = false;
        }
        return answer;
    }

    size_type space_used() {
        // Set up an HTTP HEAD request message
        stream_->connect(results_);
        std::cout << "Setting up HEAD (space used) request...\n";
        http::request<http::empty_body> req;
        req.version(HTTPVersion_);
        req.method(http::verb::head);
        req.set(http::field::host, host_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        std::cout << "Writing request to the stream...\n";
        // Send the HTTP request to the remote host
        http::write(*stream_, req); 

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::string_body> res;
        std::cout << "Reading response from the stream...\n";
        // Receive the HTTP response
        http::read(*stream_, buffer, res); // PROBLEM SPOT

        std::cout << "Sending back results...\n";
        // Write the message to standard out
        std::cout << res << std::endl;
        //https://github.com/boostorg/beast/issues/819
        std::string result = res.body();
        Cache::size_type spaceInUse;
        std::stringstream ss(result);
        ss >> spaceInUse;
        return spaceInUse;
    }

    void reset() {
        // Set up an HTTP POST request message
        stream_->connect(results_);
        http::request<http::string_body> req{ http::verb::post, "/reset", HTTPVersion_ };
        req.set(http::field::host, host_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(*stream_, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(*stream_, buffer, res);

        // Write the message to standard out
        std::cout << res << std::endl;
    }

};

/*
    Cache::Cache(size_type,
    float,
    Evictor*,
    hash_func) { return; }
*/

Cache::Cache(std::string host, std::string port) {

    Impl cache_Impl(host, port);
    pImpl_ = (std::make_unique<Impl>(cache_Impl));

    net::io_context ioc;
    tcp::resolver resolver(ioc);
    pImpl_->stream_ = new beast::tcp_stream(ioc);

    // Look up the domain name
    pImpl->results_ = resolver.resolve(host, port);
    std::cout << "Cache constructed\n";
}

void Cache::set(key_type key, val_type val, size_type size) { pImpl_->set(key, val, size); }
Cache::val_type Cache::get(key_type key, size_type& val_size) const { return pImpl_->get(key, val_size); }
bool Cache::del(key_type key) { return pImpl_->del(key); }
Cache::size_type Cache::space_used() const {
    std::cout << "Making call to cache call to Impl.space_used()...\n";
    return pImpl_->space_used(); 
}
void Cache::reset() { pImpl_->reset(); }
Cache::~Cache() { pImpl_->reset(); }