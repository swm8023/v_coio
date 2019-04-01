#pragma once

#include "sysinc.h"
#include "stdio.h"

namespace v {
namespace io {

inline int SysAccept(int fd, struct sockaddr *addr, socklen_t *addrlen) {
	return FD_DESC(accept(FD_HANDLE(fd), addr, addrlen));
}

inline int SysRead(int fd, char* buf, int nbytes) {
	return recv(FD_HANDLE(fd), buf, nbytes, 0);
}

inline int SysWrite(int fd, const char* buf, int nbytes) {
	return send(FD_HANDLE(fd), buf, nbytes, 0);
}

inline int SysSocket(int af, int type, int protocol) {
	return FD_DESC(socket(af, type, protocol));
}

inline int SysBind(int fd, const struct sockaddr* addr, socklen_t addrlen) {
	return bind(FD_HANDLE(fd), addr, addrlen);
}

inline int SysListen(int fd, int backlog) {
	return listen(FD_HANDLE(fd), backlog);
}

inline int SysConnect(int fd, const struct sockaddr* addr, socklen_t addrlen) {
	return connect(FD_HANDLE(fd), addr, addrlen);
}

inline void SysClose(int fd) {
#ifdef _WIN32
	closesocket(FD_HANDLE(fd));
#else
	close(fd);
#endif
}

inline int SysError() {
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

int GetSocketError(int fd);


void MakeFdReuse(int fd);
void MakeFdNonblock(int fd);
void MakeFdColexec(int fd);

}
}
