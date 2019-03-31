#include "syscall.h"

namespace v {
namespace io {

void MakeFdReuse(int fd) {
	const int on = 1;
	const int off = 0;
	setsockopt(FD_HANDLE(fd), SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int));
	setsockopt(FD_HANDLE(fd), SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&off, sizeof(int));
}


void MakeFdNonblock(int fd) {
	u_long arg = 1;
	ioctlsocket(FD_HANDLE(fd), FIONBIO, &arg);
}

inline void MakeFdColexec(int fd) {
	// TODO, how to do ???
}

}
}