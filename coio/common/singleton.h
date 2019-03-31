#ifndef TIO_SINGLETON_H_
#define TIO_SINGLETON_H_

#include "noncopyable.h"

namespace v {
namespace io {
template<typename T>
class Singleton : NonCopyable{
public:
    static T& Instance() {
        static T t;
        return t;
    }
    
protected:
    Singleton() {};
    ~Singleton() {};
};

}
}

#endif // TIO_SINGLETON_H_
