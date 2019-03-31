#pragma once

#include "coev.h"
#include "syscall.h"

namespace v {
namespace io {

inline int CoAccept(int fd, struct sockaddr *addr, int *addrlen) {
	CoEvent* ev = GetCoEvent(fd);
	ev->EnterCoRead();
	// only support nonblock now, assert nonblock
	return SysAccept(fd, addr, addrlen);
}

inline int CoRead(int fd, char* buf, int nbytes) {
	CoEvent *ev = GetCoEvent(fd);
	ev->EnterCoRead();
	// only support nonblock now, assert nonblock
	int ret = 0;
	// handle EINTR here, just recall system read.
	// if errno is EWOULDBLOCK, should call CoRead again.
	do {
		ret = SysRead(fd, buf, nbytes);
	} while (ret < 0 && CHECK_ERROR(SysError(), EINTR));
	return ret;
}

inline int CoWrite(int fd, const char* buf, int nbytes) {
	CoEvent *ev = GetCoEvent(fd);
	ev->EnterCoWrite();
	// same as CoRead
	int ret = 0;
	do {
		ret = SysWrite(fd, buf, nbytes);
	} while (ret < 0 && CHECK_ERROR(SysError(), EINTR));
	return ret;
}

} // v
} // io
