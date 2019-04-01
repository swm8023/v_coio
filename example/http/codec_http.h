#pragma once

#include <functional>
#include <unordered_map>

#include <http_parser/http_parser.h>
#include <coio/coio.h>

using namespace v::io;

using HttpRequest = http::HttpRequest;
using HttpResponse = http::HttpResponse;

template<typename ReqType, typename RespType>
class CodecHttp : public BaseCodec<ReqType, RespType> {
public:
	CodecHttp() {
		parser_.Init(&request_cahce_, ReqType::PARSER_TYPE,
			std::bind(&CodecHttp::ExecWhenParsed, this, std::placeholders::_1));
	}
	
	int OnReadData(const char* data, int size) {
		parser_.Execute(data, size);
		return read_error_ ? -1 : size;
	};

	virtual std::string ResponseTostr(RespPackType const& pack, int &size) {
		std::string ret = std::move(pack.GetStr());
		size = ret.length();
		return std::move(ret);
	}

	void ExecWhenParsed(bool ret) {
		if (ret) {
			
			if (read_cb_) read_cb_(request_cahce_);
		} else {
			read_error_ = true;
		}
	}

private:
	ReqPackType request_cahce_;
	http::HttpParser parser_;

	bool read_error_{ false };
};

typedef CodecHttp<HttpRequest, HttpResponse> CodecHttpServer;
typedef CodecHttp<HttpResponse, HttpRequest> CodecHttpClient;


class HttpService {
public:
	typedef std::function<void(HttpRequest const&, HttpResponse&)> Handler;

	void Register(std::string path, Handler handler) {
		// TODO, preprocessing path string
		handler_map_[path] = handler;
	}

	bool Route(HttpRequest const& req, HttpResponse &resp) {
		auto iter = handler_map_.find(GetRouteId(req));
		if (iter == handler_map_.end()) {
			MakeResponseError(resp);
			return false;
		}
		(iter->second)(req, resp);
		return true;
	}

	std::string GetRouteId(HttpRequest const& req) {
		return req.url;
	}

	void MakeResponseError(HttpResponse &resp) {

	}

private:

	std::unordered_map<std::string, Handler> handler_map_;
};
