#pragma once

#include "coev.h"
#include "syscall.h"

namespace v {
namespace io {

inline int CoAccept(int fd, struct sockaddr *addr, socklen_t *addrlen) {
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

inline int CoConnect(int fd, const struct sockaddr *addr, socklen_t len) {
	// only support nonblock now, assert nonblock
	int ret = 0;
	do {
		ret = SysConnect(fd, addr, len);
	} while (ret < 0 && CHECK_ERROR(SysError(), EINTR));
	if (ret < 0) {
		int err = SysError();
		while (CHECK_ERROR(err, EWOULDBLOCK) || CHECK_ERROR(err, EINPROGRESS)) {
			CoEvent *ev = GetCoEvent(fd);
			ev->EnterCoWrite();
			err =  GetSocketError(fd);
			if (err == 0) {
				return 0;
			}
		}
	}
	return ret;
}

} // v
} // io
