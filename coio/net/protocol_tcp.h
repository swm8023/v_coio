#pragma once

#include <sys/types.h>
#include <coio/common/logger.h>
#include <coio/cosys/syscall.h>
#include <coio/cosys/cosyscall.h>

#include "protocol.h"


namespace v {
namespace io {

class Tcp : public Protocol<Tcp> {
public:
	int GetFD() const {
		return fd_;
	};

	bool Listen(NetAddr const& addr) {
		addr_local_ = addr;

		fd_ = SysSocket(addr.SockProtocol(), SOCK_STREAM, 0);
		RETURN_FALSE_IF_ERROR(fd_ < 0, "Create socket error");

		MakeFdReuse(fd_);
		MakeFdNonblock(fd_);

		if (SysBind(fd_, addr.SockAddr(), addr.SockAddrSize())) {
			Close();
			return false;
		}

		SysListen(fd_, 1000);

		LogInfo("Tcp server listen success on", addr.Ip(), ":", addr.Port());
		return true;
	};

	bool Connect(NetAddr const& addr) {
		LogDebug("begin connect to", addr.Ip(), addr.Port());
		addr_local_ = addr;
		fd_ = SysSocket(addr.SockProtocol(), SOCK_STREAM, 0);
		RETURN_FALSE_IF_ERROR(fd_ < 0, "Create socket error");
		
		MakeFdNonblock(fd_);

		int ret = CoConnect(fd_, addr.SockAddr(), addr.SockAddrSize());
		return ret == 0 ? true : false;
	}

	bool Accept(Tcp &t) {
		// local_addr  = server addr
		// remote_addr = saclient addr
		t.addr_local_ = addr_local_;
		t.addr_remote_.SetV4(addr_remote_.IsV4());
		socklen_t sock_len = t.addr_remote_.SockAddrSize();

		if (0 > (t.fd_ = CoAccept(fd_, t.addr_remote_.SockAddr(), &sock_len))) {
			return false;
		}

		MakeFdNonblock(t.fd_);
		t.addr_remote_.RecalIpPort();
		return true;
	}

	int Read(char* buf, int bufsize) {
		do {
			bufsize = CoRead(fd_, buf, bufsize);
		} while (bufsize < 0 && CHECK_ERROR(SysError(), EWOULDBLOCK));
		return bufsize;
	}

	int Write(const char *buf, int bufsize) {
		// try write directly first
		do {
			bufsize = SysWrite(fd_, buf, bufsize);
		} while (bufsize < 0 && CHECK_ERROR(SysError(), EINTR));
		// wouldblock, begin cowrite
		while (bufsize < 0 && CHECK_ERROR(SysError(), EWOULDBLOCK)) {
			bufsize = CoWrite(fd_, buf, bufsize);
		}
		return bufsize;
	}

	void Close() {
		if (fd_ != -1) {
			ReleaseCoEvent(fd_);
			SysClose(fd_);
			fd_ = -1;
		}
		return;
	}


	const NetAddr& LocalAddr() const {
		return addr_local_;
	}

	const NetAddr& RemoteAddr() const {
		return addr_remote_;
	}


private:
	int fd_{ -1 };
	NetAddr addr_local_;
	NetAddr addr_remote_;
};

}
}