
#include <functional>
#include <thread>

#include <coio/coio.h>
#include "codec_http.h"
#include <ev++.h>
#include <chrono>
#include <vector>
#include <atomic>

using namespace v;
using namespace v::io;
using namespace v::co;


static int g_thread_num = 2;
static int g_conn_num = 10;
static int g_req_num = 1000000;
static std::string g_ip = "127.0.0.1";
static uint16_t g_port = 8383;

static std::atomic<int> g_remain_req = 0;
std::chrono::high_resolution_clock::time_point g_begin, g_end;

void Report() {
	static bool reported = false;
	if (reported)
		return;
	reported = true;

	g_end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(g_end - g_begin).count();

	LogInfo("Total run ", dur / 1000.0, "s. request", g_req_num);
	LogInfo("qps: ", g_req_num * 1.0 / dur * 1000.0);
}

void HttpClient() {
	HttpRequest req;
	req.Write(HTTP_GET, "/test");
	req.SetHeader("Connection", "Keep-Alive");

	for (int c = 0; c < g_conn_num; c++) {
		RunCoroutine([&]() {
			Connection<CodecHttpClient, Tcp> conn;
			if (false == conn.Connect(NetAddr{ g_ip, g_port })) {
				LogError("Connect failed");
				ev::loop_ref loop = this_io::Get()->GetRawLoop();
				loop.unloop();
				return;
			}

			conn.Write(req);
			conn.ReadUntilClose([&](HttpResponse const& resp) {
				--g_remain_req;
				if (g_remain_req <= 0) {
					Report();
					ev::loop_ref loop = this_io::Get()->GetRawLoop();
					loop.unloop();
					return;
				}
				conn.Write(req);
					
			});
			
		});
	}

	this_io::Get()->Run();
	// LogInfo("thread end..... ");
}


int main(int argc, char *argv[]) {
#ifdef _WIN32
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
#endif

	Logger::Instance().SetLogLevel(LOG_LEVEL::LINFO);

	if (argc > 1) g_thread_num = atoi(argv[1]);
	if (argc > 2) g_conn_num = atoi(argv[2]);
	if (argc > 3) g_req_num = atoi(argv[3]);
	if (argc > 4) g_ip = std::string(argv[4]);
	if (argc > 5) g_port = (uint16_t)atoi(argv[5]);

	g_remain_req = g_req_num;

	LogInfo(g_ip, ":", g_port, " thread ", g_thread_num, ", conn ", g_conn_num,", request ", g_req_num);
	std::vector<std::thread> t(g_thread_num);

	g_begin = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::system_clock::now();
	for (int i = 0; i < g_thread_num; i++) {
		t[i] = std::thread(HttpClient);
	}
	for (int i = 0; i < g_thread_num; i++) {
		t[i].join();
	}
	

	getchar();

	return 0;
}