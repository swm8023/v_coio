#pragma once
#include <string>

#include "protocol.h"

namespace v {
namespace io {

template<typename T, typename = IsProtocol<T>>
class Socket {
public:
	Socket() {
	}

	~Socket() {
	}

	int GetFD() const {
		return protocol_.GetFD();
	}

	bool Listen(NetAddr const& addr) {
		return protocol_.Listen(addr);
	}

	bool Connect(NetAddr const& addr) {
		return protocol_.Connect(addr);
	}

	bool Accept(Socket<T> &sock) {
		return protocol_.Accept(sock.protocol_);
	}

	int Read(char *buf, int size) {
		return protocol_.Read(buf, size);
	}

	int Write(const char *buf, int size) {
		return protocol_.Write(buf, size);
	}

	void Close() {
		return protocol_.Close();
	}


	const T& Protocol() const {
		return protocol_;
	}


protected:
	T protocol_;
};

}
}