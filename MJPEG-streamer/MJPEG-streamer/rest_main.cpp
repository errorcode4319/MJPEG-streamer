#include "RestAPIServer.h"


void view(http_request& req) {
	concurrency::streams::fstream::open_istream(U("html/viewer.html"), std::ios::in)
	.then([=](concurrency::streams::istream is) {
		req.reply(status_codes::OK, is, U("text/html"))
		.then([](pplx::task<void> t) {
			try {
				t.get();
			}
			catch (...) {}
		});
	})
	.then([=](pplx::task<void>t) {
		try {
			t.get();
		}
		catch (...) {
			req.reply(status_codes::InternalError, U("INTERNAL ERROR "));
		}
	});
	return;
}


int main() {
	auto server = std::make_unique<RestAPIServer>(U("http://127.0.0.1:12345"));

	server->route("/view", view, { "get", "post" });

	server->run();

	std::cout << "Press ENTER to quit" << std::endl;
	std::string line;
	std::getline(std::cin, line);

	server->shutdown();
	return 0;
}