#pragma once

#include <iostream>
#include <cstdint>
#include <functional>
#include <thread>
#include <condition_variable>
#include <array>
#include <vector>
#include <queue>
#include <exception>
#include <string>

#ifdef _WIN32 || _WIN64
#include <Windows.h>
#include <WinSock2.h>
#endif 

namespace ec4::Http {

	constexpr int NUM_SEND_MUTICES = 128;

	class HttpStreamer
	{
	public:
		HttpStreamer(int port) : mPort(port) {};
		~HttpStreamer() { stop(); }

		void start(size_t numWorkers = 1);
		void stop();
		void streaming(std::string_view path, std::string_view data);

		bool isRunning() {
			std::scoped_lock lock(mPayloadsMutex);
			return mhServSock > 0;
		}
	private:

		std::function<void()> worker();
		std::function<void()> listener();

		struct Payload {
			std::string path;
			std::string buffer;
			SOCKET sock;
		};

		std::thread mThreadListener;
		std::mutex mClientsMutex;
		std::mutex mPayloadsMutex;
		std::array<std::mutex, NUM_SEND_MUTICES> mSendMutices;
		std::condition_variable mCondition;
		std::vector<std::thread> mWorkers;
		std::queue<Payload> mPayloadQueue;
		std::unordered_map<std::string, std::vector<SOCKET>> path2clients;

		static int readBuf(SOCKET& sock, char* buf, size_t size) {
			if (int ret = recv(sock, buf, size, 0); ret >= 0) {
				return ret;
			}
			else {
				throw std::runtime_error("Data Receive Error");
			}
		}
		static int writeBuf(SOCKET& sock, const char* buf, size_t size) {
			if (int ret = send(sock, buf, size, 0); ret >= 0) {
				return ret;
			}
			else {
				throw std::runtime_error("Data Send Error");
			}
		}

		WSADATA mWsaData;
		SOCKET mhServSock;
		SOCKADDR_IN mServAddr;
		std::string mShutdownTarget = "/shutdown";
		int mPort;
	};

}

