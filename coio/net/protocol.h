#pragma once

#include <functional>
#include <coio/common/noncopyable.h>

#include "netaddr.h"

namespace v {
namespace io {


template<typename T>
class Protocol {
public:
	
    virtual int GetFD() const = 0;
    virtual bool Listen(NetAddr const& addr) = 0;
    virtual bool Accept(T& t) = 0;
    
    virtual bool Connect(NetAddr const& addr)  = 0;
	// read data to buffer, return when read somedata(>0) or error(<0) or eof(=0)
    virtual int Read(char* buf, int bufsize) = 0;
	// write data, return when write error or all data sended
	virtual int Write(const char *buf, int bufsize) = 0;
	virtual void Close() = 0;
    

    
};

template<typename T>
using IsProtocol = std::enable_if_t<std::is_base_of_v<Protocol<T>, std::remove_reference_t<T>>>;

}
}