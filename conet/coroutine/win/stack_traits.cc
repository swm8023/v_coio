#include "conet/coroutine/stack_traits.h"

extern "C" {
#include <windows.h>
}

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <mutex>

// x86_64
// test x86_64 before i386 because icc might
// define __i686__ for x86_64 too
#if defined(__x86_64__) || defined(__x86_64) \
    || defined(__amd64__) || defined(__amd64) \
    || defined(_M_X64) || defined(_M_AMD64)
// Windows seams not to provide a constant or function
// telling the minimal stacksize
# define MIN_STACKSIZE  8 * 1024
#else
# define MIN_STACKSIZE  4 * 1024
#endif

namespace {

void system_info_( SYSTEM_INFO * si) {
    ::GetSystemInfo( si);
}

SYSTEM_INFO system_info() {
    static SYSTEM_INFO si;
    static std::once_flag flag;
    std::call_once( flag, static_cast< void(*)( SYSTEM_INFO *) >( system_info_), & si);
    return si;
}

std::size_t pagesize() {
    return static_cast< std::size_t >( system_info().dwPageSize);
}

}

namespace v {
namespace co {

// Windows seams not to provide a limit for the stacksize
// libcoco uses 32k+4k bytes as minimum
bool StackTraits::is_unbounded() {
    return true;
}

std::size_t StackTraits::page_size() {
    return pagesize();
}

std::size_t StackTraits::default_size() {
    return 128 * 1024;
}

// because Windows seams not to provide a limit for minimum stacksize
std::size_t StackTraits::minimum_size() {
    return MIN_STACKSIZE;
}

// because Windows seams not to provide a limit for maximum stacksize
// maximum_size() can never be called (pre-condition ! is_unbounded() )
std::size_t StackTraits::maximum_size() {
    return  1 * 1024 * 1024 * 1024; // 1GB
}

}
}