#include "RestAPIServer.h"


RestAPIServer::RestAPIServer(utility::string_t url) : Router(url)
{
}

RestAPIServer::~RestAPIServer()
{
}

void RestAPIServer::route(std::string_view url, std::function<void(http_request&)> func, std::initializer_list<std::string> methods) {
	addRoute(url, func, methods);
}

void RestAPIServer::run() {
	if (mIsRunning) {
		std::cout << "Already Running" << std::endl;
		return;
	}
	open().wait();
	mIsRunning = true;
}

void RestAPIServer::shutdown() {
	close().wait();
	mIsRunning = false;
}


void RestAPIServer::showInfo() {
	auto routes = getRouteInfo();
	for (auto method : {"get", "post", "put", "delete"}) {
		auto funclist = routes[method];
		std::cout << method << std::endl;
		for (auto& func : funclist) {
			std::cout << "\t" << func << std::endl;
		}
	}
}
