#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define _WIN32_WINNT 0x0600 // Windows Vista or newer
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
#endif

#define TIMEOUT_SECONDS 20

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <optional>

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using error_code = boost::system::error_code;
namespace ssl = asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = beast::http;    // from <boost/beast/http.hpp>
namespace cro = std::chrono;
								 
//------------------------------------------------------------------------------

// Report a failure
void fail(error_code ec, std::string what);
								 
//------------------------------------------------------------------------------

// Performs an HTTP GET and prints the response
class session : public std::enable_shared_from_this<session>
{
	tcp::resolver resolver_;
	ssl::stream<tcp::socket> stream_;
	beast::flat_buffer buffer_; // (Must persist between reads)
	http::request<http::empty_body> req_;
	//http::response<http::string_body> res_;
	http::response_parser<http::empty_body> par_;
	//http::response_parser<http::string_body> par_; // TODO: how to keep buffer empty??
	asio::steady_timer heartbeat_timer_;
	std::string incomplete_chunk_;
	bool is_stopped_;

public:
	// Resolver and stream require an io_context
	explicit session(asio::io_context& ioc, ssl::context& ctx);

	// Start the asynchronous operation
	void run(std::string host, std::string port, std::string target, int version, std::string token);

	void on_check_heartbeat();

	void on_resolve(error_code ec, tcp::resolver::results_type results);

	void on_connect(error_code ec);

	void on_handshake(error_code ec);

	void on_write(error_code ec, std::size_t bytes_transferred);

	void do_read();
	
	std::size_t on_chunk_body(
		std::uint64_t remain,       // Octets remaining in this chunk, includes `body`
		beast::string_view body,    // A buffer holding some or all of the remainder of the chunk body
		error_code& ec);            // May be set by the callback to indicate an error
	
	// https://github.com/aerilon/bpistats/blob/2c53509cea3a84d87c0ae97c3622c6175fe5fd8c/src/network_session.hpp#L73
	// XXX al -
	// boost::beast::http::response_parser::on_chunk_cb takes a *reference*
	// to a callback, so we have to manage its lifetime manually. This is a
	// *major* pain :-/
	std::optional<std::function<size_t(uint64_t, beast::string_view, boost::system::error_code&)>> on_chunk_body_trampoline;

	void on_read_completed(error_code ec, std::size_t bytes_transferred);
	void do_shutdown();
	void on_shutdown(error_code ec);

};