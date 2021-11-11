#include "HttpHandler.h"

using namespace web;
using namespace http;

HttpHandler::HttpHandler(utility::string_t url): mListener(url)
{
	mListener.support(methods::GET,  std::bind(&HttpHandler::handle_GET,	this, std::placeholders::_1));
	mListener.support(methods::POST, std::bind(&HttpHandler::handle_POST,	this, std::placeholders::_1));
	mListener.support(methods::PUT,  std::bind(&HttpHandler::handle_PUT,	this, std::placeholders::_1));
	mListener.support(methods::DEL,  std::bind(&HttpHandler::handle_DEL,	this, std::placeholders::_1));
}

HttpHandler::~HttpHandler()
{
}

void HttpHandler::handle_GET(web::http::http_request message)
{
	ucout << message.to_string() << std::endl;
	auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
	auto path = message.relative_uri().path();

	concurrency::streams::fstream::open_istream(U("html/viewer.html"), std::ios::in)
	.then([=](concurrency::streams::istream is) {
		message.reply(status_codes::OK, is, U("text/html"))
			.then([](pplx::task<void> t) {
					try {
						t.get();
					}
					catch (...) {

					}
				});
	})
	.then([=](pplx::task<void>t) {
		try {
			t.get();
		}
		catch (...) {
			message.reply(status_codes::InternalError, U("INTERNAL ERROR "));
		}
	});

	return;
}

void HttpHandler::handle_POST(web::http::http_request message)
{
	ucout << message.to_string() << std::endl;
	message.reply(status_codes::OK, message.to_string());
	return;
}

void HttpHandler::handle_PUT(web::http::http_request message)
{
	ucout << message.to_string() << std::endl;
	message.reply(status_codes::OK, "Put");
	return;
}

void HttpHandler::handle_DEL(web::http::http_request message)
{
	ucout << message.to_string() << std::endl;
	message.reply(status_codes::OK, "Delete");
	return;
}

void HttpHandler::handle_ERROR(pplx::task<void>& t)
{
	try {
		t.get();
	}
	catch (...) {

	}
}
