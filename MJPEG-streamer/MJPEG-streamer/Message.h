#pragma once
#include <iostream>
#include <map>
#include <functional>
#include <unordered_map>
#include <sstream>

/*
I referenced by this Link. => https://github.com/nadjieb/cpp-mjpeg-streamer
*/


class Message
{
public:
	Message() = default;
	explicit Message(std::string_view msg);

	void parse(std::string_view msg);

	std::string to_string() const;

	void setStartLine(std::string_view startLine) { mStartLine = startLine; }
	std::string_view getStartLine() const { return mStartLine; }
		
	void setHeader(std::string_view key, std::string_view value) {
		if (auto iter = mHeader.find(key.data()); iter == std::end(mHeader)) {
			mHeader[std::string(key)] = std::string(value);
		}
	}
	const std::map<std::string, std::string>& getHeader() const { return mHeader; }

	void setBody(std::string_view body) { mBody = body; }
	std::string_view getBody() const { return mBody; }

	std::string target() const {
		std::string result(mStartLine.c_str() + mStartLine.find(' ') + 1);
		result = result.substr(0, result.find(' '));
		return std::string(result);
	}
	std::string method() const { return mStartLine.substr(0, mStartLine.find(' ')); }

private:
	std::string mMethod;
	/* map or hash(unordered) map ?? */
	std::map<std::string, std::string> mHeader;
	// std::unordered<std::string, std::string> mHeader;
	std::string mStartLine;
	std::string mBody;
};

