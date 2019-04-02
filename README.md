# v_coio
A coroutine based Network lib

## example
http server
```cpp
HttpService serv;
serv.Register("/test", [](HttpRequest const& req, HttpResponse& resp) {
	resp.Write(HTTP_STATUS_OK);
	resp.Write("yeah!");
});

// create aerver coroutine
RunCoroutine([&serv]() {
	Accepter<Tcp> ac;
	ac.Listen(NetAddr{ "0.0.0.0", 8383 });
	while (true) {
		auto sock = ac.Accept();
		// create client coroutine
		RunCoroutine([sock, &serv]() {
			// create a connection use http codec with socket from accept
			Connection<CodecHttpServer, Tcp> conn(sock);
			// read http request until close called
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
```
http client
```c++
// create a http request
HttpRequest req;
req.Write(HTTP_GET, "/test");
req.SetHeader("Connection", "Keep-Alive");
req.SetHeader("Host", "127.0.0.1");

// create client coroutine
RunCoroutine([&]() {
	Connection<CodecHttpClient, Tcp> conn;
	// connect
	if (false == conn.Connect(NetAddr{ g_ip, g_port })) {
		LogError("Connect failed");
		conn.Close();
		return;
	}

	// write request and get respoinse from server
	conn.Write(req);
	conn.ReadUntilClose([&](HttpResponse const& resp) {
		LogDebug(resp);
		conn.Close();
	});
});

this_io::Get()->Run();
```

## benchmark
use http_client and http_server in example

CPU: Intel Xeon E312xx 16 kernel
8 kernel for client and 8 client for server

QPS: aboud 26W/s
