#include "Message.h"

namespace ec4::Http {

	const std::string kHeaderDelimiter = "\r\n";
	const std::string kBodyDelimiter = "\r\n\r\n";

	Message::Message(std::string_view msg) {
		parse(msg);
	}

	void Message::parse(std::string_view msg) {

#if _DEBUG
		std::cout << __func__ << ":" << msg << std::endl;
#endif 

		mStartLine = msg.substr(0, msg.find(kHeaderDelimiter));
		auto target = [&]() -> std::string {
			std::string ret(mStartLine.c_str() + mStartLine.find(' ') + 1);
			ret = ret.substr(0, ret.find(' '));
			return std::string(ret);
		}();
		auto method = mStartLine.substr(0, mStartLine.find(' '));

#if _DEBUG 
		std::cout << __func__ << ":" << "StartLine => " << mStartLine << std::endl;
		std::cout << __func__ << ":" << "Target => " << target << std::endl;
		std::cout << __func__ << ":" << "Method => " << method << std::endl;
#endif 

		auto rawHeader = msg.substr(
			msg.find(kHeaderDelimiter) + kHeaderDelimiter.size(),
			msg.find(kBodyDelimiter) - msg.find(kHeaderDelimiter)
		);

		while (rawHeader.find(kHeaderDelimiter) != std::string::npos) {
			auto header_deli_iter = rawHeader.find(kHeaderDelimiter);
			auto key_value_deli_iter = rawHeader.find(':');

			auto header = rawHeader.substr(0, header_deli_iter);
			auto key = header.substr(0, key_value_deli_iter);
			auto value = header.substr(key_value_deli_iter + 1, header_deli_iter);
			while (value[0] == ' ') {
				value = value.substr(1);
			}
			mHeader[std::string(key)] = std::string(value);
			rawHeader = rawHeader.substr(header_deli_iter + kHeaderDelimiter.size());
		}
		mBody = msg.substr(msg.find(kBodyDelimiter) + kBodyDelimiter.size());

	}

	std::string Message::to_string() const {
		std::stringstream stream;

		stream << mStartLine << kHeaderDelimiter;
		for (const auto& header : mHeader) {
			stream << header.first << ": " << header.second << kHeaderDelimiter;
		}
		stream << kHeaderDelimiter << mBody;

		return stream.str();
	}
}