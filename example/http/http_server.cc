
#include <functional>
#include <thread>

#include <coio/coio.h>
#include "codec_http.h"

using namespace v;
using namespace v::io;
using namespace v::co;

void HttpServer() {
	HttpService serv;
	serv.Register("/test", [](HttpRequest const& req, HttpResponse& resp) {
		resp.Write(HTTP_STATUS_OK);
		resp.Write("yeah!");
	});

	RunCoroutine([&serv]() {
		Accepter<Tcp> ac;
		ac.Listen(NetAddr{ "0.0.0.0", 8383 });
		while (true) {
			auto sock = ac.Accept();
			// create client
			RunCoroutine([sock, &serv]() {
				Connection<CodecHttpServer, Tcp> conn(sock);
				conn.ReadUntilClose([&conn, &serv](HttpRequest const& req) {
					HttpResponse resp;
					serv.Route(req, resp);
					if (strcmp(req.GetHeader("Connection").c_str(), "Keep-Alive") == 0) {
						resp.SetHeader("Connection", "Keep-Alive");
						conn.Write(resp);
					} else {
						conn.Write(resp);
						conn.Close();
					}
				});
			});

		}

	});

	this_io::Get()->Run();
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
#endif

	int thread_num = 1;
	if (argc > 2)  {
		thread_num = atoi(argv[1]);
	}

	v::io::Logger::Instance().SetLogLevel(LOG_LEVEL::LINFO);

	std::vector<std::thread> v(thread_num);;
	for (int i = 0; i < thread_num; i++) {
		v[i] = std::thread(HttpServer);
	}

	for (int i = 0; i < thread_num; i++) {
		v[i].join();
	}
	getchar();

	return 0;
}