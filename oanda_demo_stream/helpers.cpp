#include "session.h"

void fail(error_code ec, std::string what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}