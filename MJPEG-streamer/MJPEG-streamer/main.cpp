#include "HttpHandler.h"

int main() {
	std::unique_ptr<HttpHandler> httpHandler;

	web::uri_builder uri(U("http://127.0.0.1:12345"));
	auto address = uri.to_uri().to_string();
	httpHandler = std::make_unique<HttpHandler>(address);
	httpHandler->open().wait();

	ucout << utility::string_t(U("Listening for requests at: ")) << address << std::endl;


	std::cout << "Press AnyKey to Exit." << std::endl;
	std::string line;
	std::getline(std::cin, line);

	httpHandler->close().wait();
	return 0;
}