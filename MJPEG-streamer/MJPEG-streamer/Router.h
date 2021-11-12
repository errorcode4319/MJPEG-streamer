#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <initializer_list>
#include <map>
#include <exception>
#include <algorithm>
#include <cctype>

#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>
#include <cpprest/json.h>
#include <cpprest/filestream.h>
#include <cpprest/containerstream.h>
#include <cpprest/producerconsumerstream.h>

using namespace web;
using namespace http;
using namespace http::experimental::listener;



class Router
{
public:
	explicit Router(utility::string_t url);
	virtual ~Router() = default;

protected:
	void addRoute(std::string_view uri, std::function<void(http_request&)> func, std::initializer_list<std::string> methods);

	std::map<std::string, std::vector<std::string>> getRouteInfo();

	pplx::task<void>open() { return mListener.open(); }
	pplx::task<void>close() { return mListener.close(); }

	std::string getUrl() {
		std::string url;
		url.assign(std::begin(mUrl), std::end(mUrl)); // Convert Unicode to multibyte
		return url;
	}

private:
	void route_get		(http_request req);
	void route_post		(http_request req);
	void route_put		(http_request req);
	void route_delete	(http_request req);
	void route_error	(pplx::task<void>& t);

	void addGetRoute(std::string_view uri, std::function<void(http_request&)> func) {
		if (auto iter = mMapper_GET.find(uri.data()); iter == std::end(mMapper_GET)) {
			mMapper_GET.insert({ uri.data(), func });
		} else { throw std::exception(uri.data());}
	}
	void addPostRoute(std::string_view uri, std::function<void(http_request&)> func) {
		if (auto iter = mMapper_POST.find(uri.data()); iter == std::end(mMapper_POST)) {
			mMapper_POST.insert({ uri.data(), func });
		} else {throw std::exception(uri.data());}
	}
	void addPutRoute(std::string_view uri, std::function<void(http_request&)> func) {
		if (auto iter = mMapper_PUT.find(uri.data()); iter == std::end(mMapper_PUT)) {
			mMapper_PUT.insert({ uri.data(), func });
		} else { throw std::exception(uri.data());}
	}
	void addDeleteRoute(std::string_view uri, std::function<void(http_request&)> func) {
		if (auto iter = mMapper_DEL.find(uri.data()); iter == std::end(mMapper_DEL)) {
			mMapper_DEL.insert({ uri.data(), func });
		} else { throw std::exception(uri.data());}
	}
	std::map<std::string, std::function<void(http_request&)>> mMapper_GET;
	std::map<std::string, std::function<void(http_request&)>> mMapper_POST;
	std::map<std::string, std::function<void(http_request&)>> mMapper_PUT;
	std::map<std::string, std::function<void(http_request&)>> mMapper_DEL;

	utility::string_t   mUrl;
	http_listener		mListener;
};

