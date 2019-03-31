#pragma once

#include <vector>
#include <cstring>

#include <coio/net/codec.h>

namespace v {
namespace io {

struct PackSimple{
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
