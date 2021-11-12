#include "Router.h"

Router::Router(utility::string_t url) : mListener(url), mUrl(url)
{
	std::cout << "Router Constructor " << std::endl;
	ucout << url << std::endl;
	mListener.support(methods::GET,  std::bind(&Router::route_get,    this, std::placeholders::_1));
	mListener.support(methods::POST, std::bind(&Router::route_post,   this, std::placeholders::_1));
	mListener.support(methods::PUT,  std::bind(&Router::route_put,    this, std::placeholders::_1));
	mListener.support(methods::DEL,  std::bind(&Router::route_delete, this, std::placeholders::_1));
}

void Router::addRoute(std::string_view uri, std::function<void(http_request&)> func, std::initializer_list<std::string> methods)
{
	for (auto method : methods) {
		//to lowercase
		std::transform(std::begin(method), std::end(method), std::begin(method),
			[](unsigned char c) {return std::tolower(c); });
		try {
			if (method == "get")	addGetRoute(uri, func);
			if (method == "post")	addPostRoute(uri, func);
			if (method == "put")	addPutRoute(uri, func);
			if (method == "delete") addDeleteRoute(uri, func);
		}
		catch (const std::exception& e) {
			std::cerr << "Add Route Error => " << method << " " << e.what() << std::endl;
			return;
		}
	}
}

std::map<std::string, std::vector<std::string>> Router::getRouteInfo() {
	auto to_list = [](auto mapper) {
		std::vector<std::string> vec;
		for (auto& iter : mapper) { vec.push_back(iter.first); }
		return vec;
	};
	std::map<std::string, std::vector<std::string>> ret;
	ret.insert({ "get",		to_list(mMapper_GET) });
	ret.insert({ "post",	to_list(mMapper_POST) });
	ret.insert({ "put",		to_list(mMapper_PUT) });
	ret.insert({ "delete",	to_list(mMapper_DEL) });
	return ret;
}

void Router::route_get(http_request req)
{

	auto paths = http::uri::split_path(http::uri::decode(req.relative_uri().path()));

	utility::string_t Lpath = req.relative_uri().path();
	std::string path{ std::begin(Lpath), std::end(Lpath) };

#if _DEBUG
	std::cout << path << std::endl;
	ucout << req.to_string() << std::endl;
#endif 

	if (auto iter = mMapper_GET.find(path); iter != std::cend(mMapper_GET)) {
		auto func = iter->second;
		func(req);
	}
	else { //Cannot Find
		req.reply(status_codes::OK, U("There is No Implemented API"));
	}
	return;
}

void Router::route_post(http_request req)
{

	auto paths = http::uri::split_path(http::uri::decode(req.relative_uri().path()));

	utility::string_t Lpath = req.relative_uri().path();
	std::string path{ std::begin(Lpath), std::end(Lpath) };

#if _DEBUG
	std::cout << path << std::endl;
	ucout << req.to_string() << std::endl;
#endif 

	if (auto iter = mMapper_POST.find(path); iter != std::cend(mMapper_POST)) {
		auto func = iter->second;
		func(req);
	}
	else { //Cannot Find
		req.reply(status_codes::OK, U("There is No Implemented API"));
	}
	return;
}

void Router::route_put(http_request req)
{
}

void Router::route_delete(http_request req)
{
}

void Router::route_error(pplx::task<void>& t)
{
	try {
		t.get();
	} 
	catch (const std::exception& e) {

	}
}
