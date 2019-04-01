#include "syscall.h"

namespace v {
namespace io {

void MakeFdReuse(int fd) {
	const int on = 1;
#ifdef SO_REUSEPORT
	setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(int));
#endif
#ifdef SO_REUSEADDR
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
#endif
}


void MakeFdNonblock(int fd) {
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

void MakeFdColexec(int fd) {
	fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | FD_CLOEXEC);
}

}
}