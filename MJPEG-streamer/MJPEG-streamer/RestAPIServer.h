#pragma once
#include "Router.h"

class RestAPIServer : public Router
{
public:
	explicit RestAPIServer(utility::string_t url);
	~RestAPIServer();

	void route(std::string_view uri, std::function<void(http_request&)> func, std::initializer_list<std::string> methods = { "get" });

	void run();

	void shutdown();

	void showInfo();

	bool isRunning() const { return mIsRunning; }

private:
	bool mIsRunning = false;
};

