#pragma once

#include<iostream>
#include<string>
#include<cstdint>

#ifdef _WIN32 || _WIN64
#define NOMINMAX 
#include <Windows.h>
#endif 
#include<locale>

#include<cpprest/http_listener.h>
#include<cpprest/uri.h>
#include<cpprest/asyncrt_utils.h>
#include<cpprest/filestream.h>

using namespace web::http::experimental::listener;

class HttpHandler
{
public:
	explicit HttpHandler(utility::string_t url);
	virtual ~HttpHandler();

	pplx::task<void>open() { return mListener.open(); }
	pplx::task<void>close() { return mListener.close(); }

protected:
	/*
		Add Custom Events 
	*/

private:
	virtual void handle_GET		(web::http::http_request message);
	virtual void handle_POST	(web::http::http_request message);
	virtual void handle_PUT		(web::http::http_request message);
	virtual void handle_DEL		(web::http::http_request message);
	virtual void handle_ERROR	(pplx::task<void>& t);

	// web::http::experimental::listener::
	http_listener mListener;
};

