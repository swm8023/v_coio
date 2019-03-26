#pragma once

#include <functional>
#include <cstdio>

#include "stack_traits.h"
#include "fcontext.h"
#include "stack_allocator_basic.h"

namespace v {
namespace co {

typedef std::function<void()> ExeFunc;

template<typename StackAllocator>
class ExeUnit {
public:

	typename std::decay<StackAllocator>::type	stk_allocator_;
	StackContext                                       sctx_;
	ExeFunc                     fn_;

	ExeUnit(StackAllocator &stk_allocator, ExeFunc && func)
		: stk_allocator_(stk_allocator)
	
	{
	}

	void Create() {
		
		make_fcontext()
	}
	fcontext_t Run(fcontext_t fctx) {

	}

	void SwapOut() {

	}

	void SwapIn() {
	}
    
private:
};

static void JobInitDone(transfer_t t) {
	printf("done");
}

template<typename StackAllocator>
static fcontext_t CreateJobContext(StackAllocator &alloc_, ExeFunc &&func) {
	StackContext sctx = alloc_.Alloc();
	

	const std::size_t size = sctx.size - sizeof(ExeUnit<StackAllocator>);
	void * sp = static_cast<char *>(sctx.sp) - sizeof(ExeUnit<StackAllocator>);

	const fcontext_t fctx = make_fcontext(sp, size, &JobInitDone);

	return jump_fcontext(fctx, nullptr).fctx;
}


}
}
