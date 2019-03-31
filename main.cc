
#include <functional>

#include <co/coroutine.h>
#include <coio/common/logger.h>
#include <coio/net/accepter.h>
#include <coio/net/connection.h>
#include <coio/net/protocol_tcp.h>
#include <coio/codec/codec_http.h>
#include <coio/cosys/sysinc.h>
#include <coio/cosys/context.h>



using namespace v;
using namespace v::io;

int main(int argc, char *argv[]) {
#ifdef _WIN32
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
#endif

	io::HttpService serv;
	serv.Register("/test", [](HttpRequest const& req, HttpResponse& resp) {
		resp.Write(HTTP_STATUS_OK);
		resp.Write("yeah!");
	});

	co::Coroutine<void>([&serv]() {
		io::Accepter<io::Tcp> ac;
		ac.Listen(io::NetAddr{ "127.0.0.1", 8383 });
		while (true) {
			auto sock = ac.Accept();
			// create client
			co::Coroutine<void>([sock, &serv]() {
				io::Connection<io::CodecHttp, io::Tcp> conn(sock);
				conn.ReadUntilClose([&conn, &serv](io::HttpRequest const& req) {
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
			})();
			
		}

	})();

	io::this_io::Get()->Run();
	
	getchar();
	
	return 0;
}