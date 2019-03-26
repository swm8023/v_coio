#pragma once

#include <cstddef>
#include <cstdlib>
#include <new>

#include "stack_traits.h"

namespace v {
namespace co {

template< typename traitsT >
class StackAllocatorBasic {
public:
    typedef traitsT traits_type;

    StackAllocatorBasic(std::size_t size = traits_type::default_size()) :
        size_(size) {
	}

    StackContext Alloc() {
        void *vp = std::malloc(size_);
        if (!vp) {
            throw std::bad_alloc();
        }
        StackContext sctx;
        sctx.size = size_;
        sctx.sp = static_cast<char *>(vp) + sctx.size;
		printf("%p %d\n", sctx.sp, sctx.size);
        return sctx;
    }

    void Free(StackContext &sctx) {
        void *vp = static_cast<char*>(sctx.sp) - sctx.size;
        std::free( vp);
    }

private:
    std::size_t     size_;


};

typedef StackAllocatorBasic<StackTraits> FixedsizeStackAllocator;

}}

