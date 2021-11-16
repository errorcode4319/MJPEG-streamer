#include "HttpStreamer.h"
#include "Message.h"


void HttpStreamer::start(size_t numWorkers) {

    WSAStartup(MAKEWORD(2, 2), &mWsaData);
    mhServSock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&mServAddr, 0, sizeof(mServAddr));
    mServAddr.sin_family = AF_INET;
    mServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    mServAddr.sin_port = htons(mPort);

    bind(mhServSock, (SOCKADDR*)&mServAddr, sizeof(mServAddr));

    listen(mhServSock, 5);

    while (numWorkers--) {
        mWorkers.emplace_back(worker());
    }

    mThreadListener = std::thread(listener());
}

void HttpStreamer::stop() {
    if (isRunning()) {
        std::unique_lock lock(mPayloadsMutex);
        closesocket(mhServSock);
        mCondition.notify_all();
    }

    for (auto& w : mWorkers) {
        if (w.joinable()) w.join();
    }
    mWorkers.clear();

    for (auto& p2c : path2clients) {
        for (auto sock : p2c.second) {
            closesocket(sock);
        }
    }
    path2clients.clear();
    if (mThreadListener.joinable()) {
        mThreadListener.join();
    }
    WSACleanup();
}

void HttpStreamer::streaming(std::string_view path, std::string_view data) {
    std::vector<SOCKET> clients;
    {
        std::unique_lock lock(mClientsMutex);
        auto iter = path2clients.find(path.data());
        if (iter == std::end(path2clients)) {
            return;
        }
        clients = iter->second;
    }

    for (auto& sock : clients) {
        std::unique_lock lock(mPayloadsMutex);
        mPayloadQueue.emplace(Payload{ std::string(path), std::string(data), sock });
        mCondition.notify_one();
    }
}

std::function<void()> HttpStreamer::worker() {
    return [&]() {
        while (isRunning()) {
            Payload payload;
            {
                std::unique_lock lock(mPayloadsMutex);
                mCondition.wait(lock, [this]() {
                    return mhServSock < 0 || !mPayloadQueue.empty();
                    });
                if ((mhServSock < 0) && (mPayloadQueue.empty())) {
                    return;
                }
                payload = std::move(mPayloadQueue.front());
                mPayloadQueue.pop();
            }

            Message res;
            res.setStartLine("--boundarydonotcross");
            res.setHeader("Content-Type", "image/jpeg");
            res.setHeader("Content-Length", std::to_string(payload.buffer.size()));
            res.setBody(payload.buffer);

            auto res_str = res.to_string();
                
            int n;
            {
                std::unique_lock lock(mSendMutices.at(payload.sock % NUM_SEND_MUTICES));
                n = writeBuf(payload.sock, res_str.c_str(), res_str.size());
            }

            if (n < static_cast<int>(res_str.size())) {
                std::unique_lock lock(mClientsMutex);
                auto& p2c = path2clients[payload.path];
                if (std::find(std::begin(p2c), std::end(p2c), payload.sock) != std::end(p2c)) {
                    p2c.erase(std::remove(std::begin(p2c), std::end(p2c), payload.sock), p2c.end());
                    shutdown(payload.sock, 2);
                }
            }
        }
    };
}

std::function<void()> HttpStreamer::listener() {
    return [&]() {
        Message bad_request_res;
        bad_request_res.setStartLine("HTTP/1.1 400 Bad Request");

        Message shutdown_res;
        shutdown_res.setStartLine("HTTP/1.1 200 OK");

        Message method_not_allowed_res;
        method_not_allowed_res.setStartLine("HTTP/1.1 405 Method Not Allowed");

        Message init_res;
        init_res.setStartLine("HTTP/1.1 200 OK");
        init_res.setHeader("Connection", "close");
        init_res.setHeader("Cache-Control", "no-cache, no-store, must-revalidate, pre-check=0, post-check=0, max-age=0");
        init_res.setHeader("Pragma", "no-cache");
        init_res.setHeader("Content-Type", "multipart/x-mixed-replace; boundary=boundarydonotcross");

        auto bad_request_res_str = bad_request_res.to_string();
        auto shutdown_res_str = shutdown_res.to_string();
        auto method_not_allowed_res_str = method_not_allowed_res.to_string();
        auto init_res_str = init_res.to_string();

        int addrlen = sizeof(mServAddr);

        fd_set fd;
        FD_ZERO(&fd);

        auto master_socket = mhServSock;

        while (isRunning()) {
            struct timeval to;
            to.tv_sec = 1;
            to.tv_usec = 0;

            FD_SET(mhServSock, &fd);

            if (select(mhServSock + 1, &fd, nullptr, nullptr, &to) > 0) {
                SOCKET new_socket = accept(mhServSock, (SOCKADDR*)&mServAddr, &addrlen);
                if (new_socket < 0) throw std::runtime_error("ERROR: accept \n");

                std::string buff(4094, 0);
                readBuf(new_socket, &buff[0], buff.size());

                Message req(buff);
                if (req.target() == mShutdownTarget) {
                    writeBuf(new_socket, shutdown_res_str.c_str(), shutdown_res_str.size());
                    closesocket(new_socket);

                    std::unique_lock lock(mPayloadsMutex);
                    closesocket(mhServSock);
                    mCondition.notify_all();
                    continue;
                }

                if (req.method() != "GET") {
                    writeBuf(new_socket, method_not_allowed_res_str.c_str(), method_not_allowed_res_str.size());
                    closesocket(new_socket);
                    continue;
                }

                writeBuf(new_socket, init_res_str.c_str(), init_res_str.size());

                std::unique_lock lock(mClientsMutex);
                path2clients[req.target()].push_back(new_socket);
            }
        }

        shutdown(master_socket, 2);
    };
}

