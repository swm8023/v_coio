#pragma once
#include <functional>

#include <coio/common/logger.h>
#include <co/coroutine.h>

#include "socket.h"
#include "protocol.h"
#include "netaddr.h"


namespace v {
namespace io {
template<typename Protocol>
class Accepter {
public:
    Accepter() {}
    
    ~Accepter() {}
    
	bool Listen(NetAddr const& addr) {
		if (!socket_.Listen(addr)) {
            return false;
        }
		return true;
    }

	Socket<Protocol> Accept() {
		Socket<Protocol> new_sock;
		while (!socket_.Accept(new_sock)) {}
		return new_sock;
	}

private:
    Socket<Protocol> socket_;
	
};

}
}