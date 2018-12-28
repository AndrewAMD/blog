#include "session.h"

int main(int argc, char** argv)
{
	// Check command line arguments.
	if (argc != 4)
	{
		std::cerr <<
			"Usage: oanda_demo_stream <demo api token> <demo account number> <comma-separated list of symbols>\n" <<
			"Example:\n" <<
			"    oanda_demo_stream 6e037344eb418ba497d27e74c12751cb-4df27571500e2591b8bd2b714989a185 101-001-1338807-002 EUR_USD,USD_JPY,USD_CAD,AUD_USD \n";
		return EXIT_FAILURE;
	}

	const std::string token = argv[1];
	const std::string account = argv[2];
	const std::string symbols = argv[3];
	const std::string host = "stream-fxpractice.oanda.com";
	const std::string port = "443";
	const std::string target = "/v3/accounts/" + account + "/pricing/stream?instruments=" + symbols;
	
	int version = 11;

	// The io_context is required for all I/O
	asio::io_context ioc;

	// The SSL context is required, and holds certificates
	ssl::context ctx{ ssl::context::tlsv12_client };
	ctx.set_verify_mode(asio::ssl::verify_none);

	
	// Launch the asynchronous operation
	std::make_shared<session>(ioc, ctx)->run(host, port, target, version, token);

	// Run the I/O service. The call will return when
	// the get operation is complete.
	ioc.run();

	std::cerr << "Press Enter to continue..." << std::endl;
	std::cin.get();

	return EXIT_SUCCESS;
}