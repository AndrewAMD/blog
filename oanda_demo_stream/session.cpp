#include "session.h"
#include "boost/optional/optional_io.hpp" // TODO: Do I really need this??

session::session(asio::io_context& ioc, ssl::context& ctx)
	:
	resolver_(ioc),
	stream_(ioc, ctx),
	heartbeat_timer_(ioc),
	incomplete_chunk_(""),
	is_stopped_(false)
{
	heartbeat_timer_.expires_at(cro::steady_clock::now() + cro::seconds(TIMEOUT_SECONDS));
}

void session::run(
	std::string host,
	std::string port,
	std::string target,
	int version, 
	std::string token)
{
	if (is_stopped_)
		return;

	// Set SNI Hostname (many hosts need this to handshake successfully)
	if (!SSL_set_tlsext_host_name(stream_.native_handle(), host.c_str()))
	{
		error_code ec{ static_cast<int>(::ERR_get_error()), asio::error::get_ssl_category() };
		std::cerr << ec.message() << "\n";
		return;
	}

	// set up chunk callback, with a "trampoline" per github.com/aerilon/bpistats
	{
		// https://github.com/aerilon/bpistats/blob/2c53509cea3a84d87c0ae97c3622c6175fe5fd8c/src/network_session.cpp#L121
		this->on_chunk_body_trampoline.emplace(
			[self = this->shared_from_this()](auto remain, auto body, auto ec)
		{
			return self->on_chunk_body(remain, body, ec);
		});
		this->par_.on_chunk_body(*this->on_chunk_body_trampoline);
	}

	// Set up timer
	heartbeat_timer_.async_wait([this, self = shared_from_this()]
	(const boost::system::error_code& ec_ignored){
		on_check_heartbeat();
	});

	// Set up an HTTP GET request message
	req_.version(version);
	req_.method(http::verb::get);
	req_.target(target);
	req_.set(http::field::host, host);
	req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
	req_.set(http::field::authorization, ("Bearer " + (std::string)token));
	

	// Look up the domain name
	auto self{ shared_from_this() };
	resolver_.async_resolve(
		host,
		port,
		[this, self](error_code ec, tcp::resolver::results_type results) {
		return on_resolve(ec, results);
	});
}

void session::on_check_heartbeat() {
	if (is_stopped_)
		return;
	
	// if heartbeat timer expired
	if (heartbeat_timer_.expiry() <= cro::steady_clock::now())
	{
		std::cerr << "Error: Heartbeat timer expired!" << std::endl;
		do_shutdown();
	}

	// Put the actor back to sleep.
	heartbeat_timer_.async_wait([this, self = shared_from_this()]
	(const boost::system::error_code& ec_ignored){
		on_check_heartbeat();
	});
}



void session::on_resolve(
	error_code ec,
	tcp::resolver::results_type results)
{
	if (is_stopped_)
		return;
	if (ec) {
		do_shutdown();
		return fail(ec, "resolve");
	}

	auto begin = results.begin();
	auto end = results.end();


	auto self{ shared_from_this() };
	asio::async_connect(
		stream_.lowest_layer(),
		results.begin(),
		results.end(),
		[this, self](error_code ec, tcp::resolver::iterator iterator) {
			on_connect(ec);
	});
}

void session::on_connect(error_code ec)
{
	if (is_stopped_)
		return;
	if (ec) {
		do_shutdown();
		return fail(ec, "connect");
	}

	// Perform the SSL handshake
	auto self{ shared_from_this() };
	stream_.async_handshake(
		ssl::stream_base::client,
		[this, self](error_code ec) {
		on_handshake(ec);
	});

}

void session::on_handshake(error_code ec)
{
	if (is_stopped_)
		return;
	if (ec) {
		do_shutdown();
		return fail(ec, "handshake");
	}

	// Send the HTTP request to the remote host
	auto self{ shared_from_this() };
	http::async_write(stream_, req_,
		[this, self](error_code ec,	std::size_t bytes_transferred) {
		self->on_write(ec, bytes_transferred);
	});
}

void session::on_write(
	error_code ec,
	std::size_t bytes_transferred)
{
	if (is_stopped_)
		return;
	boost::ignore_unused(bytes_transferred);
	if (ec) {
		do_shutdown();
		return fail(ec, "write");
	}

	// Receive the HTTP response
	do_read();
}

void session::do_read() {
	if (is_stopped_)
		return;

	auto self{ shared_from_this() };
	http::async_read(
		stream_,
		buffer_,
		par_,
		[this, self](error_code ec, std::size_t bytes_transferred) {
		on_read_completed(ec, bytes_transferred);
	});

}


std::size_t session::on_chunk_body(
	std::uint64_t remain,
	beast::string_view body,
	error_code& ec) {
	if (is_stopped_)
		return body.length();

	if (ec) {
		do_shutdown();
		fail(ec, "chunk");
	}

	// move timer
	heartbeat_timer_.expires_at(cro::steady_clock::now() + cro::seconds(TIMEOUT_SECONDS));

	try {
		// Oanda puts new lines "}/n" at the END of chunks.
		// Therefore, these strings must end with "}\n"
		beast::string_view chk = body.substr(body.length() - 2, 2),
			end = "}\n";
		if (chk != end)
		{
			incomplete_chunk_ += (std::string)body;
			return body.length();
		}

		// complete
		if (incomplete_chunk_.length()) {
			std::cout << incomplete_chunk_;
			incomplete_chunk_ = ""; // this must be reset when this is spent
		}
		std::cout << body;
		std::cout << "--------------------------------" << std::endl;
		return body.length();
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		exit(9001);
	}

}


void session::on_read_completed(
	error_code ec,
	std::size_t bytes_transferred)
{
	if (is_stopped_)
		return;
	boost::ignore_unused(bytes_transferred);

	if (ec) {
		do_shutdown();
		return fail(ec, "read");
	}

	do_shutdown();
}


void session::do_shutdown() {
	if (is_stopped_)
		return;
	is_stopped_ = true;
	heartbeat_timer_.expires_at(cro::steady_clock::now());

	// Gracefully close the stream
	auto self{ shared_from_this() };
	stream_.async_shutdown(
		[this, self](error_code ec) {
		on_shutdown(ec);
	});
}

void session::on_shutdown(error_code ec)
{
		
	if (ec == asio::error::eof)
	{
		// Rationale:
		// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
		ec.assign(0, ec.category());
	}
	if (ec) {
		return fail(ec, "shutdown");
	}

	// If we get here then the connection is closed gracefully
}




