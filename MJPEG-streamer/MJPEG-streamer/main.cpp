#include "RestAPIServer.h"
#include "HttpStreamer.h"
#include <opencv2/opencv.hpp>

void view(http_request& req) {
	concurrency::streams::fstream::open_istream(U("html/viewer.html"), std::ios::in)
	.then([=](concurrency::streams::istream is) {
		req.reply(status_codes::OK, is, U("text/html"))
		.then([](pplx::task<void> t) {
			try {
				t.get();
			}
			catch (...) {}
		});
	})
	.then([=](pplx::task<void>t) {
		try {
			t.get();
		}
		catch (...) {
			req.reply(status_codes::InternalError, U("INTERNAL ERROR "));
		}
	});
	return;
}


int main() {

	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		std::cerr << "VideoCapture not opened" << std::endl;
		exit(1);
	}

	std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, 90 };

	auto streamer = std::make_unique<HttpStreamer>(8080);
	streamer->start(64);

	auto server = std::make_unique<RestAPIServer>(U("http://127.0.0.1:12345"));

	server->route("/view", view, { "get", "post" });
	server->run();

	while (streamer->isRunning()) {
		cv::Mat frame;
		cap >> frame;
		if (frame.empty()) {
			std::cerr << "frame not grabbed" << std::endl;
			exit(1);
		}
		cv::imshow("cap", frame);
		if (cv::waitKey(1) == 27) break;
		std::vector<uchar> buff_bgr;
		cv::imencode(".jpg", frame, buff_bgr, params);
		streamer->streaming("/stream", std::string(std::begin(buff_bgr), std::end(buff_bgr)));
	}

	streamer->stop();
	server->shutdown();
	return 0;
}