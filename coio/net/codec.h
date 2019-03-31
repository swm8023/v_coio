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

	virtual int OnReadData(const char* data, int size, ReadCallback const& read_cb) = 0;
	virtual std::string ResponseTostr(RespPackType const&, int &size) = 0;

	void SetReadCallback(ReadCallback const& read_cb) {
		read_cb_ = read_cb;
	}

protected:
	ReadCallback read_cb_;
};
 
}
}