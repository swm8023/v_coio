#pragma once

#include <functional>
#include <string>

namespace v {
namespace io {

template<typename ReqPackT, typename RespPackT>
class BaseCodec {
public:
	typedef ReqPackT ReqPackType;
	typedef RespPackT RespPackType;

	typedef std::function<void(ReqPackType const& f)> ReadCallback;

	virtual int OnReadData(const char* data, int size) = 0;
	virtual std::string ResponseTostr(RespPackType const&, int &size) = 0;

	void SetReadCallback(ReadCallback const& read_cb) {
		read_cb_ = read_cb;
	}

protected:
	ReadCallback read_cb_;
};
 
// Codec Simple
struct PackSimple {
	char buf[4096];
};

class CodecSimple : public BaseCodec<PackSimple, PackSimple> {
public:
	int OnReadData(const char* data, int size) {
		ReqPackType req;
		memcpy(req.buf, data, size);
		if (read_cb_) read_cb_(req);
		return size;
	};
	virtual std::string ResponseTostr(RespPackType const& pack, int &size) {
		size = strlen(pack.buf);
		return std::move(std::string(pack.buf));
	}
};

}
}