
#include <functional>
#include <thread>

#include <coio/coio.h>
#include "codec_http.h"
#include <ev++.h>
#include <chrono>
#include <vector>

using namespace v;
using namespace v::io;
using namespace v::co;


void HttpClient(std::string ip, uint16_t port, int conn_num, int req_num) {
	HttpRequest req;
	req.Write(HTTP_GET, "/test");
	req.SetHeader("Connection", "Keep-Alive");

	int n_req = req_num;
	int n_conn = conn_num;
	for (int c = 0; c < conn_num; c++) {
		RunCoroutine([&]() {
			Connection<CodecHttpClient, Tcp> conn;
			bool ret = conn.Connect(NetAddr{ ip, port });
			if (ret) {
				conn.Write(req);
				conn.ReadUntilClose([&conn, &req, &n_req, &n_conn](HttpResponse const& resp) {
					conn.Write(req);
					if (--n_req <= 0) {
						conn.Close();
						if (--n_conn <= 0) {
							ev::loop_ref loop = this_io::Get()->GetRawLoop();
							loop.unloop();
						}
					}	
				});
			} else {
				LogError("Connect failed");
				conn.Close();
				if (--n_conn <= 0) {
					ev::loop_ref loop = this_io::Get()->GetRawLoop();
					loop.unloop();
				}
			}
		});
	}

	this_io::Get()->Run();
	LogInfo("thread end..... failed, finish", req_num - n_req);
}


int main(int argc, char *argv[]) {
#ifdef _WIN32
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
#endif

	Logger::Instance().SetLogLevel(LOG_LEVEL::LINFO);

	int thread_num = 1;
	int conn_num = 10;
	int request_num = 100000;
	std::string ip = "127.0.0.1";
	uint16_t port = 8383;

	if (argc > 2) thread_num = atoi(argv[1]);
	if (argc > 3) conn_num = atoi(argv[2]);
	if (argc > 4) request_num = atoi(argv[3]);
	if (argc > 5) ip = std::string(argv[4]);
	if (argc > 6) port = (uint16_t)atoi(argv[5]);

	LogInfo(ip, ":", port, " thread ", thread_num, "conn ", conn_num," request ", request_num);
	std::vector<std::thread> t(thread_num);

	std::chrono::high_resolution_clock::time_point t_begin = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::system_clock::now();
	for (int i = 0; i < thread_num; i++) {
		t[i] = std::thread(std::bind(&HttpClient, ip, port, conn_num, request_num));
	}
	for (int i = 0; i < thread_num; i++) {
		t[i].join();
	}
	std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_begin).count();

	LogInfo("Total run ", dur /1000.0, "s. request", request_num * thread_num);
	LogInfo("qps: ", request_num * thread_num * 1.0 / dur * 1000.0);

	getchar();

	return 0;
}