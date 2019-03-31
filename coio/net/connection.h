#pragma once

#include <functional>
#include <cassert>

#include <co/coroutine.h>

#include "socket.h"
#include "protocol.h"
#include "netaddr.h"

namespace v {
namespace io {

enum {
    CONN_FLG_PASV       = 0x0001, // passive, for server
    CONN_FLG_PORT       = 0x0002,
    CONN_FLG_CONNECTING = 0x0004,
    CONN_FLG_CONNECTED  = 0x0008,
    CONN_FLG_CONNFAILED = 0x0010,
    CONN_FLG_WAIT_CLOSE = 0x0020,
	CONN_FLG_CLOSED     = 0x0040,
	CONN_FLG_IN_READ    = 0x0080,
	CONN_FLG_IN_WRITE   = 0x0100
};

const int BUF_SIZE = 4096;

template<typename Codec, typename Protocol>
class Connection {
public:
	typedef typename Codec::RespPackType RespPackType;
	typedef typename Codec::ReqPackType ReqPackType;
	typedef typename Codec::ReadCallback ReadCallback;

	Connection() {}

	Connection(Socket<Protocol> socket) : Connection() {
		// accept from server, init io and just focus on read
		socket_ = socket;
		flag_ |= CONN_FLG_PASV | CONN_FLG_CONNECTED;
		LogDebug("client", socket_.GetFD(), "connected");
	}

	~Connection() {
	}

	bool Connect(NetAddr const& addr) {
		// create by client
		flag_ |= CONN_FLG_PORT | CONN_FLG_CONNECTING;
		bool ret = socket_.Connect(addr);
		flag |= ret ? CONN_FLG_CONNECTED : CONN_FLG_CONNFAILED;
		flag &= ~CONN_FLG_CONNECTING;
		return ret;
	}

	void ReadUntilClose(ReadCallback const& callback) {
		// only allow one reader
		assert(0 == (flag & CONN_FLG_IN_READ));
		codec_.SetReadCallback(callback);

		flag_ |= CONN_FLG_IN_READ;
		char buf[BUF_SIZE];
		while (flag_ & CONN_FLG_CONNECTED) {
			int read_ret = socket_.Read(buf, BUF_SIZE);
			// maybe close in callback
			if (read_ret > 0) {
				if (0 > codec_.OnReadData(buf, read_ret)) {
					LogError("parse error");
					break;
				}
			}
			// error or eof
			if (read_ret <= 0) {
				LogError("%d Connection error ", socket_.GetFD(), SysError());
				break;
			}
		}
		flag_ &= ~CONN_FLG_IN_READ;
		Close();
	}

	void Write(RespPackType const& pack) {
		// TODO, wait other writer
		// only allow one writer
		flag_ |= CONN_FLG_IN_WRITE;
		int size = 0, writed = 0, write_ret = 0;
		std::string s = codec_.ResponseTostr(pack, size);
		while (writed < size) {
			write_ret = socket_.Write(s.c_str() + writed, size - writed);
			if (write_ret < 0) {
				Close();
				break;
			}
			writed += write_ret;
		}
		flag_ &= ~CONN_FLG_IN_WRITE;
	}

	void Close() {
		// if in reading, close delay
		flag_ &= ~(CONN_FLG_CONNECTING | CONN_FLG_CONNECTED | CONN_FLG_CONNFAILED);
		if ((flag_ & CONN_FLG_IN_READ) == 0) {
			LogDebug("client", socket_.GetFD(), "close");
			socket_.Close();
			flag_ |= CONN_FLG_CLOSED;
		}
		return;
	}

private:
    Socket<Protocol> socket_;
    Codec codec_;
    
    int flag_ {0};
};

}
}