#pragma once

#include <string>
#include <cstring>
#include <coio/cosys/sysinc.h>

namespace v {
namespace io {


class NetAddr {
public:

    NetAddr() {}
    NetAddr(std::string ip, uint16_t port) : ip_(ip), port_(port) {
        // todo ipv6
        memset(&addr_v4_, 0, sizeof(sockaddr_in));
        addr_v4_.sin_family = AF_INET;
        addr_v4_.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr_v4_.sin_addr);
    }
    
    const std::string& Ip() const {
        return ip_;
    }
    
    const uint16_t Port() const {
        return port_;
    }
    
    int SockProtocol() const {
        return IsV4() ? AF_INET : AF_INET6;
    }
    
    struct sockaddr* SockAddr() const {
        return IsV4() ? (struct sockaddr*)&addr_v4_ : (struct sockaddr*)&addr_v6_;
    }
    
    socklen_t SockAddrSize() const {
        return IsV4() ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
    }
    
    void SetV4(bool v4) {
        is_v4_ = v4;
    }
    
    bool IsV4() const {
        return is_v4_;
    }
    
    bool IsV6() const {
        return !is_v4_;
    }
    
    void RecalIpPort() {
        if (is_v4_) {
            port_ = ntohs(addr_v4_.sin_port);
            char str[30];
            inet_ntop(AF_INET, &addr_v4_.sin_addr, str, 30);
            ip_ = std::string(str);
        } else {
        
        }
    }
    
private:
    std::string ip_;
    uint16_t port_;
    bool is_v4_ {true};
    
    union {
        struct sockaddr_in addr_v4_;
        struct sockaddr_in6 addr_v6_;
    };
    
};

} // io
} // v

