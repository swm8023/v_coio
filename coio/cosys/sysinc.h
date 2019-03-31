#pragma once

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <io.h>

#define FD_DESC(handle) _open_osfhandle ((handle), 0)
#define FD_HANDLE(fd) _get_osfhandle(fd)
#define CHECK_ERROR(err, ErrName) ((err) ==  WSA##ErrName)

#else
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define FD_DESC(handle) handle
#define FD_HANDLE(fd)
#define CHECK_ERROR(err, ErrName) ((err) ==  ErrName)
#endif
