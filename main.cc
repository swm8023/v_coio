
#include <functional>
#include <conet/coroutine/exeunit.h>

using namespace v::co;

int main(int argc, char *argv) {
	FixedsizeStackAllocator alloc;
	CreateJobContext(alloc, []() {
		
	});

    return 0;
}